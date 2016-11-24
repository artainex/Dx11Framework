#pragma once

#include <d3d11.h>

class UAVTexture
{
public:
	// Constructor
	UAVTexture();

	// Destructor
	~UAVTexture();

	// Initialize
	bool Initialize(ID3D11Device* device, int width, int height);

	// Shutdown
	void Shutdown();

	// Get Shader Resource View
	ID3D11UnorderedAccessView * GetUnorderedAccessView() { return mUnorderedAccessView; }

	// Get Shader Resource View
	ID3D11ShaderResourceView * GetShaderResourceView() { return mShaderResourceView; }

	void ReleaseShaderResourceView(ID3D11DeviceContext* deviceContext);

	// Get Dimension
	int UAVTexture::GetWidth() const { return mWidth; }
	int UAVTexture::GetHeight() const { return mHeight; }

private:
	ID3D11UnorderedAccessView * mUnorderedAccessView;
	ID3D11ShaderResourceView * mShaderResourceView;
	ID3D11Texture2D * mTexture;
	int mWidth;
	int mHeight;
};