/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** MeshInfo.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include "ISerialize.h"
#include "AnimationDef.h"

namespace ursine
{
	struct MeshVertex
	{
		pseudodx::XMFLOAT3	pos;
		pseudodx::XMFLOAT3	normal;
		pseudodx::XMFLOAT3	binormal;
		pseudodx::XMFLOAT3	tangent;
		pseudodx::XMFLOAT2	uv;
		pseudodx::XMUINT4	ctrlIndices;
		pseudodx::XMFLOAT4	ctrlBlendWeights;

		MeshVertex()
		{
			pos = pseudodx::XMFLOAT3(0.f, 0.f, 0.f);
			normal = pseudodx::XMFLOAT3(0.f, 0.f, 0.f);
			binormal = pseudodx::XMFLOAT3(0.f, 0.f, 0.f);
			tangent = pseudodx::XMFLOAT3(0.f, 0.f, 0.f);
			uv = pseudodx::XMFLOAT2(0.f, 0.f);
			ctrlIndices = pseudodx::XMUINT4(0, 0, 0, 0);
			ctrlBlendWeights = pseudodx::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		}

		bool operator==(const MeshVertex& rhs)
		{
			if ((pos == rhs.pos)
				&& (normal == rhs.normal)
				&& (binormal == rhs.binormal)
				&& (tangent == rhs.tangent)
				&& (uv == rhs.uv)
				&& (ctrlIndices == rhs.ctrlIndices)
				&& (ctrlBlendWeights == rhs.ctrlBlendWeights))
				return true;

			return false;
		}

		bool IsValidWeights()
		{
			if ((0.0f == ctrlBlendWeights.x)
				&& (0.0f == ctrlBlendWeights.y)
				&& (0.0f == ctrlBlendWeights.z)
				&& (0.0f == ctrlBlendWeights.w))
				return false;

			return true;
		}

		bool IsValidControls()
		{
			if ((0 == ctrlIndices.x)
				&& (0 == ctrlIndices.y)
				&& (0 == ctrlIndices.z)
				&& (0 == ctrlIndices.w))
				return false;

			return true;
		}

		bool CheckSum()
		{
			float sum = ctrlBlendWeights.x + ctrlBlendWeights.y + ctrlBlendWeights.z + ctrlBlendWeights.w;

			if (sum > 1.0f)
				return false;

			if (!IsEqualEpsilon(1.0f, sum))
				return false;

			return true;
		}
	};

	class MeshInfo : public ISerialize
	{
	public:
		std::string     name;
		XMMATRIX		meshTM;
		UINT    meshVtxInfoCount;
		UINT    meshVtxIdxCount;
		UINT    mtrlCount;
		UINT    mtrlIndexCount;
		std::vector< std::string > mtrlName;

		std::vector<MeshVertex>     meshVtxInfos;
		std::vector<UINT>   meshVtxIndices; // this will be used when we do vtx reconstruction
		std::vector<UINT>   materialIndices;

		MeshInfo();
		virtual ~MeshInfo();

		virtual bool SerializeIn(HANDLE hFile);
		virtual bool SerializeOut(HANDLE hFile);
	};
};