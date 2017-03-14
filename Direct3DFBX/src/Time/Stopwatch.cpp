// Precompiled Headers
#include "Time/Stopwatch.h"
#include "Time/ClockCycle.h"

void Stopwatch::Start()
{
	mStart = GetInternalTicks();
	mLap = mStart;
	mStarted = true;
}

float Stopwatch::Stop()
{
	if (!mStarted)
		return 0.0f;

	mElapsed += GetInternalTicks() - mStart;
	mStarted = false;

	long long t = (mElapsed * 10000000) / GetSystemFrequency();
	return float(t) / 10000000.0f;
}

void Stopwatch::Reset()
{
	mStart = 0;
	mElapsed = 0;
	mLap = 0;
	mStarted = false;
}