/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** AnimationDef.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
**
** Contributors:
** - Matt Yan - m.yan@digipen.edu
** -------------------------------------------------------------------------*/

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <set>
#include <unordered_map>
#include <iostream>
#include <SMat4.h>
#include <ConstantBuffers.h>

#pragma warning (disable : 4458)

using namespace DirectX;

#define POS		0x0000	// POSITION
#define NOR		0x0001	// NORAML
#define BITAN	0x0011	// BINORMAL, TANGENT
#define	TEX		0x0100	// TEXTURE
#define SKIN	0x1000	// SKIN

namespace ursine
{
	inline bool IsEqualEpsilon(float A, float B) { return fabs(A - B) <= 1e-5f; }

	namespace FBX_DATA
	{
		// UVSet
		typedef std::tr1::unordered_map<std::string, int> UVsetID;
		// UVSet
		typedef std::tr1::unordered_map<std::string, std::vector<std::string>> TextureSet;

		// layout
		enum eLayout
		{
			NONE = -1,
			LAYOUT0,	// POS NOR
			LAYOUT1, 	// POS NOR TEX
			LAYOUT2, 	// POS NOR BIN TAN
			LAYOUT3, 	// POS NOR BIN TAN TEX
			LAYOUT4, 	// POS NOR BW BI
			LAYOUT5, 	// POS NOR TEX BW BI
			LAYOUT6, 	// POS NOR BIN TAN BW BI
			LAYOUT7		// POS NOR BIN TAN TEX BW BI
		};

		// 노말 맵 만들려면 binormal이랑 tangent를 써야 하는데,
		// 이걸 갖고 있는 놈도 있고 안갖고 나오는 놈도 있어.
		// 동적으로 이걸 만들어내면 수맣은 레이아웃과 VERTEX_DATA~를 만들어내야하는데
		// 이건 상당히 멍청한 짓이야. 맷이 한 거처럼 .Add()를 써서 여럿 붙일 수 있으면 좋을텐데.
		struct LAYOUT
		{
			D3D11_INPUT_ELEMENT_DESC LAYOUT0[2];
			D3D11_INPUT_ELEMENT_DESC LAYOUT1[3];
			D3D11_INPUT_ELEMENT_DESC LAYOUT2[4];
			D3D11_INPUT_ELEMENT_DESC LAYOUT3[5];		
			D3D11_INPUT_ELEMENT_DESC LAYOUT4[4];
			D3D11_INPUT_ELEMENT_DESC LAYOUT5[5];
			D3D11_INPUT_ELEMENT_DESC LAYOUT6[6];
			D3D11_INPUT_ELEMENT_DESC LAYOUT7[7];

			// TRY OPTIMIZE THESE FORMATS 
			LAYOUT() 
			{
				// pos, material, nbt, tex, bw, bi				
				LAYOUT0[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT0[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
								
				LAYOUT1[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT1[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT1[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				LAYOUT2[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT2[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT2[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT2[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				LAYOUT3[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				LAYOUT4[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT4[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT4[2] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT4[3] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				LAYOUT5[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT5[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT5[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT5[3] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT5[4] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				LAYOUT6[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT6[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT3[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT6[4] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT6[5] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };				
				
				LAYOUT7[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT7[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT7[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT7[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT5[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT7[5] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				LAYOUT7[6] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}		  
		};

		// POS NOR
		struct VERTEX_DATA_L0
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
		};

		// POS NOR TEX
		struct VERTEX_DATA_L1
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT2	vTexcoord;
		};

		// POS NOR BIN TAN
		struct VERTEX_DATA_L2
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT3	vBin;
			XMFLOAT3	vTan;
		};

		// POS NOR BIN TAN TEX
		struct VERTEX_DATA_L3
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT3	vBin;
			XMFLOAT3	vTan;
			XMFLOAT2	vTexcoord;
		};

		// POS NOR BW BI
		struct VERTEX_DATA_L4
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT4	vBWeight;
			BYTE		vBIdx[4];
		};

		// POS NOR TEX BW BI
		struct VERTEX_DATA_L5
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT2	vTexcoord;
			XMFLOAT4	vBWeight;
			BYTE		vBIdx[4];
		};

		// POS NOR BIN TAN BW BI
		struct VERTEX_DATA_L6
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT3	vBin;
			XMFLOAT3	vTan;
			XMFLOAT4	vBWeight;
			BYTE		vBIdx[4];
		};

		// POS NOR BIN TAN TEX BW BI
		struct VERTEX_DATA_L7
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT3	vBin;
			XMFLOAT3	vTan;
			XMFLOAT2	vTexcoord;
			XMFLOAT4	vBWeight;
			BYTE		vBIdx[4];
		};

		struct Material_Eles
		{
			// determine if material only holds material or only textures
			// or both
			enum eMaterial_Fac
			{
				Fac_None = 0,
				Fac_Only_Color,
				Fac_Only_Texture,
				Fac_Both,
				Fac_Max,
			};

			eMaterial_Fac type;
			pseudodx::XMFLOAT3 color;
			TextureSet textureSetArray;

			Material_Eles()
				:type(Fac_None), color(0, 0, 0)
			{
				textureSetArray.clear();
			}

			~Material_Eles()
			{
				Release();
			}

			void Release()
			{
				for (TextureSet::iterator it = textureSetArray.begin(); it != textureSetArray.end(); ++it)
				{
					it->second.clear();
				}
				textureSetArray.clear();
			}

			Material_Eles& operator=(const Material_Eles& rhs)
			{
				type = rhs.type;
				color = rhs.color;

				for (auto iter = rhs.textureSetArray.begin(); iter != rhs.textureSetArray.end(); ++iter)
				{
					for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
					{
						textureSetArray[iter->first].push_back(*iter2);
					}
				}
				return *this;
			}
		};

		// structure which will be used to pass data to shaer
		// this will replace MATERIAL_DATA
		struct Material_Data
		{
			MaterialBufferType			mtrlConst;
			ID3D11ShaderResourceView*	pSRV[2]; // for diffuse and normalmap. do I need multiple of this?
			ID3D11SamplerState*         pSampler[2];
			ID3D11Buffer*				pMaterialCb;

			Material_Data()
				: pMaterialCb(nullptr)
			{
				pSRV[0] = nullptr;
				pSRV[1] = nullptr;

				pSampler[0] = nullptr;
				pSampler[1] = nullptr;
			}

			void Release()
			{
				SAFE_RELEASE(pMaterialCb);
				for (UINT i = 0; i < 2; ++i)
				{
					SAFE_RELEASE(pSRV[i]);
					SAFE_RELEASE(pSampler[i]);
				}
			}
		};

		struct FbxMaterial
		{
			enum eMaterial_Type
			{
				Type_None = 0,
				Type_Lambert,
				Type_Phong,
				Type_Max
			};

			std::string  name;
			eMaterial_Type type;
			// ambiet material and texture
			Material_Eles ambient;
			// diffuse material and texture
			Material_Eles diffuse;
			// emmisive material and texture
			Material_Eles emissive;
			// specular material and texture
			Material_Eles specular;
			float shineness;
			float TransparencyFactor;

			FbxMaterial()
				:name(""), type(Type_None),
				shineness(0), TransparencyFactor(0)
			{}

			void Release()
			{
				ambient.Release();
				diffuse.Release();
				emissive.Release();
				specular.Release();
			}

			FbxMaterial& operator=(const FbxMaterial& rhs)
			{
				if (this == &rhs)
					return *this;

				name = rhs.name;
				type = rhs.type;
				ambient = rhs.ambient;
				diffuse = rhs.diffuse;
				emissive = rhs.emissive;
				specular = rhs.specular;
				shineness = rhs.shineness;
				TransparencyFactor = rhs.TransparencyFactor;
				return *this;
			}
		};

		struct KeyFrame
		{
			float time;
			pseudodx::XMFLOAT3 trans;
			pseudodx::XMFLOAT4 rot;
			pseudodx::XMFLOAT3 scl;

			KeyFrame() :
			time(0.f), 
			trans(0.f, 0.f, 0.f), rot(0.f, 0.f, 0.f, 1.f), scl(1.f, 1.f, 1.f)
			{
			}
		};

		struct BoneAnimation
		{
			std::vector<KeyFrame> keyFrames;
		};

		struct BlendIdxWeight
		{
			UINT mBlendingIndex;
			double mBlendingWeight;

			BlendIdxWeight() :
				mBlendingIndex(0),
				mBlendingWeight(0)
			{}

			bool operator<(const BlendIdxWeight& rhs)
			{
				return mBlendingWeight < rhs.mBlendingWeight;
			}
		};

		INLINE bool compare_bw_ascend(const BlendIdxWeight &lhs, BlendIdxWeight &rhs)
		{
			return lhs.mBlendingWeight < rhs.mBlendingWeight;
		}

		INLINE bool compare_bw_descend(const BlendIdxWeight &lhs, BlendIdxWeight &rhs)
		{
			return lhs.mBlendingWeight > rhs.mBlendingWeight;
		}

		// Each Control Point in FBX is basically a vertex  in the physical world. For example, a cube has 8
		// vertices(Control Points) in FBX Joints are associated with Control Points in FBX
		// The mapping is one joint corresponding to 4 Control Points(Reverse of what is done in a game engine)
		// As a result, this struct stores a XMFLOAT3 and a vector of joint indices
		struct CtrlPoint
		{
			std::vector<BlendIdxWeight> mBlendingInfo;
		};

		// Control Points
		typedef std::unordered_map<UINT, CtrlPoint> ControlPoints;

		struct AnimationClip
		{
			// animation of each bones
			std::vector<BoneAnimation> boneAnim;
		};

		struct AnimationData
		{
			std::unordered_map<std::string, AnimationClip> animations;
		};

		// This is the actual representation of a joint in a game engine
		struct Joint
		{
			std::string mName;
			int mParentIndex;
			FbxAMatrix mToRoot;
			FbxAMatrix mToParent;

			// bind - local coord system that the entire skin is defined relative to
			// local tm. local about to the skinned mesh
			pseudodx::XMFLOAT3 bindPosition;
			pseudodx::XMFLOAT3 bindScaling;
			pseudodx::XMFLOAT4 bindRotation;

			// bone space - the space that influences the vertices. so-called offset transformation
			// bone offset tm
			pseudodx::XMFLOAT3 boneSpacePosition;
			pseudodx::XMFLOAT3 boneSpaceScaling;
			pseudodx::XMFLOAT4 boneSpaceRotation;

			Joint()
				:
			mParentIndex(-1),
			bindPosition(pseudodx::XMFLOAT3(0, 0, 0)),
			bindRotation(pseudodx::XMFLOAT4(0, 0, 0, 1)),
			bindScaling(pseudodx::XMFLOAT3(1, 1, 1)),
			boneSpacePosition(pseudodx::XMFLOAT3(0, 0, 0)),
			boneSpaceRotation(pseudodx::XMFLOAT4(0, 0, 0, 1)),
			boneSpaceScaling(pseudodx::XMFLOAT3(1, 1, 1))
			{
				mToRoot.SetIdentity();
				mToParent.SetIdentity();
			}
		};

		struct FbxBoneData
		{
			std::vector<Joint>		mbonehierarchy;
			std::vector<FbxNode*>	mboneNodes;
			std::vector<SMat4>		mboneLocalTM;

			~FbxBoneData()
			{
				Release();
			}

			void Release()
			{
				mbonehierarchy.clear();
				mboneNodes.clear();
				mboneLocalTM.clear();
			}
		};

		struct MeshData
		{
			eLayout mLayout;
			WORD m_LayoutFlag;
			
			std::string name;
			int parentIndex;

			FbxLayerElement::EMappingMode normalMode;
			FbxLayerElement::EMappingMode binormalMode;
			FbxLayerElement::EMappingMode tangentMode;
			SMat4 meshTM;

			std::vector<pseudodx::XMFLOAT3> vertices;
			std::vector<UINT> indices;
			std::vector<pseudodx::XMFLOAT3> normals;
			std::vector<pseudodx::XMFLOAT3> binormals;
			std::vector<pseudodx::XMFLOAT3> tangents;
			std::vector<pseudodx::XMFLOAT2> uvs;
			std::vector<UINT> materialIndices;

			// material
			std::vector<FbxMaterial> fbxmaterials;

			MeshData() : mLayout(eLayout::NONE),
				normalMode(FbxLayerElement::eNone),
				binormalMode(FbxLayerElement::eNone),
				tangentMode(FbxLayerElement::eNone)
			{
			}
		};

		struct FbxModel
		{
			std::vector<FbxPose*>   mAnimPose;

			// ===== Data we need to export =======
			// need to be exported as binary
			eLayout                 mLayout;
			WORD					mLayoutFlag;
			std::string             name;
			FbxBoneData             mBoneData;
			std::vector<MeshData>	mMeshData;
			std::vector<FbxMaterial> mMaterials;
			std::vector<ControlPoints> mCtrlPoints;
			std::vector<AnimationData> mAnimationData;
			// ====================================

			FbxModel() {}

			~FbxModel()
			{
				mMeshData.clear();
				mMaterials.clear();
				mCtrlPoints.clear();
				mAnimationData.clear();
			}
		};

		// for rendering
		struct	MESH_NODE
		{
			std::string				m_meshName;
			ID3D11Buffer*			m_pVB;
			ID3D11Buffer*			m_pIB;
			ID3D11InputLayout*		m_pInputLayout;
			XMMATRIX				m_meshTM;
			std::vector<XMMATRIX>	m_meshMtxPal;
			DWORD					vertexCount;
			DWORD					indexCount;
			FBX_DATA::eLayout		m_Layout;

			std::vector<FBX_DATA::Material_Data> fbxmtrlData;

			// INDEX BUFFER BIT
			enum INDEX_BIT
			{
				INDEX_NOINDEX = 0,
				INDEX_16BIT,		// 16bit
				INDEX_32BIT,		// 32bit
			};

			INDEX_BIT	m_indexBit;

			MESH_NODE()
				: 
				m_pVB(nullptr), m_pIB(nullptr), m_pInputLayout(nullptr),
				m_indexBit(INDEX_NOINDEX), vertexCount(0), indexCount(0),
				m_meshTM( DirectX::XMMatrixIdentity() )
			{}

			void Release()
			{
				for (UINT i = 0; i < fbxmtrlData.size(); ++i)
					fbxmtrlData[i].Release();
				fbxmtrlData.clear();

				SAFE_RELEASE(m_pInputLayout);
				SAFE_RELEASE(m_pIB);
				SAFE_RELEASE(m_pVB);
			}

			void SetIndexBit(const size_t indexCount)
			{
#if 0
				if (indexCount == 0)
					m_indexBit = INDEX_NOINDEX;
				else if (indexCount < 0xffff)
					m_indexBit = INDEX_16BIT;
				else if (indexCount >= 0xffff)
					m_indexBit = INDEX_32BIT;
#else
				m_indexBit = INDEX_NOINDEX;
				if (indexCount != 0)
					m_indexBit = INDEX_32BIT;
#endif
			};
		};
	}
}