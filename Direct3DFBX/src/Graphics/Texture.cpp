#pragma once

#include <Texture.h>

Texture::Texture()
	: mTextureView(nullptr)
	, mTexture(nullptr)
{
}

Texture::~Texture()
{

}

void Texture::Initialize(const std::string & filename)
{
	CreateTextureFromFile(filename.c_str());
}

void Texture::Shutdown()
{
	if (mTexture)
	{
		mTexture->Release();
		mTexture = nullptr;
	}

	if (mTextureView)
	{
		mTextureView->Release();
		mTextureView = nullptr;
	}
}

void Texture::CreateTextureFromFile(const std::string& filename)
{
	//CreateWICTextureFromFile(GRAPHICS.GetPipeline()->GetDevice(),
	//	filename,
	//	&mTexture,
	//	&mTextureView,
	//	0);
	//
	//HRESULT CreateWICTextureFromFile(ID3D11Device * device,
	//	const WCHAR * filename,
	//	ID3D11Resource ** texture,
	//	ID3D11ShaderResourceView ** textureView,
	//	size_t maxsize)
	//{
	//	IWICImagingFactory * pWIC = GetWIC();
	//
	//	if (!pWIC)
	//	{
	//		CreateMessageBox("Failed to create WIC Factory");
	//		return E_NOINTERFACE;
	//	}
	//
	//	// Initialize WIC
	//	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	//	HRESULT hr = pWIC->CreateDecoderFromFilename(filename,
	//		0,
	//		GENERIC_READ,
	//		WICDecodeMetadataCacheOnDemand,
	//		decoder.GetAddressOf());
	//	if (FAILED(hr))
	//	{
	//		CreateMessageBox("Failed to create WIC Decoder");
	//		return hr;
	//	}
	//
	//	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	//	hr = decoder->GetFrame(0, frame.GetAddressOf());
	//	if (FAILED(hr))
	//	{
	//		CreateMessageBox("Failed to create WIC Decoder Frame");
	//		return hr;
	//	}
	//
	//	hr = CreateTextureFromWIC(device,
	//		nullptr,
	//		frame.Get(),
	//		maxsize,
	//		D3D11_USAGE_DEFAULT,
	//		D3D11_BIND_SHADER_RESOURCE,
	//		0,
	//		0,
	//		false,
	//		texture,
	//		textureView);
	//
	//	if (SUCCEEDED(hr))
	//	{
	//		if (texture != 0 || textureView != 0)
	//		{
	//			CHAR strFileA[MAX_PATH];
	//			int result = WideCharToMultiByte(CP_ACP,
	//				WC_NO_BEST_FIT_CHARS,
	//				filename,
	//				-1,
	//				strFileA,
	//				MAX_PATH,
	//				nullptr,
	//				FALSE
	//				);
	//			if (result > 0)
	//			{
	//				const CHAR* pstrName = strrchr(strFileA, '\\');
	//				if (!pstrName)
	//				{
	//					pstrName = strFileA;
	//				}
	//				else
	//				{
	//					pstrName++;
	//				}
	//
	//				if (texture != 0 && *texture != 0)
	//				{
	//					(*texture)->SetPrivateData(WKPDID_D3DDebugObjectName,
	//						static_cast<UINT>(strnlen_s(pstrName, MAX_PATH)),
	//						pstrName);
	//				}
	//
	//				if (textureView != 0 && *textureView != 0)
	//				{
	//					(*textureView)->SetPrivateData(WKPDID_D3DDebugObjectName,
	//						static_cast<UINT>(strnlen_s(pstrName, MAX_PATH)),
	//						pstrName);
	//				}
	//			}
	//		}
	//	}
	//
	//	return hr;
	//}
}