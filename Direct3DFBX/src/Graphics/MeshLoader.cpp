// Precompiled Header
#include <Utilities.h>
#include "MeshLoader.h"
#include "Mesh.h"
//#include "Platform/MessageBox.h"
//#include "Platform/FileInfo.h"

// Dependencies
#include <queue>
#include <cmath>

MeshLoader::MeshLoader()
//: mFBXManager( FbxManager::Create() )	// Initialize the sdk manager. This object handles all our memory management.
{
	//// Create the io settings object.
	//FbxIOSettings * ios = FbxIOSettings::Create( mFBXManager, IOSROOT );
	//mFBXManager->SetIOSettings( ios );

	// Import options determine what kind of data is to be imported.
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_MATERIAL, true );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_TEXTURE, true );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_LINK, false );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_SHAPE, false );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_GOBO, false );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_ANIMATION, true );
	//mFBXManager->GetIOSettings()->SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );
}

MeshLoader::~MeshLoader()
{
	//mFBXManager->Destroy();
}

//FbxMatrix MeshLoader::GetConversionMatrix( FbxScene * scene )
//{
//	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
//
//	FbxMatrix conversionMatrix;
//	conversionMatrix.SetIdentity();
//
//	if ( sceneAxisSystem == FbxAxisSystem::eMax )
//	{
//		conversionMatrix.SetRow( 0, FbxVector4( 1, 0, 0, 0 ) );
//		conversionMatrix.SetRow( 1, FbxVector4( 0, 0, 1, 0 ) );
//		conversionMatrix.SetRow( 2, FbxVector4( 0, 1, 0, 0 ) );
//	}
//	else if ( sceneAxisSystem == FbxAxisSystem::eMayaYUp )
//	{
//		conversionMatrix.SetRow( 0, FbxVector4( 1, 0, 0, 0 ) );
//		conversionMatrix.SetRow( 1, FbxVector4( 0, 1, 0, 0 ) );
//		conversionMatrix.SetRow( 2, FbxVector4( 0, 0, -1, 0 ) );
//	}
//	else
//	{
//		CreateMessageBox( "MeshLoader::GetConversionMatrix - Unsupported Axis System, no conversion available", "Error" );
//	}
//
//	// Not direct x axis system, we convert scene
//	if ( sceneAxisSystem != FbxAxisSystem::DirectX )
//	{
//		FbxAxisSystem::DirectX.ConvertScene( scene );
//	}
//
//	return conversionMatrix;
//}
//
//void MeshLoader::ExtractGeometry( FbxNode * node, FbxPose * pose, MeshData & meshdata, 
//								  const FbxMatrix & conversionMatrix )
//{
//	FbxString nodeName = node->GetName();
//
//	if ( node->GetNodeAttribute() != nullptr )
//	{
//		FbxNodeAttribute::EType attributeType = node->GetNodeAttribute()->GetAttributeType();
//
//		if ( attributeType == FbxNodeAttribute::eMesh )
//		{
//			FbxMatrix meshTransform;
//			// Move mesh from model space to bind pose if one is provided
//			if ( pose )
//			{
//				meshTransform = pose->GetMatrix( pose->Find( node ) );
//			}
//			else
//			{
//				// Ignore transformation in scene
//				meshTransform.SetIdentity();
//			}
//
//			// Geometry transform
//			FbxMatrix geoTransform;
//			FbxVector4 geoTrans = node->GetGeometricTranslation( FbxNode::eSourcePivot );
//			FbxVector4 geoRot = node->GetGeometricRotation( FbxNode::eSourcePivot );
//			FbxVector4 geoScale = node->GetGeometricScaling( FbxNode::eSourcePivot );
//			geoTransform.SetTRS( geoTrans, geoRot, geoScale );
//
//			// Convert matrix to destination coordinate space
//			meshTransform = conversionMatrix * meshTransform * geoTransform;
//
//			FbxMesh * mesh = node->GetMesh();
//			int polyCount = mesh->GetPolygonCount();
//
//			// Indices
//			int vertexCount = mesh->GetPolygonVertexCount();
//			int * indices = mesh->GetPolygonVertices();
//
//			std::copy( indices, indices + vertexCount, std::back_inserter( meshdata.mPointsIndexBuffer ) );
//
//			meshdata.mPolygonSizeArray.reserve( polyCount );
//			for ( int i = 0; i < polyCount; ++i )
//				meshdata.mPolygonSizeArray.push_back( mesh->GetPolygonSize( i ) );
//
//			// Get points
//			FbxVector4 * points = mesh->GetControlPoints();
//			int pointsCount = mesh->GetControlPointsCount();
//			meshdata.mPoints.reserve( pointsCount );
//			for ( int i = 0; i < pointsCount; ++i )
//				meshdata.mPoints.push_back( meshTransform.MultNormalize( points[i] ) );
//				//meshdata.mPoints.push_back( points[i] );
//
//			FbxLayer * layer = mesh->GetLayer( 0 );
//
//			// Get normals
//			if ( FbxLayerElementNormal * layerNormal = layer->GetNormals() )
//			{
//				int objNum = layerNormal->GetDirectArray().GetCount();
//				void * data = layerNormal->GetDirectArray().GetLocked();
//				meshdata.mNormals.resize( objNum );
//				std::memcpy( meshdata.mNormals.data(), data, sizeof( FbxVector4 ) * objNum );
//				layerNormal->GetDirectArray().Release( &data );
//
//				// Remove translation from the matrix for normals
//				meshTransform.SetColumn( 3, FbxVector4( 0.0, 0.0, 0.0, 1.0 ) );
//				for ( int i = 0; i < objNum; ++i )
//				{
//					meshdata.mNormals[i] = meshTransform.MultNormalize( meshdata.mNormals[i] );
//					//meshdata.mNormals[i] = meshdata.mNormals[i];
//					//meshdata.mNormals[i].mData[0] = -meshdata.mNormals[i].mData[0];
//					//meshdata.mNormals[i].mData[1] = -meshdata.mNormals[i].mData[1];
//					meshdata.mNormals[i].Normalize();
//				}
//
//				if ( layerNormal->GetReferenceMode() == FbxLayerElement::eIndexToDirect )
//				{
//					int indexNum = layerNormal->GetIndexArray().GetCount();
//					void * idata = layerNormal->GetIndexArray().GetLocked();
//					meshdata.mNormalIndexBuffer.resize( indexNum );
//					std::memcpy( meshdata.mNormalIndexBuffer.data(), idata, sizeof( int ) * indexNum );
//					layerNormal->GetIndexArray().Release( &idata );
//				}
//				else
//				{
//					FbxLayerElement::EMappingMode mapping = layerNormal->GetMappingMode();
//
//					// Normals map directly to control points
//					if ( mapping == FbxLayerElement::eByControlPoint )
//					{
//						meshdata.mNormalIndexBuffer = meshdata.mPointsIndexBuffer;
//					}
//					else if ( mapping == FbxLayerElement::eByPolygonVertex )
//					{
//						for ( int i = 0; i < vertexCount; ++i )
//							meshdata.mNormalIndexBuffer.push_back( i );
//					}
//				}
//			}
//
//			// If there is UV
//			if( FbxLayerElementUV * uvLayer = layer->GetUVs() )
//			{
//				int uvNum = uvLayer->GetDirectArray().GetCount();
//				void * uvdata = uvLayer->GetDirectArray().GetLocked();
//				meshdata.mUVs.resize( uvNum );
//				std::memcpy( meshdata.mUVs.data(), uvdata, sizeof( FbxVector2 ) * uvNum );
//				uvLayer->GetDirectArray().Release( &uvdata );
//
//				//FbxLayerElement::EMappingMode mapping = uvLayer->GetMappingMode();
//				if( uvLayer->GetReferenceMode() == FbxLayerElement::eIndexToDirect )
//				{
//					int indexNum = uvLayer->GetIndexArray().GetCount();
//					void * idata = uvLayer->GetIndexArray().GetLocked();
//					meshdata.mUVIndexBuffer.resize( indexNum );
//					std::memcpy( meshdata.mUVIndexBuffer.data(), idata, sizeof( int ) * indexNum );
//					uvLayer->GetIndexArray().Release( &idata );
//				}
//				else
//				{
//					meshdata.mUVIndexBuffer = meshdata.mPointsIndexBuffer;
//				}
//			}
//
//			// Generate vertices
//			GenerateVertices( meshdata );
//		}
//	}
//}
//
//void MeshLoader::GenerateVertices( MeshData & meshdata )
//{
//	if ( meshdata.mUVIndexBuffer.empty() )
//	{
//		// Fill UV indices with 0
//		meshdata.mUVIndexBuffer.resize( meshdata.mPointsIndexBuffer.size(), 0 );
//
//		// Push in zero vector at 0 index
//		meshdata.mUVs.push_back( FbxVector2( 0.0, 0.0 ) );
//	}
//
//	// Create vertex and index buffer
//	int indicesNum = meshdata.mPointsIndexBuffer.size();
//	std::vector< std::vector<int> > controlMap( indicesNum );
//	std::vector<int> sourcePosIndices;
//	std::vector<int> sourceNormIndices;
//	std::vector<int> sourceUVIndices;
//	for ( int i = 0; i < indicesNum; ++ i )
//	{
//		int index = FindMatchingVertex( meshdata, controlMap, i, sourcePosIndices, sourceNormIndices, sourceUVIndices );
//		meshdata.mIndexBuffer.push_back( index );
//	}
//
//	Triangulate( meshdata );
//	ConvertTriWinding( meshdata );
//}
//
//int MeshLoader::FindMatchingVertex( MeshData & meshdata, std::vector< std::vector<int> > & controlMap, int num,
//									std::vector<int> & sourcePosIndices, std::vector<int> & sourceNormIndices, 
//									std::vector<int> & sourceUVIndices )
//{
//	// Find matching vertices
//	std::vector<int> & mappedVertices = controlMap[ meshdata.mPointsIndexBuffer[num] ];
//	int mapSize = mappedVertices.size();
//	for ( int i = 0; i < mapSize; ++i )
//	{
//		if ( meshdata.mNormalIndexBuffer[num] == sourceNormIndices[mappedVertices[i]] &&
//			 meshdata.mUVIndexBuffer[num] == sourceUVIndices[mappedVertices[i]] )
//			return mappedVertices[i];
//	}
//
//	// No matching vertices mapped for this index, just add one
//	Vertex v;
//	FbxVector4 pos = meshdata.mPoints[ meshdata.mPointsIndexBuffer[num] ];
//	FbxVector4 normal = meshdata.mNormals[ meshdata.mNormalIndexBuffer[num] ];
//	FbxVector2 uv = meshdata.mUVs[ meshdata.mUVIndexBuffer[num] ];
//	v.position = Vector3f( static_cast<f32>( pos[0] ), 
//						   static_cast<f32>( pos[1] ), 
//						   static_cast<f32>( pos[2] ) );
//	v.color = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
//	v.normal = Vector3f( static_cast<f32>( normal[0] ), 
//						 static_cast<f32>( normal[1] ), 
//						 static_cast<f32>( normal[2] ) );
//	v.tex = Vector2f( static_cast<f32>( uv[0] ), 
//					  static_cast<f32>( uv[1] ) );
//	meshdata.mVertexBuffer.push_back( v );
//
//	// Update info
//	sourcePosIndices.push_back( meshdata.mPointsIndexBuffer[num] );
//	sourceNormIndices.push_back( meshdata.mNormalIndexBuffer[num] );
//	sourceUVIndices.push_back( meshdata.mUVIndexBuffer[num] );
//	int newIndex = meshdata.mVertexBuffer.size() - 1;
//	mappedVertices.push_back( newIndex );
//	return newIndex;
//}
//
//void MeshLoader::Triangulate( MeshData & meshdata )
//{
//	std::vector<int> NewIndices;
//	int c = 0;
//	int asize = meshdata.mPolygonSizeArray.size();
//	for ( int i = 0; i < asize; ++i )
//	{
//		int size = meshdata.mPolygonSizeArray[i];
//
//		// Simple Convex Polygon Triangulation: always n-2 triangles
//		const int NumberOfTris = size - 2;
//
//		for ( int p = 0; p<NumberOfTris; ++p )
//		{
//			NewIndices.push_back( meshdata.mIndexBuffer[c + 0] );
//			NewIndices.push_back( meshdata.mIndexBuffer[c + 1 + p] );
//			NewIndices.push_back( meshdata.mIndexBuffer[c + 2 + p] );
//		}
//		c += size;
//	}
//
//	// Swap the new triangulated indices with the old vertices
//	meshdata.mIndexBuffer.swap( NewIndices );
//}
//
//void MeshLoader::ConvertTriWinding( MeshData & meshdata )
//{
//	for ( unsigned int i = 0; i < meshdata.mIndexBuffer.size(); i += 3 )
//	{
//		std::swap( meshdata.mIndexBuffer[i], meshdata.mIndexBuffer[i + 2] );
//	}
//}
//
//void MeshLoader::ExtractSkeleton( FbxNode * node, SkeletonData & skeleton, std::vector<FbxNode *> & boneNodes )
//{
//	//skeleton.reserve( node->GetChildCount() );
//	//boneNodes.reserve( node->GetChildCount() );
//
//	std::queue< std::pair<int, FbxNode *> > nodeStack;
//	nodeStack.push( std::make_pair( -1, node ) );
//
//	while ( !nodeStack.empty() )
//	{
//		std::pair<int, FbxNode *> current = nodeStack.front();
//		nodeStack.pop();
//
//		FbxNode * currentNode = current.second;
//		int parentIndex = current.first;
//		int nodeIndex = -1;
//
//		FbxNodeAttribute * nodeAttribute = currentNode->GetNodeAttribute();
//		if ( nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton )
//		{
//			nodeIndex = skeleton.size();
//			skeleton.emplace_back();
//
//			BoneData & bone = skeleton.back();
//			bone.mParentIndex = parentIndex;
//			bone.mBoneIndex = nodeIndex;
//			bone.mName = currentNode->GetName();
//			bone.mBoneNode = currentNode;
//
//			boneNodes.push_back( currentNode );
//		}
//
//		int childCount = currentNode->GetChildCount();
//		for ( int i = 0; i < childCount; ++i )
//			nodeStack.push( std::make_pair( nodeIndex, currentNode->GetChild( i ) ) );
//	}
//}
//
//void MeshLoader::ExtractBindPose( FbxNode * node, FbxPose * pose, BoneData & bone, const FbxMatrix & conversionMatrix )
//{
//	int nodeIndex = pose->Find( node );
//	FbxAMatrix localMatrix;
//	if ( nodeIndex == -1 )
//	{
//		// Bone does not have a bind pose export identity
//		localMatrix.SetIdentity();
//	}
//	else
//	{
//		FbxMatrix poseMatrix = pose->GetMatrix( nodeIndex );
//		memcpy( (f64 *)localMatrix, (f64 *)poseMatrix, sizeof( poseMatrix.mData ) );
//	}
//	ExtractBindMatrix( bone, localMatrix, conversionMatrix );
//}
//
//inline bool IsEqualEpsilon( f64 A, f64 B )
//{
//	return std::fabs( A - B ) <= DBL_EPSILON;
//}
//
//bool CheckPositive( FbxVector4 scale )
//{
//	return !( scale[0] < 0.f ||
//			  scale[1] < 0.f ||
//			  scale[2] < 0.f );
//}
//
//bool CheckScaling( FbxVector4 scale )
//{
//	return !( !IsEqualEpsilon( scale[0], scale[1] ) ||
//			  !IsEqualEpsilon( scale[1], scale[2] ) ||
//			  !IsEqualEpsilon( scale[0], scale[2] ) );
//}
//
//void MeshLoader::ExtractBindMatrix( BoneData & bone, FbxAMatrix & mat, const FbxMatrix & conversionMatrix )
//{
//	FbxAMatrix affinedConversionMatrix;
//	memcpy( (f64 *)affinedConversionMatrix, (f64 *)( const_cast<FbxMatrix &>( conversionMatrix ) ), sizeof( conversionMatrix.mData ) );
//	mat = affinedConversionMatrix * mat * affinedConversionMatrix;
//
//	FbxVector4 scale = mat.GetS();
//	if ( !CheckScaling(scale) || !CheckPositive(scale))
//	{
//		// If there is negative scaling the matrix inverse might have problems
//		mat.SetS( FbxVector4( 1, 1, 1 ) );
//	}
//
//	FbxAMatrix Inverse = mat.Inverse();
//
//	bone.mBindPos = mat.GetT();
//	bone.mBindRot = mat.GetQ();
//
//	bone.mBoneSpacePos = Inverse.GetT();
//	bone.mBoneSpaceRot = Inverse.GetQ();
//}
//
//bool MeshLoader::ExtractSkinWeights( FbxPose * pose, FbxNode * node, SkinData & skinData, SkeletonData & skeleton )
//{
//	FbxString nodeName = node->GetName();
//	FbxNodeAttribute * nodeAttribute = node->GetNodeAttribute();
//	if ( nodeAttribute && nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh )
//	{
//		FbxMesh * mesh = node->GetMesh();
//		int vertexCount = mesh->GetControlPointsCount();
//
//		// All the links must have the same link mode
//		//FbxCluster::ELinkMode clusterMode = static_cast<FbxSkin *>( mesh->GetDeformer( 0, FbxDeformer::eSkin ) )->GetCluster( 0 )->GetLinkMode();
//
//		skinData.mPointWeights.resize( vertexCount );
//		int skinCount = mesh->GetDeformerCount( FbxDeformer::eSkin );
//		for ( int i = 0; i < skinCount; ++i )
//		{
//			FbxSkin * skin = static_cast<FbxSkin *>( mesh->GetDeformer( i, FbxDeformer::eSkin ) );
//			int clusterCount = skin->GetClusterCount();
//
//			for ( int j = 0; j < clusterCount; ++j )
//			{
//				FbxCluster * cluster = skin->GetCluster( j );
//				if ( !cluster->GetLink() )
//					continue;
//
//				FbxNode * link = cluster->GetLink();
//				int boneIndex = GetBoneIndex( link, skeleton );
//
//				// Bone does not have a bind pose, so the link transform matrix as the bind pose
//				int nodeIndex = pose->Find( link );
//				if ( nodeIndex == -1 )
//				{
//					FbxAMatrix linkBindMatrix;
//					cluster->GetTransformLinkMatrix( linkBindMatrix );
//					pose->Add( link, FbxMatrix( linkBindMatrix ) );
//				}
//
//				int vertexIndexCount = cluster->GetControlPointIndicesCount();
//				for ( int k = 0; k < vertexIndexCount; ++k )
//				{
//					int index = cluster->GetControlPointIndices()[k];
//					f64 weight = cluster->GetControlPointWeights()[k];
//
//					JointWeight j = { static_cast<f32>( weight ), boneIndex };
//					skinData.mPointWeights[index].push_back( j );
//				}
//			}
//		}
//
//		const unsigned int MAX_WEIGHTS = 4;
//		unsigned int pointWeightsSize = skinData.mPointWeights.size();
//		// Normalise the skin weights for 4 weights for vertex that sum to one
//		for ( unsigned int i = 0; i < pointWeightsSize; ++i )
//		{
//			// Make sure there is max weights by inserting dummies
//			while ( skinData.mPointWeights[i].size() < MAX_WEIGHTS )
//			{
//				JointWeight j = { 0, 0 };
//				skinData.mPointWeights[i].push_back( j );
//			}
//
//			// Normalise the weights
//			f32 sum = 0.0f;
//			for ( int w = 0; w < MAX_WEIGHTS; ++w )
//				sum += skinData.mPointWeights[i][w].mWeight;
//
//			for ( int w = 0; w < MAX_WEIGHTS; ++w )
//				skinData.mPointWeights[i][w].mWeight /= sum;
//		}
//
//		skinData.mSkin = true;
//		return true;
//	}
//	else
//	{
//		skinData.mSkin = false;
//		return false;
//	}
//}
//
//int MeshLoader::GetBoneIndex( FbxNode * node, SkeletonData & skeleton )
//{
//	SkeletonData::iterator it = std::find_if( skeleton.begin(), skeleton.end(),
//												[=]( const BoneData & bone )
//												{
//													return bone.mBoneNode == node;
//												} );
//
//	if ( it != skeleton.end() )
//		return it - skeleton.begin();
//	else
//		return -1;
//}
//
//void CollectKeyTimes( std::set<FbxTime> & keyTimes, FbxAnimLayer * layer, FbxPropertyT<FbxDouble3> & attribute, const Char * curveName )
//{
//	// Get all the keyframes for this control curve
//	FbxAnimCurve * curve = attribute.GetCurve( layer, curveName );
//	if ( curve )
//	{
//		int keyCount = curve->KeyGetCount();
//		for ( int i = 0; i<keyCount; ++i )
//		{
//			FbxAnimCurveKey curveKey = curve->KeyGet( i );
//			keyTimes.insert( curveKey.GetTime() );
//		}
//	}
//}
//
//FbxAMatrix GetLocalMatrixFromTime( FbxAnimEvaluator * animationEvaluator, FbxNode* node, FbxTime & current )
//{
//	/*
//		We should be extracting the local transform data directly from the node
//		buy this does not work due to other transforms like PreRotation
//		the simplest what to extract the data is to use GetGlobalFromCurrentTake
//		which always returns the correct matrix then use the parent's inverse to
//		get the local matrix (slower but effective)
//	*/
//
//	// This is what we want to do but it does not work in all cases
//	//KFbxVector4 Rotate = pNode->GetLocalRFromCurrentTake( current );
//	//KFbxVector4 Trans = pNode->GetLocalTFromCurrentTake( current );
//	//KFbxVector4 Scale = pNode->GetLocalSFromCurrentTake( current );
//
//	FbxNode* parent = node->GetParent();
//	FbxAMatrix localTransForm;
//	FbxNodeAttribute * attribute = parent->GetNodeAttribute();
//	if ( attribute && attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton )
//	{
//		/*
//			Compute the local transform by getting the global of this
//			node and the parent then multiplying the this nodes global
//			transform by the inverse of the parents global transform
//		*/
//		FbxAMatrix PX = animationEvaluator->GetNodeGlobalTransform( parent, current );
//		FbxAMatrix LX = animationEvaluator->GetNodeGlobalTransform( node, current );
//		PX = PX.Inverse();
//		localTransForm = PX * LX;
//	}
//	else
//	{
//		// We want root bones in global space
//		localTransForm = animationEvaluator->GetNodeGlobalTransform( node, current );
//	}
//
//	return localTransForm;
//}
//
//void MeshLoader::ExtractAnimationTracksFromTake( FbxScene * scene, Animation & animation, const FbxMatrix & conversionMatrix, 
//												 std::vector<FbxNode*> animationNodes, const Char * takeName )
//{
//	// Name can be renamed
//	animation.mName = takeName;
//
//	FbxTime start, stop;
//
//	FbxTakeInfo * currentTakeInfo = scene->GetTakeInfo( takeName );
//	if ( currentTakeInfo )
//	{
//		start = currentTakeInfo->mLocalTimeSpan.GetStart();
//		stop = currentTakeInfo->mLocalTimeSpan.GetStop();
//		FbxTime duration = stop - start;
//		animation.mDuration = static_cast<f32>( duration.GetSecondDouble() );
//	}
//
//	FbxAnimStack * animationStack = scene->GetSrcObject<FbxAnimStack>();
//	animationStack->Reset( currentTakeInfo );
//	FbxAnimLayer * layer = animationStack->GetMember<FbxAnimLayer>();
//	FbxAnimEvaluator * animationEvaluator = scene->GetAnimationEvaluator();
//	int animationNodeNum = animationNodes.size();
//	animation.mTracks.resize( animationNodeNum );
//	for ( int i = 0; i < animationNodeNum; ++i )
//		ExtractAnimationTrack( animationNodes[i], animation, layer, animationEvaluator, conversionMatrix, i, start );
//}
//
//void MeshLoader::ExtractAnimationTrack( FbxNode * node, Animation & animation, FbxAnimLayer * layer, 
//										FbxAnimEvaluator * animationEvaluator, const FbxMatrix & conversionMatrix, 
//										int trackIndex, FbxTime start )
//{
//	FbxAMatrix affinedConversionMatrix;
//	memcpy( (f64 *)affinedConversionMatrix, (f64 *)( const_cast<FbxMatrix &>( conversionMatrix ) ), sizeof( conversionMatrix.mData ) );
//
//	Track & track = animation.mTracks[trackIndex];
//
//	/*
//		Collect all the times that a keyframe occurs. This implementation combines
//		all the curves (position,rotation,scale) together into one track for each node
//	*/
//
//	std::set< FbxTime > keyTimes;
//	keyTimes.insert( start );		//Make sure there at least one keyframe
//	CollectKeyTimes( keyTimes, layer, node->LclRotation, FBXSDK_CURVENODE_COMPONENT_X );
//	CollectKeyTimes( keyTimes, layer, node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y );
//	CollectKeyTimes( keyTimes, layer, node->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z );
//
//	CollectKeyTimes( keyTimes, layer, node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X );
//	CollectKeyTimes( keyTimes, layer, node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y );
//	CollectKeyTimes( keyTimes, layer, node->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z );
//
//	// Make space for all the keyframes
//	track.mKeyFrames.resize( keyTimes.size() );
//
//	// Iterate through the keyframe set storing the key frame data for each time
//	int keyIndex = 0;
//	std::set<FbxTime>::iterator it = keyTimes.begin();
//	for (; it != keyTimes.end(); ++it, ++keyIndex)
//	{
//		FbxTime current = *it;
//
//		FbxAMatrix localXForm = GetLocalMatrixFromTime( animationEvaluator, node, current );
//
//		//Convert the matrix into the destination coordinate space
//		localXForm = affinedConversionMatrix * localXForm * affinedConversionMatrix;
//
//		FbxVector4 scale = localXForm.GetS();
//
//		if ( !CheckScaling(scale) )
//		{
//			localXForm.SetS( FbxVector4(1, 1, 1) );
//		}
//
//		track.mKeyFrames[keyIndex].mTime = static_cast<f32>( current.GetSecondDouble() );
//		FbxVector4 trans = localXForm.GetT();
//		track.mKeyFrames[keyIndex].mTranslate = Vector3f( static_cast<f32>( trans[0] ), 
//														  static_cast<f32>( trans[1] ), 
//														  static_cast<f32>( trans[2] ) );	//Store translation
//		FbxQuaternion rot = localXForm.GetQ();
//		track.mKeyFrames[keyIndex].mRotation = Quaternion( static_cast<f32>( rot[0] ), 
//														   static_cast<f32>( rot[1] ), 
//														   static_cast<f32>( rot[2] ), 
//														   static_cast<f32>( rot[3] ) );	//Store rotation
//	}
//}
//
//	void MeshLoader::ExtractSceneData( FbxScene * scene, Mesh & mesh, AABB & aabb )
//	{
//		// Get scene conversion matrix
//		FbxMatrix conversionMatrix = GetConversionMatrix( scene );
//
//		// Get the list of all the takes
//		FbxArray<FbxString *> takeNameArray;
//		scene->FillAnimStackNameArray( takeNameArray );
//		int poseCount = scene->GetPoseCount();
//		int takeCount = takeNameArray.GetCount();
//
//		bool extractAnimation = false;
//		bool extractSkinData = false;
//		bool extractSkeletonData = false;
//		FbxPose * bindPose = nullptr;
//
//		// If there is a bind pose and default animation
//		if ( poseCount > 0 && scene->GetPose( 0 )->IsBindPose() && takeCount > 1 )
//		{
//			extractAnimation = true;
//			extractSkinData = true;
//			extractSkeletonData = true;
//			scene->GetPose( 0 );
//		}
//
//		// Extract mesh
//		std::vector<FbxNode *> fbxMeshes;
//		FbxNode * root = scene->GetRootNode();
//		CollectMeshes( root, fbxMeshes, extractSkinData );
//
//		// No mesh in scene, return
//		if ( fbxMeshes.size() == 0 )
//		{
//			CreateMessageBox( "MeshLoader::ExtractSceneData - No valid meshes in scene", "Error" );
//			return;
//		}
//
//		std::vector<MeshData> meshdatas( fbxMeshes.size() );
//		int meshNum = fbxMeshes.size();
//		for ( int i = 0; i < meshNum; ++i )
//			ExtractGeometry( fbxMeshes[i], bindPose, meshdatas[i], conversionMatrix );
//
//		// Extracting bones from scene (Skin data need bones data too!)
//		SkeletonData skeleton;
//		std::vector<FbxNode *> boneNodes;
//		if ( extractSkeletonData || extractSkinData )
//			ExtractSkeleton( root, skeleton, boneNodes );
//
//		// Extract skin data
//		if ( extractSkinData )
//		{
//			for ( int i = 0; i < meshNum; ++i )
//				ExtractSkinWeights( bindPose, fbxMeshes[i], meshdatas[i].mSkin, skeleton );
//		}
//
//		// Extract Bind Pose
//		if ( extractSkeletonData || extractSkinData )
//		{
//			unsigned int boneNodesNum = boneNodes.size();
//			for ( unsigned int i = 0; i < boneNodesNum; ++i )
//				ExtractBindPose( boneNodes[i], bindPose, skeleton[i], conversionMatrix );
//		}
//
//		// Extract Animation
//		std::vector< Animation > animations;
//		if ( extractAnimation )
//		{
//			animations.reserve( takeCount );
//			for ( int i = 0; i < takeCount; ++i )
//			{
//				// Skip default take
//				if ( takeNameArray[i]->Compare( FBXSDK_TAKENODE_DEFAULT_NAME ) == 0 )
//					continue;
//
//				animations.emplace_back();
//				ExtractAnimationTracksFromTake( scene, animations.back(), conversionMatrix, boneNodes , takeNameArray[i]->Buffer() );
//			}
//		}
//
//		// @todo Send skin data, bone data, animation data to mesh
//		// @todo May want to handle multiple meshes case
//		// Initialise Mesh
//
//#if 0
//		unsigned int count = 0;
//		for ( const auto & i : meshdatas[0].mVertexBuffer )
//		{
//			if ( ( i.position - i.normal ).LengthSq() > 0.1f )
//				std::cout << "[" << count << "] Position: " << i.position.x << ", " << i.position.y << ", " << i.position.z << "| Normal: " << i.normal.x << ", " << i.normal.y << ", " << i.normal.z << std::endl;
//			++count;
//		}
//
//		std::cout << "---------------------------" << std::endl;
//#endif
//
//		mesh.InitialiseBuffers( meshdatas[0].mVertexBuffer, meshdatas[0].mIndexBuffer );
//		GetBoundingVolume( meshdatas[0], aabb );
//
//	}
//	
//	void MeshLoader::CollectMeshes( FbxNode * root, std::vector<FbxNode *> & fbxMeshes, bool skin )
//	{
//		if ( root )
//		{
//			for ( int i = 0; i < root->GetChildCount(); ++i )
//			{
//				FbxNode * node = root->GetChild( i );
//
//				// If node is invisible,it is usually a construction or dummy mesh, we ignore it
//				if ( node->GetVisibility() )
//				{
//					FbxNodeAttribute * nodeAttr = node->GetNodeAttribute();
//
//					// Node type is mesh
//					if ( nodeAttr && nodeAttr->GetAttributeType() == FbxNodeAttribute::eMesh )
//					{
//						FbxMesh * fbxMesh = node->GetMesh();
//
//						// If look for only skinned models, ignore meshes without deformers
//						if ( skin && fbxMesh->GetDeformer( 0, FbxDeformer::eSkin ) == nullptr )
//							continue;
//
//						fbxMeshes.push_back( node );
//					}
//
//					// If more than 1 mesh
//					if ( !nodeAttr || nodeAttr->GetAttributeType() == FbxNodeAttribute::eNull )
//					{
//						CollectMeshes( node, fbxMeshes, skin );
//					}
//				}
//			}
//		}
//	}
//
//	void MeshLoader::LoadMeshFromFile( const FileInfo & fileinfo, Mesh & mesh, AABB & aabb )
//	{
//		// Create an importer using our sdk manager.
//		FbxImporter * importer = FbxImporter::Create( mFBXManager, "" );
//
//		// Use the first argument as the filename for the importer.
//		if ( !importer->Initialize( fileinfo.GetFullFile().c_str(), -1, mFBXManager->GetIOSettings() ) ) 
//		{
//			CreateMessageBox( "FbxImporter could not initialise", "Error" );
//			return;
//		}
//
//		// Create a new scene so it can be populated by the imported file.
//		FbxScene * scene = FbxScene::Create( mFBXManager, fileinfo.GetFileNoExt().c_str() );
//
//		// Import the contents of the file into the scene.
//		importer->Import( scene );
//
//		// The file has been imported, we can get rid of the importer.
//		importer->Destroy();
//
//		// Extract scene data
//		ExtractSceneData( scene, mesh, aabb );
//
//		// Finish business with scene
//		scene->Destroy();
//	}
//
//	void MeshLoader::GetBoundingVolume( const MeshData & meshdata, AABB & aabb )
//	{
//		if( meshdata.mVertexBuffer.empty() )
//			return;
//
//		Vector3f min( meshdata.mVertexBuffer[0].position.x, meshdata.mVertexBuffer[0].position.y, meshdata.mVertexBuffer[0].position.z ),
//				 max( meshdata.mVertexBuffer[0].position.x, meshdata.mVertexBuffer[0].position.y, meshdata.mVertexBuffer[0].position.z );
//
//		unsigned int size = meshdata.mVertexBuffer.size();
//
//		for ( unsigned int i = 1; i < size; ++i )
//		{
//			min.x = std::min( min.x, meshdata.mVertexBuffer[i].position.x );
//			min.y = std::min( min.y, meshdata.mVertexBuffer[i].position.y );
//			min.z = std::min( min.z, meshdata.mVertexBuffer[i].position.z );
//															
//			max.x = std::max( max.x, meshdata.mVertexBuffer[i].position.x );
//			max.y = std::max( max.y, meshdata.mVertexBuffer[i].position.y );
//			max.z = std::max( max.z, meshdata.mVertexBuffer[i].position.z );
//		}
//
//		aabb.mMin = min;
//		aabb.mMax = max;
//	}

void MeshLoader::CreateQuad(Mesh & mesh)
{
	std::vector< VERTEX_DATA_LT > vertexBuffer
	{
		VERTEX_DATA_LT{ XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT2(0.f, 1.f), },		// Bottom left
		VERTEX_DATA_LT{ XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(1.f, 1.f) },		// Bottom right
		VERTEX_DATA_LT{ XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f) },		// Top left
		VERTEX_DATA_LT{ XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT2(1.f, 0.f) }		// Top right
	};

	std::vector< unsigned int > indexBuffer{
		2, 1, 0,
		2, 3, 1
	};

	mesh.InitializeBuffers(vertexBuffer, indexBuffer);
}

//void MeshLoader::CreateSprite(Sprite& mesh)
//{
//    std::vector< Vertex2D > vertexBuffer
//    {
//        Vertex2D{ Vector2f::ZERO, Vector4f::UNIT_W, Vector2f::UNIT_Y },		// Bottom left
//        Vertex2D{ Vector2f::UNIT_X, Vector4f::UNIT_W, Vector2f::ONE },		// Bottom right
//        Vertex2D{ Vector2f::UNIT_Y, Vector4f::UNIT_W, Vector2f::ZERO },		// Top left
//        Vertex2D{ Vector2f::ONE, Vector4f::UNIT_W, Vector2f::UNIT_X }		// Top right
//    };

//    std::vector< int > indexBuffer{
//        2, 1, 0,
//        2, 3, 1
//    };
//    
//    mesh.SetAnimation(2, 2, 0.f);
//    mesh.InitialiseBuffers(vertexBuffer, indexBuffer);
//}
//
//	void MeshLoader::CreateLineCube( Mesh & mesh )
//	{
//		std::vector< Vertex > vertexBuffer {
//			Vertex{ Vector3f( -0.5f, -0.5f, -0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Left Bottom Back
//			Vertex{ Vector3f( -0.5f, -0.5f,  0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Left Bottom Front
//			Vertex{ Vector3f( -0.5f,  0.5f, -0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Left Top Back
//			Vertex{ Vector3f( -0.5f,  0.5f,  0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Left Top Front
//			Vertex{ Vector3f(  0.5f, -0.5f, -0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Right Bottom Back
//			Vertex{ Vector3f(  0.5f, -0.5f,  0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Right Bottom Front
//			Vertex{ Vector3f(  0.5f,  0.5f, -0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Right Top Back
//			Vertex{ Vector3f(  0.5f,  0.5f,  0.5f ),	Vector4f::UNIT_W, Vector2f::ZERO },		// Right Top Front
//		};
//
//		std::vector< int > indexBuffer{
//			0, 1, 
//			0, 2,
//			0, 4,
//			1, 3,
//			1, 5,
//			2, 3,
//			2, 6,
//			3, 7,
//			4, 5,
//			4, 6,
//			5, 7,
//			6, 7
//		};
//
//		mesh.InitialiseBuffers( vertexBuffer, indexBuffer, true );
//	}
//
//	void MeshLoader::CreateLineCircle( Mesh & mesh )
//	{
//		std::vector< Vertex > vertexBuffer;
//		std::vector< int > indexBuffer;
//
//		const int SEG_NUM = 24;
//		const f32 ANGLE_STEP = PI2 / static_cast<f32>( SEG_NUM );
//		const f32 RADIUS = 1.f;
//
//		Vector3f p;
//		f32 angle = 0.f;
//
//		// XY Sphere
//		for(int i = 0; i < SEG_NUM; ++i)
//		{
//			angle += ANGLE_STEP;
//			p.x = RADIUS * std::cos( angle );
//			p.y = RADIUS * std::sin( angle );
//
//			vertexBuffer.push_back( Vertex{ p,	Vector4f::UNIT_W, Vector2f::ZERO } );
//		}
//
//		// Indices
//		for(int i = 0; i < SEG_NUM; ++i)
//		{
//			indexBuffer.push_back( i );
//			indexBuffer.push_back( ( i + 1 ) % SEG_NUM );
//		}
//
//		mesh.InitialiseBuffers( vertexBuffer, indexBuffer, true );
//	}
//
//	void MeshLoader::CreateXLine( Mesh & mesh )
//	{
//		std::vector< Vertex > vertexBuffer {
//			Vertex{ Vector3f( 0.0f, 0.0f, 0.0f ),	Vector4f::UNIT_W, Vector2f::ZERO },
//			Vertex{ Vector3f( 1.0f, 0.0f, 0.0f ),	Vector4f::UNIT_W, Vector2f::ZERO },
//		};
//
//		std::vector< int > indexBuffer{	0, 1 };
//
//		mesh.InitialiseBuffers( vertexBuffer, indexBuffer, true );
//	}