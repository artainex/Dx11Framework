#pragma once

#include <unordered_map>
#include <ctime>

#include <WholeInformation.h>
#include <Camera.h>
#include <Light.h>
#include <DebugWindow.h>

#include <RenderTarget.h>
#include <MultiRenderTarget.h>

#include <LightShader.h>
#include <DepthShader.h>
#include <ExponentialDepthShader.h>
#include <HorizontalBlurShader.h>
#include <VerticalBlurShader.h>
#include <UAVTexture.h>
#include <SceneRenderer.h>
#include <Constants.h>
#include "FBXModel.h"

#if DEBUG
#else
#include "SpriteFont.h"
#endif

using namespace DirectX;
using namespace ursine::FBX_DATA;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;
const float SCREEN_NEAR = 0.01f;
const float SCREEN_FAR = 10000.f;

HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;

// device
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11DeviceContext*                g_pDeviceContext = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;

// views, states
ID3D11RenderTargetView*				g_pRenderTargetView = nullptr;
ID3D11Texture2D*                    g_pDepthStencil = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;
ID3D11DepthStencilState*			g_pDepthStencilState = nullptr;
ID3D11DepthStencilState*			g_pDepthDisabledStencilState = nullptr;
ID3D11RasterizerState *				g_RasterState;
ID3D11RasterizerState *				g_FrontCullRasterState;
ID3D11RasterizerState *				g_NoCullRasterState;
ID3D11BlendState *					g_AlphaEnableBlendingState;
ID3D11BlendState *					g_AlphaDisableBlendingState;
ID3D11BlendState *					g_AlphaAdditiveBlendState;

// Matrices
XMMATRIX                            g_World; 
XMMATRIX							g_ScreenView;
XMMATRIX                            g_Projection;
XMMATRIX							g_OrthoMatrix;
XMMATRIX                            g_lightView;
XMMATRIX							g_lightProjection;
XMMATRIX							g_TranslMatrix = XMMatrixIdentity();
XMMATRIX							g_ScaleMatrix = XMMatrixIdentity();
XMMATRIX							g_RotationMatrix = XMMatrixIdentity();
XMMATRIX							g_ScreenRTMatrix = XMMatrixIdentity();

// Camera
XMFLOAT3 InitTsl = XMFLOAT3(0.f, 0.f, 0.f);
XMFLOAT3 InitRot = XMFLOAT3(0.f, 0.f, 0.f);
static XMFLOAT3 tsl = InitTsl;
static XMFLOAT3 rot = InitRot;
static float scl = 1.0f;
static float lrot = 0.f;
Camera								g_Camera;

// Lights
ursine::Light						g_AmbientLight; // global ambient
ursine::Light						g_GlobalLight; // global
ursine::Light						g_LocalLights[MAX_LIGHT];

RenderTarget						g_SceneBufferRenderTarget;
MultiRenderTarget					g_GBufferRenderTarget;	// geometry buffer - Model only
RenderTarget						g_DepthBufferRenderTarget;	// depth buffer - FQS
UAVTexture							g_HBlurBuffer;
UAVTexture							g_FinalBlurBuffer;
DebugWindow							g_DebugWindow;
LightShader							g_LightShader;
DepthShader							g_DepthShader;
ExponentialDepthShader				g_ExpDepthShader;
HorizontalBlurShader				g_HBlurShader;
VerticalBlurShader					g_VBlurShader;
SceneRenderer						g_SceneRenderer;

// ¸ðµ¨·Î´Â ½ºÅÂÆ½ ¸ðµ¨, ½ºÄÌ·¹Å» ¸ðµ¨ µîÀÌ ÀÖ°ÚÁö
// ÇÏÁö¸¸ LightModel, GeoStaticModel, GeoAnimateModel µîÀ¸·Î ³ª´©ÀÚ
typedef std::unordered_map< std::string, std::vector<ursine::FBXModel*> > Models;

//const UINT	NUMBER_OF_MODELS = 1;

// FBX file
std::string g_files[50] =
{
	"Assets/Models/sphere.fbx",
	"Assets/Models/dragonplane.fbx",
	//"Assets/Models/dragonsplane.fbx"
	//"Assets/Models/Plane.fbx"
	//"Assets/Animations/Player/Player_Win.fbx"
};

#if DEBUG
#else
// font
DirectX::SpriteBatch*			g_pSpriteBatch = nullptr;
DirectX::SpriteFont*			g_pFont = nullptr;
#endif

ID3D11Buffer*					g_pmtxBuffer = nullptr;
ID3D11Buffer*					g_pmtxPaletteBuffer = nullptr;

ID3D11VertexShader*				g_pVShader[8] = { nullptr, };
ID3D11ComputeShader*			g_pCShader[8] = { nullptr, };
ID3D11PixelShader*              g_pPShader[8] = { nullptr, };

// Instancing
bool	g_bInstancing = false;
const uint32_t g_InstanceMAX = MAX_LIGHT;
ID3D11VertexShader*				g_pvsInstancing = nullptr;
ID3D11Buffer*					g_pTransformStructuredBuffer = nullptr;
ID3D11ShaderResourceView*		g_pTransformSRV = nullptr;
