template< typename T >
ObjectAllocator<T>::ObjectAllocator()
	: mElementsPerPage(1024)
	, mNumPage(0)
{
	mPageSize = mElementsPerPage * sizeof(T);
}

template< typename T >
ObjectAllocator<T>::~ObjectAllocator()
{
	for (unsigned int i = 0; i < mNumPage; ++i)
	{
		mAllocator.deallocate(mBlocks[i], mElementsPerPage);
	}
}

// Allocate and construct 1 Object
template< typename T >
template< typename... Arguments >
T * ObjectAllocator<T>::New(Arguments&&... args)
{
	T* obj = nullptr;

	for (unsigned int i = 0; i < mNumPage; ++i)
	{
		if (mFreeLists[i].empty())
		{
			continue;
		}
		obj = mBlocks[i] + mFreeLists[i].top();
		mFreeLists[i].pop();
	}
	if (obj == nullptr)
		obj = PrepareBlock();

	mAllocator.construct(obj, std::forward< Arguments >(args)...);
	return obj;
}

// Destruct and deallocate 1 Object
template< typename T >
void ObjectAllocator<T>::Delete(void * obj)
{
	if (obj == nullptr)
		return;
	// Destruct obj and return it to freelist
	mAllocator.destroy(reinterpret_cast<T *>(obj));

	for (unsigned int i = 0; i < mNumPage; ++i)
	{
		if (obj < mBlocks[i] || mBlocks[i] + mPageSize > obj)
		{
			continue;
		}
		mFreeLists[i].push(reinterpret_cast<T *>(obj) - mBlocks[i]);
	}
}

// Preallocate a block of memory
template< typename T >
T* ObjectAllocator<T>::PrepareBlock()
{
	T* ret;

	mBlocks.push_back(mAllocator.allocate(mElementsPerPage));
	mFreeLists.push_back(FreeList());

	for (unsigned int i = 0; i < mElementsPerPage; ++i)
		mFreeLists[mNumPage].push(i);

	ret = mBlocks[mNumPage] + mFreeLists[mNumPage].top();
	mFreeLists[mNumPage].pop();

	mNumPage++;

	return ret;
}