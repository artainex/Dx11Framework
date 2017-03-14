#pragma once

#include "Duration.h"

#include <chrono>

#if 0
	namespace TimeType
	{

		typedef std::chrono::high_resolution_clock HighResolutionClock;
		typedef std::chrono::system_clock NormalResolutionClock;

	}

	template <typename T = TimeType::NormalResolutionClock>
	class Stopwatch
	{
	public:

		Stopwatch( ) = default;
		~Stopwatch( ) = default;

		void Start( bool result = true );
		Real Stop( );
		void Reset( );


	private:

		bool mStarted = false;
		typename T::time_point mPoint1;
		typename T::time_point mPoint2;

	};

	typedef Stopwatch<TimeType::NormalResolutionClock> NormalStopwatch;
	typedef Stopwatch<TimeType::HighResolutionClock> HighResStopwatch;
#else
	class Stopwatch
	{
	public:
		Stopwatch( ) = default;
		~Stopwatch( ) = default;

		void Start( );
		float Stop( );
		void Reset( );

	private:
		mutable long long	mLap = 0;			//!< Lap total time in internal ticks.
		long long			mStart = 0;			//!< Stopwatch start time in internal ticks.
		long long			mElapsed = 0;		//!< Elapsed time since stopwatch started in internal ticks.
		bool				mStarted = false;	//!< Stopwatch started flag.

	};
#endif

#include "Time/Stopwatch.inl"
