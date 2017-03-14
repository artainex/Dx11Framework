#pragma once

#include <windows.h>

class Mutex
{
public:
	// Constructor
	Mutex();

	// Destructor
	~Mutex();

	// Enter critical section
	void Lock();

	// Only enter critical section when it is not locked
	bool TryLock(unsigned int milliseconds = 0);

	// Exit critical section
	void Unlock();

private:
	// No copy constructor
	Mutex(const Mutex &);

	// No assignment operator
	const Mutex & operator = (const Mutex &);

private:
	typedef CRITICAL_SECTION PlatformMutex;	//!< Platform specific mutex handle
	PlatformMutex mCS;						//!< Critical Section

};

class MutexScopedLock
{
public:
	// Constructor (mutex starts to lock)
	explicit MutexScopedLock(Mutex & mtx);

	// Destructor (mutex starts to unlock)
	~MutexScopedLock();

private:
	// No copy constructor
	MutexScopedLock(const MutexScopedLock &);

	// No assignment operator
	const MutexScopedLock & operator = (const MutexScopedLock &);

private:
	Mutex & mMutex;		//!< Mutex

};

#include "Mutex.inl"

#define MUTEX_DECLARE(name)		mutable Mutex name
#define MUTEX_LOCK(name)		name.Lock()
#define MUTEX_TRYLOCK(name)		name.TryLock()
#define MUTEX_UNLOCK(name)		name.Unlock()
#define MUTEX_SCOPEDLOCK(name)	MutexScopedLock BH_MutexScopedLock( name )