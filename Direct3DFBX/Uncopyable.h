#pragma once
/*
	Inherit from this class if the class cannot be copied.
	E.g. Manager class, mutex.
*/

// Disable annoying warning about non dll-interface class.
#pragma warning( disable:4275 )

class Uncopyable
{
private:
	Uncopyable(const Uncopyable&);
	Uncopyable & operator=(const Uncopyable &);

public:
	Uncopyable() {};
	virtual ~Uncopyable() {};
};
