#pragma once

#include <Utilities.h>
#include <Light.h>

using namespace DirectX;

class SceneRenderer
{
private:

public:
	SceneRenderer();
	SceneRenderer(const SceneRenderer&);
	~SceneRenderer();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		const ursine::Light& light,
		const ID3D11ShaderResourceView* light_texture,
		const std::vector<ID3D11ShaderResourceView*>& textures);
	void RenderMode(ID3D11DeviceContext* deviceContext, int rendertype);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename);
	void ShutdownShader();	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		const std::vector<ID3D11ShaderResourceView*>& texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_rendermodeBuffer;
	ID3D11SamplerState* m_sampleState;
	eRenderType m_renderType;
};