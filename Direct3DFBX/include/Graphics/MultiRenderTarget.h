#pragma once
#include <Utilities.h>

class MultiRenderTarget
{
public:
	MultiRenderTarget();
	MultiRenderTarget(const MultiRenderTarget&);
	~MultiRenderTarget();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, const float* color);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float r, float g, float b, float a);
	ID3D11ShaderResourceView* GetShaderResourceView(int index);
	ID3D11ShaderResourceView** GetShaderResourceViews();

private:
	// pos, norm, diff, spec+shineness
	ID3D11Texture2D* m_renderTargetTexture[RT_COUNT];
	ID3D11RenderTargetView* m_renderTargetView[RT_COUNT];
	ID3D11ShaderResourceView* m_shaderResourceView[RT_COUNT];
};
