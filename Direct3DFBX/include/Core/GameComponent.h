#pragma once

#include <Core/FunctionsFramework.h>
#include <Uncopyable.h>

class System;

// Interface class of game component
class GameComponent : Uncopyable
{
public:
	GETTER_AUTO_PTR(System, System);	//!< Get System Pointer

public:
	// Constructor
	GameComponent();

	// Destructor
	virtual ~GameComponent();

	// Initialise function
	virtual void Initialize() = 0;

	// Shutdown function
	virtual void Shutdown() = 0;

protected:
	System * mSystem;					//!< Pointer to the system it belongs to

	friend class System;				//!< Befriend system

};