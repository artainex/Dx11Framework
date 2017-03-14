////////////////////////////////////////////////////////////////////////////////
// Filename: HDRTexture.cpp
////////////////////////////////////////////////////////////////////////////////
#include <HDRTexture.h>
#include <fstream>
#include <memory>

HDRTexture::HDRTexture()
	: mTextureView(nullptr)
	, mTexture(nullptr)
	, mImageData()
	, mWidth(0)
	, mHeight(0)
	, mExposure(1.0f)
	, mGamma(1.0f)
{
}

HDRTexture::~HDRTexture()
{

}

void HDRTexture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string & filename)
{
	LoadHDRTexture(filename);
	CreateTextureFromImageData(device, deviceContext);
}

void HDRTexture::Shutdown()
{
	SAFE_RELEASE(mTexture);
	SAFE_RELEASE(mTextureView);
	mImageData.clear();
}

void HDRTexture::CreateTextureFromImageData(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = mImageData.data();
	initData.SysMemPitch = static_cast<int>(mWidth * 4 * sizeof(float));
	initData.SysMemSlicePitch = static_cast<int>(mHeight);

	D3D11_TEXTURE2D_DESC textureDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = mWidth;
	textureDesc.Height = mHeight;
	textureDesc.MipLevels = static_cast<int>(log10(static_cast<float>(mWidth)) / log10(2.0f));
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT result = device->CreateTexture2D(&textureDesc, &initData, &mTexture);
	//HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, &mTexture);
	if (FAILED(result))
		return;

	D3D11_BOX destBox;
	destBox.left = 0;
	destBox.right = mWidth;
	destBox.top = 0;
	destBox.bottom = mHeight;
	destBox.front = 0;
	destBox.back = 1;

	deviceContext->UpdateSubresource(mTexture,			// the texture to update
		0,												// first mip-level
		&destBox,										// position of the pixels to update in the texture
		mImageData.data(),								// image data
		static_cast<int>(mWidth * 4 * sizeof(float)),	// row pitch
		0												// not used for 2D textures
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mTextureView);

	deviceContext->GenerateMips(mTextureView);
}

void HDRTexture::LoadHDRTexture(const std::string& filename)
{
	// Load file
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp)
	{
		printf("Can't open file: %s\n", filename.c_str());
		return;
	}

	rgbe_header_info info;
	char errbuf[100] = { 0 };
	if (RGBE_RETURN_SUCCESS != RGBE_ReadHeader(fp, &mWidth, &mHeight, &info, errbuf))
	{
		printf("RGBE read error: %s\n", errbuf);
		return;
	}

	mExposure = info.exposure;
	mGamma = info.gamma;

	mImageData.resize(mWidth * mHeight * 4, 0.0f);

	if (RGBE_RETURN_SUCCESS != RGBE_ReadPixels_RLE(fp, mImageData.data(), mWidth, mHeight, errbuf))
	{
		printf("RGBE read error: %s\n", errbuf);
		return;
	}
	fclose(fp);
}