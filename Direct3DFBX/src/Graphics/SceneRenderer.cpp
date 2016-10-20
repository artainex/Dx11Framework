#include "SceneRenderer.h"
#include <FileSystem.h>
#include <AnimationDef.h>

#pragma warning (disable : 4101)

SceneRenderer::SceneRenderer()
	:
	m_vertexShader(0), 
	m_pixelShader(0),
	m_layout(0), 
	m_matrixBuffer(0),
	m_rendermodeBuffer(0),
	m_sampleState(0),
	m_renderType(POSITION)
{
}

SceneRenderer::SceneRenderer(const SceneRenderer& other)
{
}

SceneRenderer::~SceneRenderer()
{
}

bool SceneRenderer::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	
	std::string vsshaderName = "Shader/SceneVertexShader.hlsl";
	std::string psshaderName = "Shader/ScenePixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void SceneRenderer::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool SceneRenderer::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	FAIL_CHECK_WITH_MSG(result, "SceneRenderer Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool SceneRenderer::Render(ID3D11DeviceContext* deviceContext, 
	int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	const std::vector<ID3D11ShaderResourceView*>& textures)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, 
		worldMatrix, viewMatrix, projectionMatrix,
		textures);
	
	FAIL_CHECK_WITH_MSG(result, "SceneRenderer Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool SceneRenderer::InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename)
{
	HRESULT hr = S_OK;

	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;

	D3D11_BUFFER_DESC matrixBufferDesc, rendermodeBufferDesc, lightBufferDesc;
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
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "SceneVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "ScenePixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	LAYOUT input_layout;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(input_layout.LAYOUT_TEX, 2,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "SceneRenderer input layout creation fail");

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
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "MatrixBufferType buffer creation fail");

	// Setup the description of the dynamic render type constant buffer that is in the vertex shader.
	BufferInitialize(rendermodeBufferDesc, sizeof(RenderBufferType),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE);

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = device->CreateBuffer(&rendermodeBufferDesc, nullptr, &m_rendermodeBuffer);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "RenderBufferType buffer creation fail");

	SamplerInitialize(samplerDesc,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS,
		0,
		D3D11_FLOAT32_MAX);

	// Create a texture sampler state description.
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

void SceneRenderer::ShutdownShader()
{
	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);

	// Release the matrix constant buffer.
	SAFE_RELEASE(m_matrixBuffer);

	// Release the render mode constant buffer.
	SAFE_RELEASE(m_rendermodeBuffer);

	// Release the layout.
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);

	return;
}

bool SceneRenderer::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;	
	
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	unsigned int bufferNumber = 0;
	// Set the position of the constant buffer in the vertex shader.
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
		dataPtr->mWVP = worldMatrix * viewMatrix * projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool SceneRenderer::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	const std::vector<ID3D11ShaderResourceView*>& textures)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;	

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	unsigned int srNumber = 0;
	// render target textures
	// Set shader texture resources in the pixel shader.
	for (UINT i = 0; i < textures.size(); ++i, ++srNumber)
	{
		deviceContext->PSSetShaderResources(srNumber, 1, &textures[i]);
	}

	unsigned int bufferNumber = 0;
	// Set the position of the constant buffer in the vertex shader.
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
		dataPtr->mWVP = worldMatrix * viewMatrix * projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	bufferNumber = 1;
	// render mode
	{
		result = deviceContext->Map(m_rendermodeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FAIL_CHECK_BOOLEAN(result);

		RenderBufferType* rmBuffer = (RenderBufferType*)mappedResource.pData;

		rmBuffer->type = m_renderType;

		// unmap constant buffer
		deviceContext->Unmap(m_rendermodeBuffer, 0);

		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_rendermodeBuffer);
	}

	return true;
}

void SceneRenderer::RenderMode(ID3D11DeviceContext* deviceContext, int renderType)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	deviceContext->Map(m_rendermodeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	RenderBufferType* rmBuffer = (RenderBufferType*)MappedResource.pData;

	switch (renderType)
	{
	case 1: m_renderType = POSITION;	break;
	case 2:	m_renderType = NORMAL;		break;
	case 3:	m_renderType = DIFFUSE;		break;
	case 4:	m_renderType = SPECULAR;	break;
	case 5:	m_renderType = DEPTH;		break;
	}
	
	rmBuffer->type = m_renderType;

	// unmap constant buffer
	deviceContext->Unmap(m_rendermodeBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &m_rendermodeBuffer);
}

void SceneRenderer::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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
