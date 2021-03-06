#pragma once

#include <string>

#include <d3d11.h>

	class Texture
	{
	public:
		// Constructor
		Texture();

		// Destructor
		~Texture();

		// Initialise
		void Initialize( const std::string& filename );

		// Shutdown
		void Shutdown();

	private:
		void CreateTextureFromFile( const std::string& filename );

	private:
		ID3D11ShaderResourceView * mTextureView;
		ID3D11Resource * mTexture;

		friend class Shader;
		friend class Graphics;
	};