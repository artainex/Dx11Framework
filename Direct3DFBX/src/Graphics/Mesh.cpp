#include "Mesh.h"

Mesh::Mesh()
	: mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mLine(false)
	, mVertexCount(0)
	, mIndexCount(0)
{
}

Mesh::~Mesh()
{
}

void Mesh::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void Mesh::BindBuffers() const
{
	// Set vertex buffer stride and offset.
	unsigned int stride = mStride;
	unsigned int offset = 0;

	// Using this one context for now
	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(mLine ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::ShutdownBuffers()
{
	// Release the index buffer.
	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = nullptr;
	}
}

unsigned int Mesh::GetIndexCount() const
{
	return mIndexCount;
}