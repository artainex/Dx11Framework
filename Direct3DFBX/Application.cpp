// *********************************************************************************************************************
/// 
/// @file 		Application.cpp
/// @brief		FBX Loader tester application
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************

#include "Core/CommonFunctions.h"
#include <Core/InputManager.h>
#include <Renderer.h>
#include <Game.h>


//// 스카이 돔은 했다 치고, HDR읽어와서 모델에 프로젝팅 하는것부터 해보자
//
////--------------------------------------------------------------------------------------
//// Entry point to the program. Initializes everything and goes into a message processing 
//// loop. Idle time is used to render the scene.
////--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SYSTEM_MANAGER.AddGameComponentToSystem<InputManager>();
	SYSTEM_MANAGER.AddGameComponentToSystem<Renderer>();

	//// Make dependency of Things which are needed to be rendered to Render class
	//SYSTEM_MANAGER.GameComponentDependency<MeshManager, Renderer>();
	//SYSTEM_MANAGER.GameComponentDependency<TextureManager, Renderer>();
	//SYSTEM_MANAGER.GameComponentDependency<HDRTextureManager, :Renderer>();

	// For interop, set the global instance variable
	// These variables are used in script functions 
	SetRenderer(SYSTEM_MANAGER.GetGameComponentFromSystem<Renderer>());
	
	//GAME.Window.SetInstance(hInstance);
	//GAME.Window.SetTitle( "FBX_Loader" );
	GAME.Window.GetSettings().w = Scr_Width;
	GAME.Window.GetSettings().h = Scr_Height;
	GAME.Initialize();
	GAME.Run();
	GAME.Shutdown();

	return 0;
}