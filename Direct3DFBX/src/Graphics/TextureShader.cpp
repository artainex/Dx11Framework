#include "TextureShader.h"
#include <FileSystem.h>
#include <AnimationDef.h>

TextureShader::TextureShader()
	:
	m_vertexShader(0), m_pixelShader(0),
	m_layout(0), 
	m_matrixBuffer(0),
	m_sampleState(0)
{
	for (UINT i = 0; i < 4; ++i)
		m_lightBuffer[i] = nullptr;
}

TextureShader::TextureShader(const TextureShader& other)
{
}

TextureShader::~TextureShader()
{
}

bool TextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	
	std::string vsshaderName = "Shader/TextureShader/TextureVertexShader.hlsl";
	std::string psshaderName = "Shader/TextureShader/TexturePixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void TextureShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	FAIL_CHECK_WITH_MSG(result, "TextureShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, 
	int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ursine::Light* ambiLight,
	ursine::Light* gloLight,
	ursine::Light* locLight,
	ID3D11ShaderResourceView** textures)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, 
		worldMatrix, viewMatrix, projectionMatrix, 
		ambiLight, gloLight, locLight,
		textures);
	FAIL_CHECK_WITH_MSG(result, "TextureShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool TextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename)
{
	HRESULT hr = S_OK;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_BUFFER_DESC matrixBufferDesc;
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
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "TextureVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "TexturePixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	LAYOUT input_layout;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(input_layout.LAYOUT_TEX, 2,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	FAIL_CHECK_BOOLEAN(hr);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	FAIL_CHECK_BOOLEAN(hr);

	// Create Buffer - For light
	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	for (UINT i = 0; i < 4; ++i)
	{
		hr = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer[i]);
		FAIL_CHECK(hr);
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	FAIL_CHECK_BOOLEAN(hr);

	return true;
}

void TextureShader::ShutdownShader()
{
	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);

	// Release the matrix constant buffer.
	SAFE_RELEASE(m_matrixBuffer);

	// Release the matrix constant buffer.
	for (UINT i = 0; i < 4; ++i)
		SAFE_RELEASE(m_lightBuffer[i]);

	// Release the layout.
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);

	return;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	FAIL_CHECK_BOOLEAN(result);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->mWorld = worldMatrix;
	dataPtr->mView = viewMatrix;
	dataPtr->mProj = projectionMatrix;
	dataPtr->mWVP = worldMatrix * viewMatrix * projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ursine::Light* ambiLight,
	ursine::Light* gloLight,
	ursine::Light* locLight,
	ID3D11ShaderResourceView** textures)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 4 render target textures
	// Set shader texture resources in the pixel shader.
	deviceContext->PSSetShaderResources(0, 4, textures);

	// matrices
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		// Get a pointer to the data in the constant buffer.
		dataPtr = (MatrixBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->mWorld = worldMatrix;
		dataPtr->mView = viewMatrix;
		dataPtr->mProj = projectionMatrix;
		dataPtr->mWVP = worldMatrix * viewMatrix * projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Set the position of the constant buffer in the vertex shader.
		bufferNumber = 0;

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	// light
	{
		// ambient light
		{
			// Lock the constant buffer so it can be written to.
			result = deviceContext->Map(m_lightBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			FAIL_CHECK_BOOLEAN(result);

			// Get a pointer to the data in the constant buffer.
			LightBufferType* lightBuffer = (LightBufferType*)mappedResource.pData;

			// Copy the matrices into the constant buffer.
			lightBuffer->lightPosition = ambiLight->GetPosition();
			lightBuffer->ambientColor = ambiLight->GetAmbientColor();
			lightBuffer->diffuseColor = ambiLight->GetDiffuseColor();
			lightBuffer->specularColor = ambiLight->GetSpecularColor();
			lightBuffer->lightDirection = ambiLight->GetDirection();

			// Unlock the constant buffer.
			deviceContext->Unmap(m_lightBuffer[0], 0);

			// Set the position of the constant buffer in the pixel shader.
			bufferNumber = 1;

			deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer[0]);		// setting lights
		}
		// global light
		{
			// Lock the constant buffer so it can be written to.
			result = deviceContext->Map(m_lightBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			FAIL_CHECK_BOOLEAN(result);

			// Get a pointer to the data in the constant buffer.
			LightBufferType* lightBuffer = (LightBufferType*)mappedResource.pData;

			// Copy the matrices into the constant buffer.
			lightBuffer->lightPosition = gloLight->GetPosition();
			lightBuffer->ambientColor = gloLight->GetAmbientColor();
			lightBuffer->diffuseColor = gloLight->GetDiffuseColor();
			lightBuffer->specularColor = gloLight->GetSpecularColor();
			lightBuffer->lightDirection = gloLight->GetDirection();

			// Unlock the constant buffer.
			deviceContext->Unmap(m_lightBuffer[1], 0);

			// Set the position of the constant buffer in the pixel shader.
			bufferNumber = 2;

			deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer[1]);		// setting lights
		}
		// local lights
		{
			// Lock the constant buffer so it can be written to.
			result = deviceContext->Map(m_lightBuffer[2], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			FAIL_CHECK_BOOLEAN(result);

			// Get a pointer to the data in the constant buffer.
			LightBufferType* lightBuffer = (LightBufferType*)mappedResource.pData;

			// Copy the matrices into the constant buffer.
			lightBuffer->lightPosition = locLight->GetPosition();
			lightBuffer->ambientColor = locLight->GetAmbientColor();
			lightBuffer->diffuseColor = locLight->GetDiffuseColor();
			lightBuffer->specularColor = locLight->GetSpecularColor();
			lightBuffer->lightDirection = locLight->GetDirection();

			// Unlock the constant buffer.
			deviceContext->Unmap(m_lightBuffer[2], 0);

			// Set the position of the constant buffer in the pixel shader.
			bufferNumber = 3;

			deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer[2]);		// setting lights
		}
	}

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
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
