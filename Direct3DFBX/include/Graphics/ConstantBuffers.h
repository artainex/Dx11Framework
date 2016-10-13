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
	XMFLOAT4 ambientColor;  // 16
	XMFLOAT4 diffuseColor;  // 32
	XMFLOAT4 specularColor; // 48
	XMFLOAT4 lightPosition; // 64
	XMFLOAT3 lightDirection;// 76
	UINT type;				// 80

	LightBufferType()
		:
		ambientColor(0, 0, 0, 1),
		diffuseColor(0, 0, 0, 1),
		specularColor(0, 0, 0, 1),
		lightPosition(0, 0, 0, 1),
		lightDirection(0, 0, 0),
		type(ursine::LIGHT_DIRECTIONAL)
	{}
};

struct LocalLightBufferType
{
	LightBufferType localLights[MAX_LIGHT];
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
