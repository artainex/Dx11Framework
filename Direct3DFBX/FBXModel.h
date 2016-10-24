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
	struct Line
	{
		VERTEX_DATA_LP start;
		VERTEX_DATA_LP end;
		Line(VERTEX_DATA_LP st, VERTEX_DATA_LP ed) :start(st), end(ed) {}
	};

	class FBXModel
	{
		CFBXLoader*		m_pFBX;
		std::vector<FBX_DATA::MESH_NODE>	m_meshNodeArray;
		HRESULT CreateNodes(ID3D11Device*	pd3dDevice);
		HRESULT VertexConstructionByModel(ID3D11Device* pd3dDevice, FBX_DATA::MESH_NODE& meshNode, const MeshInfo& meshInfo);
		HRESULT MaterialConstructionByModel(ID3D11Device* pd3dDevice, FBX_DATA::MESH_NODE& meshNode, const ModelInfo& modelInfo, const UINT& index);
		HRESULT CreateVertexBuffer(ID3D11Device* pd3dDevice, ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount);
		HRESULT CreateIndexBuffer(ID3D11Device*	pd3dDevice, ID3D11Buffer** pBuffer, void* pIndices, uint32_t indexCount);

	public:
		FBXModel();
		~FBXModel();
		void Release();

		HRESULT LoadFBX(const char* filename, ID3D11Device*	pd3dDevice);
		HRESULT CreateInputLayout(ID3D11Device*	pd3dDevice, const void* pShaderBytecodeWithInputSignature, size_t BytecodeLength, D3D11_INPUT_ELEMENT_DESC* pLayout, UINT layoutSize);

		HRESULT RenderAll(ID3D11DeviceContext* pImmediateContext);
		HRESULT RenderNode(ID3D11DeviceContext* pImmediateContext, const size_t node);
		HRESULT RenderNodeDepth(ID3D11DeviceContext* pImmediateContext, const size_t node, ID3D11InputLayout* depthLayout);
		HRESULT RenderNodeInstancing(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, const uint32_t InstanceCount);
		HRESULT RenderNodeInstancingIndirect(ID3D11DeviceContext* pImmediateContext, const size_t nodeId, ID3D11Buffer* pBufferForArgs, const uint32_t AlignedByteOffsetForArgs);
		
		XMFLOAT4 GetMeshColor(const int& id) { return m_meshNodeArray[id].m_meshColor; }
		void SetMeshColor(const int& id, const float& r, const float& g, const float& b, const float& a) { m_meshNodeArray[id].SetMeshColor(XMFLOAT4(r,g,b,a)); }
		void SetMeshColor(const int& id, const XMFLOAT4& color) { m_meshNodeArray[id].SetMeshColor(color); }
		size_t GetMeshNodeCount() { return m_meshNodeArray.size(); }
		FBX_DATA::MESH_NODE& GetMeshNode(const int& id) { return m_meshNodeArray[id]; };
		XMMATRIX GetNodeTM(const int& id) { return m_meshNodeArray[id].m_meshTM; }
		FBX_DATA::Material_Data& GetNodeFbxMaterial(const int& id, const int& mtrl_id = 0) { return m_meshNodeArray[id].fbxmtrlData[mtrl_id]; };
		eLayout GetLayoutType(const int& id) { return m_meshNodeArray[id].m_Layout; }
		bool IsSkinned();

		void Update(double timedelta);
		void UpdateMatPal(XMMATRIX* matPal = nullptr);
		void SetNodeMtxPal(XMMATRIX* matPal = nullptr, FBX_DATA::MESH_NODE* pMesh = nullptr);
	};

}	// namespace ursine