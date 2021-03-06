/* ---------------------------------------------------------------------------
** Team Bear King
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** MaterialInfo.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#include "MaterialInfo.h"

namespace ursine
{
	MaterialInfo::MaterialInfo(void)
		: type(FBX_DATA::FbxMaterial::Type_None)
		, ambitype(FBX_DATA::Material_Eles::Fac_None)
		, difftype(FBX_DATA::Material_Eles::Fac_None)
		, emistype(FBX_DATA::Material_Eles::Fac_None)
		, spectype(FBX_DATA::Material_Eles::Fac_None)
		, ambi_mcolor(1, 1, 1), diff_mcolor(1, 1, 1)
		, emis_mcolor(1, 1, 1), spec_mcolor(1, 1, 1)
		, ambi_mapCount(0)
		, diff_mapCount(0)
		, emis_mapCount(0)
		, spec_mapCount(0)
		, shineness(0.1f)
		, transparency(1.0f) 
	{ }

	MaterialInfo::~MaterialInfo(void)
	{
		name = "";

		type = FBX_DATA::FbxMaterial::Type_None;
		ambitype = FBX_DATA::Material_Eles::Fac_None;
		difftype = FBX_DATA::Material_Eles::Fac_None;
		emistype = FBX_DATA::Material_Eles::Fac_None;
		spectype = FBX_DATA::Material_Eles::Fac_None;

		ambi_mapCount = 0;
		diff_mapCount = 0;
		emis_mapCount = 0;
		spec_mapCount = 0;

		ambi_texNames.clear();
		diff_texNames.clear();
		emis_texNames.clear();
		spec_texNames.clear();

		shineness = 0.1f;
		transparency = 1.f;
	}

	bool MaterialInfo::SerializeIn(HANDLE hFile)
	{
		DWORD nBytesRead;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char tmp_name[MAXTEXTLEN];
			ReadFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
			name = tmp_name;
			ReadFile(hFile, &type, sizeof(UINT), &nBytesRead, nullptr);

			ReadFile(hFile, &ambitype, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &difftype, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &emistype, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &spectype, sizeof(UINT), &nBytesRead, nullptr);

			ReadFile(hFile, &ambi_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesRead, nullptr);
			ReadFile(hFile, &diff_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesRead, nullptr);
			ReadFile(hFile, &emis_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesRead, nullptr);
			ReadFile(hFile, &spec_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesRead, nullptr);

			ReadFile(hFile, &ambi_mapCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &diff_mapCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &emis_mapCount, sizeof(UINT), &nBytesRead, nullptr);
			ReadFile(hFile, &spec_mapCount, sizeof(UINT), &nBytesRead, nullptr);

			ambi_texNames.resize(ambi_mapCount);
			diff_texNames.resize(diff_mapCount);
			emis_texNames.resize(emis_mapCount);
			spec_texNames.resize(spec_mapCount);
			for (UINT i = 0; i < ambi_mapCount; ++i)
			{
				ReadFile(hFile, &tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
				ambi_texNames[i] = tmp_name;
			}
			for (UINT i = 0; i < diff_mapCount; ++i)
			{
				ReadFile(hFile, &tmp_name[i], sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
				diff_texNames[i] = tmp_name;
			}
			for (UINT i = 0; i < emis_mapCount; ++i)
			{
				ReadFile(hFile, &tmp_name[i], sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
				emis_texNames[i] = tmp_name;
			}
			for (UINT i = 0; i < spec_mapCount; ++i)
			{
				ReadFile(hFile, &tmp_name[i], sizeof(char) * MAXTEXTLEN, &nBytesRead, nullptr);
				spec_texNames[i] = tmp_name;
			}

			ReadFile(hFile, &shineness, sizeof(float), &nBytesRead, nullptr);
			ReadFile(hFile, &transparency, sizeof(float), &nBytesRead, nullptr);
		}
		return true;
	}

	bool MaterialInfo::SerializeOut(HANDLE hFile)
	{
		DWORD nBytesWrite;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			char tmp_name[MAXTEXTLEN];
			lstrcpy(tmp_name, name.c_str());
			WriteFile(hFile, tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
			UINT int_type = static_cast<UINT>(type);
			WriteFile(hFile, &int_type, sizeof(UINT), &nBytesWrite, nullptr);

			int_type = static_cast<UINT>(ambitype);
			WriteFile(hFile, &int_type, sizeof(UINT), &nBytesWrite, nullptr);
			int_type = static_cast<UINT>(difftype);
			WriteFile(hFile, &int_type, sizeof(UINT), &nBytesWrite, nullptr);
			int_type = static_cast<UINT>(emistype);
			WriteFile(hFile, &int_type, sizeof(UINT), &nBytesWrite, nullptr);
			int_type = static_cast<UINT>(spectype);
			WriteFile(hFile, &int_type, sizeof(UINT), &nBytesWrite, nullptr);

			WriteFile(hFile, &ambi_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesWrite, nullptr);
			WriteFile(hFile, &diff_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesWrite, nullptr);
			WriteFile(hFile, &emis_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesWrite, nullptr);
			WriteFile(hFile, &spec_mcolor, sizeof(pseudodx::XMFLOAT4), &nBytesWrite, nullptr);

			WriteFile(hFile, &ambi_mapCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &diff_mapCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &emis_mapCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &spec_mapCount, sizeof(UINT), &nBytesWrite, nullptr);

			if (ambi_texNames.size() > 0)
			{
				for (auto iter : ambi_texNames)
				{
					lstrcpy(tmp_name, iter.c_str());
					WriteFile(hFile, &tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
				}
			}
			if (diff_texNames.size() > 0)
			{
				for (auto iter : diff_texNames)
				{
					lstrcpy(tmp_name, iter.c_str());
					WriteFile(hFile, &tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
				}
			}
			if (emis_texNames.size() > 0)
			{
				for (auto iter : emis_texNames)
				{
					lstrcpy(tmp_name, iter.c_str());
					WriteFile(hFile, &tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
				}
			}
			if (spec_texNames.size() > 0)
			{
				for (auto iter : spec_texNames)
				{
					lstrcpy(tmp_name, iter.c_str());
					WriteFile(hFile, &tmp_name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
				}
			}

			WriteFile(hFile, &shineness, sizeof(float), &nBytesWrite, nullptr);
			WriteFile(hFile, &transparency, sizeof(float), &nBytesWrite, nullptr);
		}
		return true;
	}
};