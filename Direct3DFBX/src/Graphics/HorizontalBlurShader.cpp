#pragma once

#include <HorizontalBlurShader.h>

#pragma warning(disable : 4100)

HorizontalBlurShader::HorizontalBlurShader()
	:
	m_computeShader(0),
	m_sampleState(0),
	mBlurKernelRadius(50)
{
}

HorizontalBlurShader::HorizontalBlurShader(const HorizontalBlurShader& other)
{
}

HorizontalBlurShader::~HorizontalBlurShader()
{
}

bool HorizontalBlurShader::Initialize(ID3D11Device* device, HWND hwnd, const D3D_SHADER_MACRO* macros)
{
	bool result;
	// Initialize the vertex and pixel shaders.
	std::string csshaderName = "Shader/HorizontalBlurShader.hlsl";
	result = InitializeShader(device, hwnd, csshaderName, macros);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void HorizontalBlurShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool HorizontalBlurShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	bool result;
	
	// Set the shader parameters that it will use for rendering.
	FAIL_CHECK_WITH_MSG(result, "HorizontalBlurShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext);
	return true;
}

bool HorizontalBlurShader::InitializeShader(ID3D11Device* device, HWND hwnd, string csFilename, const D3D_SHADER_MACRO* macros)
{
	HRESULT hr = S_OK;

	ID3DBlob* errorMessage;
	ID3DBlob* computeShaderBuffer;

	D3D11_BUFFER_DESC resolBufferDesc, weightBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	computeShaderBuffer = 0;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// Create the vertex / pixel shader from the buffer.
	CompileShaderFromFile(COMPUTE_SHADER, csFilename.c_str(), "HorizontalBlurShader", "cs_5_0", &device, &computeShaderBuffer, macros, &m_computeShader);
	
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HorizontalBlurShader input layout creation failed");

	// Release the compute shader buffer since they are no longer needed.
	SAFE_RELEASE(computeShaderBuffer);
	
	// Setup the description of the resolution constant buffer that is in the vertex shader.
	BufferInitialize(resolBufferDesc, sizeof(ResolutionBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);
	
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&resolBufferDesc, nullptr, &m_ResolutionBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HorizontalBlurShader resolution buffer creation failed");

	// Setup the description of the resolution constant buffer that is in the vertex shader.
	BufferInitialize(weightBufferDesc, sizeof(WeightBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&weightBufferDesc, nullptr, &m_WeightBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HorizontalBlurShader resolution buffer creation failed");

	// Create a texture sampler state description.
	SamplerInitialize(samplerDesc,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS,
		0,
		D3D11_FLOAT32_MAX);

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "Sampleer state creation fail");

	return true;
}

void HorizontalBlurShader::ShutdownShader()
{
	// Release the compute shader.
	SAFE_RELEASE(m_computeShader);

	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);

	return;
}

bool HorizontalBlurShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMINT3 resolution)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;
	// resolution
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_ResolutionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		ResolutionBufferType* dataPtr = (ResolutionBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->resolution = resolution;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_ResolutionBuffer, 0);
	
		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->CSSetConstantBuffers(bufferNumber, 1, &m_ResolutionBuffer);
	}

	bufferNumber = 1;

	// weight
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_WeightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		WeightBufferType* dataPtr = (WeightBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		memcpy(dataPtr->weights, mWeights, sizeof(XMFLOAT4) * 2 * MAX_KERNEL_RADIUS + 1);

		// Unlock the constant buffer.
		deviceContext->Unmap(m_WeightBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->CSSetConstantBuffers(bufferNumber, 1, &m_WeightBuffer);
	}

	return true;
}

void HorizontalBlurShader::SetBlurKernelRadius(int radius)
{
	mBlurKernelRadius = Clamp(mBlurKernelRadius + radius, 0, MAX_KERNEL_RADIUS);
	BuildWeights();
}

void HorizontalBlurShader::BuildWeights()
{
	int kernalWidth = 2 * mBlurKernelRadius + 1;

	float variance = 8.f;
	float covariance = variance * variance;
	float sum = 0.f;
	for (int i = 0; i < kernalWidth; ++i)
	{
		float x = static_cast<float>(i) - static_cast<float>(mBlurKernelRadius);
		mWeights[i].x = std::exp(-((x * x) / (2.f * covariance)));
		sum += mWeights[i].x;
	}

	for (int i = 0; i < kernalWidth; ++i)
	{
		mWeights[i].x /= sum;
	}
}

void HorizontalBlurShader::RenderShader(ID3D11DeviceContext* deviceContext)
{
	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->CSSetShader(m_computeShader, nullptr, 0);

	deviceContext->CSSetSamplers(0, 1, &m_sampleState);

	return;
}
