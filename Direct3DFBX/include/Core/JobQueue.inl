inline JobQueue::JobEntry::JobEntry()
	: mJob(Job::FromFunction<nullptr>())
{
}

inline JobQueue::JobEntry::JobEntry(JobEntry && job)
	: mJob(job.mJob)
	, mPriority(job.mPriority)
	, mMessage(std::move(job.mMessage))
{
}

inline JobQueue::JobEntry & JobQueue::JobEntry::operator = (JobEntry && job)
{
	mPriority = job.mPriority;
	//std::swap(mMessage, job.mMessage);
	mJob = job.mJob;
	return *this;
}

inline bool JobQueue::JobEntry::operator > (const JobEntry & rhs) const
{
	return mPriority > rhs.mPriority;
}

inline bool JobQueue::JobEntry::operator == (const Job & job) const
{
	return mJob == job;
}

inline void JobQueue::AddJobs(unsigned int priority, const Job & job, Message & msg)
{
	MUTEX_SCOPEDLOCK(mJobMutex);
	//mJobs.push( JobEntry( priority, job, msg ) );
	mJobs.emplace(priority, job, msg);
}

inline bool JobQueue::JobAvailable() const
{
	MUTEX_SCOPEDLOCK(mJobMutex);
	return !(mJobs.empty()) || mWorking;
}