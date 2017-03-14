// Precompiled Headers
#include "Duration.h"

const Duration Duration::ZERO = Duration(0.0f);

Duration::Duration()
{

}

Duration::Duration(float seconds) :
	mDuration(std::chrono::duration_cast<std::chrono::system_clock::duration>(
		std::chrono::duration<float>(seconds)))
{

}

Duration & Duration::operator+= (const Duration & rhs)
{
	mDuration += rhs.mDuration;
	return *this;
}

Duration & Duration::operator-= (const Duration & rhs)
{
	mDuration -= rhs.mDuration;
	return *this;
}

bool Duration::operator == (const Duration & rhs)
{
	return mDuration == rhs.mDuration;
}

bool Duration::operator != (const Duration & rhs)
{
	return mDuration != rhs.mDuration;
}

bool Duration::operator <= (const Duration & rhs)
{
	return mDuration <= rhs.mDuration;
}

bool Duration::operator <  (const Duration & rhs)
{
	return mDuration < rhs.mDuration;
}

bool Duration::operator >= (const Duration & rhs)
{
	return mDuration >= rhs.mDuration;
}

bool Duration::operator >(const Duration & rhs)
{
	return mDuration > rhs.mDuration;
}

std::string Duration::ToStringMinutes() const
{
	return std::to_string(std::chrono::duration<float, std::ratio< 60 > >(mDuration).count()) + "min";
}

std::string Duration::ToStringSeconds() const
{
	return std::to_string(std::chrono::duration<float>(mDuration).count()) + "s";
}

std::string Duration::ToStringMilliseconds() const
{
	return std::to_string(mDuration.count()) + "ms";
}

const Duration operator + (const Duration & lhs, const Duration & rhs)
{
	Duration r = lhs;
	return r += rhs;
}

const Duration operator - (const Duration & lhs, const Duration & rhs)
{
	Duration r = lhs;
	return r -= rhs;
}