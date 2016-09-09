/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** MeshInfo.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include "MeshInfo.h"

namespace ursine
{
	MeshInfo::MeshInfo(void)
		: meshVtxInfoCount(0)
		, meshVtxIdxCount(0)
		, mtrlCount(0)
		, mtrlIndexCount(0) { }

	MeshInfo::~MeshInfo(void)
	{
		name = "";

		meshVtxInfoCount = 0;
		meshVtxIdxCount = 0;
		mtrlCount = 0;
		mtrlIndexCount = 0;

		mtrlName.clear();
		meshVtxInfos.clear();
		meshVtxIndices.clear();
		materialIndices.clear();
	}

	bool MeshInfo::SerializeIn(HANDLE hFile)
	{
		DWORD nBytesRead;
		UINT i = 0;

		if (hFile != INVALID_HANDLE_VALUE)
		{
			char tmp_name[MAXTEXTLEN];
			ReadFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
			name = tmp_name;
			ReadFile(hFile, &meshVtxInfoCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &meshVtxIdxCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &mtrlCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &mtrlIndexCount, sizeof(UINT), &nBytesRead, nullptr);

			meshVtxInfos.resize(meshVtxInfoCount);
			for (i = 0; i < meshVtxInfoCount; ++i)
			{
				MeshVertex newMV;
				ReadFile(hFile, &newMV, sizeof(MeshVertex), &nBytesRead, nullptr);
				meshVtxInfos[i] = newMV;
			}
			meshVtxIndices.resize(meshVtxIdxCount);
			for (i = 0; i < meshVtxIdxCount; ++i)
			{
				UINT mvi;
				ReadFile(hFile, &mvi, sizeof(UINT), &nBytesRead, nullptr);
				meshVtxIndices[i] = mvi;
			}
			mtrlName.resize(mtrlCount);
			for (i = 0; i < mtrlCount; ++i)
			{
				ReadFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
				mtrlName[i] = tmp_name;
			}
			materialIndices.resize(mtrlIndexCount);
			for (i = 0; i < mtrlIndexCount; ++i)
			{
				UINT mi;
				ReadFile(hFile, &mi, sizeof(UINT), &nBytesRead, nullptr);
				materialIndices[i] = mi;
			}
		}
		return true;
	}

	bool MeshInfo::SerializeOut(HANDLE hFile)
	{
		DWORD nBytesWrite;

		if (hFile != INVALID_HANDLE_VALUE)
		{
			char tmp_name[MAXTEXTLEN];
			lstrcpy(tmp_name, name.c_str());
			WriteFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
			WriteFile(hFile, &meshVtxInfoCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &meshVtxIdxCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &mtrlCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &mtrlIndexCount, sizeof(UINT), &nBytesWrite, nullptr);

			if (meshVtxInfos.size() > 0)
			{
				for (auto iter : meshVtxInfos)
					WriteFile(hFile, &iter, sizeof(MeshVertex), &nBytesWrite, nullptr);
			}

			if (meshVtxIndices.size() > 0)
			{
				for (auto iter : meshVtxIndices)
					WriteFile(hFile, &iter, sizeof(UINT), &nBytesWrite, nullptr);
			}

			if (mtrlName.size() > 0)
			{
				for (auto iter : mtrlName)
				{
					lstrcpy(tmp_name, iter.c_str());
					WriteFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
				}
			}

			if (materialIndices.size() > 0)
			{
				for (auto iter : materialIndices)
					WriteFile(hFile, &iter, sizeof(UINT), &nBytesWrite, nullptr);
			}
		}
		return true;
	}
};