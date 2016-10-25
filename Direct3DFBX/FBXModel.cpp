// *********************************************************************************************************************
/// 
/// @file 		FBXModel.cpp
/// @brief		FBX Rnderer
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************


#include < locale.h >
#include "CFBXLoader.h"
#include "FBXModel.h"

namespace ursine
{
	FBXModel::FBXModel() :
		m_pFBX(nullptr)
		//m_bonevsLayout(nullptr),
		//m_pboneInputLayout(nullptr),
		//m_boneVS(nullptr),
		//m_boneVB(nullptr)
	{
	}

	FBXModel::~FBXModel()
	{
		Release();
	}

	void FBXModel::Release()
	{
		for (auto &meshNode : m_meshNodeArray)
			meshNode.Release();
		m_meshNodeArray.clear();

		//SAFE_RELEASE(m_boneVB);
		//SAFE_RELEASE(m_boneVS);
		//SAFE_RELEASE(m_pboneInputLayout);
		//SAFE_RELEASE(m_bonevsLayout);
		SAFE_DELETE(m_pFBX);
	}

	HRESULT FBXModel::LoadFBX(const char* filename, ID3D11Device*	pd3dDevice)
	{
		if (!filename || !pd3dDevice)
			return E_FAIL;

		HRESULT hr = S_OK;

		m_pFBX = new CFBXLoader;
		hr = m_pFBX->LoadFBX(filename);
		FAIL_CHECK(hr);

		hr = CreateNodes(pd3dDevice);
		FAIL_CHECK(hr);

		//hr = CreatePoint(pd3dDevice);
		//FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::CreateNodes(ID3D11Device* pd3dDevice)
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

			// constructing mesh color buffer
			D3D11_BUFFER_DESC meshBufferDesc;
			BufferInitialize(meshBufferDesc,
				sizeof(MeshBufferType),
				D3D11_USAGE_DYNAMIC,
				D3D11_BIND_CONSTANT_BUFFER,
				D3D11_CPU_ACCESS_WRITE);
			
			hr = pd3dDevice->CreateBuffer(&meshBufferDesc, nullptr, &meshNode.m_meshColorBuffer);
			FAIL_CHECK_WITH_MSG(hr, "MeshBuffer creation failed");

			m_meshNodeArray.push_back(meshNode);
			delete indices;
		}
		return hr;
	}

	HRESULT FBXModel::CreateVertexBuffer(ID3D11Device* pd3dDevice, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
	{
		if (!pd3dDevice || stride == 0 || vertexCount == 0)
			return E_FAIL;

		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC bd;
		BufferInitialize(bd, 
			stride * vertexCount,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		InitData.pSysMem = pVertices;
		hr = pd3dDevice->CreateBuffer(&bd, &InitData, pBuffer);
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::CreateIndexBuffer(ID3D11Device* pd3dDevice, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount)
	{
		if (!pd3dDevice || indexCount == 0)
			return E_FAIL;

		HRESULT hr = S_OK;
		size_t stride = sizeof(UINT);

		D3D11_BUFFER_DESC bd;
		BufferInitialize(bd,
			static_cast<uint32_t>(stride*indexCount),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_INDEX_BUFFER);

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		InitData.pSysMem = pIndices;

		hr = pd3dDevice->CreateBuffer(&bd, &InitData, pBuffer);
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::VertexConstructionByModel(ID3D11Device* pd3dDevice, FBX_DATA::MESH_NODE& meshNode, const MeshInfo& meshInfo)
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
		case LAYOUT0:
		{
			VERTEX_DATA_L0* pVS = new VERTEX_DATA_L0[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L0), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT1:
		{
			VERTEX_DATA_L1* pVS = new VERTEX_DATA_L1[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT2 currUV = XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vTexcoord = currUV;

				++i;
			}

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L1), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT2:
		{
			VERTEX_DATA_L2* pVS = new VERTEX_DATA_L2[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L2), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT3:
		{
			VERTEX_DATA_L3* pVS = new VERTEX_DATA_L3[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L3), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT4:
		{
			VERTEX_DATA_L4* pVS = new VERTEX_DATA_L4[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L4), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT5:
		{
			VERTEX_DATA_L5* pVS = new VERTEX_DATA_L5[currMI.meshVtxInfoCount];
			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
				XMFLOAT3 currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				XMFLOAT2 currUV = XMFLOAT2(iter.uv.x, iter.uv.y);

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L5), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT6:
		{
			VERTEX_DATA_L6* pVS = new VERTEX_DATA_L6[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L6), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		case LAYOUT7:
		{
			VERTEX_DATA_L7* pVS = new VERTEX_DATA_L7[currMI.meshVtxInfoCount];
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

			hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_L7), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}
		break;
		}

		i = 0;
		VERTEX_DATA_LT* pVSDepth = new VERTEX_DATA_LT[currMI.meshVtxInfoCount];
		for (auto &iter : currMI.meshVtxInfos)
		{
			XMFLOAT3 currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
			XMFLOAT2 currTex = XMFLOAT2(0.f, 0.f);
			pVSDepth[i].vPos = currVtx;
			pVSDepth[i].vTexcoord = currTex;
			++i;
		}
		hr = CreateVertexBuffer(pd3dDevice, &meshNode.m_pVBDepth, pVSDepth, sizeof(VERTEX_DATA_LT), meshNode.vertexCount);
		SAFE_DELETE_ARRAY(pVSDepth);

		return hr;
	}

	HRESULT FBXModel::MaterialConstructionByModel(ID3D11Device* pd3dDevice,
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
							D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
						}
					}
				}
				// if there's no texture, just use default texture
				else {
					std::string path1 = "Assets/uv.png";
					D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
				}

				// for default normal map
				std::string path2 = "Assets/norm.png";
				D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path2.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[1], nullptr);

				// Setting sampler as default texture
				hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
				FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

				// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
				hr = pd3dDevice->CreateBuffer(&mtrlBufferDesc, nullptr, &currFbxMtrl.pMaterialCb);
				FAIL_CHECK_WITH_MSG(hr, "Constant buffer is not size of multiple of 16");

				currFbxMtrl.mtrlConst.ambient		= XMFLOAT4(currMI.ambi_mcolor.x, currMI.ambi_mcolor.y, currMI.ambi_mcolor.z, 1.f);
				currFbxMtrl.mtrlConst.diffuse		= XMFLOAT4(currMI.diff_mcolor.x, currMI.diff_mcolor.y, currMI.diff_mcolor.z, 1.f);
				currFbxMtrl.mtrlConst.specular		= XMFLOAT4(currMI.spec_mcolor.x, currMI.spec_mcolor.y, currMI.spec_mcolor.z, 1.f);
				currFbxMtrl.mtrlConst.emissive		= XMFLOAT4(currMI.emis_mcolor.x, currMI.emis_mcolor.y, currMI.emis_mcolor.z, 1.f);
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
			BufferInitialize(mtrlBufferDesc, 
				sizeof(MaterialBufferType), 
				D3D11_USAGE_DYNAMIC, 
				D3D11_BIND_CONSTANT_BUFFER, 
				D3D11_CPU_ACCESS_WRITE);

			// Create Shader Resource View from default texture and material
			std::string path1 = "Assets/uv.png";
			D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
			// for default normal map
			std::string path2 = "Assets/norm.png";
			D3DX11CreateShaderResourceViewFromFile(pd3dDevice, path2.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[1], nullptr);

			// Setting sampler as default texture
			hr = pd3dDevice->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
			FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

			// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
			hr = pd3dDevice->CreateBuffer(&mtrlBufferDesc, nullptr, &currFbxMtrl.pMaterialCb);
			FAIL_CHECK_WITH_MSG(hr, "Constant buffer is not size of multiple of 16");

			currFbxMtrl.mtrlConst.ambient		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.f);
			currFbxMtrl.mtrlConst.diffuse		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.f);
			currFbxMtrl.mtrlConst.specular		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.f);
			currFbxMtrl.mtrlConst.emissive		= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f);
			currFbxMtrl.mtrlConst.shineness		= 0.1f;
			currFbxMtrl.mtrlConst.transparency	= 1.0f;
		}

		// where should I add code for default normalmap?

		return S_OK;
	}

	HRESULT FBXModel::CreateInputLayout(ID3D11Device*	pd3dDevice,
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

	HRESULT FBXModel::RenderAll(ID3D11DeviceContext* pImmediateContext)
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
			case LAYOUT0: stride = sizeof(VERTEX_DATA_L0); break;
			case LAYOUT1: stride = sizeof(VERTEX_DATA_L1); break;
			case LAYOUT2: stride = sizeof(VERTEX_DATA_L2); break;
			case LAYOUT3: stride = sizeof(VERTEX_DATA_L3); break;
			case LAYOUT4: stride = sizeof(VERTEX_DATA_L4); break;
			case LAYOUT5: stride = sizeof(VERTEX_DATA_L5); break;
			case LAYOUT6: stride = sizeof(VERTEX_DATA_L6); break;
			case LAYOUT7: stride = sizeof(VERTEX_DATA_L7); break;
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

	HRESULT FBXModel::RenderNode(ID3D11DeviceContext* pImmediateContext, const size_t nodeId)
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
		case LAYOUT0: stride = sizeof(VERTEX_DATA_L0); break;
		case LAYOUT1: stride = sizeof(VERTEX_DATA_L1); break;
		case LAYOUT2: stride = sizeof(VERTEX_DATA_L2); break;
		case LAYOUT3: stride = sizeof(VERTEX_DATA_L3); break;
		case LAYOUT4: stride = sizeof(VERTEX_DATA_L4); break;
		case LAYOUT5: stride = sizeof(VERTEX_DATA_L5); break;
		case LAYOUT6: stride = sizeof(VERTEX_DATA_L6); break;
		case LAYOUT7: stride = sizeof(VERTEX_DATA_L7); break;
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

	HRESULT FBXModel::RenderNodeDepth(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, ID3D11InputLayout* depthLayout)
	{
		size_t nodeCount = m_meshNodeArray.size();
		if (nodeCount == 0 || nodeCount <= nodeId)
			return S_OK;

		HRESULT hr = S_OK;
		FBX_DATA::MESH_NODE* node = &m_meshNodeArray[nodeId];
		if (node->vertexCount == 0)
			return S_OK;

		UINT stride = sizeof(VERTEX_DATA_LT);

		UINT offset = 0; 
		pImmediateContext->IASetVertexBuffers(0, 1, &node->m_pVBDepth, &stride, &offset);
		pImmediateContext->IASetInputLayout(depthLayout);
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

	HRESULT FBXModel::RenderNodeInstancing(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, const uint32_t InstanceCount)
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
		case LAYOUT0: stride = sizeof(VERTEX_DATA_L0); break;
		case LAYOUT1: stride = sizeof(VERTEX_DATA_L1); break;
		case LAYOUT2: stride = sizeof(VERTEX_DATA_L2); break;
		case LAYOUT3: stride = sizeof(VERTEX_DATA_L3); break;
		case LAYOUT4: stride = sizeof(VERTEX_DATA_L4); break;
		case LAYOUT5: stride = sizeof(VERTEX_DATA_L5); break;
		case LAYOUT6: stride = sizeof(VERTEX_DATA_L6); break;
		case LAYOUT7: stride = sizeof(VERTEX_DATA_L7); break;
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

	HRESULT FBXModel::RenderNodeInstancingIndirect(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, ID3D11Buffer* pBufferForArgs, const uint32_t AlignedByteOffsetForArgs)
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
		case LAYOUT0: stride = sizeof(VERTEX_DATA_L0); break;
		case LAYOUT1: stride = sizeof(VERTEX_DATA_L1); break;
		case LAYOUT2: stride = sizeof(VERTEX_DATA_L2); break;
		case LAYOUT3: stride = sizeof(VERTEX_DATA_L3); break;
		case LAYOUT4: stride = sizeof(VERTEX_DATA_L4); break;
		case LAYOUT5: stride = sizeof(VERTEX_DATA_L5); break;
		case LAYOUT6: stride = sizeof(VERTEX_DATA_L6); break;
		case LAYOUT7: stride = sizeof(VERTEX_DATA_L7); break;
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
	
	void FBXModel::Update(double timedelta)
	{
		if (m_pFBX)
			m_pFBX->Update(timedelta);
	}

	void FBXModel::UpdateMatPal(XMMATRIX* matPal)
	{
		if (m_pFBX)
			m_pFBX->UpdateMatPal(matPal);
	}

	void FBXModel::SetNodeMtxPal(XMMATRIX* matPal, FBX_DATA::MESH_NODE* pMesh)
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

	bool FBXModel::IsSkinned()
	{
		return m_pFBX->IsSkinned();
	}

}	// namespace FBX_LOADER
