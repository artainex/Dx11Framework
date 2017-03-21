// *********************************************************************************************************************
/// 
/// @file 		Game.cpp
/// @brief		FBX Loader tester application
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************

#include <Game.h>
#include <rgbe.h>
#include <Time/Time.h>
#include <Graphics.h>
#include "Renderer.h"

Game::Game()
	: IsFixedTimeStep(true)
	, mLeftoverTime(0.0f)
	, mShutdown(false)
{
}

Game::~Game()
{
}

void Game::Initialize()
{
	Time::mFixedDt = 1.f / 60.f; //Set deference of time which would be represented by delta time
	Time::mTimeScale = 1.f;

	// Create Window
	this->Window.SetFullscreen(false);
	this->Window.OpenWindow();
	this->Window.GetEventList().mOnWindowClosed = std::bind(&Game::EndGame, this);

	// Initialise all systems
	SystemManager.Initialize();
}

void Game::Shutdown()
{
	// Shutdown in reverse order

	// Shutdown systems
	SystemManager.Shutdown();

	// Close Window
	Window.CloseWindow();

	//ObjectManager.UnloadArcheTypes();
}

void Game::EndGame()
{
	mShutdown = true;
}

void Game::Run()
{
	Time::mGameStartTime = TimePoint::Now();
	Time::mTime = 0.0f;
	
	while (!mShutdown)
	{
		Tick();
	}
}

void Game::Tick()
{
	Window.Update();
	Time::mUnscaledDt = mStopwatch.Stop();
	Time::mDt = Time::mTimeScale * Time::mUnscaledDt;

	// Continue timer
	mStopwatch.Reset();
	mStopwatch.Start();

	float timeElapsed = Time::mDt + mLeftoverTime;

	// @cleanup
	//std::cout << "FPS: " << Time::mUnscaledDt << std::endl;

	// If not enough time has past, do not update
	if (IsFixedTimeStep && timeElapsed < Time::mFixedDt)
	{
		mLeftoverTime += Time::mDt;
		return;
	}

	float fps = 1.f / timeElapsed;
	if (fps < 30.f)
	{
		std::cout << "Wait! Performance is bad!! " << std::endl << "Current FPS is " << 1.f / timeElapsed << std::endl;
	}

	Update(timeElapsed);

	// Spin
	while (!SystemManager.IsAllIdling())
		Thread::ThreadYield();

	// @cleanup
	//std::cout << "---------------------------" << std::endl;

	if (BeginDraw())
		Draw(timeElapsed);

	EndDraw();

	////Clean up deleted object
	//ObjectManager.Purge();
}

void Game::Update(float elapsed)
{
	if (IsFixedTimeStep)
	{
		while (elapsed > Time::mFixedDt)
		{
			// Spin
			while (!SystemManager.IsAllIdling())
				Thread::ThreadYield();
			
			// update
			SystemManager.Update(Time::mFixedDt);

			elapsed -= Time::mFixedDt;
			Time::mTime += Time::mFixedDt;
		}

		mLeftoverTime = elapsed;
	}
	else
	{
		SystemManager.Update(elapsed);
		Time::mTime += elapsed;
		mLeftoverTime = 0.0f;
	}
}

bool Game::BeginDraw()
{
	// @todo Graphics stuff (E.g, lost device, swap buffer)
	return true;
}

void Game::Draw(float elapsed)
{
	// Spin
	while (!SystemManager.IsAllIdling())
		Thread::ThreadYield();
	
	// update
	SystemManager.Draw(elapsed);
}

void Game::EndDraw()
{
	// @todo Graphics stuff
}
