#include <PointShader.h>
#include <FileSystem.h>
#include <AnimationDef.h>

PointShader::PointShader()
	:
	m_vertexShader(0), m_pixelShader(0),
	m_layout(0), m_matrixBuffer(0),
	m_sampleState(0)
{
}

PointShader::PointShader(const PointShader& other)
{
}

PointShader::~PointShader()
{
}

bool PointShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	
	std::string vsshaderName = "Shader/PointShader/PointVertexShader.hlsl";
	std::string psshaderName = "Shader/PointShader/PointPixelShader.hlsl";
	result = InitializeShader(device, hwnd, vsshaderName, psshaderName);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

void PointShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

bool PointShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	FAIL_CHECK_WITH_MSG(result, "PointShader Render Fail");

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);
	return true;
}

int PointShader::GetIndexCount()
{
	return 6;
}

bool PointShader::InitializeShader(ID3D11Device* device, HWND hwnd, std::string vsFilename, std::string psFilename)
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
	CompileShaderFromFile(VERTEX_SHADER, vsFilename.c_str(), "PointVertexShader", "vs_5_0", &device, &vertexShaderBuffer, &m_vertexShader, nullptr);
	CompileShaderFromFile(PIXEL_SHADER, psFilename.c_str(), "PointPixelShader", "ps_5_0", &device, &pixelShaderBuffer, nullptr, &m_pixelShader);

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

void PointShader::ShutdownShader()
{
	// Release the sampler state.
	SAFE_RELEASE(m_sampleState);

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

bool PointShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
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

	return true;
}

void PointShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

bool PointShader::InitializeBuffers(ID3D11Device* device)
{
	VERTEX_DATA_LT* vertices;
	unsigned long* indices;
	HRESULT result;
	int i;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 36;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VERTEX_DATA_LT[m_vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VERTEX_DATA_LT) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i < m_indexCount; ++i)
		indices[i] = i;

	size_t stride = sizeof(VERTEX_DATA_LT);

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = stride * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	FAIL_CHECK_BOOLEAN(result);

	// Set up the description of the static index buffer.
	stride = sizeof(UINT);
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = static_cast<uint32_t>(stride * m_indexCount);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	FAIL_CHECK_BOOLEAN(result);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	return true;
}

void PointShader::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);
	return;
}

bool PointShader::UpdateBuffers(ID3D11DeviceContext* deviceContext, XMFLOAT3 position)
{
	VERTEX_DATA_LT* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VERTEX_DATA_LT* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ( (position.x == m_previousPos.x) &&
		(position.y == m_previousPos.y) &&
		(position.z == m_previousPos.z) )
		return true;
	
	//// front
	//{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,255), 1.0f, 0.0f },
	//// back
	//{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 0.0f, 1.0 },
	//{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,255,  255,255), 1.0f, 0.0f },
	//// left
	//{ D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,  255), 1.0f, 0.0f },
	//	// ©Л
	//{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,  255,255), 1.0f, 0.0f },
	//	// ╩С
	//{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DCOLOR_ARGB(255,  255,255,255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DCOLOR_ARGB(255,  255,255,255), 1.0f, 0.0f },
	//	// го
	//{ D3DXVECTOR3(1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,  255), 1.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,255,  255), 1.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,255,  255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DCOLOR_ARGB(255,255,255,  255), 0.0f, 1.0f },
	//{ D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,  255), 0.0f, 0.0f },
	//{ D3DXVECTOR3(1.0f,-1.0f,-1.0f), D3DCOLOR_ARGB(255,255,255,  255), 1.0f, 0.0f }
	//
	//// If it has changed then update the position it is being rendered to.
	//m_previousPos = position;
	//
	//// Calculate the screen coordinates of the left side of the bitmap.
	//left = m_size * -1 + (float)position.x;
	//
	//// Calculate the screen coordinates of the right side of the bitmap.
	//right = m_size * 1 + (float)position.x;
	//
	//// Calculate the screen coordinates of the top of the bitmap.
	//top = (float)(m_screenHeight / 2) - (float)position.y;
	//
	//// Calculate the screen coordinates of the bottom of the bitmap.
	//bottom = top - (float)m_bitmapHeight;
	//
	//// Create the vertex array.
	//vertices = new VERTEX_DATA_LT[m_vertexCount];
	//if (!vertices)
	//	return false;
	//
	//// Load the vertex array with data.
	//// First triangle.
	//// Top left
	//vertices[0].vPos = XMFLOAT3(left, top, 0.0f);
	//vertices[0].vTexcoord = XMFLOAT2(0.0f, 0.0f);
	//
	//// Bottom right.
	//vertices[1].vPos = XMFLOAT3(right, bottom, 0.0f);
	//vertices[1].vTexcoord = XMFLOAT2(1.0f, 1.0f);
	//
	//// Bottom left.
	//vertices[2].vPos = XMFLOAT3(left, bottom, 0.0f);
	//vertices[2].vTexcoord = XMFLOAT2(0.0f, 1.0f);
	//
	//// Second triangle.
	//// Top left.
	//vertices[3].vPos = XMFLOAT3(left, top, 0.0f);
	//vertices[3].vTexcoord = XMFLOAT2(0.0f, 0.0f);
	//
	//// Top right.
	//vertices[4].vPos = XMFLOAT3(right, top, 0.0f);
	//vertices[4].vTexcoord = XMFLOAT2(1.0f, 0.0f);
	//
	//// Bottom right.
	//vertices[5].vPos = XMFLOAT3(right, bottom, 0.0f);
	//vertices[5].vTexcoord = XMFLOAT2(1.0f, 1.0f);
	//
	//// Lock the vertex buffer so it can be written to.
	//result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//FAIL_CHECK_BOOLEAN(result);
	//
	//// Get a pointer to the data in the vertex buffer.
	//verticesPtr = (VERTEX_DATA_LT*)mappedResource.pData;
	//
	//// Copy the data into the vertex buffer.
	//memcpy(verticesPtr, (void*)vertices, sizeof(VERTEX_DATA_LT) * m_vertexCount);
	//
	//// Unlock the vertex buffer.
	//deviceContext->Unmap(m_vertexBuffer, 0);
	//
	//// Release the vertex array as it is no longer needed.
	//SAFE_DELETE_ARRAY(vertices);

	return true;
}

void PointShader::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VERTEX_DATA_LT);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}