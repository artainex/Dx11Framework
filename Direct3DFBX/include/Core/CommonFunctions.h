#pragma once

#include <Renderer.h>

// Genric functions expose for scripts uses
#ifdef __cplusplus
extern "C"
{
#endif

	//// Start profiler
	//__declspec( dllexport ) void StartProfiler( const char * profiler_name );
	//
	//// Stop profiler
	//__declspec( dllexport ) void StopProfiler( const char * profiler_name );

	// Set renderer
	__declspec( dllexport ) void SetRenderer( Renderer * renderer );

	//// Set mesh manager
	//__declspec( dllexport ) void SetMeshManager( MeshManager * meshManager );
	//
	//// Set texture manager
	//__declspec( dllexport ) void SetTextureManager( TextureManager * textureManager );
	//
	//// Set material manager
	//__declspec( dllexport ) void SetMaterialManager( MaterialManager * materialManager );
	//
	//// Set collision manager
	//__declspec( dllexport ) void SetCollisionManager( CollisionManager * collisionManager );
	//
	//// daeyeop jei. set audio manager
	//__declspec(dllexport) void SetAudioManager(AudioManager * audioManager);
	//
	//
    ////daeyeop jei
    //__declspec(dllexport) void SetLevelScriptMSG(LevelScriptMSG* levelScriptMSG);
	//
    //__declspec(dllexport) void SetObjectManager(ObjectManager * objectmanager);

	// Draw instance
	__declspec( dllexport ) void DrawInstance( float posX,	
											   float posY,
											   float posZ,
											   float scaleX,
											   float scaleY,
											   float scaleZ,
											   float rotX,
											   float rotY,
											   float rotZ,
											   const char * mesh,		
											   const char * material );

    //// Draw sprites
    //__declspec(dllexport) void DrawSprites( float posX,
    //                                        float posY,
    //                                        float posZ,
    //                                        float scaleX,
    //                                        float scaleY,
    //                                        float scaleZ,
    //                                        float rotX,
    //                                        float rotY,
    //                                        float rotZ,
    //                                        float index,
    //                                        float row,
    //                                        float column,
    //                                        float frame,
    //                                        bool animated,
    //                                        const char * mesh,
    //                                        const char * material);
	//
	//// Draw global light
	//__declspec( dllexport ) void DrawGlobalLight( float posX,	
	//										      float posY,
	//										      float posZ,
	//										      float colorR,
	//										      float colorG,
	//										      float colorB,
	//										      float colorA );
	//
	//// Draw local light
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
	//												   float depth );
	//
	//// Draw local light
	//__declspec( dllexport ) void DrawLocalLight( float posX,	
	//										     float posY,
	//										     float posZ,
	//										     float colorR,
	//										     float colorG,
	//										     float colorB,
	//										     float colorA,
	//											 float radius );

	////daeyeop jei. Enable sound
	//__declspec(dllexport) void EnableSoundEffect(const char * name);
	//
	////daeyeop jei. Enable sound
	//__declspec(dllexport) void DisableSoundEffect(const char * name);
	//
	////daeyeop jei. Change the level
	//__declspec(dllexport) void LevelMessage(const char * name);
	//
	//// Add Static OBB
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
	//										   const char * tag );
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
	//											  const char * tag );
	//
	//// Remove Static OBB
	//__declspec( dllexport ) void RemoveStaticOBB( const char * name );
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
	//										   const char * tag );
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
	//											  const char * tag );
	//
	//// Remove Moving OBB
	//__declspec( dllexport ) void RemoveMovingOBB( const char * name );

#ifdef __cplusplus
}
#endif