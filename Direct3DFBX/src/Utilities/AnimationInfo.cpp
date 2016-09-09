/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** AnimationInfo.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#include "AnimationInfo.h"
#include "Utilities.h"

namespace ursine
{
	void AnimData::Interpolate(const int& clipindex, int boneindex, double timePos, XMMATRIX& bonetoParentTMs)
	{
		// rot * trsnl is the correct order in row major
		// which clips which bone? need which clip index
		size_t keyCount = keyIndices[clipindex][boneindex];
		KFrame& keyFrms = keyframes[clipindex][boneindex];

		//t is before the animation started, so just return the first key frame.
		if (timePos <= keyFrms[0].time)
		{
			const FBX_DATA::KeyFrame& curr = keyFrms[0];

			XMVECTOR S = Utilities::ConvertPseudoXMFloat3ToXMVEC(curr.scl);
			XMVECTOR P = Utilities::ConvertPseudoXMFloat3ToXMVEC(curr.trans);
			XMVECTOR Q = Utilities::ConvertPseudoXMFloat4ToXMVEC(curr.rot);
			XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 1.0f);
			bonetoParentTMs = XMMatrixAffineTransformation(S, zero, Q, P);
		} // t is after the animation ended, so just return the last key frame.
		else if (timePos >= keyFrms[keyCount - 1].time)
		{
			const FBX_DATA::KeyFrame& curr = keyFrms[keyCount - 1];

			XMVECTOR S = Utilities::ConvertPseudoXMFloat3ToXMVEC(curr.scl);
			XMVECTOR P = Utilities::ConvertPseudoXMFloat3ToXMVEC(curr.trans);
			XMVECTOR Q = Utilities::ConvertPseudoXMFloat4ToXMVEC(curr.rot);
			XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 1.0f);
			bonetoParentTMs = XMMatrixAffineTransformation(S, zero, Q, P);
		} // t is between two key frames, so interpolate.
		else
		{
			for (UINT j = 0; j < keyCount - 1; ++j)
			{
				if (timePos >= keyFrms[j].time && timePos <= keyFrms[j + 1].time)
				{
					float lerpPercent = ((float)timePos - keyFrms[j].time) / (keyFrms[j + 1].time - keyFrms[j].time);
					XMVECTOR s0 = Utilities::ConvertPseudoXMFloat3ToXMVEC(keyFrms[j].scl);
					XMVECTOR s1 = Utilities::ConvertPseudoXMFloat3ToXMVEC(keyFrms[j + 1].scl);
					XMVECTOR p0 = Utilities::ConvertPseudoXMFloat3ToXMVEC(keyFrms[j].trans);
					XMVECTOR p1 = Utilities::ConvertPseudoXMFloat3ToXMVEC(keyFrms[j + 1].trans);
					XMVECTOR q0 = Utilities::ConvertPseudoXMFloat4ToXMVEC(keyFrms[j].rot);
					XMVECTOR q1 = Utilities::ConvertPseudoXMFloat4ToXMVEC(keyFrms[j + 1].rot);
					XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
					XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
					XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);
					XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 1.0f);
					bonetoParentTMs = XMMatrixAffineTransformation(S, zero, Q, P);
					break;
				}
			}
		}
	}

	AnimInfo::AnimInfo(void)
		: name("")
		, animCount(0)
		, animDataArr(0) { }

	AnimInfo::~AnimInfo(void)
	{
		name = "";

		animCount = 0;

		for (auto iter : animDataArr)
		{
			iter.keyIndices.clear();
			iter.keyframes.clear();
		}
	}

	bool AnimInfo::SerializeIn(HANDLE hFile)
	{
		DWORD nByteRead;
		UINT i = 0, j = 0, k = 0, l = 0;

		ReadFile(hFile, &name, sizeof(char) * MAXTEXTLEN, &nByteRead, nullptr);
		ReadFile(hFile, &animCount, sizeof(UINT), &nByteRead, nullptr);
		for (i = 0; i < animCount; ++i)
		{
			// serializing counts
			ReadFile(hFile, &animDataArr[i].clipname, sizeof(char) * MAXTEXTLEN, &nByteRead, nullptr);
			ReadFile(hFile, &animDataArr[i].clipCount, sizeof(UINT), &nByteRead, nullptr);
			ReadFile(hFile, &animDataArr[i].boneCount, sizeof(UINT), &nByteRead, nullptr);
			animDataArr[i].keyIndices.resize( animDataArr[i].clipCount );
			animDataArr[i].keyframes.resize( animDataArr[i].clipCount );
			for (j = 0; j < animDataArr[i].clipCount; ++j)
			{
				animDataArr[i].keyIndices[j].resize( animDataArr[i].boneCount );
				animDataArr[i].keyframes[j].resize( animDataArr[i].boneCount );
				for (k = 0; k < animDataArr[i].boneCount; ++k)
				{
					ReadFile(hFile, &animDataArr[i].keyIndices[j][k], sizeof(UINT), &nByteRead, nullptr);
					animDataArr[i].keyframes[j][k].resize( animDataArr[i].keyIndices[j][k] );
					for (l = 0; l <animDataArr[i].keyIndices[j][k]; ++l)
					{
						FBX_DATA::KeyFrame* currKF = &animDataArr[i].keyframes[j][k][l];
						ReadFile(hFile, currKF, sizeof(FBX_DATA::KeyFrame), &nByteRead, nullptr);
					}
				}
			}
		}
		return true;
	}

	bool AnimInfo::SerializeOut(HANDLE hFile)
	{
		DWORD nBytesWrite;
		UINT i = 0, j = 0, k = 0, l = 0;

		WriteFile(hFile, &name, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
		WriteFile(hFile, &animCount, sizeof(UINT), &nBytesWrite, nullptr);
		for (i = 0; i < animCount; ++i)
		{
			// serializing counts
			WriteFile(hFile, &animDataArr[i].clipname, sizeof(char) * MAXTEXTLEN, &nBytesWrite, nullptr);
			WriteFile(hFile, &animDataArr[i].clipCount, sizeof(UINT), &nBytesWrite, nullptr);
			WriteFile(hFile, &animDataArr[i].boneCount, sizeof(UINT), &nBytesWrite, nullptr);
			for (j = 0; j < animDataArr[i].clipCount; ++j)
			{
				for (k = 0; k < animDataArr[i].boneCount; ++k)
				{
					WriteFile(hFile, &animDataArr[i].keyIndices[j][k], sizeof(UINT), &nBytesWrite, nullptr);
					for (l = 0; l <animDataArr[i].keyIndices[j][k]; ++l)
					{
						FBX_DATA::KeyFrame* currKF = &animDataArr[i].keyframes[j][k][l];
						WriteFile(hFile, currKF, sizeof(FBX_DATA::KeyFrame), &nBytesWrite, nullptr);
					}
				}
			}
		}
		return true;
	}

	bool AnimInfo::IsValid(void) const
	{
		return !animDataArr.empty();
	}

	void AnimInfo::GetFinalTransform(const std::vector<BoneInfo>& boneInfoVec, const std::string& clipName, double timePos, std::vector<XMMATRIX>& finalTransform)
	{
		// if there is no animation, just return
		if (0 == animCount)
			return;

		// find animation clip by name
		int animClipIdx = -1;
		int numBones = 0;
		FindAnimClip(&animClipIdx, &numBones, clipName);
		if (-1 == animClipIdx || 0 == numBones )
			return;

		// Calculating Final Transform
		std::vector<XMMATRIX> toParentTransforms(numBones);
		std::vector<XMMATRIX> toRootTransforms(numBones);

		// currently, we can only handle one animation
		// Interpolate all the bones of this clip at the given time instance.
		Interpolate(animClipIdx, timePos, toParentTransforms); // need clip index
															   // Traverse the hierarchy and transform all the bones to the root space.
															   // The root bone has index 0. The root bone has no parent, so
															   // its toRootTransform is just its local bone transform.
		toRootTransforms[0] = toParentTransforms[0];
		// Now find the toRootTransform of the children.
		for (size_t i = 1; i < numBones; ++i)
		{
			// toParent = animTM
			XMMATRIX toParent = toParentTransforms[i];
			int parentIndex = boneInfoVec[i].mParentIndex;
			// parentToRoot = parent to root transformation
			XMMATRIX parentToRoot = toRootTransforms[parentIndex];
			// toRootTransform = local bone transform
			toRootTransforms[i] = XMMatrixMultiply(toParent, parentToRoot);
		}

		// Premultiply by the bone offset transform to get the final transform.
		for (size_t i = 0; i < numBones; ++i)
		{
			XMVECTOR S = Utilities::ConvertPseudoXMFloat3ToXMVEC(boneInfoVec[i].boneSpaceScaling);
			XMVECTOR P = Utilities::ConvertPseudoXMFloat3ToXMVEC(boneInfoVec[i].boneSpacePosition);
			XMVECTOR Q = Utilities::ConvertPseudoXMFloat4ToXMVEC(boneInfoVec[i].boneSpaceRotation);
			XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 1.0f);
			XMMATRIX offset = XMMatrixAffineTransformation(S, zero, Q, P);
			XMMATRIX toroot = toRootTransforms[i];
			finalTransform[i] = XMMatrixMultiply(offset, toroot);
		}
	}

	void AnimInfo::Interpolate(const int& clipindex, double timePos, std::vector<XMMATRIX>& toParentTMs)
	{
		// for the number of the meshes in the model
		for (auto &iter : animDataArr)
		{
			for (UINT i = 0; i < iter.boneCount; ++i)
				iter.Interpolate(clipindex, i, timePos, toParentTMs[i]);
		}
	}

	void AnimInfo::FindAnimClip(int* index, int* boneCount, const std::string& clipName) const
	{
		int i = 0;
		for (auto& iter : animDataArr)
		{
			if (clipName == iter.clipname)
			{
				*index = i;
				*boneCount = iter.boneCount;
				++i;
			}
		}
	}
};