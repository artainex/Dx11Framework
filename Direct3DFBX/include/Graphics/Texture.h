#pragma once

#include <d3d11.h>
#include <string>

class Texture
{
public:
	// Constructor
	Texture();

	// Destructor
	~Texture();

	// Initialise
	void Initialize(const std::string & filename);

	// Shutdown
	void Shutdown();

private:
	void CreateTextureFromFile(const std::string& filename);

private:
	ID3D11ShaderResourceView * mTextureView;
	ID3D11Resource * mTexture;
};