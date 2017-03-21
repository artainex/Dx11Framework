#pragma once

#include <chrono>
#include <string>

class Duration
{
private:
	friend class TimePoint;

private:
	typedef float DataType;
	typedef std::milli DurationType;

public:
	static const Duration ZERO;

public:
	Duration();
	Duration(const Duration & dur) = default;
	Duration(float seconds);

	Duration & operator = (const Duration & rhs) = default;

	Duration & operator += (const Duration & rhs);
	Duration & operator -= (const Duration & rhs);

	bool operator == (const Duration & rhs);
	bool operator != (const Duration & rhs);
	bool operator <= (const Duration & rhs);
	bool operator <  (const Duration & rhs);
	bool operator >= (const Duration & rhs);
	bool operator >  (const Duration & rhs);

	std::string ToStringMinutes() const;
	std::string ToStringSeconds() const;
	std::string ToStringMilliseconds() const;

private:
	std::chrono::system_clock::duration mDuration;
};

const Duration operator + (const Duration & lhs, const Duration & rhs);
const Duration operator - (const Duration & lhs, const Duration & rhs);
