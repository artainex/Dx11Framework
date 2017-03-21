#pragma once
#include <Utilities.h>

class DepthRenderTarget
{
public:
	DepthRenderTarget();
	DepthRenderTarget(const DepthRenderTarget& other);
	~DepthRenderTarget();

	bool Initialize(ID3D11Device* device, int width, int height);
	void Shutdown();

	void SetRenderTarget();
	void ReleaseRenderTarget(eShaderType shaderType);
	void ClearRenderTarget(const float* color);
	void ClearRenderTarget(float r, float g, float b, float a);
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
