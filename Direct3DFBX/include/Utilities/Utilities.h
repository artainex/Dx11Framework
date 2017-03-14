/* ---------------------------------------------------------------------------
** Team Bear King
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** Utilities.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/
#pragma once

#include <directxmath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <D3DX11async.h>
#include <fbxsdk.h>
#include <SVec3.h>
#include <SVec4.h>
#include <SMat3.h>
#include <SMat4.h>
#include <algorithm>
#include <vector>
#include <string>
#include <rgbe.h>

#pragma warning(disable : 4458)

template <typename T>
inline T Max(const T & v1, const T & v2)
{
	return std::max<T>(v1, v2);
}

template <typename T, typename... Arguments >
inline T Max(const T & v1, const T & v2, Arguments... args)
{
	return Max(Max(v1, v2), args...);
}

template <typename T>
inline T Min(const T & v1, const T & v2)
{
	return std::min<T>(v1, v2);
}

template <typename T, typename... Arguments >
inline T Min(const T & v1, const T & v2, Arguments... args)
{
	return Min(Min(v1, v2), args...);
}

template <typename T>
inline T Clamp(const T & value, const T & low, const T & high)
{
	return Max(low, Min(value, high));
}

//--------------------------------------------------------------------------------------
// HDR file related helper functions
//--------------------------------------------------------------------------------------
// Read an HDR image in .hdr (RGBE) format.
void read(const std::string& inName, std::vector<float>& image, int& width, int& height);
// Write an HDR image in .hdr (RGBE) format.
void write(const std::string& outName, std::vector<float>& image, const int width, const int height);
bool InitHDRTextures(const std::string& hdrfile);
void HDR_sRGB2Linear(const int& width, const int& height, std::vector<float>* image);
void HDR_Linear2sRGB(const int& width, const int& height, std::vector<float>* image);

//--------------------------------------------------------------------------------------
// MACROS
//--------------------------------------------------------------------------------------
#define FAIL_CHECK(expression) if( FAILED(expression) )	{ return expression; }
#define FAIL_CHECK_VOID(expression) if( FAILED(expression) )	{ return; }
#define FAIL_CHECK_VOID_WITH_MSG(expression, msg) if( FAILED(expression) )		\
{																				\
	MessageBox(NULL, msg, "Error", MB_OK);										\
	return;																		\
}
#define FAIL_CHECK_BOOLEAN(expression) if( FAILED(expression) )	{ return false; }
#define FAIL_CHECK_WITH_MSG(expression, msg) if( FAILED(expression) )	\
{																		\
	MessageBox(NULL, msg, "Error", MB_OK);								\
	return expression;													\
}
#define FAIL_CHECK_BOOLEAN_WITH_MSG(expression, msg) if( FAILED(expression) )	\
{																				\
	MessageBox(NULL, msg, "Error", MB_OK);										\
	return false;																\
}

#define SAFE_RELEASE(pt) if( nullptr != pt )\
{ pt->Release(); pt = nullptr; }			\

#define SAFE_DELETE(pt) if( nullptr != pt )	\
{ delete pt; pt = nullptr; }				\

#define SAFE_DELETE_ARRAY(pt) if( nullptr != pt )	\
{ delete [] pt; pt = nullptr; }				\

void SamplerInitialize(D3D11_SAMPLER_DESC &sampler,
	D3D11_FILTER Filter,
	D3D11_TEXTURE_ADDRESS_MODE AddressU,
	D3D11_TEXTURE_ADDRESS_MODE AddressV,
	D3D11_TEXTURE_ADDRESS_MODE AddressW,
	D3D11_COMPARISON_FUNC ComparisonFunc,
	FLOAT MinLOD,
	FLOAT MaxLOD);

void BufferInitialize(D3D11_BUFFER_DESC& buffer,
	UINT byteWidth = 0,
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
	UINT bindFlags = 0,
	UINT cpuAccFlags = 0,
	UINT miscFlags = 0,
	UINT stride = 0);

// shader type
enum eShaderType
{
	NONE_SHADER = -1,
	VERTEX_SHADER,
	PIXEL_SHADER,
	GEOMETRY_SHADER,
	COMPUTE_SHADER,
	HULL_SHADER
};

// render type(geo model or light model)
enum eRenderType
{
	RENDER_NONE = -1,
	RENDER_GEO,
	RENDER_LIGHT,
	RENDER_SKYDOME,
	RENDER_COUNT
};

// render target type
enum eRenderTargetType
{
	RT_BASIC = -1,
	RT_POSITION,	// world position
	RT_NORMAL,		// world normal
	RT_DIFFUSE,		// diffuse
	RT_SPECULAR,	// specular with shineness
	RT_COUNT
};

// layout
enum eLayout
{
	NONE_LAYOUT = -1,
	LAYOUT0,		// POS NOR
	LAYOUT1, 		// POS NOR TEX
	LAYOUT2, 		// POS NOR BIN TAN
	LAYOUT3, 		// POS NOR BIN TAN TEX
	LAYOUT4, 		// POS NOR BW BI
	LAYOUT5, 		// POS NOR TEX BW BI
	LAYOUT6, 		// POS NOR BIN TAN BW BI
	LAYOUT7			// POS NOR BIN TAN TEX BW BI
};

// This will be used for both geometry and light
struct VERTEX_DATA_COMPATIBLE
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT2	vTexcoord;
};

struct VERTEX_DATA_GEO
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
	DirectX::XMFLOAT2	vTexcoord;
};

struct VERTEX_DATA_SKIN
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
	DirectX::XMFLOAT2	vTexcoord;
	DirectX::XMFLOAT4	vBWeight;
	BYTE				vBIdx[4];
};

// POS TEX
struct VERTEX_DATA_LT
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT2	vTexcoord;
};

//--------------------------------------------------------------------------------------
// D3D11  Utility functions
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(eShaderType shaderType,
	LPCTSTR szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device** ppD3Ddevice = nullptr,
	ID3DBlob** ppBlobOut = nullptr,
	ID3D11VertexShader** ppVSLayout = nullptr,
	ID3D11PixelShader** ppPSLayout = nullptr);

HRESULT CompileShaderFromFile(eShaderType shaderType,
	LPCTSTR szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device** ppD3Ddevice = nullptr,
	ID3DBlob** ppBlobOut = nullptr,
	const D3D_SHADER_MACRO* macro = nullptr,
	ID3D11ComputeShader** ppComputeShader = nullptr);

namespace pseudodx
{
	struct XMUINT4
	{
		uint32_t x;
		uint32_t y;
		uint32_t z;
		uint32_t w;
		XMUINT4() {}
		XMUINT4(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) : x(_x), y(_y), z(_z), w(_w) {}
		XMUINT4& operator= (const XMUINT4& Uint4) { x = Uint4.x; y = Uint4.y; z = Uint4.z; w = Uint4.w; return *this; }
		bool operator== (const XMUINT4& Uint4) { return ((x == Uint4.x) && (y == Uint4.y) && (z == Uint4.z) && (w == Uint4.w)) ? true : false; }
		bool operator!= (const XMUINT4& Uint4) { return (!(*this == Uint4)); }
	};

	struct XMFLOAT4
	{
		float x;
		float y;
		float z;
		float w;
		XMFLOAT4() {}
		XMFLOAT4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		XMFLOAT4& operator= (const XMFLOAT4& Float4) { x = Float4.x; y = Float4.y; z = Float4.z; w = Float4.w; return *this; }
		bool operator== (const XMFLOAT4& Float4) { return ((x == Float4.x) && (y == Float4.y) && (z == Float4.z) && (w == Float4.w)) ? true : false; }
		bool operator!= (const XMFLOAT4& Float4) { return (!(*this == Float4)); }
	};

	struct XMFLOAT3
	{
		float x;
		float y;
		float z;

		XMFLOAT3() {}
		XMFLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		XMFLOAT3& operator+ (const XMFLOAT3& Float3) { x += Float3.x; y += Float3.y; z += Float3.z; return *this; }
		XMFLOAT3& operator- (const XMFLOAT3& Float3) { x -= Float3.x; y -= Float3.y; z -= Float3.z; return *this; }
		XMFLOAT3& operator/ (const float& floatval) { x /= floatval; y /= floatval; z /= floatval; return *this; }
		XMFLOAT3& operator= (const XMFLOAT3& Float3) { x = Float3.x; y = Float3.y; z = Float3.z; return *this; }
		bool operator== (const XMFLOAT3& Float3) { return ((x == Float3.x) && (y == Float3.y) && (z == Float3.z)) ? true : false; }
		bool operator!= (const XMFLOAT3& Float3) { return (!(*this == Float3)); }
	};

	struct XMFLOAT2
	{
		float x;
		float y;

		XMFLOAT2() {}
		XMFLOAT2(float _x, float _y) : x(_x), y(_y) {}
		XMFLOAT2& operator= (const XMFLOAT2& Float2) { x = Float2.x; y = Float2.y; return *this; }
		bool operator== (const XMFLOAT2& Float2) { return ((x == Float2.x) && (y == Float2.y)) ? true : false; }
		bool operator!= (const XMFLOAT2& Float2) { return (!(*this == Float2)); }
	};
}

namespace Utilities
{
    /*===============================
    Utility Functions for FBX
    ===============================*/

    /** @brief convert XMFLOAT3 to SVec3 function
    *
    *  this will convert XMFLOAT3 format to SVec3
    *
    *  @param rhs XMFLOAT3
    *  @return converted SVec3 result
    */
    ursine::SVec3 SetFloat3ToSVec3(const pseudodx::XMFLOAT3& rhs);
    /** @brief convert XMFLOAT3 to SVec4 function
    *
    *  this will convert XMFLOAT3 format to SVec4
    *
    *  @param rhs XMFLOAT3
    *  @return converted SVec4 result
    */
    ursine::SVec4 SetFloat3ToSVec4(const pseudodx::XMFLOAT3& rhs);
    /** @brief convert XMFLOAT4 to SVe43 function
    *
    *  this will convert XMFLOAT4 format to SVec4
    *
    *  @param rhs XMFLOAT4
    *  @return converted SVec4 result
    */
    ursine::SVec4 SetFloat4ToSVec4(const pseudodx::XMFLOAT4& rhs);
    /** @brief convert SVec3 to XMFLOAT3 function
    *
    *  this will convert SVec3 format to XMFLOAT3
    *
    *  @param rhs SVec3
    *  @return converted XMFLOAT3 result
    */
    pseudodx::XMFLOAT3 SetSVec3ToFloat3(const ursine::SVec3& rhs);
    /** @brief convert SVec4 to XMFLOAT3 function
    *
    *  this will convert SVec4 format to XMFLOAT3
    *
    *  @param rhs SVec4
    *  @return converted XMFLOAT3 result
    */
    pseudodx::XMFLOAT3 SetSVec4ToFloat3(const ursine::SVec4& rhs);
    /** @brief convert SVec4 to XMFLOAT4 function
    *
    *  this will convert SVec4 format to XMFLOAT4
    *
    *  @param rhs SVec4
    *  @return converted XMFLOAT4 result
    */
    pseudodx::XMFLOAT4 SetSVec4rToFloat4(const ursine::SVec4& rhs);
    /** @brief convert FbxMatrix to FbxAMatrix function
    *
    *  this will convert FbxMatrix format to FbxAMatrix
    *
    *  @param src FbxMatrix
    *  @return converted FbxAMatrix result
    */
    FbxAMatrix FBXMatrixToFBXAMatrix(FbxMatrix* src);
    /** @brief convert FbxAMatrix to SMat4 function
    *
    *  this will convert FbxAMatrix format to SMat4
    *
    *  @param src FbxAMatrix
    *  @return converted SMat4 result
    */
    ursine::SMat4 FBXAMatrixToSMat4(FbxAMatrix* src);
    FbxAMatrix SMat4ToFBXAMatrix(ursine::SMat4* src);
    /** @brief convert FbxVector4 to SVec4 function
    *
    *  this will convert FbxVector4 format to SVec4
    *
    *  @param src FbxVector4
    *  @return converted SVec4 result
    */
    ursine::SVec4 ConvertVector4(const FbxVector4& vec);
    /** @brief convert FbxQuaternion to SVec4 function
    *
    *  this will convert FbxQuaternion format to SVec4
    *
    *  @param src FbxQuaternion
    *  @return converted SVec4 result
    */
    ursine::SVec4 ConvertQuaternion(const FbxQuaternion& quat);
    /** @brief convert SVec4 to FbxVector4 function
    *
    *  this will convert SVec4 format to FbxVector4
    *
    *  @param src SVec4
    *  @return converted FbxVector4 result
    */
    FbxVector4 SVec4ToFBXVector(const ursine::SVec4& src);
    /** @brief convert XMFLOAT3 to FbxVector4 function
    *
    *  this will convert XMFLOAT3 format to FbxVector4
    *
    *  @param src XMFLOAT3
    *  @return converted FbxVector4 result
    */
    FbxVector4 XMFloat3ToFBXVector4(const pseudodx::XMFLOAT3& src);
	/** @brief convert FbxDouble3 to FbxVector3 function
	*
	*  this will convert FbxDouble3 format to FbxVector3
	*
	*  @param src FbxDouble3
	*  @return converted FbxVector3 result
	*/
	pseudodx::XMFLOAT3 FBXDouble3ToXMFLOAT3(const FbxDouble3& src);
    /** @brief convert FbxDouble3 to FbxVector4 function
    *
    *  this will convert FbxDouble3 format to FbxVector4
    *
    *  @param src FbxDouble3
    *  @return converted FbxVector4 result
    */
	pseudodx::XMFLOAT4 FBXDouble3ToXMFLOAT4(const FbxDouble3& src);
    /** @brief convert FbxVector4 to XMFLOAT3 function
    *
    *  this will convert FbxVector4 format to XMFLOAT3
    *
    *  @param src FbxVector4
    *  @return converted XMFLOAT3 result
    */
	pseudodx::XMFLOAT3 FBXVectorToXMFLOAT3(const FbxVector4& src);
    /** @brief convert XMFLOAT2 to FbxVector2 function
    *
    *  this will convert XMFLOAT2 format to FbxVector2
    *
    *  @param src XMFLOAT2
    *  @return converted FbxVector2 result
    */
    FbxVector2 XMFloat2ToFBXVector2(const pseudodx::XMFLOAT2& src);
    /** @brief convert FbxQuaternion to XMFLOAT4 function
    *
    *  this will convert FbxQuaternion format to XMFLOAT4
    *
    *  @param src FbxQuaternion
    *  @return converted XMFLOAT4 result
    */
	pseudodx::XMFLOAT4 FBXQuaternionToXMLOAT4(const FbxQuaternion& quat);
    /** @brief convert FbxVector4 to XMFLOAT3 function
    *
    *  this will convert FbxVector4 format to XMFLOAT3
    *
    *  @param src FbxVector4
    *  @return converted XMFLOAT3 result
    */
	pseudodx::XMFLOAT3 FBXVectorToXMFLOAT3(const FbxVector4& src);
    /** @brief convert FbxVector4 to XMFLOAT4 function
    *
    *  this will convert FbxVector4 format to XMFLOAT4
    *
    *  @param src FbxVector4
    *  @return converted XMFLOAT4 result
    */
	pseudodx::XMFLOAT4 FBXVectorToXMFLOAT4(const FbxVector4& src);

	DirectX::XMVECTOR ConvertPseudoXMFloat3ToXMVEC(const pseudodx::XMFLOAT3& src);

	DirectX::XMVECTOR ConvertPseudoXMFloat4ToXMVEC(const pseudodx::XMFLOAT4& src);
}	// namespace utilities