#include "Core/CommonFunctions.h"
//#include "Core/Profiling.h"
//
//#include "Platform/Platform.h"
//#include "Platform/Vector2f.h"
//#include "Platform/XMFLOAT3.h"
//#include "Platform/Vector4f.h"
//#include "Platform/Matrix4.h"
//#include "Platform/AABB.h"
//#include "Core/CoreEssential.h"
#include "Core/Hash.h"
#include "Core/SystemManager.h"
#include "Core/System.h"
#include "Renderer.h"
#include "Game.h"
//#include "Core/InputManager.h"
//#include "Core/MeshManager.h"
//#include "Core/TextureManager.h"
//#include "Core/MaterialManager.h"
//#include "Core/GameStateManager.h"
//#include "Core/LevelManager.h"
//#include "Core/CollisionManager.h"
//#include "Core/Audio.h"
//#include "Core/Light.h"
//#include "Core/Math.h"
//#include "Core/OBB.h"

static Renderer *			sRenderer			= nullptr;
//static MeshManager *		sMeshManager		= nullptr;
//static TextureManager *		sTextureManager		= nullptr;
//static MaterialManager *	sMaterialManager	= nullptr;
//static CollisionManager *	sCollisionManager	= nullptr;
//static AudioManager *       sAudioManager       = nullptr;
//
//static LevelScriptMSG*		sLevelScriptMSG = nullptr;
//static ObjectManager *		sObjectManager = nullptr;
//
//static OBB GetOBBFromAABB( const AABB & aabb, const XMFLOAT3 & pos, const XMFLOAT3 & size, const XMFLOAT3 & rotation )
//{
//	Matrix4 rot = Matrix4::CreateFromYawPitchRoll( rotation.y, rotation.x, rotation.z );
//	Matrix4 trans = Matrix4::CreateTranslation( pos );
//	Matrix4 scale = Matrix4::CreateScale( size );
//
//	OBB obb;
//	obb.mAxis[0] = XMFLOAT3( rot.m[0][0], rot.m[0][1], rot.m[0][2] );
//	obb.mAxis[1] = XMFLOAT3( rot.m[1][0], rot.m[1][1], rot.m[1][2] );
//	obb.mAxis[2] = XMFLOAT3( rot.m[2][0], rot.m[2][1], rot.m[2][2] );
//
//	trans = trans * scale;
//
//	XMFLOAT3 n_min = trans * aabb.mMin;
//	XMFLOAT3 n_max = trans * aabb.mMax;
//	XMFLOAT3 n_scale = n_max - n_min;
//	XMFLOAT3 n_pos = n_min + 0.5f * n_scale;
//
//	Matrix4 trans_o = Matrix4::CreateTranslation( n_pos - pos );
//
//	trans = Matrix4::CreateTranslation( pos );
//	scale = Matrix4::CreateScale( n_scale );
//
//	trans = trans * rot * trans_o * scale;
//
//	obb.mHalfWidth = 0.5f * n_scale;
//	obb.mPosition = XMFLOAT3( trans.m[3][0], trans.m[3][1], trans.m[3][2] );
//
//	return obb;
//}

#ifdef __cplusplus
extern "C"
{
#endif

	//__declspec( dllexport ) void StartProfiler( const char * profiler_name )
	//{
	//	START_PROFILE( profiler_name );
	//}
	//
	//__declspec( dllexport ) void StopProfiler( const char * profiler_name )
	//{
	//	END_PROFILE( profiler_name );
	//}

	__declspec( dllexport ) void SetRenderer( Renderer * renderer )
	{
		sRenderer = renderer;
	}

	//__declspec( dllexport ) void SetMeshManager( MeshManager * meshManager )
	//{
	//	sMeshManager = meshManager;
	//}
	//
	//__declspec( dllexport ) void SetTextureManager( TextureManager * textureManager )
	//{
	//	sTextureManager = textureManager;
	//}
	//
	//__declspec( dllexport ) void SetMaterialManager( MaterialManager * materialManager )
	//{
	//	sMaterialManager = materialManager;
	//}
	//
	//__declspec( dllexport ) void SetCollisionManager( CollisionManager * collisionManager )
	//{
	//	sCollisionManager = collisionManager;
	//}
	//
	////daeyeop jei
	//__declspec(dllexport) void SetAudioManager(AudioManager* audioManager)
	//{
	//	sAudioManager = audioManager;
	//}
	//
    //__declspec(dllexport) void SetObjectManager(ObjectManager * objectmanager)
    //{
    //    sObjectManager = objectmanager;
    //}
	//
    ////daeyeop jei
    //__declspec(dllexport) void SetLevelScriptMSG(LevelScriptMSG* levelScriptMSG)
    //{
    //    sLevelScriptMSG = levelScriptMSG;
    //}

	//__declspec( dllexport ) void DrawInstance( float posX,	
	//										   float posY,
	//										   float posZ,
	//										   float scaleX,
	//										   float scaleY,
	//										   float scaleZ,
	//										   float rotX,
	//										   float rotY,
	//										   float rotZ,
	//										   const char * mesh,			
	//										   const char * material )
	//{
	//	if ( !sRenderer || !sMeshManager || !sTextureManager || !sMaterialManager )
	//		return;
	//	
	//	Mesh * pMesh = sMeshManager->GetMesh( mesh );
	//	Material * pMaterial = sMaterialManager->GetMaterial( material );
	//	
	//	sRenderer->DrawInstance( XMFLOAT3( posX, posY, posZ ),
	//							 XMFLOAT3( scaleX, scaleY, scaleZ ), 
	//							 XMFLOAT3( Math::DegToRad( rotX ), Math::DegToRad( rotY ), Math::DegToRad( rotZ ) ), 
	//							 pMesh, pMaterial );
	//}

    //__declspec(dllexport) void DrawSprites( float posX,
    //                                        float posY,
    //                                        float posZ,
    //                                        float scaleX,
    //                                        float scaleY,
    //                                        float scaleZ,
    //                                        float rotX,
    //                                        float rotY,
    //                                        float rotZ,
    //                                        unsigned int index,
    //                                        unsigned int row,
    //                                        unsigned int column,
    //                                        float frame,
    //                                        bool animated,
    //                                        const char * mesh,
    //                                        const char * material)
    //{
    //    //Currently I don't need Sprite Manager for this semester but ...
    //    if (!sRenderer || !sMeshManager || !sTextureManager || !sMaterialManager)
    //        return;
	//
    //    Mesh * pMesh = sMeshManager->GetMesh(mesh);
    //    Material * pMaterial = sMaterialManager->GetMaterial(material);
	//
    //    sRenderer->DrawSprite(XMFLOAT3(posX, posY, posZ),
    //                          XMFLOAT3(scaleX, scaleY, scaleZ),
    //                          XMFLOAT3(Math::DegToRad(rotX), Math::DegToRad(rotY), Math::DegToRad(rotZ)),
    //                          Sprite(index, row, column, frame, animated),
    //                          pMesh, pMaterial);
    //    /*
    //    sRenderer->DrawSprite2D(XMFLOAT3(posX, posY, posZ),
    //    XMFLOAT3(scaleX, scaleY, scaleZ),
    //    XMFLOAT3(Math::DegToRad(rotX), Math::DegToRad(rotY), Math::DegToRad(rotZ)),
    //    Sprite(index, row, column, frame),
    //    pMesh, pMaterial);
    //    */
    //}
	//
	//__declspec(dllexport) void DrawInstance2(XMFLOAT3 pos,
	//	XMFLOAT3 sca, XMFLOAT3 rot,
	//	const char * mesh,
	//	const char * material)
	//{
	//	if (!sRenderer || !sMeshManager || !sTextureManager || !sMaterialManager)
	//		return;
	//
	//	Mesh * pMesh = sMeshManager->GetMesh(mesh);
    //    Material * pMaterial = sMaterialManager->GetMaterial(material);
	//
    //    sRenderer->DrawInstance(pos,
    //                            sca,
    //                            rot,
    //                            pMesh, pMaterial);
	//}
	//
	//__declspec( dllexport ) void DrawGlobalLight( float posX,	
	//										      float posY,
	//										      float posZ,
	//										      float colorR,
	//										      float colorG,
	//										      float colorB,
	//										      float colorA )
	//{
	//	if ( !sRenderer )
	//		return;
	//
	//	sRenderer->DrawGlobalLight( Light( XMFLOAT3( posX, posY, posZ ),
	//										   Vector4f( colorR, colorG, colorB, colorA ) ) );
	//}
	//
	//__declspec( dllexport ) void DrawDirectionalLight( float posX,	
	//												   float posY,
	//												   float posZ,
	//												   float colorR,
	//												   float colorG,
	//												   float colorB,
	//												   float colorA,
	//												   float dirX,
	//												   float dirY,
	//												   float dirZ,
	//												   float width,
	//												   float height,
	//												   float depth )
	//{
	//	if ( !sRenderer )
	//		return;
	//
	//	sRenderer->DrawDirectionalLight( DirectionalLight( XMFLOAT3( posX, posY, posZ ),
	//														   Vector4f( colorR, colorG, colorB, colorA ),
	//														   XMFLOAT3( dirX, dirY, dirZ ),
	//														   XMFLOAT3( width, height, depth ) ) );
	//}
	//
	//__declspec( dllexport ) void DrawLocalLight( float posX,	
	//										     float posY,
	//										     float posZ,
	//										     float colorR,
	//										     float colorG,
	//										     float colorB,
	//										     float colorA,
	//											 float radius )
	//{
	//	if ( !sRenderer )
	//		return;
	//
	//	sRenderer->DrawLocalLight( LocalLight( XMFLOAT3( posX, posY, posZ ),
	//											   Vector4f( colorR, colorG, colorB, colorA ),
	//											   radius ) );
	//}

	////daeyeop jei
	//__declspec (dllexport) void EnableSoundEffect(const char * name)
	//{
	//	if (!sAudioManager)
	//		return;
	//
	//	sAudioManager->Audio_Enable(name);
	//}
	//
	////daeyeop jei
	//__declspec (dllexport) void DisableSoundEffect(const char * name)
	//{
	//	if (!sAudioManager)
	//		return;
	//
	//	sAudioManager->Audio_Disable(name);
    //}
	//
	////daeyeop jei
	//__declspec(dllexport) void LevelMessage(const char * name)
	//{
	//	if (!sLevelScriptMSG)
	//		return;
	//
	//	sLevelScriptMSG->SetMSG(name);
	//}
	//
	//__declspec( dllexport ) void AddStaticOBB( const char * name,
	//										   float posX,	
	//										   float posY,
	//										   float posZ,
	//										   float scaleX,
	//										   float scaleY,
	//										   float scaleZ,
	//										   float rotX,
	//										   float rotY,
	//										   float rotZ,
	//										   const char * mesh,
	//										   bool passable,
	//										   const char * tag )
	//{
	//	if ( !sCollisionManager || !sMeshManager )
	//		return;
	//
	//	AABB aabb = sMeshManager->GetAABB( mesh );
	//	OBB obb = GetOBBFromAABB( aabb,
	//								  XMFLOAT3( posX, posY, posZ ),
	//								  XMFLOAT3( scaleX, scaleY, scaleZ ),
	//								  XMFLOAT3( Math::DegToRad( rotX ), Math::DegToRad( rotY ), Math::DegToRad( rotZ ) ) );
	//	sCollisionManager->AddToStaticList( name, obb, passable, tag );
	//}
	//
	//// Update Static OBB
	//__declspec( dllexport ) void UpdateStaticOBB( const char * name,
	//											  float posX,	
	//											  float posY,
	//											  float posZ,
	//											  float scaleX,
	//											  float scaleY,
	//											  float scaleZ,
	//											  float rotX,
	//											  float rotY,
	//											  float rotZ,
	//											  const char * mesh,
	//											  bool passable,
	//											  const char * tag )
	//{
	//	if ( !sCollisionManager || !sMeshManager )
	//		return;
	//
	//	AABB aabb = sMeshManager->GetAABB( mesh );
	//	OBB obb = GetOBBFromAABB( aabb,
	//								  XMFLOAT3( posX, posY, posZ ),
	//								  XMFLOAT3( scaleX, scaleY, scaleZ ),
	//								  XMFLOAT3( Math::DegToRad( rotX ), Math::DegToRad( rotY ), Math::DegToRad( rotZ ) ) );
	//	sCollisionManager->UpdateStaticObject( name, obb, passable, tag );
	//}
	//
	//// Remove Static OBB
	//__declspec( dllexport ) void RemoveStaticOBB( const char * name )
	//{
	//	if ( !sCollisionManager )
	//		return;
	//
	//	sCollisionManager->RemoveFromStaticList( name );
	//}
	//
	//// Add Moving OBB
	//__declspec( dllexport ) void AddMovingOBB( const char * name,
	//										   float posX,	
	//										   float posY,
	//										   float posZ,
	//										   float scaleX,
	//										   float scaleY,
	//										   float scaleZ,
	//										   float rotX,
	//										   float rotY,
	//										   float rotZ,
	//										   const char * mesh,
	//										   bool passable,
	//										   const char * tag )
	//{
	//	if ( !sCollisionManager || !sMeshManager )
	//		return;
	//
	//	AABB aabb = sMeshManager->GetAABB( mesh );
	//	OBB obb = GetOBBFromAABB( aabb,
	//								  XMFLOAT3( posX, posY, posZ ),
	//								  XMFLOAT3( scaleX, scaleY, scaleZ ),
	//								  XMFLOAT3( Math::DegToRad( rotX ), Math::DegToRad( rotY ), Math::DegToRad( rotZ ) ) );
	//	sCollisionManager->AddToMovingList( name, obb, passable, tag );
	//}
	//
	//// Update Moving OBB
	//__declspec( dllexport ) void UpdateMovingOBB( const char * name,
	//											  float posX,	
	//											  float posY,
	//											  float posZ,
	//											  float scaleX,
	//											  float scaleY,
	//											  float scaleZ,
	//											  float rotX,
	//											  float rotY,
	//											  float rotZ,
	//											  const char * mesh,
	//											  bool passable,
	//											  const char * tag )
	//{
	//	if ( !sCollisionManager || !sMeshManager )
	//		return;
	//
	//	AABB aabb = sMeshManager->GetAABB( mesh );
	//	OBB obb = GetOBBFromAABB( aabb,
	//								  XMFLOAT3( posX, posY, posZ ),
	//								  XMFLOAT3( scaleX, scaleY, scaleZ ),
	//								  XMFLOAT3( Math::DegToRad( rotX ), Math::DegToRad( rotY ), Math::DegToRad( rotZ ) ) );
	//	sCollisionManager->UpdateMovingObject( name, obb, passable, tag );
	//}
	//
	//// Remove Moving OBB
	//__declspec( dllexport ) void RemoveMovingOBB( const char * name )
	//{
	//	if ( !sCollisionManager )
	//		return;
	//
	//	sCollisionManager->RemoveFromMovingList( name );
	//}
	//
    //__declspec(dllexport) void CreateObject(const char * achetype, const char * name)
    //{
    //    sObjectManager->CreateFromArcheType(Hash(achetype), CName(name));
    //}
	//
    //__declspec(dllexport) void DeleteObjectByName(const char * name)
    //{
    //    sObjectManager->Destroy(CName(name));
    //}

#ifdef __cplusplus
}
#endif