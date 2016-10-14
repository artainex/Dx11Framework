/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
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
#include <SMat4.h>

#pragma warning(disable : 4458)

//--------------------------------------------------------------------------------------
// MACROS
//--------------------------------------------------------------------------------------
#define FAIL_CHECK(expression) if( FAILED(expression) )	{ return expression; }
#define FAIL_CHECK_BOOLEAN(expression) if( FAILED(expression) )	{ return false; }
#define FAIL_CHECK_WITH_MSG(expression, msg) if( FAILED(expression) )	\
{																		\
	MessageBox(NULL, msg, "Error", MB_OK);								\
	return expression;													\
}
#define FAIL_CHECK_BOOLEAN_WITH_MSG(expression, msg) if( FAILED(expression) )	\
{																		\
	MessageBox(NULL, msg, "Error", MB_OK);								\
	return false;													\
}

#define SAFE_RELEASE(pt) if( nullptr != pt )\
{ pt->Release(); pt = nullptr; }			\

#define SAFE_DELETE(pt) if( nullptr != pt )	\
{ delete pt; pt = nullptr; }				\

#define SAFE_DELETE_ARRAY(pt) if( nullptr != pt )	\
{ delete [] pt; pt = nullptr; }				\

// shader type
enum eShaderType
{
	NONE_SHADER = -1,
	VERTEX_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
	HULL_SHADER
};

// render type(render target type)
enum eRenderType
{
	BASIC = -1,
	POSITION,	// world position
	NORMAL,		// world normal
	DIFFUSE,	// diffuse
	SPECULAR,	// specular with shineness
	DEPTH,		// depth
	RT_COUNT
};

// layout
enum eLayout
{
	NONE_LAYOUT = -1,
	LAYOUT0,	// POS NOR
	LAYOUT1, 	// POS NOR TEX
	LAYOUT2, 	// POS NOR BIN TAN
	LAYOUT3, 	// POS NOR BIN TAN TEX
	LAYOUT4, 	// POS NOR BW BI
	LAYOUT5, 	// POS NOR TEX BW BI
	LAYOUT6, 	// POS NOR BIN TAN BW BI
	LAYOUT7//,	// POS NOR BIN TAN TEX BW BI
		   //LAYOUTT		// POS TEX
};

// 노말 맵 만들려면 binormal이랑 tangent를 써야 하는데,
// 이걸 갖고 있는 놈도 있고 안갖고 나오는 놈도 있어.
// 동적으로 이걸 만들어내면 수맣은 레이아웃과 VERTEX_DATA~를 만들어내야하는데
// 이건 상당히 멍청한 짓이야. 맷이 한 거처럼 .Add()를 써서 여럿 붙일 수 있으면 좋을텐데.
// 나중에 이거 벡터로 바꾸든가 해. 진짜 못해먹겠네
struct LAYOUT
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT0[2];
	D3D11_INPUT_ELEMENT_DESC LAYOUT1[3];
	D3D11_INPUT_ELEMENT_DESC LAYOUT2[4];
	D3D11_INPUT_ELEMENT_DESC LAYOUT3[5];
	D3D11_INPUT_ELEMENT_DESC LAYOUT4[4];
	D3D11_INPUT_ELEMENT_DESC LAYOUT5[5];
	D3D11_INPUT_ELEMENT_DESC LAYOUT6[6];
	D3D11_INPUT_ELEMENT_DESC LAYOUT7[7];
	D3D11_INPUT_ELEMENT_DESC LAYOUT_TEX[2];
	D3D11_INPUT_ELEMENT_DESC LAYOUT_PT[2];

	// TRY OPTIMIZE THESE FORMATS 
	LAYOUT()
	{
		// pos, material, nbt, tex, bw, bi	
		LAYOUT0[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT0[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT1[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT1[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT1[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT2[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT2[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT2[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT2[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT3[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT3[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT3[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT3[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT3[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT4[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT4[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT4[2] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT4[3] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT5[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT5[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT5[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT5[3] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT5[4] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT6[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT6[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT6[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT6[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT6[4] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT6[5] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT7[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[2] = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[5] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT7[6] = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT_TEX[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT_TEX[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		LAYOUT_PT[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		LAYOUT_PT[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}
};


// POS NOR
struct VERTEX_DATA_L0
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
};

// POS NOR TEX
struct VERTEX_DATA_L1
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT2	vTexcoord;
};

// POS NOR BIN TAN
struct VERTEX_DATA_L2
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
};

// POS NOR BIN TAN TEX
struct VERTEX_DATA_L3
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
	DirectX::XMFLOAT2	vTexcoord;
};

// POS NOR BW BI
struct VERTEX_DATA_L4
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT4	vBWeight;
	BYTE		vBIdx[4];
};

// POS NOR TEX BW BI
struct VERTEX_DATA_L5
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT2	vTexcoord;
	DirectX::XMFLOAT4	vBWeight;
	BYTE		vBIdx[4];
};

// POS NOR BIN TAN BW BI
struct VERTEX_DATA_L6
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
	DirectX::XMFLOAT4	vBWeight;
	BYTE		vBIdx[4];
};

// POS NOR BIN TAN TEX BW BI
struct VERTEX_DATA_L7
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT3	vNor;
	DirectX::XMFLOAT3	vBin;
	DirectX::XMFLOAT3	vTan;
	DirectX::XMFLOAT2	vTexcoord;
	DirectX::XMFLOAT4	vBWeight;
	BYTE		vBIdx[4];
};

// POS TEX
struct VERTEX_DATA_LT
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT2	vTexcoord;
};

// POS NOR
struct VERTEX_DATA_LP
{
	DirectX::XMFLOAT3	vPos;
	DirectX::XMFLOAT4	vColor;
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