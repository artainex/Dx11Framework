#include "Core/IUpdatable.h"

// Constructor.
IUpdatable::IUpdatable(unsigned int updateOrder, bool enabled)
	: mUpdateOrder(updateOrder)
	, mEnabled(enabled)
	, mFlip(false)
{
}

bool operator < (const IUpdatable & lhs, const IUpdatable & rhs)
{
	return lhs.GetUpdateOrder() < rhs.GetUpdateOrder();
}