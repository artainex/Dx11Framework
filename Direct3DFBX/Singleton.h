#pragma once

/*
	Declare a singleton type.
	Constructor for singleton class should be declared as private.

	Singleton class are class that has only 1 instances throughout the life of program. e.g GameStateManager

	Usage:

	class GameStateManager : Uncopyable
	{
		DECLARE_SINGLETON(GameStateManager);

		private:
			GameStateManager();
	};
*/
#define DECLARE_SINGLETON(type)\
	public:\
		static type & Instance() {\
			static type sInstance;\
			return sInstance;\
		}\
		inline static type * InstancePtr() {\
			return &(Instance());\
		}\
	private:\
		type( type const& ) = delete;\
		void operator = ( type const& ) = delete;
/*type sInstance;\*/