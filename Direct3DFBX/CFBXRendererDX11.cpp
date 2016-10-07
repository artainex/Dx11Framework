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

void SamplerInitialize(D3D11_SAMPLER_DESC &sampler,
	D3D11_FILTER Filter,
	D3D11_TEXTURE_ADDRESS_MODE AddressU,
	D3D11_TEXTURE_ADDRESS_MODE AddressV,
	D3D11_TEXTURE_ADDRESS_MODE AddressW,
	D3D11_COMPARISON_FUNC ComparisonFunc,
	FLOAT MinLOD,
	FLOAT MaxLOD)
{
	ZeroMemory(&sampler, sizeof(sampler));
	sampler.Filter = Filter;
	sampler.AddressU = AddressU;
	sampler.AddressV = AddressV;
	sampler.AddressW = AddressW;
	sampler.ComparisonFunc = ComparisonFunc;
	sampler.MinLOD = MinLOD;
	sampler.MaxLOD = MaxLOD;
}

void BufferInitialize(D3D11_BUFFER_DESC& buffer,
	UINT byteWidth = 0,
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
	UINT bindFlags = 0,
	UINT cpuAccFlags = 0,
	UINT miscFlags = 0,
	UINT stride = 0)
{
	ZeroMemory(&buffer, sizeof(buffer));
	buffer.ByteWidth = byteWidth;
	buffer.Usage = usage;
	buffer.BindFlags = bindFlags;
	buffer.CPUAccessFlags = cpuAccFlags;
	buffer.MiscFlags = miscFlags;
	buffer.StructureByteStride = stride;
}

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
		for (auto &meshNode : m_meshNodeArray)
			meshNode.Release();
		m_meshNodeArray.clear();

		SAFE_DELETE(m_pFBX);
	}

	HRESULT CFBXRenderDX11::LoadFBX(const char* filename, ID3D11Device*	pd3dDevice)
	{
		if (!filename || !pd3dDevice)
			return E_FAIL;

		HRESULT hr = S_OK;

		m_pFBX = new CFBXLoader;
		hr = m_pFBX->LoadFBX(filename);
		FAIL_CHECK(hr);

		hr = CreateNodes(pd3dDevice);
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT CFBXRenderDX11::CreateNodes(ID3D11Device* pd3dDevice)
	{
		if (!pd3dDevice) 
			return E_FAIL;

		HRESULT hr = S_OK;
		const ModelInfo& modelInfo = m_pFBX->GetModelInfo();

		// if there is no mesh, doesn't mean it failed to loading FBX
		UINT meshCnt = modelInfo.mmeshCount;
		if (0 == meshCnt) 
			return E_FAIL;
		
		for (UINT i = 0; i < meshCnt; ++i)
		{
			const MeshInfo& currMI = modelInfo.mMeshInfoVec[i];
			
			FBX_DATA::MESH_NODE meshNode;
			meshNode.m_meshName = currMI.name;
			meshNode.m_Layout = currMI.mLayout;
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
		FAIL_CHECK(hr);

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
		FAIL_CHECK(hr);

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
		case FBX_DATA::LAYOUT0:
		{
			FBX_DATA::VERTEX_DATA_L0* pVS = new FBX_DATA::VERTEX_DATA_L0[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				
				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L0), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT1:
		{
			FBX_DATA::VERTEX_DATA_L1* pVS = new FBX_DATA::VERTEX_DATA_L1[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L1), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT2:
		{
			FBX_DATA::VERTEX_DATA_L2* pVS = new FBX_DATA::VERTEX_DATA_L2[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT3 currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
				XMFLOAT3 currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vBin = currBin;
				pVS[i].vTan = currTan;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L2), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT3:
		{
			FBX_DATA::VERTEX_DATA_L3* pVS = new FBX_DATA::VERTEX_DATA_L3[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT3 currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
				XMFLOAT3 currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
				XMFLOAT2 currUV = XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vBin = currBin;
				pVS[i].vTan = currTan;
				pVS[i].vTexcoord = currUV;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L3), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT4:
		{
			FBX_DATA::VERTEX_DATA_L4* pVS = new FBX_DATA::VERTEX_DATA_L4[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;

				// blend bone idx, weight
				pVS[i].vBIdx[0] = (BYTE)iter.ctrlIndices.x;
				pVS[i].vBIdx[1] = (BYTE)iter.ctrlIndices.y;
				pVS[i].vBIdx[2] = (BYTE)iter.ctrlIndices.z;
				pVS[i].vBIdx[3] = (BYTE)iter.ctrlIndices.w;
				pVS[i].vBWeight.x = (float)iter.ctrlBlendWeights.x;
				pVS[i].vBWeight.y = (float)iter.ctrlBlendWeights.y;
				pVS[i].vBWeight.z = (float)iter.ctrlBlendWeights.z;
				pVS[i].vBWeight.w = (float)iter.ctrlBlendWeights.w;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L4), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT5:
		{
			FBX_DATA::VERTEX_DATA_L5* pVS = new FBX_DATA::VERTEX_DATA_L5[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx	= XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm	= XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT2 currUV		= XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos			= currVtx;
				pVS[i].vNor			= currNorm;
				pVS[i].vTexcoord	= currUV;

				// blend bone idx, weight
				pVS[i].vBIdx[0]		= (BYTE)iter.ctrlIndices.x;
				pVS[i].vBIdx[1]		= (BYTE)iter.ctrlIndices.y;
				pVS[i].vBIdx[2]		= (BYTE)iter.ctrlIndices.z;
				pVS[i].vBIdx[3]		= (BYTE)iter.ctrlIndices.w;
				pVS[i].vBWeight.x	= (float)iter.ctrlBlendWeights.x;
				pVS[i].vBWeight.y	= (float)iter.ctrlBlendWeights.y;
				pVS[i].vBWeight.z	= (float)iter.ctrlBlendWeights.z;
				pVS[i].vBWeight.w	= (float)iter.ctrlBlendWeights.w;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L5), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT6:
		{
			FBX_DATA::VERTEX_DATA_L6* pVS = new FBX_DATA::VERTEX_DATA_L6[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT3 currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
				XMFLOAT3 currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vBin = currBin;
				pVS[i].vTan = currTan;

				// blend bone idx, weight
				pVS[i].vBIdx[0] = (BYTE)iter.ctrlIndices.x;
				pVS[i].vBIdx[1] = (BYTE)iter.ctrlIndices.y;
				pVS[i].vBIdx[2] = (BYTE)iter.ctrlIndices.z;
				pVS[i].vBIdx[3] = (BYTE)iter.ctrlIndices.w;
				pVS[i].vBWeight.x = (float)iter.ctrlBlendWeights.x;
				pVS[i].vBWeight.y = (float)iter.ctrlBlendWeights.y;
				pVS[i].vBWeight.z = (float)iter.ctrlBlendWeights.z;
				pVS[i].vBWeight.w = (float)iter.ctrlBlendWeights.w;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L6), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		case FBX_DATA::LAYOUT7:
		{
			FBX_DATA::VERTEX_DATA_L7* pVS = new FBX_DATA::VERTEX_DATA_L7[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT3 currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
				XMFLOAT3 currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
				XMFLOAT2 currUV = XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vBin = currBin;
				pVS[i].vTan = currTan;
				pVS[i].vTexcoord = currUV;

				// blend bone idx, weight
				pVS[i].vBIdx[0] = (BYTE)iter.ctrlIndices.x;
				pVS[i].vBIdx[1] = (BYTE)iter.ctrlIndices.y;
				pVS[i].vBIdx[2] = (BYTE)iter.ctrlIndices.z;
				pVS[i].vBIdx[3] = (BYTE)iter.ctrlIndices.w;
				pVS[i].vBWeight.x = (float)iter.ctrlBlendWeights.x;
				pVS[i].vBWeight.y = (float)iter.ctrlBlendWeights.y;
				pVS[i].vBWeight.z = (float)iter.ctrlBlendWeights.z;
				pVS[i].vBWeight.w = (float)iter.ctrlBlendWeights.w;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_L7), meshNode.vertexCount);
			if (pVS)
				delete[] pVS;
		}
		break;
		//case FBX_DATA::LAYOUTT:
		//{
		//	FBX_DATA::VERTEX_DATA_LT* pVS = new FBX_DATA::VERTEX_DATA_LT[currMI.meshVtxInfoCount];
		//	for (auto &iter : currMI.meshVtxInfos)
		//	{
		//		XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
		//		XMFLOAT2 currUV = XMFLOAT2(iter.uv.x, iter.uv.y);
		//
		//		pVS[i].vPos = currVtx;
		//		pVS[i].vTexcoord = currUV;
		//
		//		++i;
		//	}
		//
		//	hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(FBX_DATA::VERTEX_DATA_LT), meshNode.vertexCount);
		//	if (pVS)
		//		delete[] pVS;
		//}
		//break;
		}

		return hr;
	}

	HRESULT CFBXRenderDX11::MaterialConstructionByModel(ID3D11Device* pd3dDevice,
		FBX_DATA::MESH_NODE& meshNode, const ModelInfo& modelInfo, const UINT& index)
	{
		const MaterialInfo& currMI = modelInfo.mMtrlInfoVec[index];
		if (!pd3dDevice || 0 == modelInfo.mmaterialCount)
			return E_FAIL;

		//--------------------------------------------------------
		// Will this work for multi-material?
		//--------------------------------------------------------
		HRESULT hr = S_OK;
		
		// Pass Data to Material_Data structure
		if (0 != modelInfo.mmaterialCount)
		{
			meshNode.fbxmtrlData.resize(modelInfo.mmaterialCount);
			for (unsigned i = 0; i < modelInfo.mmaterialCount; ++i)
			{
				auto &currFbxMtrl = meshNode.fbxmtrlData[i];

				// samplerstate
				D3D11_SAMPLER_DESC sampDesc;
				SamplerInitialize(sampDesc,
					D3D11_FILTER_MIN_MAG_MIP_LINEAR,
					D3D11_TEXTURE_ADDRESS_WRAP,
					D3D11_TEXTURE_ADDRESS_WRAP,
					D3D11_TEXTURE_ADDRESS_WRAP,
					D3D11_COMPARISON_NEVER,
					0, D3D11_FLOAT32_MAX);

				// material Constant Buffer
				D3D11_BUFFER_DESC mtrlBufferDesc;
				BufferInitialize(mtrlBufferDesc, 
					sizeof(MaterialBufferType), 
					D3D11_USAGE_DYNAMIC, 
					D3D11_BIND_CONSTANT_BUFFER, 
					D3D11_CPU_ACCESS_WRITE);

				// Just handle diffuse for now. handle others later
				// Create Shader Resource View from default texture and material
				if (currMI.diff_mapCount > 0)
				{
					for (unsigned j = 0; j < currMI.diff_mapCount; ++j)
					{
						if (!currMI.diff_texNames[j].empty())
						{
							std::string path1 = "Assets/";
							std::string folder = currMI.diff_texNames[j];
							path1 += folder;
							D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), NULL, NULL, &currFbxMtrl.pSRV[0], NULL);
						}
					}
				}
				// if there's no texture, just use default texture
				else {
					std::string path1 = "Assets/uv.png";
					D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), NULL, NULL, &currFbxMtrl.pSRV[0], NULL);
				}

				// for default normal map
				std::string path2 = "Assets/norm.png";
				D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path2.c_str(), NULL, NULL, &currFbxMtrl.pSRV[1], NULL);

				// Setting sampler as default texture
				hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
				FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");
				//hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler[0]);
				//FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");
				//hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler[1]);
				//FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

				// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
				hr = pd3dDevice->CreateBuffer(&mtrlBufferDesc, NULL, &currFbxMtrl.pMaterialCb);
				FAIL_CHECK_WITH_MSG(hr, "Constant buffer is not size of multiple of 16");

				currFbxMtrl.mtrlConst.ambient		= currMI.ambi_mcolor;
				currFbxMtrl.mtrlConst.diffuse		= currMI.diff_mcolor;
				currFbxMtrl.mtrlConst.specular		= currMI.spec_mcolor;
				currFbxMtrl.mtrlConst.emissive		= currMI.emis_mcolor;
				currFbxMtrl.mtrlConst.shineness		= currMI.shineness;
				currFbxMtrl.mtrlConst.transparency	= currMI.transparency;
			}
		}
		else
		{
			meshNode.fbxmtrlData.resize(1);

			auto &currFbxMtrl = meshNode.fbxmtrlData[0];

			// samplerstate
			D3D11_SAMPLER_DESC sampDesc;
			SamplerInitialize(sampDesc, 
				D3D11_FILTER_MIN_MAG_MIP_LINEAR, 
				D3D11_TEXTURE_ADDRESS_WRAP, 
				D3D11_TEXTURE_ADDRESS_WRAP, 
				D3D11_TEXTURE_ADDRESS_WRAP,
				D3D11_COMPARISON_NEVER,
				0, D3D11_FLOAT32_MAX);

			// material Constant Buffer
			D3D11_BUFFER_DESC mtrlBufferDesc;
			BufferInitialize(mtrlBufferDesc, sizeof(MaterialBufferType), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);

			// Create Shader Resource View from default texture and material
			std::string path1 = "Assets/uv.png";
			D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), NULL, NULL, &currFbxMtrl.pSRV[0], NULL);
			// for default normal map
			std::string path2 = "Assets/norm.png";
			D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path2.c_str(), NULL, NULL, &currFbxMtrl.pSRV[1], NULL);

			// Setting sampler as default texture
			hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
			FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");
			//hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler[0]);
			//FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");
			//hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler[1]);
			//FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

			// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
			hr = pd3dDevice->CreateBuffer(&mtrlBufferDesc, NULL, &currFbxMtrl.pMaterialCb);
			FAIL_CHECK_WITH_MSG(hr, "Constant buffer is not size of multiple of 16");

			currFbxMtrl.mtrlConst.ambient		= pseudodx::XMFLOAT3(0.1f, 0.1f, 0.1f);
			currFbxMtrl.mtrlConst.diffuse		= pseudodx::XMFLOAT3(1.0f, 1.0f, 1.0f);
			currFbxMtrl.mtrlConst.specular		= pseudodx::XMFLOAT3(1.0f, 1.0f, 1.0f);
			currFbxMtrl.mtrlConst.emissive		= pseudodx::XMFLOAT3(0.0f, 0.0f, 0.0f);
			currFbxMtrl.mtrlConst.shineness		= 0.1f;
			currFbxMtrl.mtrlConst.transparency	= 1.0f;
		}

		// where should I add code for default normalmap?

		return S_OK;
	}

	HRESULT CFBXRenderDX11::CreateInputLayout(ID3D11Device*	pd3dDevice,
		const void* pShaderBytecodeWithInputSignature,
		size_t BytecodeLength,
		D3D11_INPUT_ELEMENT_DESC* pLayout, 
		UINT layoutSize)
	{
		// InputeLayout
		if (!pd3dDevice || !pShaderBytecodeWithInputSignature || !pLayout)
			return E_FAIL;

		HRESULT hr = S_OK;

		for (auto &iter : m_meshNodeArray)
		{
			pd3dDevice->CreateInputLayout(pLayout,
				layoutSize,
				pShaderBytecodeWithInputSignature,
				BytecodeLength,
				&iter.m_pInputLayout);
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
			case FBX_DATA::LAYOUT0: stride = sizeof(FBX_DATA::VERTEX_DATA_L0); break;
			case FBX_DATA::LAYOUT1: stride = sizeof(FBX_DATA::VERTEX_DATA_L1); break;
			case FBX_DATA::LAYOUT2: stride = sizeof(FBX_DATA::VERTEX_DATA_L2); break;
			case FBX_DATA::LAYOUT3: stride = sizeof(FBX_DATA::VERTEX_DATA_L3); break;
			case FBX_DATA::LAYOUT4: stride = sizeof(FBX_DATA::VERTEX_DATA_L4); break;
			case FBX_DATA::LAYOUT5: stride = sizeof(FBX_DATA::VERTEX_DATA_L5); break;
			case FBX_DATA::LAYOUT6: stride = sizeof(FBX_DATA::VERTEX_DATA_L6); break;
			case FBX_DATA::LAYOUT7: stride = sizeof(FBX_DATA::VERTEX_DATA_L7); break;
			//case FBX_DATA::LAYOUTT: stride = sizeof(FBX_DATA::VERTEX_DATA_LT); break;
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
		case FBX_DATA::LAYOUT0: stride = sizeof(FBX_DATA::VERTEX_DATA_L0); break;
		case FBX_DATA::LAYOUT1: stride = sizeof(FBX_DATA::VERTEX_DATA_L1); break;
		case FBX_DATA::LAYOUT2: stride = sizeof(FBX_DATA::VERTEX_DATA_L2); break;
		case FBX_DATA::LAYOUT3: stride = sizeof(FBX_DATA::VERTEX_DATA_L3); break;
		case FBX_DATA::LAYOUT4: stride = sizeof(FBX_DATA::VERTEX_DATA_L4); break;
		case FBX_DATA::LAYOUT5: stride = sizeof(FBX_DATA::VERTEX_DATA_L5); break;
		case FBX_DATA::LAYOUT6: stride = sizeof(FBX_DATA::VERTEX_DATA_L6); break;
		case FBX_DATA::LAYOUT7: stride = sizeof(FBX_DATA::VERTEX_DATA_L7); break;
		//case FBX_DATA::LAYOUTT: stride = sizeof(FBX_DATA::VERTEX_DATA_LT); break;
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
		case FBX_DATA::LAYOUT0: stride = sizeof(FBX_DATA::VERTEX_DATA_L0); break;
		case FBX_DATA::LAYOUT1: stride = sizeof(FBX_DATA::VERTEX_DATA_L1); break;
		case FBX_DATA::LAYOUT2: stride = sizeof(FBX_DATA::VERTEX_DATA_L2); break;
		case FBX_DATA::LAYOUT3: stride = sizeof(FBX_DATA::VERTEX_DATA_L3); break;
		case FBX_DATA::LAYOUT4: stride = sizeof(FBX_DATA::VERTEX_DATA_L4); break;
		case FBX_DATA::LAYOUT5: stride = sizeof(FBX_DATA::VERTEX_DATA_L5); break;
		case FBX_DATA::LAYOUT6: stride = sizeof(FBX_DATA::VERTEX_DATA_L6); break;
		case FBX_DATA::LAYOUT7: stride = sizeof(FBX_DATA::VERTEX_DATA_L7); break;
		//case FBX_DATA::LAYOUTT: stride = sizeof(FBX_DATA::VERTEX_DATA_LT); break;
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
		case FBX_DATA::LAYOUT0: stride = sizeof(FBX_DATA::VERTEX_DATA_L0); break;
		case FBX_DATA::LAYOUT1: stride = sizeof(FBX_DATA::VERTEX_DATA_L1); break;
		case FBX_DATA::LAYOUT2: stride = sizeof(FBX_DATA::VERTEX_DATA_L2); break;
		case FBX_DATA::LAYOUT3: stride = sizeof(FBX_DATA::VERTEX_DATA_L3); break;
		case FBX_DATA::LAYOUT4: stride = sizeof(FBX_DATA::VERTEX_DATA_L4); break;
		case FBX_DATA::LAYOUT5: stride = sizeof(FBX_DATA::VERTEX_DATA_L5); break;
		case FBX_DATA::LAYOUT6: stride = sizeof(FBX_DATA::VERTEX_DATA_L6); break;
		case FBX_DATA::LAYOUT7: stride = sizeof(FBX_DATA::VERTEX_DATA_L7); break;
		//case FBX_DATA::LAYOUTT: stride = sizeof(FBX_DATA::VERTEX_DATA_LT); break;
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
