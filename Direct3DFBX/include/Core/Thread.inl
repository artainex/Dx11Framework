template <typename Fn, typename... Args>
void Thread::Start(Fn && f, Args&&... args)
{
	mThread = std::thread(f, std::forward<Args>(args)...);
}

//inline Thread::ThreadID Thread::GetThreadID() const
//{
//	return mThread.get_id().hash();
//}

inline void Thread::Join()
{
	if (mThread.joinable())
		mThread.join();
	// May want to log something
}

inline void Thread::Detach()
{
	mThread.detach();
}

inline void Thread::ThreadSleep(float seconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(UINT(seconds * 1000.f)));
}

inline void Thread::ThreadSleep(UINT milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

inline void Thread::ThreadYield()
{
	std::this_thread::yield();
}

//inline Thread::ThreadID Thread::GetCurrentID()
//{
//	return std::this_thread::get_id().hash();
//}