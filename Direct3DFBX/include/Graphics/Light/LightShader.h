#pragma once

//////////////
// INCLUDES //
//////////////
#include <Utilities.h>
#include <ConstantBuffers.h>
#include <Light.h>
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
		XMMATRIX lightviewMatrix, XMMATRIX lightprojectionMatrix,
		const std::vector<ID3D11ShaderResourceView*>& textures,
		ID3D11ShaderResourceView* depthtexture,
		const ursine::Light& light,
		const XMFLOAT3& eyePos);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename);
	void ShutdownShader();

	//bool SetTexture(ShaderType shaderType, u32 texNum, const MultiRenderTarget * mrt, u32 rtIndex);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		XMMATRIX lightviewMatrix, XMMATRIX lightprojectionMatrix,
		const std::vector<ID3D11ShaderResourceView*>& textures,
		ID3D11ShaderResourceView* depthtexture,
		const ursine::Light& light,
		const XMFLOAT3& eyePos);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11SamplerState* m_sampleState;
};
