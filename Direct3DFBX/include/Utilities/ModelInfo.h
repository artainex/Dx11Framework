/* ---------------------------------------------------------------------------
** Team Bear King
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** ModelInfo.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include "MeshInfo.h"
#include "MaterialInfo.h"
#include "BoneInfo.h"
#include "LvlHierarchy.h"

namespace ursine
{
	class ModelInfo : public ISerialize
	{
	public:
		std::string name;

		// mesh data	 
		UINT mmeshCount;
		std::vector<MeshInfo> mMeshInfoVec;

		// material data
		UINT mmaterialCount;
		std::vector<MaterialInfo> mMtrlInfoVec;

		// skin data
		UINT mboneCount;
		std::vector<BoneInfo> mBoneInfoVec;

		// level info - hierarchy
		UINT mmeshlvlCount;
		UINT mriglvlCount;
		std::vector<MeshInLvl> mMeshLvVec;
		std::vector<RigInLvl> mRigLvVec;

		ModelInfo(void);

		virtual ~ModelInfo(void);

		virtual bool SerializeIn(HANDLE hFile);
		virtual bool SerializeOut(HANDLE hFile);
	};
};

//if (m_LayoutFlag == POS | NOR)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT0;
//else if (m_LayoutFlag == POS | NOR | TEX)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT1;
//else if (m_LayoutFlag == POS | NOR | BITAN)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT2;
//else if (m_LayoutFlag == POS | NOR | BITAN | TEX)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT3;
//else if (m_LayoutFlag == POS | NOR | SKIN)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT4;
//else if (m_LayoutFlag == POS | NOR | TEX | SKIN)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT5;
//else if (m_LayoutFlag == POS | NOR | BITAN | SKIN)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT6;
//else if (m_LayoutFlag == POS | NOR | TEX | BITAN | SKIN)
//m_ModelInfo.mLayout = FBX_DATA::eLayout::LAYOUT7;