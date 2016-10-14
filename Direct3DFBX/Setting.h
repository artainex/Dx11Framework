#pragma once

#include <WholeInformation.h>
#include <ctime>
#include <Camera.h>
#include <Light.h>
#include <DebugWindow.h>
#include <RenderTarget.h>
#include <MultiRenderTarget.h>
#include <PointShader.h>
#include <SceneRenderer.h>
#include "CFBXRendererDX11.h"

#if DEBUG
#else
#include "SpriteFont.h"
#endif

using namespace DirectX;
using namespace ursine::FBX_DATA;
//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const float SCREEN_NEAR = 0.01f;
const float SCREEN_FAR = 10000.f;

UINT frame = 0;
UINT frame_per_sec = 0;

HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11DeviceContext*                g_pDeviceContext = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;
ID3D11RenderTargetView*				g_pRenderTargetView = nullptr;
ID3D11Texture2D*                    g_pDepthStencil = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;
ID3D11DepthStencilState*			g_pDepthStencilState = nullptr;
ID3D11DepthStencilState*			g_pDepthDisabledStencilState = nullptr;

// Matrices
XMMATRIX                            g_World;
XMMATRIX                            g_Projection;
XMMATRIX							g_OrthoMatrix;
XMMATRIX							g_ScaleMatrix = XMMatrixIdentity();

// Camera
XMFLOAT3 InitTsl = XMFLOAT3(0.f, 0.f, -100.f);
XMFLOAT3 InitRot = XMFLOAT3(0, 0, 0);
static XMFLOAT3 tsl = InitTsl;
static XMFLOAT3 rot = InitRot;
static float scl = 1.0f;
Camera								g_Camera;

// Lights
ursine::Light						g_AmbientLight; // global ambient
ursine::Light						g_GlobalLight; // global
ursine::Light						g_LocalLights[MAX_LIGHT];
ID3D11Buffer*						g_pLightbuffer = nullptr;

// 4 texture maps - world position, world normal, 
// diffuse+transparency(나중에 메테리얼로 뺄것), spec+shineness, 
RenderTarget						g_SceneBufferRenderTarget;
MultiRenderTarget					g_GBufferRenderTarget;
//DebugWindow							g_DebugWindow;
SceneRenderer						g_SceneRenderer;

UINT updateSpeed = 1;

const UINT	NUMBER_OF_MODELS = 1;

// FBX file
std::string g_files[50] =
{
	"Assets/Models/sphere.fbx",
	//"Assets/Models/dragonplane.fbx",
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

ID3D11RasterizerState *			mRasterState;
ID3D11RasterizerState *			mFrontCullRasterState;
ID3D11RasterizerState *			mNoCullRasterState;
ID3D11BlendState *				mAlphaEnableBlendingState;
ID3D11BlendState *				mAlphaDisableBlendingState;
ID3D11BlendState *				mAlphaAdditiveBlendState;
ID3D11RasterizerState*			g_pRS = nullptr;
ID3D11Buffer*					g_pmtxBuffer = nullptr;
ID3D11Buffer*					g_pmtxPaletteBuffer = nullptr;

ID3D11VertexShader*				g_pVShader[9] = { nullptr, };
ID3D11ComputeShader*			g_pCShader[9] = { nullptr, };
ID3D11PixelShader*              g_pPShader[9] = { nullptr, };

// Instancing
bool	g_bInstancing = false;
const uint32_t g_InstanceMAX = MAX_LIGHT;
ID3D11VertexShader*				g_pvsInstancing = nullptr;
ID3D11Buffer*					g_pTransformStructuredBuffer = nullptr;
ID3D11ShaderResourceView*		g_pTransformSRV = nullptr;