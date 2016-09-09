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

#include <directxmath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <D3DX11async.h>
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <unordered_map>
#include <fbxsdk.h>
#include <iostream>
#include <SMat4.h>

#pragma warning (disable : 4458)

using namespace DirectX;

namespace pseudodx
{
    struct XMUINT4
    {
        uint32_t x;
        uint32_t y;
        uint32_t z;
        uint32_t w;
        XMUINT4() {}
        XMUINT4(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) : x(_x), y(_y), z(_z), w(_w) {}
        XMUINT4& operator= (const XMUINT4& Uint4) { x = Uint4.x; y = Uint4.y; z = Uint4.z; w = Uint4.w; return *this; }
        bool operator== (const XMUINT4& Uint4) { return ( (x == Uint4.x) && (y == Uint4.y) && (z == Uint4.z) && (w == Uint4.w) ) ? true : false; }
        bool operator!= (const XMUINT4& Uint4) { return ( !(*this == Uint4) ); }
    };

    struct XMFLOAT4
    {
        float x;
        float y;
        float z;
        float w;
        XMFLOAT4() {}
        XMFLOAT4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
        XMFLOAT4& operator= (const XMFLOAT4& Float4) { x = Float4.x; y = Float4.y; z = Float4.z; w = Float4.w; return *this; }
        bool operator== (const XMFLOAT4& Float4) { return ( (x == Float4.x) && (y == Float4.y) && (z == Float4.z) && (w == Float4.w) ) ? true : false; }
        bool operator!= (const XMFLOAT4& Float4) { return ( !(*this == Float4) ); }
    };

    struct XMFLOAT3
    {
        float x;
        float y;
        float z;

        XMFLOAT3() {}
        XMFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        XMFLOAT3& operator+ (const XMFLOAT3& Float3) { x += Float3.x; y += Float3.y; z += Float3.z; return *this; }
        XMFLOAT3& operator/ (const float& floatval) { x /= floatval; y /= floatval; z /= floatval; return *this; }
        XMFLOAT3& operator= (const XMFLOAT3& Float3) { x = Float3.x; y = Float3.y; z = Float3.z; return *this; }
        bool operator== (const XMFLOAT3& Float3) { return ( (x == Float3.x) && (y == Float3.y) && (z == Float3.z) ) ? true : false; }
        bool operator!= (const XMFLOAT3& Float3) { return ( !(*this == Float3) ); }
    };

    struct XMFLOAT2
    {
        float x;
        float y;

        XMFLOAT2() {}
        XMFLOAT2(float _x, float _y) : x(_x), y(_y) {}
        XMFLOAT2& operator= (const XMFLOAT2& Float2) { x = Float2.x; y = Float2.y; return *this; }
        bool operator== (const XMFLOAT2& Float2) { return ( (x == Float2.x) && (y == Float2.y) ) ? true : false; }
        bool operator!= (const XMFLOAT2& Float2) { return ( !(*this == Float2) ); }
    };
}

namespace ursine
{
	inline bool IsEqualEpsilon(float A, float B)
	{
		return fabs(A - B) <= 1e-5f;
	}

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
			STATIC = 0,
			INSTANCE = 1,
			SKINNED = 2,
		};

		struct LAYOUT
		{
			D3D11_INPUT_ELEMENT_DESC STATIC_LAYOUT[3];
			D3D11_INPUT_ELEMENT_DESC SKINNED_LAYOUT[5];

			LAYOUT()
			{
				STATIC_LAYOUT[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				STATIC_LAYOUT[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				STATIC_LAYOUT[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

				SKINNED_LAYOUT[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				SKINNED_LAYOUT[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				SKINNED_LAYOUT[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				SKINNED_LAYOUT[3] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				SKINNED_LAYOUT[4] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		};

		struct VERTEX_DATA_STATIC
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT2	vTexcoord;
		};

		struct VERTEX_DATA_SKIN
		{
			XMFLOAT3	vPos;
			XMFLOAT3	vNor;
			XMFLOAT2	vTexcoord;
			XMFLOAT4	vBWeight;
			BYTE		vBIdx[4];
		};

		struct Material_Consts
		{
			pseudodx::XMFLOAT4	ambient;
			pseudodx::XMFLOAT4	diffuse;
			pseudodx::XMFLOAT4	specular;
			pseudodx::XMFLOAT4	emissive;
			float		shineness;
			float		TransparencyFactor;
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
			pseudodx::XMFLOAT4 color;
			TextureSet textureSetArray;

			Material_Eles()
				:type(Fac_None), color(0, 0, 0, 1)
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
			Material_Consts				materialConst;
			ID3D11ShaderResourceView*	pSRV;
			ID3D11SamplerState*         pSampler;
			ID3D11Buffer*				pMaterialCb;

			Material_Data()
			{
				pSRV = nullptr;
				pSampler = nullptr;
				pMaterialCb = nullptr;
			}
			void Release()
			{
				if (pMaterialCb)
				{
					pMaterialCb->Release();
					pMaterialCb = nullptr;
				}

				if (pSRV)
				{
					pSRV->Release();
					pSRV = nullptr;
				}

				if (pSampler)
				{
					pSampler->Release();
					pSampler = nullptr;
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
			Material_Consts mtrl_consts;

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
				name = rhs.name;
				type = rhs.type;
				ambient = rhs.ambient;
				diffuse = rhs.diffuse;
				emissive = rhs.emissive;
				specular = rhs.specular;
				shineness = rhs.shineness;
				TransparencyFactor = rhs.TransparencyFactor;

				mtrl_consts.ambient = rhs.ambient.color;
				mtrl_consts.diffuse = rhs.diffuse.color;
				mtrl_consts.emissive = rhs.emissive.color;
				mtrl_consts.specular = rhs.specular.color;
				mtrl_consts.shineness = rhs.shineness;
				mtrl_consts.TransparencyFactor = rhs.TransparencyFactor;
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
				trans(0.f, 0.f, 0.f),
				rot(0.f, 0.f, 0.f, 1.f),
				scl(1.f, 1.f, 1.f)
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
			{
				mParentIndex = -1;
				bindPosition = pseudodx::XMFLOAT3(0, 0, 0);
				bindRotation = pseudodx::XMFLOAT4(0, 0, 0, 1);
				bindScaling = pseudodx::XMFLOAT3(1, 1, 1);
				boneSpacePosition = pseudodx::XMFLOAT3(0, 0, 0);
				boneSpaceRotation = pseudodx::XMFLOAT4(0, 0, 0, 1);
				boneSpaceScaling = pseudodx::XMFLOAT3(1, 1, 1);
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
			std::string             name;
			FbxBoneData             mBoneData;
			std::vector<MeshData>  mMeshData;
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

			std::vector<FBX_DATA::Material_Data> fbxmaterialData;

			// INDEX BUFFER BIT
			enum INDEX_BIT
			{
				INDEX_NOINDEX = 0,
				INDEX_16BIT,		// 16bit
				INDEX_32BIT,		// 32bit
			};
			INDEX_BIT	m_indexBit;

			MESH_NODE()
			{
				m_pVB = nullptr;
				m_pIB = nullptr;
				m_pInputLayout = nullptr;
				m_indexBit = INDEX_NOINDEX;
				vertexCount = 0;
				indexCount = 0;
				m_meshTM = DirectX::XMMatrixIdentity();
			}

			void Release()
			{
				for (UINT i = 0; i < fbxmaterialData.size(); ++i)
				{
					fbxmaterialData[i].Release();
				}
				fbxmaterialData.clear();

				if (m_pInputLayout)
				{
					m_pInputLayout->Release();
					m_pInputLayout = nullptr;
				}
				if (m_pIB)
				{
					m_pIB->Release();
					m_pIB = nullptr;
				}
				if (m_pVB)
				{
					m_pVB->Release();
					m_pVB = nullptr;
				}
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