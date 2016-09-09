/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** ModelInfo.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include "ModelInfo.h"

namespace ursine
{
	ModelInfo::ModelInfo(void)
		: name("")
		, mmeshCount(0)
		, mmaterialCount(0)
		, mboneCount(0)
		, mmeshlvlCount(0)
		, mriglvlCount(0) { }

	ModelInfo::~ModelInfo()
	{
		mmeshCount = 0;
		mmaterialCount = 0;
		mboneCount = 0;
		mmeshlvlCount = 0;
		mriglvlCount = 0;

		mMeshInfoVec.clear();
		mMtrlInfoVec.clear();
		mBoneInfoVec.clear();
		mMeshLvVec.clear();
		mRigLvVec.clear();
	}

	bool ModelInfo::SerializeIn(HANDLE hFile)
	{
		DWORD nBytesRead;
		UINT i = 0;
		if (INVALID_HANDLE_VALUE != hFile)
		{
			char tmp_name[MAXTEXTLEN];
			ReadFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
			name = tmp_name;
			ReadFile(hFile, &mmeshCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &mmaterialCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &mboneCount, sizeof(UINT), &nBytesRead, nullptr);

			for (i = 0; i < mmeshCount; ++i)
				mMeshInfoVec[i].SerializeIn(hFile);

			for (i = 0; i < mmaterialCount; ++i)
				mMtrlInfoVec[i].SerializeIn(hFile);

			for (i = 0; i < mboneCount; ++i)
				mBoneInfoVec[i].SerializeIn(hFile);
		}
		return true;
	}

	bool ModelInfo::SerializeOut(HANDLE hFile)
	{
		DWORD nBytesWrite;
		UINT i = 0;
		if (INVALID_HANDLE_VALUE != hFile)
		{
			char tmp_name[MAXTEXTLEN];
			WriteFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
			name = tmp_name;
			WriteFile(hFile, &mmeshCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &mmaterialCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &mboneCount, sizeof(UINT), &nBytesWrite, nullptr);

			for (i = 0; i < mmeshCount; ++i)
				mMeshInfoVec[i].SerializeOut(hFile);

			for (i = 0; i < mmaterialCount; ++i)
				mMtrlInfoVec[i].SerializeOut(hFile);

			for (i = 0; i < mboneCount; ++i)
				mBoneInfoVec[i].SerializeOut(hFile);
		}
		return true;
	}
};
