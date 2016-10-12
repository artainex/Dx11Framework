#include <DebugWindow.h>
#include <AnimationDef.h>

#pragma warning (diable : 4101)

DebugWindow::DebugWindow()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

DebugWindow::DebugWindow(const DebugWindow& other)
{
}

DebugWindow::~DebugWindow()
{
}

bool DebugWindow::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void DebugWindow::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
	return;
}

bool DebugWindow::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;
	
	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
		return false;

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

int DebugWindow::GetIndexCount()
{
	return m_indexCount;
}

bool DebugWindow::InitializeBuffers(ID3D11Device* device)
{
	VERTEX_DATA_LT* vertices;
	unsigned long* indices;
	HRESULT result;
	int i;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

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

void DebugWindow::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

	// Release the vertex buffer.
	SAFE_RELEASE(m_vertexBuffer);
	return;
}

bool DebugWindow::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VERTEX_DATA_LT* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VERTEX_DATA_LT* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
		return true;

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VERTEX_DATA_LT[m_vertexCount];
	if (!vertices)
		return false;
	
	// Load the vertex array with data.
	// First triangle.
	// Top left
	vertices[0].vPos		= XMFLOAT3(left, top, 0.0f);
	vertices[0].vTexcoord	= XMFLOAT2(0.0f, 0.0f);

	// Bottom right.
	vertices[1].vPos		= XMFLOAT3(right, bottom, 0.0f);
	vertices[1].vTexcoord	= XMFLOAT2(1.0f, 1.0f);

	// Bottom left.
	vertices[2].vPos		= XMFLOAT3(left, bottom, 0.0f);
	vertices[2].vTexcoord	= XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	// Top left.
	vertices[3].vPos		= XMFLOAT3(left, top, 0.0f);
	vertices[3].vTexcoord	= XMFLOAT2(0.0f, 0.0f);

	// Top right.
	vertices[4].vPos		= XMFLOAT3(right, top, 0.0f);
	vertices[4].vTexcoord	= XMFLOAT2(1.0f, 0.0f);

	// Bottom right.
	vertices[5].vPos		= XMFLOAT3(right, bottom, 0.0f);
	vertices[5].vTexcoord	= XMFLOAT2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	FAIL_CHECK_BOOLEAN(result);

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VERTEX_DATA_LT*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, sizeof(VERTEX_DATA_LT) * m_vertexCount);

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	SAFE_DELETE_ARRAY(vertices);

	return true;
}

void DebugWindow::RenderBuffers(ID3D11DeviceContext* deviceContext)
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