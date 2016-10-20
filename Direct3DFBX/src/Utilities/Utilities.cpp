/* ---------------------------------------------------------------------------
** Team Bear King
** ?2015 DigiPen Institute of Technology, All Rights Reserved.
**
** Utilities.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** - <list in same format as author if applicable>
** -------------------------------------------------------------------------*/

#include <Utilities.h>

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
void SamplerInitialize(D3D11_SAMPLER_DESC &sampler,
	D3D11_FILTER Filter,
	D3D11_TEXTURE_ADDRESS_MODE AddressU,
	D3D11_TEXTURE_ADDRESS_MODE AddressV,
	D3D11_TEXTURE_ADDRESS_MODE AddressW,
	D3D11_COMPARISON_FUNC ComparisonFunc,
	FLOAT MinLOD,
	FLOAT MaxLOD)
{
	ZeroMemory(&sampler, sizeof(sampler));
	sampler.Filter = Filter;
	sampler.AddressU = AddressU;
	sampler.AddressV = AddressV;
	sampler.AddressW = AddressW;
	sampler.ComparisonFunc = ComparisonFunc;
	sampler.MinLOD = MinLOD;
	sampler.MaxLOD = MaxLOD;
};

void BufferInitialize(D3D11_BUFFER_DESC& buffer,
	UINT byteWidth,
	D3D11_USAGE usage,
	UINT bindFlag,
	UINT cpuAccFlags,
	UINT miscFlags,
	UINT stride )
{
	ZeroMemory(&buffer, sizeof(buffer));
	buffer.ByteWidth = byteWidth;
	buffer.Usage = usage;
	buffer.BindFlags = bindFlag;
	buffer.CPUAccessFlags = cpuAccFlags;
	buffer.MiscFlags = miscFlags;
	buffer.StructureByteStride = stride;
}

HRESULT CompileShaderFromFile(eShaderType shaderType,
	LPCTSTR szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device** ppD3Ddevice,
	ID3DBlob** ppBlobOut,
	ID3D11VertexShader** ppVSLayout,
	ID3D11PixelShader** ppPSLayout)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	D3DX11CompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, 0, ppBlobOut, &pErrorBlob, &hr);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		else
		{
			MessageBox(nullptr, "Missing Shader File", "Error", MB_OK);
		}
		return E_FAIL;
	}

	// vertex shader
	if (VERTEX_SHADER == shaderType)
	{
		// Create the vertex shader
		hr = (*ppD3Ddevice)->CreateVertexShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), nullptr, &(*ppVSLayout));
	}
	else if (PIXEL_SHADER == shaderType)
	{
		// Create the pixel shader
		hr = (*ppD3Ddevice)->CreatePixelShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), nullptr, &(*ppPSLayout));
	}

	if (FAILED(hr))
	{
		(*ppBlobOut)->Release();
		return hr;
	}

	return S_OK;
}

namespace Utilities
{
    /*===============================
    Utility Functions for FBX
    ===============================*/
    ursine::SVec3 SetFloat3ToSVec3(const pseudodx::XMFLOAT3& rhs)
    {
        return ursine::SVec3(rhs.x, rhs.y, rhs.z);
    }

    ursine::SVec4 SetFloat3ToSVec4(const pseudodx::XMFLOAT3& rhs)
    {
        return ursine::SVec4(rhs.x, rhs.y, rhs.z, 0.0f);
    }

    ursine::SVec4 SetFloat4ToSVec4(const pseudodx::XMFLOAT4& rhs)
    {
        return ursine::SVec4(rhs.x, rhs.y, rhs.z, rhs.w);
    }

    pseudodx::XMFLOAT3 SetSVec3ToFloat3(const ursine::SVec3& rhs)
    {
        pseudodx::XMFLOAT3 ret;
        ret.x = rhs.X();
        ret.y = rhs.Y();
        ret.z = rhs.Z();
        return ret;
    }

    pseudodx::XMFLOAT3 SetSVec4ToFloat3(const ursine::SVec4& rhs)
    {
        pseudodx::XMFLOAT3 ret;
        ret.x = rhs.X();
        ret.y = rhs.Y();
        ret.z = rhs.Z();
        return ret;
    }

    pseudodx::XMFLOAT4 SetSVec4rToFloat4(const ursine::SVec4& rhs)
    {
        pseudodx::XMFLOAT4 ret;
        ret.x = rhs.X();
        ret.y = rhs.Y();
        ret.z = rhs.Z();
        ret.w = rhs.W();
        return ret;
    }

    FbxAMatrix FBXMatrixToFBXAMatrix(FbxMatrix* src)
    {
        FbxAMatrix ret;
        memcpy((double*)&ret.mData, &src->mData, sizeof(src->mData));
        return ret;
    }

    ursine::SMat4 FBXAMatrixToSMat4(FbxAMatrix* src)
    {
        ursine::SMat4 ret;
        ret.Set(static_cast<float>(src->Get(0, 0)), static_cast<float>(src->Get(0, 1)), static_cast<float>(src->Get(0, 2)), static_cast<float>(src->Get(0, 3)),
            static_cast<float>(src->Get(1, 0)), static_cast<float>(src->Get(1, 1)), static_cast<float>(src->Get(1, 2)), static_cast<float>(src->Get(1, 3)),
            static_cast<float>(src->Get(2, 0)), static_cast<float>(src->Get(2, 1)), static_cast<float>(src->Get(2, 2)), static_cast<float>(src->Get(2, 3)),
            static_cast<float>(src->Get(3, 0)), static_cast<float>(src->Get(3, 1)), static_cast<float>(src->Get(3, 2)), static_cast<float>(src->Get(3, 3)));
        return ret;
    }

    FbxAMatrix SMat4ToFBXAMatrix(ursine::SMat4* src)
    {
        FbxAMatrix ret;
        ret.mData[0][0] = src->GetRow(0).X(); ret.mData[0][1] = src->GetRow(0).Y(); ret.mData[0][2] = src->GetRow(0).Z(); ret.mData[0][3] = src->GetRow(0).W();
        ret.mData[1][0] = src->GetRow(1).X(); ret.mData[1][1] = src->GetRow(1).Y(); ret.mData[1][2] = src->GetRow(1).Z(); ret.mData[1][3] = src->GetRow(1).W();
        ret.mData[2][0] = src->GetRow(2).X(); ret.mData[2][1] = src->GetRow(2).Y(); ret.mData[2][2] = src->GetRow(2).Z(); ret.mData[2][3] = src->GetRow(2).W();
        ret.mData[3][0] = src->GetRow(3).X(); ret.mData[3][1] = src->GetRow(3).Y(); ret.mData[3][2] = src->GetRow(3).Z(); ret.mData[3][3] = src->GetRow(3).W();
        return ret;
    }

    ursine::SVec4 ConvertVector4(const FbxVector4& vec)
    {
        return ursine::SVec4(static_cast<float>(vec.mData[0]),
            static_cast<float>(vec.mData[2]),
            static_cast<float>(vec.mData[1]),
            static_cast<float>(vec.mData[3]));
    }

    ursine::SVec4 ConvertQuaternion(const FbxQuaternion& quat)
    {
        return ursine::SVec4(static_cast<float>(quat.mData[0]),
            static_cast<float>(quat.mData[2]),
            static_cast<float>(quat.mData[1]),
            static_cast<float>(quat.mData[3]));
    }

    FbxVector4 SVec4ToFBXVector(const ursine::SVec4& src)
    {
        return FbxVector4(src.X(), src.Y(), src.Z(), src.W());
    }

    FbxVector4 XMFloat3ToFBXVector4(const pseudodx::XMFLOAT3& src)
    {
        return FbxVector4(src.x, src.y, src.z, 1.0f);
    }

	pseudodx::XMFLOAT3 FBXDouble3ToXMFLOAT3(const FbxDouble3& src)
	{
		return pseudodx::XMFLOAT3(static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]));
	}

	pseudodx::XMFLOAT4 FBXDouble3ToXMFLOAT4(const FbxDouble3& src)
    {
        return pseudodx::XMFLOAT4(static_cast<float>(src[0]), static_cast<float>(src[1]), static_cast<float>(src[2]), 1.f);
    }

	pseudodx::XMFLOAT3 FBXVectorToXMFLOAT3(const FbxVector4& src)
    {
        return pseudodx::XMFLOAT3(
            static_cast<float>(src.mData[0]),
            static_cast<float>(src.mData[1]),
            static_cast<float>(src.mData[2]));
    }

    FbxVector2 XMFloat2ToFBXVector2(const pseudodx::XMFLOAT2& src)
    {
        return FbxVector2(src.x, src.y);
    }

	pseudodx::XMFLOAT4 FBXQuaternionToXMLOAT4(const FbxQuaternion& quat)
    {
        return pseudodx::XMFLOAT4(
            static_cast<float>(quat.mData[0]),
            static_cast<float>(quat.mData[1]),
            static_cast<float>(quat.mData[2]),
            static_cast<float>(quat.mData[3]));
    }

	pseudodx::XMFLOAT4 FBXVectorToXMFLOAT4(const FbxVector4& src)
    {
        return pseudodx::XMFLOAT4(
            static_cast<float>(src.mData[0]),
            static_cast<float>(src.mData[1]),
            static_cast<float>(src.mData[2]),
            static_cast<float>(src.mData[3]));
    }

	DirectX::XMVECTOR ConvertPseudoXMFloat3ToXMVEC(const pseudodx::XMFLOAT3& src)
	{
		DirectX::XMVECTOR ret;
		ret.m128_f32[0] = src.x; 
		ret.m128_f32[1] = src.y;
		ret.m128_f32[2] = src.z;
		return ret;
	}

	DirectX::XMVECTOR ConvertPseudoXMFloat4ToXMVEC(const pseudodx::XMFLOAT4& src)
	{
		DirectX::XMVECTOR ret;
		ret.m128_f32[0] = src.x;
		ret.m128_f32[1] = src.y;
		ret.m128_f32[2] = src.z;
		ret.m128_f32[3] = src.w;
		return ret;
	}
};