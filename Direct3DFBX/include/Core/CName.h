#pragma once

#include "Core/Hash.h"

// Constant hashed name. Intended for use with constant compile-time strings.
// To prevent additional hash calculation

// Basic constant name hash.
struct CName
{
	std::string	 mText;	//!< Name text.
	HashValue	 mHash;	//!< Name hash.

	// Default constructor.
	inline CName() : mText(0), mHash(0) {}

	// Constructor. Create constant name from constant string.
	CName(const char * txt);

	//// Constructor. Create constant name from constant string.
	//explicit CName(const Hash & hash);

	// Constructor.
	CName(const char * txt, HashValue hash);

	/*
		Hash value constructor.
		WARNING: This only sets the hash value and it's to be used for name comparisons only.
	*/
	explicit CName(HashValue hash);

	// Assignment operator.
	CName & operator = (const char * txt);

	//// Assignment operator.
	//CName & operator = (const Hash & hash);

	// Equality operator.
	bool operator == (const CName & rhs) const;

	// Equality operator.
	bool operator < (const CName & rhs) const;

	//// Conversion operator.
	//operator Hash() const;
};

// Outputs hash value and name to stream operator.
inline std::ostream & operator << (std::ostream& stream, const CName& name)
{
	return (stream << name.mText);
}

template<>
struct std::hash<CName>
{
	inline std::size_t operator() (const CName& name) const { return name.mHash; }
};

#include "CName.inl"
