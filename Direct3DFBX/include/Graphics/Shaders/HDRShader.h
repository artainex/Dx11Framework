#pragma once

//////////////
// INCLUDES //
//////////////
#include <HDRTexture.h>
#include <Utilities.h>
#include <fstream>

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: HDRShader
////////////////////////////////////////////////////////////////////////////////
class HDRShader
{
private:

public:
	HDRShader();
	HDRShader(const HDRShader& other);
	~HDRShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projectionMatrix,
		const int& indexCount,
		const std::vector<ID3D11ShaderResourceView*>& textures,
		const HDRTexture& skyTexture,
		const HDRTexture& irradiance,
		const XMFLOAT3& eyePos);
	bool SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projectionMatrix,
		const int& indexCount,
		const std::vector<ID3D11ShaderResourceView*>& textures,
		const HDRTexture& skyTexture,
		const HDRTexture& irradiance,
		const XMFLOAT3& eyePos);
	ID3D11VertexShader* GetVertexShader() const { return m_vertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return m_pixelShader; }
	ID3D11InputLayout* GetLayout() const { return m_layout; }

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename);
	void ShutdownShader();
	void RenderShader(ID3D11DeviceContext* deviceContext, const int& indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_hdrBuffer;
	ID3D11SamplerState* m_sampleState;
};