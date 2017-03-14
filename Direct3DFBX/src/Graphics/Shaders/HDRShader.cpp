#pragma once

#include <HDRShader.h>
#include <ConstantBuffers.h>

#pragma warning(disable : 4100)
#pragma warning(disable : 4189)

HDRShader::HDRShader()
	:
	m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_sampleState(0)
{
}

HDRShader::HDRShader(const HDRShader& other)
{
}

HDRShader::~HDRShader()
{
}

bool HDRShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	// Initialize the vertex and pixel shaders.
	std::string vsshaderName = "Shader/HDRVertexShader.hlsl";
	std::string psshaderName = "Shader/HDRPixelShader.hlsl";
	bool result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void HDRShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool HDRShader::Render(ID3D11DeviceContext* deviceContext,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	const int& indexCount,
	const std::vector<ID3D11ShaderResourceView*>& textures,
	const HDRTexture& skyTexture,
	const HDRTexture& irradiance,
	const XMFLOAT3& eyePos)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		indexCount,
		textures,
		skyTexture,
		irradiance,
		eyePos);
	FAIL_CHECK_WITH_MSG(result, "HDRShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool HDRShader::InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename)
{
	HRESULT hr = S_OK;

	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;

	D3D11_BUFFER_DESC matrixBufferDesc, hdrBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// Create the vertex / pixel shader from the buffer.
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "HDRVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "HDRPixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	LAYOUT input_layout;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(input_layout.LAYOUT1, 3,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HDRShader input layout creation failed");

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	BufferInitialize(matrixBufferDesc, sizeof(MatrixBuffer),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HDRShader matrix buffer creation failed");

	// Create a texture sampler state description.
	SamplerInitialize(samplerDesc,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS,
		0,
		D3D11_FLOAT32_MAX);

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	BufferInitialize(hdrBufferDesc, sizeof(HDRBuffer),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	hr = device->CreateBuffer(&hdrBufferDesc, nullptr, &m_hdrBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "HDRShader matrix buffer creation failed");

	// Create a texture sampler state description.
	SamplerInitialize(samplerDesc,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
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

void HDRShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	SAFE_RELEASE(m_matrixBuffer);

	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);

	// Release the layout.
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);

	return;
}

bool HDRShader::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	const int& indexCount,
	const std::vector<ID3D11ShaderResourceView*>& textures,
	const HDRTexture& skyTexture,
	const HDRTexture& irradiance,
	const XMFLOAT3& eyePos)
{
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX WVP = worldMatrix * viewMatrix * projectionMatrix;
	XMMATRIX world = XMMatrixTranspose(worldMatrix);
	XMMATRIX view = XMMatrixTranspose(viewMatrix);
	XMMATRIX proj = XMMatrixTranspose(projectionMatrix);
	// Set the position of the constant buffer in the vertex shader.

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// matrices
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		MatrixBuffer* dataPtr = (MatrixBuffer*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->mWorld = world;
		dataPtr->mView = view;
		dataPtr->mProj = proj;
		dataPtr->mWVP = XMMatrixTranspose(WVP);

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	}

	int bufferNumber = 0;
	// hdr
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_hdrBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		HDRBuffer* dataPtr = (HDRBuffer*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->ambient = XMFLOAT4(1,1,1,1);
		dataPtr->eyePos = eyePos;
		dataPtr->skyDimension = skyTexture.GetTextureDimension();

		// Unlock the constant buffer.
		deviceContext->Unmap(m_hdrBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_hdrBuffer);
	}

	// textures (pos, norm, diff, spec, depth, modeltype(light or geo))
	{
		for (UINT i = 0; i < textures.size(); ++i)
		{
			deviceContext->PSSetShaderResources(bufferNumber, 1, &textures[i]);
			++bufferNumber;
		}

		if (skyTexture.mTextureView)
			deviceContext->PSSetShaderResources(bufferNumber++, 1, &skyTexture.mTextureView);
		if (irradiance.mTextureView)
			deviceContext->PSSetShaderResources(bufferNumber++, 1, &irradiance.mTextureView);
	}

	return true;
}

void HDRShader::RenderShader(ID3D11DeviceContext* deviceContext, const int& indexCount)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}