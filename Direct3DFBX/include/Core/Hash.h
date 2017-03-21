#pragma once

#include <functional>
#include <string>
#include <iostream>
#include "Core/FunctionsFramework.h"

// Use std::hash for now
typedef std::hash<std::string>			HashFunctor;
typedef HashFunctor::result_type		HashValue;

/*
	The object hashes a string into an integer value and stores both the string
	and the hash value. Hash objects are compared using their hash values for
	fast hash comparisons.
*/
class Hash
{
public:
	inline std::string GetText() { return mText; }
	inline HashValue GetHash() { return mHash; }

public:
	static HashValue Generate(const char* string);
	static HashValue Generate(const std::string& string);

public:
	// Default constructor.
	Hash();

	// Destructor.
	~Hash();

	// Constructor.
	Hash(const char* name);

	// Constructor.
	Hash(const std::string& name);

	// Assignment operator for strings.
	Hash & operator = (const char* name);

	// Assignment operator for strings.
	Hash & operator = (const std::string& name);

	// Equality operator.
	bool operator == (const Hash & rhs) const;

	// Equality operator.
	bool operator == (const HashValue & rhs) const;

	// Inequality operator.
	bool operator != (const Hash & rhs) const;

	// Less than operator.
	bool operator < (const Hash & rhs) const;

	// More than operator.
	bool operator > (const Hash & rhs) const;

private:
	std::string			mText;			//!< Hash name.
	HashValue			mHash;			//!< Hash value.
	friend class		Type;
	friend class		EntityManager;
};

////! Outputs hash value and name to stream operator.
//inline std::ostream & operator << (std::ostream & stream, const Hash & hash)
//{
//	return stream << hash.GetHash();
//}
//
//// For unordered_map comparison. (They hash every value and check for it)
//template<>
//struct std::hash<Hash>
//{
//	inline std::size_t operator() (const Hash& hash) const { return hash.GetHash(); }
//};

#include "Hash.inl"
