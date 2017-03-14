#pragma once

#include <sstream>

/*
	Assertion break.
	Usage:

		char * tmp = nullptr;
		BH_ASSERT_TRUE( tmp );
		BH_ASSERT( tmp != nullptr, "tmp is null!" );
*/
void Assert(char * const expression,
	const char * const message,
	const char * const filename,
	const size_t line);

#define BH_FORCE_ASSERT( message )\
		{ Assert("Force assert.", message, __FILE__, __LINE__); }

#if BH_ASSERT_MODE == BH_ENABLED

// Assert expression is not null.
#define BH_ASSERT_TRUE( exp )\
			{ if (!(exp)) Assert(#exp, "Expression is false. Expected true.", __FILE__, __LINE__); }
		// Assert expression is null.
#define BH_ASSERT_FALSE( exp )\
			{ if ((exp)) Assert(#exp, "Expression is true. Expected false.", __FILE__, __LINE__); }
		// Assert expression.
#define BH_ASSERT( exp, message )\
			{ if (!(exp)) { std::ostringstream m; m << message; Assert(#exp, m.str().c_str(), __FILE__, __LINE__); } }
#define BH_FAIL( message )\
			{ std::ostringstream m; m << message; Assert("Assertion failed.", m.str().c_str(), __FILE__, __LINE__); }

#else
#define BH_ASSERT_TRUE( exp )			
#define BH_ASSERT_FALSE( exp )			
#define BH_ASSERT( exp, message )		
#define BH_FAIL( message )				
#endif

#define BH_STATIC_ASSERT( condition, message )	static_assert(condition, message)
