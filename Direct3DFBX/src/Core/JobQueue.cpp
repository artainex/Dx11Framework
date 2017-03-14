#include "Core/JobQueue.h"

JobQueue::JobEntry::JobEntry(unsigned int priority, const Job & job, Message & msg)
	: mPriority(priority)
	, mMessage(std::move(msg))
	, mJob(job)
{
}

JobQueue::JobQueue()
	: mWorking(false)
{
}

JobQueue::~JobQueue()
{
}

void JobQueue::Work()
{
	{
		MUTEX_SCOPEDLOCK(mJobMutex);
		mWorking = !mJobs.empty();
	}

	//while (mWorking)
	//{
	//	JobEntry j;
	//	// Remove and process current
	//	{
	//		MUTEX_SCOPEDLOCK(mJobMutex);
	//		j = std::move(mJobs.top());
	//		mJobs.pop();
	//	}
	//
	//	j.mJob(j.mMessage);
	//
	//	{
	//		MUTEX_SCOPEDLOCK(mJobMutex);
	//		mWorking = !mJobs.empty();
	//	}
	//}
}