#include <Utilities.h>
#include <LightClass.h>

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
	urColor diffuseColor;
	urColor ambientColor;
	urColor specularColor;
	urColor emissiveColor;
	ursine::SVec3 lightDirection;
	float padding;
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