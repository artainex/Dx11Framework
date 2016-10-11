#pragma once

#include <Utilities.h>

using namespace DirectX;

class PointShader
{
private:
	struct PrimitiveVertex
	{
		DirectX::XMFLOAT4 Pos;
		DirectX::XMFLOAT4 Color;
		float size;
	};

public:
	PointShader();
	PointShader(const PointShader&);
	~PointShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	int GetIndexCount();

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename);
	void ShutdownShader();	
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext*, int);

	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, XMFLOAT3 position);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	XMFLOAT3 m_previousPos;
	int m_vertexCount, m_indexCount;
	float m_size;
};