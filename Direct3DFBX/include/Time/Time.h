#pragma once

#include <string>
#include <chrono>
#include "Time/TimePoint.h"

class Time
{
public:
	static inline float GetTime();
	static inline float GetFixedTime();
	static inline float GetUnscaledTime();
	static inline float GetDt();
	static inline float GetFixedDt();
	static inline float GetUnscaledDt();
	static inline float GetMaxDt();

	static inline float GetTimeScale();
	static inline void SetTimeScale(float t);

	static inline const TimePoint & GetGameStartTime();
	static inline TimePoint GetCurrentGameTime();

	static inline std::string ToString(float seconds);

	// going to friend to class that is going to be incharge of the time
	friend class Game;

private:
	// elapsed game time in seconds
	static float mTime;
	// elapsed game time in seconds, fixed
	static float mFixedTime;
	// elapsed unscale game time in seconds
	static float mUnscaledTime;

	// dt
	static float mDt;
	// fixed dt
	static float mFixedDt;
	// unscaled dt
	static float mUnscaledDt;

	// max Dt
	static float mMaxDt;
	// time scale rate
	static float mTimeScale;

	// start time
	static TimePoint mGameStartTime;

};

#include "Time/Time.inl"