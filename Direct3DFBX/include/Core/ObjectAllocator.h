#pragma once

#include "Core/FunctionsFramework.h"
#include "Singleton.h"

#include <queue>
#include <algorithm>
#include <iterator>

// Wrapper singleton class for allocator
template< typename T >
class ObjectAllocator
{
public:
	// Declare as singleton.
	DECLARE_SINGLETON(ObjectAllocator<T>);

public:
	GETSET_AUTO(unsigned int, MaxElement);		//!< Get Max element

public:
	// Destructor
	~ObjectAllocator();

	// Allocate and construct 1 Object
	template< typename... Arguments >
	T * New(Arguments&&... args);

	// Destruct and deallocate 1 Object
	void Delete(void * obj);

private:
	// Constructor
	ObjectAllocator();

	// Preallocate a block of memory
	T* PrepareBlock();

private:
	// Freelist container
	typedef std::priority_queue< unsigned int, std::vector<unsigned int>, std::greater<unsigned int> > FreeList;

private:
	std::allocator<T> mAllocator;		//!< Use std::allocator for now

	std::vector<FreeList> mFreeLists;	//!< Which element is free (priority queue to ensure index around will always get chosen)
	std::vector<T*> mBlocks;			//!< Blocks of memory allocated
	unsigned int mElementsPerPage;				//!< Number of object that can be allocate per page
	unsigned int mNumPage;						//!< Number of allocated pages
	unsigned int mPageSize;						//!< Size of each page memory block


};

#define OBJECT_ALLOCATOR(type)\
	ObjectAllocator<type>::Instance()

#include "ObjectAllocator.inl"