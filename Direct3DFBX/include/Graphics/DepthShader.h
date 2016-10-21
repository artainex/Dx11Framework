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
// Class name: DepthShader
////////////////////////////////////////////////////////////////////////////////
class DepthShader
{
private:

public:
	DepthShader();
	DepthShader(const DepthShader& other);
	~DepthShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename);
	void ShutdownShader();

	//bool SetTexture(ShaderType shaderType, u32 texNum, const MultiRenderTarget * mrt, u32 rtIndex);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};
