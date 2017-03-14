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
#include <Graphics.h>

namespace ursine
{
	FBXModel::FBXModel() :
		m_pFBX(nullptr)
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

		SAFE_DELETE(m_pFBX);
	}

	HRESULT FBXModel::LoadFBX(const char* filename)
	{
		if (!filename)
			return E_FAIL;

		HRESULT hr = S_OK;

		m_pFBX = new CFBXLoader;
		hr = m_pFBX->LoadFBX(filename);
		FAIL_CHECK(hr);

		hr = CreateNodes();
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::CreateNodes()
	{
		HRESULT hr = S_OK;
		const ModelInfo& modelInfo = m_pFBX->GetModelInfo();
		const AnimInfo& animInfo = m_pFBX->GetAnimInfo();

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
			meshNode.m_Stride = currMI.mStride;
			meshNode.m_StrideCompatible = currMI.mStrideCompatible;

			// constructing vertex buffer
			VertexConstructionByModel(meshNode, modelInfo.mMeshInfoVec[i], animInfo.IsValid());
			meshNode.indexCount = static_cast<DWORD>(currMI.meshVtxIdxCount);
			meshNode.SetIndexBit(meshNode.indexCount);

			UINT* indices = new UINT[meshNode.indexCount];
			for (unsigned j = 0; j < meshNode.indexCount; ++j)
				indices[j] = currMI.meshVtxIndices[j];

			if (meshNode.indexCount > 0)
				hr = CreateIndexBuffer(
					&meshNode.m_pIB,
					indices,
					static_cast<uint32_t>(meshNode.indexCount));

			// constructing materials 
			MaterialConstructionByModel(meshNode, modelInfo, i);
			
			m_meshNodeArray.push_back(meshNode);
			delete indices;
		}
		return hr;
	}

	HRESULT FBXModel::CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
	{
		if (stride == 0 || vertexCount == 0)
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
		ID3D11Device* device = GRAPHICS.GetPipeline()->GetDevice();
		hr = device->CreateBuffer(&bd, &InitData, pBuffer);
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::CreateIndexBuffer(ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount)
	{
		if (indexCount == 0)
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

		ID3D11Device* device = GRAPHICS.GetPipeline()->GetDevice();
		hr = device->CreateBuffer(&bd, &InitData, pBuffer);
		FAIL_CHECK(hr);

		return hr;
	}

	HRESULT FBXModel::VertexConstructionByModel(FBX_DATA::MESH_NODE& meshNode, const MeshInfo& meshInfo, bool IsSkinned)
	{
		const MeshInfo& currMI = meshInfo;
		meshNode.vertexCount = static_cast<DWORD>(currMI.meshVtxInfoCount);
		meshNode.indexCount = static_cast<DWORD>(currMI.meshVtxIdxCount);

		if (0 == meshNode.vertexCount || 0 == meshNode.indexCount)
			return E_FAIL;

		HRESULT hr = S_OK;
		UINT i = 0;

		if (!IsSkinned)
		{
			VERTEX_DATA_GEO* pVS = new VERTEX_DATA_GEO[currMI.meshVtxInfoCount];
			VERTEX_DATA_COMPATIBLE* pVSC = new VERTEX_DATA_COMPATIBLE[currMI.meshVtxInfoCount];

			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx(-100.f, -100.f, -100.f);
				XMFLOAT3 currNorm(-100.f,-100.f,-100.f);
				XMFLOAT3 currBin(-100.f, -100.f, -100.f);
				XMFLOAT3 currTan(-100.f, -100.f, -100.f);
				XMFLOAT2 currUV(-100.f, -100.f);

				switch (meshNode.m_Layout)
				{
				case LAYOUT0:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				}
				break;
				case LAYOUT1:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currUV = XMFLOAT2(iter.uv.x, iter.uv.y);
				}
				break;
				case LAYOUT2:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
					currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
				}
				break;
				case LAYOUT3:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
					currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
					currUV = XMFLOAT2(iter.uv.x, iter.uv.y);
				}
				break;
				}

				pVS[i].vPos = currVtx;
				pVS[i].vNor = currNorm;
				pVS[i].vBin = currBin;
				pVS[i].vTan = currTan;
				pVS[i].vTexcoord = currUV;

				pVSC[i].vPos = currVtx;
				pVSC[i].vNor = currNorm;
				pVSC[i].vTexcoord = currUV;

				++i;
			}

			hr = CreateVertexBuffer(&meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_GEO), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);

			hr = CreateVertexBuffer(&meshNode.m_pVBC, pVSC, sizeof(VERTEX_DATA_COMPATIBLE), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVSC);
			// 그냥 지오랑 스킨을 나누는 건 좋은데, 만약 노말이나 바이노멀, 탄젠트, UV가 없는 애들은 어떡하지?
			// 기본적으로 -100, -100, -100 해두자.
			// 아니면 매크로로 IsNormal, IsBinormal, IsTangent이런거 하거나 해야하는데, 좀 더 생각해보던가
			// 어떻게 하면 효율적으로 버텍스 포맷을 굳이 바꾸지 않으면서 처리할 수 있을지
		}
		else
		{
			VERTEX_DATA_SKIN* pVS = new VERTEX_DATA_SKIN[currMI.meshVtxInfoCount];

			for (auto &iter : currMI.meshVtxInfos)
			{
				XMFLOAT3 currVtx(-100.f, -100.f, -100.f);
				XMFLOAT3 currNorm(-100.f, -100.f, -100.f);
				XMFLOAT3 currBin(-100.f, -100.f, -100.f);
				XMFLOAT3 currTan(-100.f, -100.f, -100.f);
				XMFLOAT2 currUV(-100.f, -100.f);

				switch (meshNode.m_Layout)
				{
				case LAYOUT4:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
				}
				break;
				case LAYOUT5:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currUV = XMFLOAT2(iter.uv.x, iter.uv.y);
				}
				break;
				case LAYOUT6:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
					currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
				}
				break;
				case LAYOUT7:
				{
					currVtx = XMFLOAT3(iter.pos.x, iter.pos.y, iter.pos.z);
					currNorm = XMFLOAT3(iter.normal.x, iter.normal.y, iter.normal.z);
					currBin = XMFLOAT3(iter.binormal.x, iter.binormal.y, iter.binormal.z);
					currTan = XMFLOAT3(iter.tangent.x, iter.tangent.y, iter.tangent.z);
					currUV = XMFLOAT2(iter.uv.x, iter.uv.y);
				}
				break;
				}

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

			hr = CreateVertexBuffer(&meshNode.m_pVB, pVS, sizeof(VERTEX_DATA_SKIN), meshNode.vertexCount);
			SAFE_DELETE_ARRAY(pVS);
		}

		return hr;
	}

	HRESULT FBXModel::MaterialConstructionByModel(FBX_DATA::MESH_NODE& meshNode, const ModelInfo& modelInfo, const UINT& index)
	{
		const MaterialInfo& currMI = modelInfo.mMtrlInfoVec[index];
		if (0 == modelInfo.mmaterialCount)
			return E_FAIL;

		//--------------------------------------------------------
		// Will this work for multi-material?
		//--------------------------------------------------------
		HRESULT hr = S_OK;
		ID3D11Device* device = GRAPHICS.GetPipeline()->GetDevice();
		
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
					sizeof(MaterialBuffer), 
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
							D3DX11CreateShaderResourceViewFromFile(device, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
						}
					}
				}
				// if there's no texture, just use default texture
				else {
					std::string path1 = "Assets/uv.png";
					D3DX11CreateShaderResourceViewFromFile(device, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
				}

				// for default normal map
				std::string path2 = "Assets/norm.png";
				D3DX11CreateShaderResourceViewFromFile(device, path2.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[1], nullptr);

				// Setting sampler as default texture
				hr = device->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
				FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

				// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
				hr = device->CreateBuffer(&mtrlBufferDesc, nullptr, &currFbxMtrl.pMaterialCb);
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
				sizeof(MaterialBuffer), 
				D3D11_USAGE_DYNAMIC, 
				D3D11_BIND_CONSTANT_BUFFER, 
				D3D11_CPU_ACCESS_WRITE);

			// Create Shader Resource View from default texture and material
			std::string path1 = "Assets/uv.png";
			D3DX11CreateShaderResourceViewFromFile(device, path1.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[0], nullptr);
			// for default normal map
			std::string path2 = "Assets/norm.png";
			D3DX11CreateShaderResourceViewFromFile(device, path2.c_str(), nullptr, nullptr, &currFbxMtrl.pSRV[1], nullptr);

			// Setting sampler as default texture
			hr = device->CreateSamplerState(&sampDesc, &currFbxMtrl.pSampler);
			FAIL_CHECK_WITH_MSG(hr, "Creating Sampler State Failed");

			// currently, constant buffer creation always failed because constant buffer size should be multiple of 16
			hr = device->CreateBuffer(&mtrlBufferDesc, nullptr, &currFbxMtrl.pMaterialCb);
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
	
	void FBXModel::BindMeshNode(const int& id)
	{
		if (id < 0 || id > m_meshNodeArray.size() - 1)
			return;

		ID3D11DeviceContext* deviceContext = GRAPHICS.GetPipeline()->GetContext();

		// Set vertex buffer stride and offset.
		UINT stride = m_meshNodeArray[id].m_Stride;
		UINT offset = 0;
		
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &m_meshNodeArray[id].m_pVB, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(m_meshNodeArray[id].m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}	// namespace FBX_LOADER
