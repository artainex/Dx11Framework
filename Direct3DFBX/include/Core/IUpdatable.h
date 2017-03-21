#pragma once

#include "Core/FunctionsFramework.h"

// Updatable interface.
class IUpdatable
{
public:
	GETTER_AUTO(unsigned int, UpdateOrder);			//!< Get update order
	GETTER_AUTO(bool, Enabled);				//!< Get updatable is enabled for update
	GETSET_AUTO(bool, Flip);				//!< Get and set the flip status

public:
	// Constructor.
	IUpdatable(unsigned int updateOrder = 0, bool enabled = true);

	// Empty virtual destructor.
	virtual ~IUpdatable() {}

	// Pure virtual update function.
	virtual void Update(float dt) = 0;

private:
	unsigned int mUpdateOrder;					//!< Update order.
	bool mEnabled;						//!< Enabled.
	bool mFlip;							//!< Bit flipping status

	friend class System;
};

// Comparison operator.
bool operator < (const IUpdatable & lhs, const IUpdatable & rhs);