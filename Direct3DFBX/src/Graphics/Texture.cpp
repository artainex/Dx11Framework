// Precompiled Header
#include <Texture.h>

Texture::Texture()
	: mTextureView(nullptr)
	, mTexture(nullptr)
{
}

Texture::~Texture()
{

}

void Texture::Initialize(const  std::string& filename)
{
	CreateTextureFromFile(filename);
}

void Texture::Shutdown()
{
	if (mTexture)
	{
		mTexture->Release();
		mTexture = nullptr;
	}

	if (mTextureView)
	{
		mTextureView->Release();
		mTextureView = nullptr;
	}
}

void Texture::CreateTextureFromFile(const std::string& filename)
{
	//CreateWICTextureFromFile(GRAPHICS.GetPipeline()->GetDevice(),
	//	filename,
	//	&mTexture,
	//	&mTextureView,
	//	0);
}