// *********************************************************************************************************************
/// 
/// @file 		CFBXRendererDX11.cpp
/// @brief		FBX Rnderer
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************


#include < locale.h >
#include "CFBXLoader.h"
#include "CFBXRendererDX11.h"

namespace ursine
{
	CFBXRenderDX11::CFBXRenderDX11() :
		m_pFBX(nullptr)
	{
	}

	CFBXRenderDX11::~CFBXRenderDX11()
	{
		Release();
	}

	void CFBXRenderDX11::Release()
	{
		for (size_t i = 0; i<m_meshNodeArray.size(); i++)
		{
			m_meshNodeArray[i].Release();
		}
		m_meshNodeArray.clear();

		if (m_pFBX)
		{
			delete m_pFBX;
			m_pFBX = nullptr;
		}
	}

	HRESULT CFBXRenderDX11::LoadFBX(const char* filename, ID3D11Device*	pd3dDevice)
	{
		if (!filename || !pd3dDevice)
			return E_FAIL;

		HRESULT hr = S_OK;

		m_pFBX = new CFBXLoader;
		hr = m_pFBX->LoadFBX(filename);
		if (FAILED(hr))
			return hr;

		hr = CreateNodes(pd3dDevice);
		if (FAILED(hr))
			return hr;

		return hr;
	}

	HRESULT CFBXRenderDX11::CreateNodes(ID3D11Device* pd3dDevice)
	{
		if (!pd3dDevice) 
			return E_FAIL;

		HRESULT hr = S_OK;
		const ModelInfo& modelInfo = m_pFBX->GetModelInfo();

		// if there is no mesh, doesn't mean it failed to loading FBX
		size_t meshCnt = modelInfo.mmeshCount;
		if (0 == meshCnt) 
			return E_FAIL;
		
		for (size_t i = 0; i<meshCnt; ++i)
		{
			const MeshInfo& currMI = modelInfo.mMeshInfoVec[i];
			
			FBX_DATA::MESH_NODE meshNode;
			meshNode.m_meshName = currMI.name;

			if (0 == modelInfo.mboneCount)
				meshNode.m_Layout = FBX_DATA::eLayout::STATIC;
			else
				meshNode.m_Layout = FBX_DATA::eLayout::SKINNED;

			meshNode.m_meshTM = currMI.meshTM;

			// constructing vertex buffer
			VertexConstructionByModel(pd3dDevice, meshNode, modelInfo.mMeshInfoVec[i]);
			meshNode.indexCount = static_cast<DWORD>(currMI.meshVtxIdxCount);
			meshNode.SetIndexBit(meshNode.indexCount);

			UINT* indices = new UINT[meshNode.indexCount];
			for (unsigned j = 0; j < meshNode.indexCount; ++j)
				indices[j] = currMI.meshVtxIndices[j];

			if (meshNode.indexCount > 0)
				hr = CreateIndexBuffer(pd3dDevice,
					&meshNode.m_pIB,
					indices,
					static_cast<uint32_t>(meshNode.indexCount));

			// constructing materials 
			MaterialConstructionByModel(pd3dDevice, meshNode, modelInfo, i);
			m_meshNodeArray.push_back(meshNode);

			delete indices;
		}
		return hr;
	}

	HRESULT CFBXRenderDX11::CreateVertexBuffer(ID3D11Device* pd3dDevice, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
	{
		if (!pd3dDevice || stride == 0 || vertexCount == 0)
			return E_FAIL;

		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = stride * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		InitData.pSysMem = pVertices;
		hr = pd3dDevice->CreateBuffer(&bd, &InitData, pBuffer);
		if (FAILED(hr))
			return hr;

		return hr;
	}

	HRESULT CFBXRenderDX11::CreateIndexBuffer(ID3D11Device*	pd3dDevice, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount)
	{
		if (!pd3dDevice || indexCount == 0)
			return E_FAIL;

		HRESULT hr = S_OK;
		size_t stride = sizeof(UINT);

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = static_cast<uint32_t>(stride*indexCount);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		InitData.pSysMem = pIndices;

		hr = pd3dDevice->CreateBuffer(&bd, &InitData, pBuffer);
		if (FAILED(hr))
			return hr;

		return hr;
	}

	HRESULT CFBXRenderDX11::VertexConstructionByModel(ID3D11Device* pd3dDevice, FBX_DATA::MESH_NODE& meshNode, const MeshInfo& meshInfo)
	{
		const MeshInfo& currMI = meshInfo;
		meshNode.vertexCount = static_cast<DWORD>(currMI.meshVtxInfoCount);
		meshNode.indexCount = static_cast<DWORD>(currMI.meshVtxIdxCount);

		if (!pd3dDevice || 0 == meshNode.vertexCount || 0 == meshNode.indexCount)
			return E_FAIL;

		HRESULT hr = S_OK;
		UINT i = 0;
		switch (meshNode.m_Layout)
		{
		case FBX_DATA::STATIC:
		{
			FBX_DATA::VERTEX_DATA_STATIC* pVS = new FBX_DATA::VERTEX_DATA_STATIC[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx	= XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm	= XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT2 currUV		= XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos			= currVtx;
				pVS[i].vNor			= currNorm;
				pVS[i].vTexcoord	= currUV;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_STATIC), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;

		case FBX_DATA::SKINNED:
		{
			FBX_DATA::VERTEX_DATA_SKIN* pVK = new FBX_DATA::VERTEX_DATA_SKIN[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx	= XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm	= XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT2 currUV		= XMFLOAT2(iter.uv.x, iter.uv.y);

				pVK[i].vPos			= currVtx;
				pVK[i].vNor			= currNorm;
				pVK[i].vTexcoord	= currUV;

				// blend bone idx, weight
				pVK[i].vBIdx[0]		= (BYTE)iter.ctrlIndices.x;
				pVK[i].vBIdx[1]		= (BYTE)iter.ctrlIndices.y;
				pVK[i].vBIdx[2]		= (BYTE)iter.ctrlIndices.z;
				pVK[i].vBIdx[3]		= (BYTE)iter.ctrlIndices.w;
				pVK[i].vBWeight.x	= (float)iter.ctrlBlendWeights.x;
				pVK[i].vBWeight.y	= (float)iter.ctrlBlendWeights.y;
				pVK[i].vBWeight.z	= (float)iter.ctrlBlendWeights.z;
				pVK[i].vBWeight.w	= (float)iter.ctrlBlendWeights.w;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVK, sizeof(FBX_DATA::VERTEX_DATA_SKIN), meshNode.vertexCount);
			if (pVK)
				delete[] pVK;
		}
		break;
		}

		return hr;
	}

	HRESULT CFBXRenderDX11::MaterialConstructionByModel(ID3D11Device* pd3dDevice,
		FBX_DATA::MESH_NODE& meshNode, const ModelInfo& modelInfo, const UINT& index)
	{
		const MaterialInfo& currMI = modelInfo.mMtrlInfoVec[index];
		if (!pd3dDevice || 0 == modelInfo.mmaterialCount)
			return E_FAIL;

		HRESULT hr = S_OK;
		// Pass Data to Material_Data structure
		meshNode.fbxmaterialData.resize(modelInfo.mmaterialCount);
		for (unsigned i = 0; i < modelInfo.mmaterialCount; ++i)
		{
			if (currMI.diff_mapCount > 0)
			{
				for (unsigned j = 0; j < currMI.diff_mapCount; ++j)
				{
					if (currMI.diff_texNames[j] != "")
					{
						std::string path = "Assets/";
						std::string folder = currMI.diff_texNames[j];
						path += folder;
						D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path.c_str(), NULL, NULL, &meshNode.fbxmaterialData[i].pSRV, NULL);
					}
				}
			}
			// if there's no texture, just use default texture
			else {
				std::string path = "Assets\\uv.png";
				D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path.c_str(), NULL, NULL, &meshNode.fbxmaterialData[i].pSRV, NULL);
			}
			
			// samplerstate
			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
			hr = pd3dDevice->CreateSamplerState(&sampDesc, &meshNode.fbxmaterialData[i].pSampler);

			// material Constant Buffer
			D3D11_BUFFER_DESC bufDesc;
			ZeroMemory(&bufDesc, sizeof(bufDesc));
			const uint32_t stride = static_cast<uint32_t>(sizeof(FBX_DATA::Material_Consts));
			bufDesc.ByteWidth = stride;
			bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufDesc.CPUAccessFlags = 0;

			// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
			hr = pd3dDevice->CreateBuffer(&bufDesc, nullptr, &meshNode.fbxmaterialData[i].pMaterialCb);

			meshNode.fbxmaterialData[i].materialConst.ambient	= currMI.ambi_mcolor;
			meshNode.fbxmaterialData[i].materialConst.diffuse	= currMI.diff_mcolor;
			meshNode.fbxmaterialData[i].materialConst.specular	= currMI.spec_mcolor;
			meshNode.fbxmaterialData[i].materialConst.emissive	= currMI.emis_mcolor;
			meshNode.fbxmaterialData[i].materialConst.shineness = currMI.shineness;
			meshNode.fbxmaterialData[i].materialConst.TransparencyFactor = currMI.TransparencyFactor;
		}
		return S_OK;
	}

	HRESULT CFBXRenderDX11::CreateInputLayout(ID3D11Device*	pd3dDevice,
		const void* pShaderBytecodeWithInputSignature,
		size_t BytecodeLength,
		D3D11_INPUT_ELEMENT_DESC* pLayout, UINT layoutSize)
	{
		// InputeLayout
		if (!pd3dDevice || !pShaderBytecodeWithInputSignature || !pLayout)
			return E_FAIL;

		HRESULT hr = S_OK;
		size_t nodeCount = m_meshNodeArray.size();
		for (size_t i = 0; i<nodeCount; ++i)
		{
			pd3dDevice->CreateInputLayout(pLayout,
				layoutSize,
				pShaderBytecodeWithInputSignature,
				BytecodeLength,
				&m_meshNodeArray[i].m_pInputLayout);
		}

		return hr;
	}

	HRESULT CFBXRenderDX11::RenderAll(ID3D11DeviceContext* pImmediateContext)
	{
		size_t nodeCount = m_meshNodeArray.size();
		if (nodeCount == 0)
			return S_OK;

		HRESULT hr = S_OK;
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto &iter : m_meshNodeArray)
		{
			if (iter.vertexCount == 0)
				continue;

			UINT stride = 0;
			switch (iter.m_Layout)
			{
			case FBX_DATA::STATIC: stride = sizeof(FBX_DATA::VERTEX_DATA_STATIC); break;
			case FBX_DATA::SKINNED: stride = sizeof(FBX_DATA::VERTEX_DATA_SKIN); break;
			}
			UINT offset = 0;
			pImmediateContext->IASetVertexBuffers(0, 1, &iter.m_pVB, &stride, &offset);

			DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
			if (iter.m_indexBit == FBX_DATA::MESH_NODE::INDEX_32BIT)
				indexbit = DXGI_FORMAT_R32_UINT;

			pImmediateContext->IASetInputLayout(iter.m_pInputLayout);
			pImmediateContext->IASetIndexBuffer(iter.m_pIB, indexbit, 0);
			pImmediateContext->DrawIndexed(iter.indexCount, 0, 0);
		}

		return hr;
	}

	// export
	HRESULT CFBXRenderDX11::RenderNode(ID3D11DeviceContext* pImmediateContext, const size_t nodeId)
	{
		size_t nodeCount = m_meshNodeArray.size();
		if (nodeCount == 0 || nodeCount <= nodeId)
			return S_OK;

		HRESULT hr = S_OK;
		FBX_DATA::MESH_NODE* node = &m_meshNodeArray[nodeId];
		if (node->vertexCount == 0)
			return S_OK;

		UINT stride = 0;
		switch (node->m_Layout)
		{
		case FBX_DATA::STATIC: stride = sizeof(FBX_DATA::VERTEX_DATA_STATIC); break;
		case FBX_DATA::SKINNED: stride = sizeof(FBX_DATA::VERTEX_DATA_SKIN); break;
		}

		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &node->m_pVB, &stride, &offset);
		pImmediateContext->IASetInputLayout(node->m_pInputLayout);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (node->m_indexBit != FBX_DATA::MESH_NODE::INDEX_NOINDEX)
		{
			DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
			if (node->m_indexBit == FBX_DATA::MESH_NODE::INDEX_32BIT)
				indexbit = DXGI_FORMAT_R32_UINT;
			pImmediateContext->IASetIndexBuffer(node->m_pIB, indexbit, 0);
			pImmediateContext->DrawIndexed(node->indexCount, 0, 0);
		}

		return hr;
	}

	HRESULT CFBXRenderDX11::RenderNodeInstancing(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, const uint32_t InstanceCount)
	{
		size_t nodeCount = m_meshNodeArray.size();
		if (nodeCount == 0 || nodeCount <= nodeId || InstanceCount == 0)
			return S_OK;

		HRESULT hr = S_OK;

		FBX_DATA::MESH_NODE* node = &m_meshNodeArray[nodeId];

		if (node->vertexCount == 0)
			return S_OK;

		UINT stride = 0;
		switch (node->m_Layout)
		{
		case FBX_DATA::STATIC: stride = sizeof(FBX_DATA::VERTEX_DATA_STATIC); break;
		case FBX_DATA::SKINNED: stride = sizeof(FBX_DATA::VERTEX_DATA_SKIN); break;
		}

		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &node->m_pVB, &stride, &offset);
		pImmediateContext->IASetInputLayout(node->m_pInputLayout);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 
		if (node->m_indexBit != FBX_DATA::MESH_NODE::INDEX_NOINDEX)
		{
			DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
			if (node->m_indexBit == FBX_DATA::MESH_NODE::INDEX_32BIT)
				indexbit = DXGI_FORMAT_R32_UINT;
			pImmediateContext->IASetIndexBuffer(node->m_pIB, indexbit, 0);
			pImmediateContext->DrawIndexedInstanced(node->indexCount, InstanceCount, 0, 0, 0);
		}

		return hr;
	}

	HRESULT CFBXRenderDX11::RenderNodeInstancingIndirect(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, ID3D11Buffer* pBufferForArgs, const uint32_t AlignedByteOffsetForArgs)
	{
		size_t nodeCount = m_meshNodeArray.size();
		if (nodeCount == 0 || nodeCount <= nodeId)
			return S_OK;

		HRESULT hr = S_OK;

		FBX_DATA::MESH_NODE* node = &m_meshNodeArray[nodeId];

		if (node->vertexCount == 0)
			return S_OK;

		UINT stride = 0;
		switch (node->m_Layout)
		{
		case FBX_DATA::STATIC: stride = sizeof(FBX_DATA::VERTEX_DATA_STATIC); break;
		case FBX_DATA::SKINNED: stride = sizeof(FBX_DATA::VERTEX_DATA_SKIN); break;
		}

		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &node->m_pVB, &stride, &offset);
		pImmediateContext->IASetInputLayout(node->m_pInputLayout);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (node->m_indexBit != FBX_DATA::MESH_NODE::INDEX_NOINDEX)
		{
			DXGI_FORMAT indexbit = DXGI_FORMAT_R16_UINT;
			if (node->m_indexBit == FBX_DATA::MESH_NODE::INDEX_32BIT)
				indexbit = DXGI_FORMAT_R32_UINT;
			pImmediateContext->IASetIndexBuffer(node->m_pIB, indexbit, 0);
			pImmediateContext->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
		}

		return hr;
	}

	void CFBXRenderDX11::Update(double timedelta)
	{
		if (m_pFBX)
			m_pFBX->Update(timedelta);
	}

	void CFBXRenderDX11::UpdateMatPal(XMMATRIX* matPal)
	{
		if (m_pFBX)
			m_pFBX->UpdateMatPal(matPal);
	}

	void CFBXRenderDX11::SetNodeMtxPal(XMMATRIX* matPal, FBX_DATA::MESH_NODE* pMesh)
	{
		if (!matPal || !pMesh)
			return;

		size_t mtSize = pMesh->m_meshMtxPal.size();
		for (UINT i = 0; i < mtSize; ++i)
		{
			XMMATRIX palette_for_hlsl = pMesh->m_meshMtxPal[i];
			matPal[i] = XMMatrixTranspose(palette_for_hlsl);
		}
	}

	bool CFBXRenderDX11::IsSkinned()
	{
		return m_pFBX->IsSkinned();
	}

}	// namespace FBX_LOADER
