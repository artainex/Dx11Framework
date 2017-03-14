#pragma once

#include <Utilities.h>
#include <Color.h>

using namespace DirectX;
typedef ursine::Color urColor;

// all constant buffers' size should be multiple of 16

// Matrix Constant Buffer
struct MatrixBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProj;
	XMMATRIX mWVP;
};

// Gradient Constant Buffer
struct GradientBuffer
{
	XMFLOAT4 apexColor;
	XMFLOAT4 centerColor;
};

// Matrix Palette Constant Buffer
struct PaletteBuffer
{
	XMMATRIX matPal[96];
};

// Ambient Constant Buffer
struct AmbientBuffer
{
	XMFLOAT4 ambient;
	XMFLOAT3 eyePosition;
	float skyDimension;
	float screenWidth;
	float screenHeight;
	float ambientAmount;
	float padding;
};

const int IBL_N = 40;
// For IBL hammersley
struct RandomBuffer
{
	XMFLOAT4 hammersley[IBL_N];
};

// Light Constant Buffer
struct LightBuffer
{
	XMFLOAT4 color;			// 16
	XMFLOAT3 eyePosition;	// 28
	float screenWidth;		// 32
	XMFLOAT3 lightPosition; // 44
	float screenHeight;		// 48
	XMFLOAT3 nearFarC;		// 60
	float lightRange;		// 64
};

// Shadow Buffer
struct ShadowBuffer
{
	XMMATRIX shadowMatrix;	// 64
	int castShadow;			// 68
	int isExponential;		// 72 : 0 is shadow, 1 is exponential shadow
	XMUINT2 padding;		// 80
};

// Exponential 
struct ExponentialBuffer
{
	XMFLOAT3 lightNearFarConst;
	float padding;
};

// Material Constant Buffer
struct MaterialBuffer
{
	XMFLOAT4	ambient;		// 16
	XMFLOAT4	diffuse;		// 32
	XMFLOAT4	specular;		// 48
	XMFLOAT4	emissive;		// 64
	float		transparency;	// 68
	float		shineness;		// 72
	XMFLOAT2	padding;		// 80
	XMFLOAT4	colorandtype;	// 96

	MaterialBuffer() : 
		ambient(XMFLOAT4(1, 1, 1, 1)),
		diffuse(XMFLOAT4(1, 1, 1, 1)),
		specular(XMFLOAT4(1, 1, 1, 1)),
		emissive(XMFLOAT4(1, 1, 1, 1)),
		transparency(1.0f),
		shineness(0.1f),
		padding(XMFLOAT2(0, 0)),
		colorandtype(XMFLOAT4(0, 1, 1, 1))
	{}
};