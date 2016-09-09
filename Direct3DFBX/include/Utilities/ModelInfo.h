/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
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
