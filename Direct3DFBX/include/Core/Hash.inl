// Hashed string object.
inline Hash::Hash()
	: mText("")
	, mHash(0)
{
}

inline Hash::Hash(const char* name)
	: mText(name ? name : "")
	, mHash(name ? Generate(name) : 0)
{
}

inline Hash::Hash(const std::string& name)
	: mText(name)
	, mHash(Generate(name))
{
}

inline bool Hash::operator != (const Hash & rhs) const
{
	return mHash != rhs.mHash;
}

inline bool Hash::operator == (const Hash & rhs) const
{
	return mHash == rhs.mHash;
}

inline bool Hash::operator == (const HashValue & rhs) const
{
	return mHash == rhs;
}

inline bool Hash::operator < (const Hash & rhs) const
{
	return mHash < rhs.mHash;
}

inline bool Hash::operator >(const Hash & rhs) const
{
	return mHash > rhs.mHash;
}

inline HashValue Hash::Generate(const char* string)
{
	return Generate(std::string(string));
}

inline HashValue Hash::Generate(const std::string& string)
{
	if (string == "")
		return 0;

	return HashFunctor()(string);
}