#pragma once

#include "Core/Assert.h"

#include <vector>

// Message that is to be sent to Job Queue (Something like std::tuple but not exactly)
class Message
{
public:
	typedef std::vector<void *> Messages;		//!< Messages

public:
	// Constructor
	Message();

	// Move Constructor
	Message(Message && msg);

	// Destructor
	~Message();

	// Add Message in sequence. (1st message is in index 0)
	template < typename T, typename... Arguments >
	void AddMsg(Arguments... args);

	// Get message with given index
	template < typename T >
	T * GetMsg(unsigned int index) const;

private:
	Messages mMessages;							//!< Stuff for sending across

};

#include "Core/Message.inl"