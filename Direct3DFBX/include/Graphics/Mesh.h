#pragma once

#include <d3d11.h>
#include <vector>
#include <Graphics.h>

// @todo: For graphics programmer to expand
class Mesh
{
public:
	// Constructor
	Mesh();

	// Destructor
	~Mesh();

	// Shutdown
	void Shutdown();

public:
	// Initialise Buffers
	template< typename VertexType >
	bool InitializeBuffers(const std::vector<VertexType> & vertexBuffer,
		const std::vector<unsigned int> & indexBuffer,
		bool line = false);

	// Bind Buffers
	void BindBuffers() const;

	// Shutdown Buffers
	void ShutdownBuffers();

	// Get Index count
	unsigned int GetIndexCount() const;

private:
	ID3D11Buffer * mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;
	bool mLine;

	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mStride;		//!< Vertex Buffer stride

	friend class Graphics;
};

#include "Mesh.inl"