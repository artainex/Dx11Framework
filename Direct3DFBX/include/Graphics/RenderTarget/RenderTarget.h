#pragma once
#include <Utilities.h>

class RenderTarget
{
public:
	RenderTarget();
	RenderTarget(const RenderTarget& other);
	~RenderTarget();

	bool Initialize(int width, int height);
	void Shutdown();

	void SetRenderTarget();
	void ReleaseRenderTarget(eShaderType shaderType);
	void ClearRenderTarget();
	ID3D11ShaderResourceView* GetShaderResourceView();

	// Get Dimension
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	int mWidth;
	int mHeight;

public:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	friend class Shader;
};
