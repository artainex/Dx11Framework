// Constant hashed string object.
inline CName::CName(const char* txt)
	: mText(txt)
	, mHash(txt ? Hash::Generate(txt) : 0) {}

//inline CName::CName(const Hash& hash)
//{
//	mText = hash.GetText();
//	mHash = hash.GetHash();
//}

inline CName::CName(const char* txt, HashValue hash)
	: mText(txt)
	, mHash(hash) {}

inline CName::CName(HashValue hash)
	: mText(nullptr)
	, mHash(hash) {}

inline CName & CName::operator = (const char* txt)
{
	mText = txt;
	mHash = Hash::Generate(mText);
	return *this;
}

//inline CName & CName::operator = (const Hash& hash)
//{
//	mText = hash.GetText();
//	mHash = Hash::Generate(mText);
//	return *this;
//}

inline bool CName::operator == (const CName& rhs) const
{
	return mHash == rhs.mHash;
}

inline bool CName::operator < (const CName& rhs) const
{
	return mHash < rhs.mHash;
}

//inline CName::operator Hash() const
//{
//	return Hash(mText);
//}
