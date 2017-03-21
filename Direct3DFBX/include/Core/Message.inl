
inline Message::Message() {}

inline Message::Message(Message && msg)
	: mMessages(std::move(msg.mMessages))
{
}

inline Message::~Message()
{
	for (auto & i : mMessages)
		delete i;
}

template < typename T, typename... Arguments >
inline void Message::AddMsg(Arguments... args)
{
	T * msg = new T(std::forward<Arguments>(args)...);
	mMessages.push_back(msg);
}

template < typename T >
T * Message::GetMsg(unsigned int index) const
{
	BH_ASSERT(index < mMessages.size(), "Message::GetMessage - index out of bound!");
	return reinterpret_cast<T *>(mMessages[index]);
}