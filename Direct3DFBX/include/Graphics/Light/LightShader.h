#pragma once

//////////////
// INCLUDES //
//////////////
#include <Utilities.h>
#include <ConstantBuffers.h>
#include <fstream>

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShader
////////////////////////////////////////////////////////////////////////////////
class LightShader
{
private:

public:
	LightShader();
	LightShader(const LightShader& other);
	~LightShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename);
	void ShutdownShader();

	bool SetTexture(ShaderType shaderType, u32 texNum, const MultiRenderTarget * mrt, u32 rtIndex);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};
