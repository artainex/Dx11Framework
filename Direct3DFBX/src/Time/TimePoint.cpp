// Precompiled Headers

#include <stdio.h>
#include <time.h>
#include <ctime>
#include <iomanip>

#include <Time/Time.h>
#include <Time/TimePoint.h>

TimePoint::TimePoint()
{

}

TimePoint::TimePoint(const Duration & dur) :
	TimePoint(Time::GetGameStartTime() + dur)
{

}

TimePoint::TimePoint(const TimePointType & tp) :
	mTimePoint(tp)
{

}

TimePoint TimePoint::Now()
{
	return TimePoint(std::chrono::system_clock::now());
}

TimePoint & TimePoint::operator += (const Duration & dur)
{
	mTimePoint += dur.mDuration;
	return *this;
}

TimePoint & TimePoint::operator -= (const Duration & dur)
{
	mTimePoint -= dur.mDuration;
	return *this;
}

bool TimePoint::operator == (const TimePoint & tp) const
{
	return mTimePoint == tp.mTimePoint;
}

bool TimePoint::operator != (const TimePoint & tp) const
{
	return mTimePoint != tp.mTimePoint;
}

bool TimePoint::operator >= (const TimePoint & tp) const
{
	return mTimePoint >= tp.mTimePoint;
}

bool TimePoint::operator >  (const TimePoint & tp) const
{
	return mTimePoint > tp.mTimePoint;
}

bool TimePoint::operator <= (const TimePoint & tp) const
{
	return mTimePoint <= tp.mTimePoint;
}

bool TimePoint::operator <  (const TimePoint & tp) const
{
	return mTimePoint < tp.mTimePoint;
}

std::string TimePoint::ToString(const std::string & format) const
{
	static char buffer[255];

	std::time_t tt = std::chrono::system_clock::to_time_t(mTimePoint);
	struct tm * timeinfo = localtime(&tt);

	strftime(buffer, 255, format.c_str(), timeinfo);
	return std::string(buffer);
}

const TimePoint operator + (const TimePoint & tp, const Duration & dur)
{
	TimePoint t(tp);
	return t += dur;
}

const TimePoint operator + (const Duration & dur, const TimePoint & tp)
{
	TimePoint t(tp);
	return t += dur;
}

const TimePoint operator - (const TimePoint & tp, const Duration & dur)
{
	TimePoint t(tp);
	return t -= dur;
}

const Duration  operator - (const TimePoint & tp1, const TimePoint & tp2)
{
	Duration d(float((tp1.mTimePoint - tp2.mTimePoint).count() *
		TimePoint::TimePointType::duration::period::num /
		TimePoint::TimePointType::duration::period::den));
	return d;
}