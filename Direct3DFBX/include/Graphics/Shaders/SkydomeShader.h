#pragma once

//////////////
// INCLUDES //
//////////////
#include <Utilities.h>
#include <ConstantBuffers.h>

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: SkydomeShader
////////////////////////////////////////////////////////////////////////////////
class SkydomeShader
{
private:

public:
	SkydomeShader();
	SkydomeShader(const SkydomeShader& other);
	~SkydomeShader();

	bool Initialize(ID3D11Device* device, HWND hwnd,
		const XMFLOAT4& apexColor, const XMFLOAT4& centerColor);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, 
		const int& indexCount,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix, 
		const XMMATRIX& projectionMatrix);

	ID3D11VertexShader* GetVertexShader() const { return m_vertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return m_pixelShader; }
	ID3D11InputLayout* GetDepthLayout() const { return m_layout; }
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const XMMATRIX& worldMatrix, 
		const XMMATRIX& viewMatrix, 
		const XMMATRIX& projectionMatrix);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename);
	void ShutdownShader();

	//bool SetTexture(ShaderType shaderType, unsigned int texNum, const MultiRenderTarget * mrt, unsigned int rtIndex);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_gradientBuffer;
	XMFLOAT4 m_apexColor, m_centerColor;
	std::vector<float> image;
};
