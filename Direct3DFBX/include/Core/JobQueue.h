#pragma once

#include <Uncopyable.h>
#include <Core/Delegate.h>
#include <Core/Message.h>
#include <Core/Mutex.h>

#include <functional>
#include <queue>
#include <tuple>

// Job System
class JobQueue : private Uncopyable
{
public:
	typedef Delegate< void(const Message &) > Job;	//!< Job type

public:
	// Constructor
	JobQueue();

	// Destructor
	~JobQueue();

	// Add a job ( Message will be emptied )
	void AddJobs(unsigned int priority, const Job & job, Message & msg);

	// Check if there is available job to work on
	bool JobAvailable() const;

	// Work on available jobs
	void Work();

private:
	// Entry for queue to manage
	struct JobEntry
	{
	public:
		// Default Constructor
		JobEntry();

		// Constructor
		JobEntry(unsigned int priority, const Job & job, Message & msg);

		// Move Constructor ( For priority move to move object about )
		JobEntry(JobEntry && job);

		// Move Assignment
		JobEntry & operator = (JobEntry && job);

		// Greater comparison operator
		bool operator > (const JobEntry & rhs) const;

		// Equality operator
		bool operator == (const Job & job) const;

	public:
		Message mMessage;
		Job mJob;
		unsigned int mPriority;

	};

	typedef std::priority_queue< JobEntry, std::vector<JobEntry>, std::greater<JobEntry> > JobQueueList;

private:
	JobQueueList mJobs;			//!< Event delegates
	MUTEX_DECLARE(mJobMutex);	//!< Mutex for multi-threading
	bool mWorking;

};

#include "Core/JobQueue.inl"