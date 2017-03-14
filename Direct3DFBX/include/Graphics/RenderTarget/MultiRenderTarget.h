#pragma once

#include <Utilities.h>

class MultiRenderTarget
{
public:
	MultiRenderTarget();
	MultiRenderTarget(const MultiRenderTarget&);
	~MultiRenderTarget();

	bool Initialize(int, int);
	void Shutdown();

	void SetRenderTargets();
	void ReleaseRenderTargets(eShaderType shadertype);
	void ClearRenderTargets();

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
