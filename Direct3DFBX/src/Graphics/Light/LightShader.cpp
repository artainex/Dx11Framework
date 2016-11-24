#pragma once

#include <LightShader.h>
#include <Constants.h>
#include <ConstantBuffers.h>

#pragma warning(disable : 4100)

LightShader::LightShader()
	:
	m_vertexShader(0), 
	m_pixelShader(0),
	m_layout(0), 
	m_matrixBuffer(0),
	m_lightBuffer(0),
	m_sampleState(0),
	m_sampleShadowState(0)
{
}

LightShader::LightShader(const LightShader& other)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	// Initialize the vertex and pixel shaders.
	std::string vsshaderName = "Shader/LightVertexShader.hlsl";
	std::string psshaderName = "Shader/LightPixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void LightShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	const std::vector<ID3D11ShaderResourceView*>& textures,
	ID3D11ShaderResourceView* depthtexture,
	const ursine::Light& light,
	const XMFLOAT3& eyePos)
{
	bool result;
	
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,	
		worldMatrix, viewMatrix, projectionMatrix, 
		textures, depthtexture, light, eyePos);
	FAIL_CHECK_WITH_MSG(result, "LightShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount, light.GetType());
	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename)
{
	HRESULT hr = S_OK;

	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;

	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc, expBufferDesc;
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
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "LightVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "LightPixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	LAYOUT input_layout;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(input_layout.LAYOUT_TEX, 2,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "LightShader input layout creation failed");

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);
	
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	BufferInitialize(matrixBufferDesc, sizeof(MatrixBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "LightShader matrix buffer creation failed");
	
	// Setup the description of the dynamic render type constant buffer that is in the vertex shader.
	BufferInitialize(lightBufferDesc, sizeof(LightBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "LightBufferType buffer creation fail");
	
	// Setup the description of the dynamic render type constant buffer that is in the vertex shader.
	BufferInitialize(expBufferDesc, sizeof(ExponentialBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&expBufferDesc, nullptr, &m_expBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "ExponentialBufferType buffer creation fail");

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

	// Create shadow sampler state
	SamplerInitialize(samplerDesc,
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_COMPARISON_GREATER,
		0, 
		D3D11_FLOAT32_MAX);

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.BorderColor[0] = 1.0;
	samplerDesc.BorderColor[1] = 1.0;
	samplerDesc.BorderColor[2] = 1.0;
	samplerDesc.BorderColor[3] = 1.0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampleShadowState);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "Creating Sampler Shadow State Failed");

	return true;
}

void LightShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	SAFE_RELEASE(m_matrixBuffer);

	// Release the light constant buffer.
	SAFE_RELEASE(m_lightBuffer);
	SAFE_RELEASE(m_expBuffer);

	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_sampleShadowState);

	// Release the layout.
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);

	return;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	const std::vector<ID3D11ShaderResourceView*>& textures,
	ID3D11ShaderResourceView* depthtexture,
	const ursine::Light& light,
	const XMFLOAT3& eyePos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX WVP = worldMatrix * viewMatrix * projectionMatrix;
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;
	// matrices
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->mWorld = worldMatrix;
		dataPtr->mView = viewMatrix;
		dataPtr->mProj = projectionMatrix;
		dataPtr->mWVP = XMMatrixTranspose(WVP);

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	bufferNumber = 0;
	// light 
	{
		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);
	
		LightBufferType* lightBuffer = (LightBufferType*)mappedResource.pData;

		lightBuffer->type = (UINT)light.GetType();
		lightBuffer->color = light.GetColor();
		lightBuffer->eyePosition = eyePos;
		lightBuffer->lightPosition = light.GetPosition();
		lightBuffer->lightRange = light.GetRange(); // fixed range
		lightBuffer->lView = XMMatrixTranspose(light.GetShadowView());
		lightBuffer->lProj = XMMatrixTranspose(light.GetShadowProjection());

		// unmap constant buffer
		deviceContext->Unmap(m_lightBuffer, 0);
	
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	}

	bufferNumber = 1;
	// exponential near, far, const
	{
		result = deviceContext->Map(m_expBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		ExponentialBufferType* expBuffer = (ExponentialBufferType*)mappedResource.pData;

		expBuffer->lightNearFarConst = XMFLOAT3(lightNear, lightFar, exponentialConst);

		// unmap constant buffer
		deviceContext->Unmap(m_expBuffer, 0);

		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_expBuffer);
	}

	bufferNumber = 0;
	// textures (pos, norm, diff, spec, depth)
	{
		for (UINT i = 0; i < textures.size(); ++i)
		{
			deviceContext->PSSetShaderResources(bufferNumber, 1, &textures[i]);
			++bufferNumber;
		}

		if (depthtexture)
			deviceContext->PSSetShaderResources(bufferNumber, 1, &depthtexture);
	}

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, const ursine::LightType& lightType)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleShadowState);	

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}