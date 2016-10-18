#pragma once
#include <Utilities.h>

class RenderTarget
{
public:
	RenderTarget();
	RenderTarget(const RenderTarget& other);
	~RenderTarget();

	bool Initialize(ID3D11Device* device, int width, int height);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, const float* color);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float r, float g, float b, float a);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	// pos, norm, diff, spec+shineness

public:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	friend class Shader;
};
