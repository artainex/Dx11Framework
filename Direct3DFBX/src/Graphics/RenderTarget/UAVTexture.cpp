// Precompiled Header
#include <Utilities.h>
#include <RenderTarget.h>
#include <UAVTexture.h>
#include <Graphics.h>

UAVTexture::UAVTexture()
	: mUnorderedAccessView(nullptr)
	, mShaderResourceView(nullptr)
	, mTexture(nullptr)
	, mWidth(0)
	, mHeight(0)
{
}

UAVTexture::~UAVTexture()
{
}

bool UAVTexture::Initialize(int width, int height)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;

	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &mTexture);
	FAIL_CHECK_BOOLEAN(result);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mShaderResourceView);
	FAIL_CHECK_BOOLEAN(result);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	ZeroMemory(&UAVdesc, sizeof(UAVdesc));
	UAVdesc.Format = textureDesc.Format;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Texture2D.MipSlice = 0;
	result = device->CreateUnorderedAccessView(mTexture, &UAVdesc, &mUnorderedAccessView);
	FAIL_CHECK_BOOLEAN(result);

	mWidth = width;
	mHeight = height;

	return true;
}

void UAVTexture::Shutdown()
{
	if (mUnorderedAccessView)
	{
		mUnorderedAccessView->Release();
		mUnorderedAccessView = nullptr;
	}

	if (mShaderResourceView)
	{
		mShaderResourceView->Release();
		mShaderResourceView = nullptr;
	}

	if (mTexture)
	{
		mTexture->Release();
		mTexture = nullptr;
	}
}

void UAVTexture::ReleaseShaderResourceView()
{
	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();
	ID3D11ShaderResourceView* pSRV = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &pSRV);
	return;
}