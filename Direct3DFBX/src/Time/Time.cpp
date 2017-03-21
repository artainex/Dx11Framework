// Precompiled Headers
#include "Time/Time.h"

float Time::mTime = 0.0f;
float Time::mFixedTime = 0.0f;
float Time::mUnscaledTime = 0.0f;

float Time::mDt = 0.0f;
float Time::mFixedDt = 0.0f;
float Time::mUnscaledDt = 0.0f;

float Time::mMaxDt = 0.0f;
float Time::mTimeScale = 0.0f;

TimePoint Time::mGameStartTime;
