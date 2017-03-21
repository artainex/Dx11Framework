// *********************************************************************************************************************
/// 
/// @file 		FBXModel.h
/// @brief		FBX Rnderer
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************


#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <Utilities.h>
#include "CFBXLoader.h"

namespace ursine
{
	class FBXModel
	{
		CFBXLoader*		m_pFBX;
		std::vector<FBX_DATA::MESH_NODE>	m_meshNodeArray;
		HRESULT CreateNodes();
		HRESULT VertexConstructionByModel(FBX_DATA::MESH_NODE& meshNode, const MeshInfo& meshInfo, bool IsSkinned);
		HRESULT MaterialConstructionByModel(FBX_DATA::MESH_NODE& meshNode, const ModelInfo& modelInfo, const UINT& index);
		HRESULT CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
		HRESULT CreateIndexBuffer(ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount);

	public:
		FBXModel();
		~FBXModel();
		void Release();

		HRESULT LoadFBX(const char* filename);
		
		size_t GetMeshNodeCount() { return m_meshNodeArray.size(); }
		FBX_DATA::MESH_NODE& GetMeshNode(const int& id) { return m_meshNodeArray[id]; };
		XMMATRIX GetNodeTM(const int& id) { return m_meshNodeArray[id].m_meshTM; }
		FBX_DATA::Material_Data& GetNodeFbxMaterial(const int& id, const int& mtrl_id = 0) { return m_meshNodeArray[id].fbxmtrlData[mtrl_id]; };
		void BindMeshNode(const int& id);
		eLayout GetLayoutType(const int& id) { return m_meshNodeArray[id].m_Layout; }
		bool IsSkinned() { return m_pFBX->IsSkinned(); }

		void Update(double timedelta);
		void UpdateMatPal(XMMATRIX* matPal = nullptr);
		void SetNodeMtxPal(XMMATRIX* matPal = nullptr, FBX_DATA::MESH_NODE* pMesh = nullptr);
	};

}	// namespace ursine