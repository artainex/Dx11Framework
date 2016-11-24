#pragma once

#include <Utilities.h>
#include <vector>

class MultiRenderTarget
{
public:
	MultiRenderTarget();
	MultiRenderTarget(const MultiRenderTarget&);
	~MultiRenderTarget();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ReleaseRenderTarget(eShaderType shadertype, ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, const float* color);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float r, float g, float b, float a);

	UINT GetRenderTargetsCount() const { return (UINT)m_renderTargetTextures.size(); }

	ID3D11ShaderResourceView* GetShaderResourceView(int index);
	const std::vector<ID3D11ShaderResourceView*>& GetShaderResourceViews() const;

	// Get Dimension
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	int mWidth;
	int mHeight;

public:
	// pos, norm, diff, spec+shineness
	std::vector<ID3D11Texture2D*> m_renderTargetTextures;
	std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
	std::vector<ID3D11ShaderResourceView*> m_shaderResourceViews;

	friend class Shader;
};
