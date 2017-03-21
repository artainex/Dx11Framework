
inline float Time::GetTime()
{
	return mTime;
}

inline float Time::GetFixedTime()
{
	return mFixedTime;
}

inline float Time::GetUnscaledTime()
{
	return mUnscaledTime;
}
inline float Time::GetDt()
{
	return mDt;
}

inline float Time::GetFixedDt()
{
	return mFixedDt;
}

inline float Time::GetUnscaledDt()
{
	return mUnscaledDt;
}

inline float Time::GetMaxDt()
{
	return mMaxDt;
}

inline float Time::GetTimeScale()
{
	return mTimeScale;
}

inline void Time::SetTimeScale(float t)
{
	mTimeScale = t;
}

inline const TimePoint & Time::GetGameStartTime()
{
	return mGameStartTime;
}

inline TimePoint Time::GetCurrentGameTime()
{
	Duration duration(GetTime());
	return GetGameStartTime() + duration;
}