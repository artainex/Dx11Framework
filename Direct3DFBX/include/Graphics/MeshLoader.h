#pragma once

#include "Mesh.h"
//#include "Platform/Sprite.h"
//#include "Platform/Animation.h"
//#include "Platform/AABB.h"

//// Dependencies
//#include "fbxsdk.h"

#include <vector>

//struct BoneData
//{
//	String mName;
//	int mParentIndex;
//	int mBoneIndex;
//	FbxNode * mBoneNode;
//
//	FbxVector4 mBindPos;
//	FbxQuaternion mBindRot;
//
//	FbxVector4 mBoneSpacePos;
//	FbxQuaternion mBoneSpaceRot;
//};
//
//struct SkinData
//{
//	/*
//		Index in PointWeight vector correspond to mesh control
//		points which are mapped by the Position Indices
//	*/
//	std::vector< WeightVector > mPointWeights;
//	bool mSkin;
//
//	SkinData() : mSkin(false) {}
//};
//
//typedef std::vector<BoneData> SkeletonData;
//
//struct MeshData
//{
//	std::vector<Vertex> mVertexBuffer;
//	std::vector<int> mIndexBuffer;
//
//	std::vector<int> mPolygonSizeArray;
//	std::vector<FbxVector4> mPoints;
//	std::vector<int> mPointsIndexBuffer;
//	std::vector<FbxVector4> mNormals;
//	std::vector<int> mNormalIndexBuffer;
//	std::vector<FbxVector2> mUVs;
//	std::vector<int> mUVIndexBuffer;
//
//	SkinData mSkin;
//};

class MeshLoader
{
public:
	// Constructor
	MeshLoader();

	// Destructor
	~MeshLoader();

	//// Load mesh
	//void LoadMeshFromFile(const FileInfo & fileinfo, Mesh & mesh, AABB & aabb);

	// Create quad
	static void CreateQuad(Mesh & mesh);

	// Create sprite Deok.
	//static void CreateSprite(Sprite& mesh);
	//
	//// Create Cube (Line)
	//static void CreateLineCube(Mesh & mesh);
	//
	//// Create XY Circle (Line)
	//static void CreateLineCircle(Mesh & mesh);
	//
	//// Create X Line
	//static void CreateXLine(Mesh & mesh);

private:
	//// Get Conversion Matrix
	//FbxMatrix GetConversionMatrix(FbxScene * scene);
	//
	//// Extract Scene Data
	//void ExtractSceneData(FbxScene * scene, Mesh & mesh, AABB & aabb);
	//
	//// Collect Meshes
	//void CollectMeshes(FbxNode * root, std::vector<FbxNode *> & fbxMeshes, bool skin);
	//
	//// Extract Geometry
	//void ExtractGeometry(FbxNode * node, FbxPose * pose, MeshData & meshdata, const FbxMatrix & conversionMatrix);
	//
	//// Extract Skeleton Data
	//void ExtractSkeleton(FbxNode * node, SkeletonData & skeleton, std::vector<FbxNode *> & boneNodes);
	//
	//// Extract Bind Pose
	//void ExtractBindPose(FbxNode * node, FbxPose * pose, BoneData & bone, const FbxMatrix & conversionMatrix);
	//
	//// Extract Bind Matrix
	//void ExtractBindMatrix(BoneData & bone, FbxAMatrix & mat, const FbxMatrix & conversionMatrix);
	//
	//// Extract Skin Data
	//bool ExtractSkinWeights(FbxPose * pose, FbxNode * node, SkinData & skinData, SkeletonData & skeleton);
	//
	//// Extract Animation Tracks from take
	//void ExtractAnimationTracksFromTake(FbxScene * scene, Animation & animation, const FbxMatrix & conversionMatrix,
	//	std::vector<FbxNode*> animationNodes, const Char * takeName);
	//
	//// Extract Animation Track
	//void ExtractAnimationTrack(FbxNode * node, Animation & animation, FbxAnimLayer * layer,
	//	FbxAnimEvaluator * animationEvaluator, const FbxMatrix & conversionMatrix,
	//	int trackIndex, FbxTime start);
	//
	//// Generate Vertices
	//void GenerateVertices(MeshData & meshdata);
	//
	//// Find matching vertex
	//int FindMatchingVertex(MeshData & meshdata, std::vector< std::vector<int> > & controlMap, int num,
	//	std::vector<int> & sourcePosIndices, std::vector<int> & sourceNormIndices,
	//	std::vector<int> & sourceUVIndices);
	//
	//// Triangulate
	//void Triangulate(MeshData & meshdata);
	//
	//// Convert Triangle Winding
	//void ConvertTriWinding(MeshData & meshdata);
	//
	//// Get Bone Index
	//int GetBoneIndex(FbxNode * node, SkeletonData & skeleton);
	//
	//// Get bounding volume
	//void GetBoundingVolume(const MeshData & meshdata, AABB & aabb);

private:
	//FbxManager * mFBXManager;
};