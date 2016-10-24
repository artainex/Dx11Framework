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

const unsigned int POS	= 0x01;	// 1 Hexa 0x0000 0001 POSITION
const unsigned int NOR	= 0x02;	// 2 Hexa 0x0000 0010 NORAML
const unsigned int BITAN= 0x04;	// 4 Hexa 0x0000 0100 BINORMAL, TANGENT
const unsigned int TEX	= 0x08;	// 8 Hexa 0x0000 1000 TEXTURE
const unsigned int SKIN	= 0x10;	// 16 Hexa 0x0001 0000 SKIN

namespace ursine
{
	inline bool IsEqualEpsilon(float A, float B) { return fabs(A - B) <= 1e-5f; }

	namespace FBX_DATA
	{
		// UVSet
		typedef std::tr1::unordered_map<std::string, int> UVsetID;
		// UVSet
		typedef std::tr1::unordered_map<std::string, std::vector<std::string>> TextureSet;		

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
			ID3D11ShaderResourceView*	pSRV [2]; // for diffuse and normalmap. do I need multiple of this?
			ID3D11SamplerState*         pSampler;// [2];
			ID3D11Buffer*				pMaterialCb;

			Material_Data()
				:pSampler(nullptr), 
				pMaterialCb(nullptr)
			{
				pSRV[0] = nullptr;
				pSRV[1] = nullptr;
				
				//pSampler[0] = nullptr;
				//pSampler[1] = nullptr;
			}

			void Release();
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
				:name(""),
				type(Type_None),
				shineness(0), 
				TransparencyFactor(0)
			{}

			void Release();

			FbxMaterial& operator=(const FbxMaterial& rhs);
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
			{}
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
			unsigned int m_LayoutFlag;
			
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

			MeshData() : mLayout(eLayout::NONE_LAYOUT),
				m_LayoutFlag(0),
				parentIndex(0),
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
			ID3D11Buffer*			m_pVBDepth;
			ID3D11Buffer*			m_pIB;
			ID3D11InputLayout*		m_pInputLayout;
			XMMATRIX				m_meshTM;
			std::vector<XMMATRIX>	m_meshMtxPal;
			DWORD					vertexCount;
			DWORD					indexCount;
			XMFLOAT4				m_meshColor;
			ID3D11Buffer*			m_meshColorBuffer;
			eLayout					m_Layout;

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
				m_pVB(nullptr), m_pVBDepth(nullptr),
				m_pIB(nullptr), 
				m_pInputLayout(nullptr),
				m_indexBit(INDEX_NOINDEX), 
				vertexCount(0), 
				indexCount(0),
				m_meshColor(XMFLOAT4(1,1,1,1)),
				m_meshColorBuffer(nullptr),
				m_meshTM( DirectX::XMMatrixIdentity() )
			{
			}

			void Release();

			XMFLOAT4 GetMeshColor() { return m_meshColor; }
			void SetMeshColor(const float& r, const float& g, const float& b, const float& a) { m_meshColor = XMFLOAT4(r, g, b, a); }
			void SetMeshColor(const XMFLOAT4& color) { m_meshColor = color; }
			FBX_DATA::Material_Data& GetNodeFbxMaterial(const int& mtrl_id = 0) { return fbxmtrlData[mtrl_id]; };

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