#pragma once 

// Direct X include
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <directxmath.h>
#include <dxgi.h>

using namespace DirectX;
const float PI = 3.14159265359f;

class GraphicsPipeline
{
public:
	// Destructor
	~GraphicsPipeline();

	// Initialise
	bool Initialize(UINT screen_width,
		UINT screen_height,
		HWND window,
		float screen_near,
		float screen_far,
		bool fullscreen = false,
		bool vsync = false);

	// Shutdown
	void Shutdown();

	// Begin scene (Before render)
	void BeginScene(const XMFLOAT4& color);

	// End scene (After render)
	void EndScene();

	// Get world matrix
	XMMATRIX GetWorld() const;

	// Get projection matrix
	XMMATRIX GetProjection() const;

	// Get orthogonal matrix
	XMMATRIX GetOrthogonal() const;

	// Set world matrix to be used in pipeline
	void SetWorld(const XMMATRIX & world);

	// Set projection matrix to be used in pipeline
	void SetProjection(const XMMATRIX & projection);

	// Set orthogonal matrix to be used in pipeline
	void SetOrthogonal(const XMMATRIX & orthogonal);

	//Change Depth to 0 so that draw in foreground
	void DrawInForeground(void);

	//Change Depth to 1 so that draw in background
	void DrawInBackground(void);

	//Change Depth value
	void SetDepthRange(const float& MinZ, const float& MaxZ);

	//Chang Depth value as default
	void SetDefaultDepthRange(void);

	inline float GetFieldOfView() const { return mFOV; }
	inline float GetAspectRatio() const { return mAspectRatio; }

	inline XMMATRIX CreatePerspectiveFieldOfView(float fov, float aspectratio, float nearPlane, float farPlane) { return XMMatrixPerspectiveFovLH(fov, aspectratio, nearPlane, farPlane); }
	inline XMMATRIX CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane) { return XMMatrixOrthographicLH(width, height, zNearPlane, zFarPlane); }

public:
	// Getters (Normally you would not touch this)
	// Get device
	ID3D11Device * GetDevice() const;

	// Get context
	ID3D11DeviceContext * GetContext() const;

	// Get Depth Stencil View
	ID3D11DepthStencilView * GetDepthStencilView() const;

	// Set Back Buffer Render Target (Setting default back buffer as render target)
	void SetBackBufferRenderTarget();

	// Set alpha blend
	void SetAlphaBlend();

	// Set no blend
	void SetNoBlend();

	// Set additive blend
	void SetAdditiveBlend();

	// Turn back face cull on/off
	void SetBackFaceCull(bool on);

	// Turn front face cull on/off
	void SetFrontFaceCull(bool on);

	// Turn Depth test on/off
	void SetZBuffer(bool on);

	// Reset Viewport
	void ResetViewport();

private:
	// Constructor (Only graphics create this)
	GraphicsPipeline();

private:
	XMMATRIX mWorldMatrix;							//!< Model to World
	XMMATRIX mProjectionMatrix;
	XMMATRIX mOrthoMatrix;

	float mFOV;
	float mAspectRatio;

	IDXGISwapChain * mSwapChain;
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mDeviceContext;
	ID3D11RenderTargetView * mRenderTargetView;
	ID3D11Texture2D * mDepthStencilBuffer;
	ID3D11DepthStencilState * mDepthStencilState;
	ID3D11DepthStencilState * mDepthDisabledStencilState;
	ID3D11DepthStencilView * mDepthStencilView;
	ID3D11RasterizerState * mRasterState;
	ID3D11RasterizerState * mFrontCullRasterState;
	ID3D11RasterizerState * mNoCullRasterState;
	ID3D11BlendState * mAlphaEnableBlendingState;
	ID3D11BlendState * mAlphaDisableBlendingState;
	ID3D11BlendState * mAlphaAdditiveBlendState;
	D3D11_VIEWPORT mViewport;

	bool mVSync;									//!< Enable v-sync

	friend class Graphics;
	friend class Mesh;
	friend class Shader;
};