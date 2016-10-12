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
	bool	bPos;
	bool	bNor;
	bool	bDiff;
	bool	bSpec;
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
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT4 specularColor;
	XMFLOAT4 lightPosition;
	XMFLOAT3 lightDirection;
	UINT type;

	LightBufferType()
		:
		ambientColor(0, 0, 0, 1),
		diffuseColor(0, 0, 0, 1),
		specularColor(0, 0, 0, 1),
		lightPosition(0, 0, 0, 1),
		lightDirection(0, 0, 0)
	{}
};

// Material Constant Buffer
struct MaterialBufferType
{
	float		shineness;
	float		transparency;
	pseudodx::XMFLOAT2	padding;
	pseudodx::XMFLOAT3	ambient;
	pseudodx::XMFLOAT3	diffuse;
	pseudodx::XMFLOAT3	specular;
	pseudodx::XMFLOAT3	emissive;

	MaterialBufferType() : transparency(1.0f) {}
};
