#pragma once

#include <Utilities.h>
#include <Light.h>

using namespace DirectX;

class TextureShader
{
private:

public:
	TextureShader();
	TextureShader(const TextureShader&);
	~TextureShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ursine::Light* light,
		ID3D11ShaderResourceView** textures);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename);
	void ShutdownShader();	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ursine::Light* light,
		ID3D11ShaderResourceView** texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11SamplerState* m_sampleState;
};