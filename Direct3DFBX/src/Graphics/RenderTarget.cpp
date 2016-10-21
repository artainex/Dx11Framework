#include <RenderTarget.h>

RenderTarget::RenderTarget()
:
	m_renderTargetTexture(nullptr),
	m_renderTargetView(nullptr),
	m_shaderResourceView(nullptr)
{
}

RenderTarget::RenderTarget(const RenderTarget& other)
{
}

RenderTarget::~RenderTarget()
{
}

// The Initialize function takes as input the width and height you want to make this render to texture.
// Important: Remember that if you are rendering your screen you should keep the aspect ratio of this render to texture the same as the aspect ratio of the screen or there will be some size distortion.
// The function creates a render target texture by first setting up the description of the texture and then creating that texture.
// It then uses that texture to setup a render target view so that the texture can be drawn to as a render target.The third and final thing we do is create a shader resource view of that texture so that we can supply the rendered data to calling objects.
bool RenderTarget::Initialize(ID3D11Device* device, int width, int height)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

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
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	FAIL_CHECK_BOOLEAN(result);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	FAIL_CHECK_BOOLEAN(result);

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	FAIL_CHECK_BOOLEAN(result);

	return true;
}

// Shutdown releases the three interfaces used by the MultiRenderTargetClass.
void RenderTarget::Shutdown()
{
	SAFE_RELEASE(m_shaderResourceView);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_renderTargetTexture);

	return;
}

// The SetRenderTarget function sets the render target view in this class as the current rendering location for all graphics to be rendered to.
void RenderTarget::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	if (depthStencilView)
		deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
	else
		deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	return;
}

void RenderTarget::ReleaseRenderTarget(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &pSRV);
	return;
}

// ClearRenderTarget mimics the functionality of the D3DClass::BeginScene function except for that it operates on the render target view 
// within this class.This should be called each frame before rendering to this render target view.
void RenderTarget::ClearRenderTarget(ID3D11DeviceContext* deviceContext,
	ID3D11DepthStencilView* depthStencilView, const float* color)
{
	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void RenderTarget::ClearRenderTarget(ID3D11DeviceContext* deviceContext,
	ID3D11DepthStencilView* depthStencilView, float r, float g, float b, float a)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

// The GetShaderResourceView function returns the render to texture data as a shader resource view.This way whatever has been rendered to the render 
// target view can be used as a texture in different shaders that call this function.Where you would normally send a texture into a shader you can instead send a call to this function in its place and the render to texture will be used.
ID3D11ShaderResourceView* RenderTarget::GetShaderResourceView()
{
	return m_shaderResourceView;
}