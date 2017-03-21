#pragma once

#define BH_DELEGATE_CALLTYPE __fastcall

// Need this line for the code to work. 
template < typename > class Delegate;

template <typename RType, typename... Arguments>
class Delegate< RType(Arguments...) >
{
public:
	typedef RType ReturnType;												//!< Delegate return type.
	typedef ReturnType(BH_DELEGATE_CALLTYPE * SignatureType)(Arguments...);	//!< Function pointer type

public:
	// Get a function delegate.
	template< ReturnType(*TMethod)(Arguments...) >
	inline static Delegate FromFunction()
	{
		return FromStub(nullptr, &FunctionStub< TMethod >);
	}

	// Get a class method delegate.
	template< class T, ReturnType(T::*TMethod)(Arguments...) >
	inline static Delegate FromMethod(T * objectPtr)
	{
		return FromStub(objectPtr, &MethodStub< T, TMethod >);
	}

	// Get a class const method delegate.
	template< class T, ReturnType(T::*TMethod)(Arguments...) const >
	inline static Delegate FromConstMethod(T const * objectPtr)
	{
		return FromStub(const_cast< T * >(objectPtr), &ConstMethodStub< T, TMethod >);
	}

	// Trigger delegate.
	inline ReturnType Call(Arguments... args) const
	{
		return ObjectCall(mObjectPtr, args...);
	}

	// Bracket operator used to trigger delegate.
	inline ReturnType ObjectCall(void * obj, Arguments... args) const
	{
		return (*mStubPtr)(obj, args...);
	}

	// Bracket operator used to trigger delegate.
	inline ReturnType operator()(Arguments... args) const
	{
		return Call(args...);
	}

	// Check if delegate is empty.
	inline operator bool() const
	{
		return mStubPtr != nullptr;
	}

	// Logical negation operator.
	inline bool operator!() const
	{
		return !(operator bool());
	}

	// For STL container storage.
	inline bool operator < (const Delegate & rhs) const
	{
		return (mObjectPtr < rhs.mObjectPtr) && (mStubPtr < rhs.mStubPtr);
	}

	// Compares two delegates if they are the same.
	inline bool operator == (const Delegate & rhs) const
	{
		return (rhs.mObjectPtr == mObjectPtr) && (rhs.mStubPtr == mStubPtr);
	}

	// Resets the delegate to nullptr (for invalidation).
	inline void Reset()
	{
		mStubPtr = nullptr;
	}

private:
	// Function callback type.
	typedef ReturnType(BH_DELEGATE_CALLTYPE * StubType)(void * objectPtr, Arguments...);

private:
	// Constructor.
	inline Delegate() : mObjectPtr(nullptr), mStubPtr(nullptr) {}

	// Overloaded constructor.
	inline Delegate(void * objPtr, StubType stubPtr) : mObjectPtr(objPtr), mStubPtr(stubPtr) {}

	// Get the delegate 'stub'
	inline static Delegate FromStub(void * objectPtr, StubType stubType)
	{
		return Delegate(objectPtr, stubType);
	}

	// Call the delegate function 'stub'.
	template< ReturnType(*TMethod)(Arguments...) >
	inline static ReturnType BH_DELEGATE_CALLTYPE FunctionStub(void *, Arguments... args)
	{
		return (TMethod)(args...);
	}

	// Call the delegate method 'stub'.
	template< class T, ReturnType(T::*TMethod)(Arguments...) >
	inline static ReturnType BH_DELEGATE_CALLTYPE MethodStub(void * objectPtr, Arguments... args)
	{
		T * p = static_cast< T * >(objectPtr);
		return (p->*TMethod)(args...);
	}

	// Call the delegate const method 'stub'.
	template< class T, ReturnType(T::*TMethod)(Arguments...) const >
	inline static ReturnType BH_DELEGATE_CALLTYPE ConstMethodStub(void * objectPtr, Arguments... args)
	{
		T const * p = static_cast< T * >(objectPtr);
		return (p->*TMethod)(args...);
	}


private:
	void *		mObjectPtr;		//!< Object pointer.
	StubType 	mStubPtr;		//!< Function mapping pointer.

};
