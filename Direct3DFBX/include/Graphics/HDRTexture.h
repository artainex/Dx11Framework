#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: HDRTexture.h
////////////////////////////////////////////////////////////////////////////////
#include <Skydome.h>
#include <fstream>

class HDRTexture
{
public:
	// Constructor
	HDRTexture();

	// Destructor
	~HDRTexture();

	// Initialise
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filename);

	// Shutdown
	void Shutdown();

	// Get image dimension
	float GetTextureDimension() const { return static_cast<float>(mWidth) * static_cast<float>(mHeight); }

private:
	void CreateTextureFromImageData(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void LoadHDRTexture(const std::string& filename);

private:
	ID3D11Texture2D * mTexture;

	std::vector< float > mImageData;
	int mWidth;
	int mHeight;

	float mExposure;
	float mGamma;

public:
	ID3D11ShaderResourceView* mTextureView;
};