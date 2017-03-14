/* ---------------------------------------------------------------------------
** Team Bear King
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** AnimationInfo.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#pragma once

#include "AnimationDef.h"
#include "BoneInfo.h"

namespace ursine
{
	typedef std::vector<UINT> KeyIndex;
	typedef std::vector<KeyIndex> KeyIndices;
	typedef std::vector<FBX_DATA::KeyFrame> KFrame;
	typedef std::vector<KFrame> KFrames;
	typedef std::vector<KFrames> KFrms;

	// mesh of the model's animation data
	struct AnimData
	{
		std::string clipname;
		UINT clipCount;	// how many clips in this animation?
		UINT boneCount;	// how many bones in this model
		KeyIndices keyIndices;	// how many keys does each bone have?
		KFrms keyframes;

		AnimData() : clipname(""), clipCount(0), boneCount(0), keyIndices(0), keyframes(0) {}
		void Interpolate(const int& clipindex, int boneindex, double timePos, XMMATRIX& bonetoParentTMs);
	};

	// model's animation data
	class AnimInfo : public ISerialize
	{
	public:
		std::string name;  // model name
		UINT animCount; // animation count is same as the number of meshes of the model
		std::vector<AnimData> animDataArr;	// and so does animData

		/** @brief animation information constructor
		*
		*  this will construct animation information object
		*
		*  @return nothing
		*/
		AnimInfo(void);

		/** @brief animation information destructor
		*
		*  this will destroy animation information object
		*
		*  @return nothing
		*/
		virtual ~AnimInfo(void);

		virtual bool SerializeIn(HANDLE hFile);

		virtual bool SerializeOut(HANDLE hFile);

		// Determines if this animation data is valid an actually contains
		// information
		bool IsValid(void) const;

		/** @brief animation info GetFinalTransform function
		*
		*  this will calculate final matrix transformation matrix based on animation keyframes
		*
		*  @return nothing
		*/
		void GetFinalTransform(const std::vector<BoneInfo>& boneInfoVec, const std::string& clipName, double timePos, std::vector<XMMATRIX>& finalTransform);

		void Interpolate(const int& clipindex, double timePos, std::vector<XMMATRIX>& toParentTMs);

		void FindAnimClip(int* index, int* boneCount, const std::string& clipName) const;
	};
};