#include "Core/IDrawable.h"

// Constructor.
IDrawable::IDrawable(unsigned int drawOrder, bool visible)
	: mDrawOrder(drawOrder)
	, mVisible(visible)
{
}

bool operator < (const IDrawable & lhs, const IDrawable & rhs)
{
	return lhs.GetDrawOrder() < rhs.GetDrawOrder();
}