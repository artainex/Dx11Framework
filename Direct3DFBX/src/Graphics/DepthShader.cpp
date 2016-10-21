#pragma once

#include <DepthShader.h>

#pragma warning(disable : 4100)

DepthShader::DepthShader()
	:
	m_vertexShader(0), 
	m_pixelShader(0),
	m_layout(0), 
	m_matrixBuffer(0)
{
}

DepthShader::DepthShader(const DepthShader& other)
{
}

DepthShader::~DepthShader()
{
}

bool DepthShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	// Initialize the vertex and pixel shaders.
	std::string vsshaderName = "Shader/DepthVertexShader.hlsl";
	std::string psshaderName = "Shader/DepthPixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void DepthShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool DepthShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;
	
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,	
		worldMatrix, viewMatrix, projectionMatrix);
	FAIL_CHECK_WITH_MSG(result, "DepthShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool DepthShader::InitializeShader(ID3D11Device* device, HWND hwnd, string vsFilename, string psFilename)
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
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "DepthVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "DepthPixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	LAYOUT input_layout;

	// Create the vertex input layout.
	hr = device->CreateInputLayout(input_layout.LAYOUT_TEX, 2,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "DepthShader input layout creation failed");

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
	FAIL_CHECK_BOOLEAN_WITH_MSG(hr, "DepthShader matrix buffer creation failed");

	return true;
}

void DepthShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	SAFE_RELEASE(m_matrixBuffer);
	
	// Release the layout.
	SAFE_RELEASE(m_layout);

	// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);

	return;
}

bool DepthShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
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

	return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	
	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}