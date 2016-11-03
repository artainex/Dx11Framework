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
#include <SceneRenderer.h>
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
XMMATRIX							g_translMatrix = XMMatrixIdentity();
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

// 4 texture maps - world position, world normal, 
// diffuse+transparency(나중에 메테리얼로 뺄것), spec+shineness,
// 라이트를 그리는 렌더 타겟은 사실 필요없어
// 바로 씬에서 합성해버리려고 하는 듯
// 아마 라이팅을 계산까지 해준 쿼드를 그리는 걸 라이트 갯수만큼 해주는 모양
// 씬픽셀 셰이더를 라이트 갯수만큼 돌리는 거지
// 그러니까 g_SceneBufferRenderTarget에 블랜딩 켠 상태로 라이팅 계산을 하는 것 같아.
// 우선 앰비언트만 해보자.
RenderTarget						g_SceneBufferRenderTarget;
MultiRenderTarget					g_GBufferRenderTarget;	// geometry buffer - Model only
RenderTarget						g_DepthBufferRenderTarget;	// depth buffer - FQS
DebugWindow							g_DebugWindow;
LightShader							g_LightShader;
DepthShader							g_DepthShader;
SceneRenderer						g_SceneRenderer;

UINT updateSpeed = 1;

// 모델로는 스태틱 모델, 스켈레탈 모델 등이 있겠지
// 하지만 LightModel, GeoStaticModel, GeoAnimateModel 등으로 나누자
// 이거 하는 이유가 라이트 모델만 랜더링 하기 쉽게 하려고 였던가?
// 랜더 지오랑 랜더 라이팅 모델에 셋 셰이더 파라미터 하는게 달라서?
// 라이트를 굳이 그리지 않아도 된다고 할 때로 생각해보자.
// 어차피 예제 사진에서는 
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

// Clear Color rgba
const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Light
const float lightNear = 0.1f;
const float lightFar = 500.f;