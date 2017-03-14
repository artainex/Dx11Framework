#pragma once

#include "Core/FunctionsFramework.h"

// Updatable interface.
class IDrawable
{
public:
	GETTER_AUTO(unsigned int, DrawOrder);			//!< Get Draw order
	GETTER_AUTO(bool, Visible);				//!< Get Visible

public:
	// Constructor.
	IDrawable(unsigned int drawOrder = 0, bool visible = true);

	// Empty virtual destructor.
	virtual ~IDrawable() {}

	// Pure virtual draw function.
	virtual void Draw(float dt) = 0;

private:
	unsigned int mDrawOrder;					//!< Draw order.
	bool mVisible;					//!< Visible.

	friend class System;
};

// Comparison operator.
bool operator < (const IDrawable & lhs, const IDrawable & rhs);