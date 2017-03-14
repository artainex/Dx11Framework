#pragma once

#include <windows.h>
#include <thread>

class Thread
{
public:
	// typedef DWORD	ThreadID;
	typedef size_t ThreadID;

public:
	//// Get this thread ID
	//ThreadID GetThreadID() const;

	// Is thread still running
	bool IsRunnning();

public:
	// Constructor
	Thread();

	// Destructor
	~Thread();

	// Start thread with supplied function and args
	template <typename Fn, typename... Args>
	void Start(Fn && f, Args&&... args);

	// Wait for this thread to complete
	void Join();

	// Wait for this thread to complete given an amount of time
	bool TryJoin(UINT milliseconds);

	// Make thread run independently
	void Detach();

public:
	// Static Functions

	// Make current active thread sleep
	static void ThreadSleep(float seconds);

	// Make current active thread sleep
	static void ThreadSleep(UINT milliseconds);

	// Hint to reschedule thread
	static void ThreadYield();

	//// Get current thread ID
	//static ThreadID GetCurrentID();

private:
	// No copy constructor
	Thread(const Thread &);

	// No assignment operator
	const Thread & operator = (const Thread &);

private:
	typedef std::thread	PlatformThread;
	PlatformThread	mThread;	//!< Platform specific thread handle

};

#include "Thread.inl"