#pragma once

#include <Utilities.h>
#include <Light.h>

using namespace DirectX;
typedef ursine::Color urColor;

// all constant buffers' size should be multiple of 16

// Matrix Constant Buffer
struct MatrixBufferType
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProj;
	XMMATRIX mWVP;
};

// Matrix Palette Constant Buffer
struct PaletteBufferType
{
	XMMATRIX matPal[96];
};

// Light Constant Buffer
struct LightBufferType
{
	// For now, try use phong model, use ursine LightClass if I understand HDR or more (this class doesn't have HDR)
	XMFLOAT4 color;			// 16
	XMFLOAT3 eyePosition;	// 28
	XMFLOAT3 lightPosition; // 40
	float lightRange;		// 44
	UINT type;				// 48

	LightBufferType()
		:
		color(0, 0, 0, 1),
		eyePosition(0, 0, 0),
		lightPosition(0, 0, 0),
		lightRange(1000.f),
		type(ursine::LIGHT_DIRECTIONAL)
	{}
};

struct MeshBufferType
{
	XMFLOAT4	color;			//16

	MeshBufferType() :
		color(XMFLOAT4(1, 1, 1, 1))
	{}
};

struct RenderBufferType
{
	UINT	type;				// 4
	XMFLOAT3 padding;			// 16

	RenderBufferType() 
		: type(0)
	{}
};

// Material Constant Buffer
struct MaterialBufferType
{
	XMFLOAT4	ambient;		// 16
	XMFLOAT4	diffuse;		// 32
	XMFLOAT4	specular;		// 48
	XMFLOAT4	emissive;		// 64
	float		transparency;	// 68
	float		shineness;		// 72
	XMFLOAT2	padding;		// 80

	MaterialBufferType() : 
		ambient(XMFLOAT4(1, 1, 1, 1)),
		diffuse(XMFLOAT4(1, 1, 1, 1)),
		specular(XMFLOAT4(1, 1, 1, 1)),
		emissive(XMFLOAT4(1, 1, 1, 1)),
		transparency(1.0f),
		shineness(0.1f),
		padding(XMFLOAT2(0, 0))
	{}
};
