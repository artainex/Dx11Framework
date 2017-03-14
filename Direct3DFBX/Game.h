// *********************************************************************************************************************
/// 
/// @file 		Game.h
/// @brief		FBX Loader tester application
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************
#pragma once 

#include <Core/Mutex.h>
#include <PlatformWindow.h>

#include <Uncopyable.h>
#include <Singleton.h>
#include <Core/SystemManager.h>
#include <Time/Stopwatch.h>


// Handles main game loop
class Game : private Uncopyable
{
public:
	DECLARE_SINGLETON(Game);

public:
	// Settings
	bool IsFixedTimeStep;				//!< Fixed Time Step
										// Managers
	SystemManager SystemManager;		//!< System Manager
	//ObjectManager ObjectManager;		//!< Object Manager
	PlatformWindow Window;				//!< Game Window

public:
	// Destructor
	~Game();

	// Intialise game
	void Initialize();

	// Run the game
	void Run();

	// Shutdown game
	void Shutdown();

	// Call this to end the game
	void EndGame();

private:
	// Constructor
	Game();

	// Tick
	void Tick();

	// Update game
	void Update(float elapsed);

	// Pre draw
	bool BeginDraw();

	// Draw all drawables ever exist
	void Draw(float elapsed);

	// Post draw
	void EndDraw();

private:
	Stopwatch mStopwatch;				//!< Stopwatch for dt
	float mLeftoverTime;				//!< Time leftover due to fixed timestep

	bool mShutdown;						//!< Mark to terminate
};

#define GAME\
	::Game::Instance()