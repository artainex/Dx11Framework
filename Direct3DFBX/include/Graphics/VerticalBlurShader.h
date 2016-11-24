#pragma once

//////////////
// INCLUDES //
//////////////
#include <ExponentialProperty.h>

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: DepthShader
////////////////////////////////////////////////////////////////////////////////
class VerticalBlurShader
{
private:

public:
	VerticalBlurShader();
	VerticalBlurShader(const VerticalBlurShader& other);
	~VerticalBlurShader();

	bool Initialize(ID3D11Device* device, HWND hwnd, const D3D_SHADER_MACRO* macros);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount);

	ID3D11ComputeShader* GetComputeShader() const { return m_computeShader; }
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMINT3 resolution);

	void SetBlurKernelRadius(int radius);
	int GetBlurKernelRadius() const { return mBlurKernelRadius; }
	void BuildWeights();
	void RenderShader(ID3D11DeviceContext* deviceContext);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, string csFilename, const D3D_SHADER_MACRO* macros);
	void ShutdownShader();

private:
	ID3D11ComputeShader* m_computeShader;
	ID3D11Buffer* m_ResolutionBuffer;
	ID3D11Buffer* m_WeightBuffer;
	ID3D11SamplerState* m_sampleState;
	XMFLOAT4 mWeights[2 * MAX_KERNEL_RADIUS + 1];	// Blur weights
	int mBlurKernelRadius;							// Blur Kernel radius
};
