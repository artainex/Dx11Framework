// *********************************************************************************************************************
/// 
/// @file 		FBXLoaderSample.cpp
/// @brief		FBX Loader tester application
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************


#include <WholeInformation.h>
#include <ctime>
#include <Camera.h>
#include <Light.h>
#include <RenderTexture.h>
#include <DebugWindow.h>
#include <TextureShader.h>
#include "CFBXRendererDX11.h"

using namespace DirectX;
using namespace ursine::FBX_DATA;

//--------------------------------------------------------------------------------------
// TODO list
// 1. Multiple Model/Animation Loading
// 2. FBX Data-> Output -> Output load instead FBX(don't need to do right now. 
// cauz I've already done this before, we need to show FBX parsing actually works
// 3. Normal mapping, Shadow Mapping, Deferred Shading
// understand how to use G-Buffer first
// Not priority 
// - create boolean structure to check if diff/ambi/spec maps exist
// struct ~{bool amap, bool dmap, bool smap}
// - if there is no texture -> cylinderical or spherical mapping
// - draw grid
// - UI add
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11DeviceContext*                g_pImmediateContext = nullptr;
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
XMMATRIX							g_TRSMatrix;

// Camera
Camera								g_Camera;

// Lights
const int MAX_LIGHT = 5;
ursine::Light						g_AmbientLight;
ursine::Light						g_GlobalLight;
ursine::Light						g_LocalLights[MAX_LIGHT];

const int MAX_RENDERTARGET = 4;
RenderTexture*						g_pRenderTexture = nullptr; // 4 texture maps - position, color, normal, depth
DebugWindow*						g_pDebugWindow = nullptr;
TextureShader*						g_pTextureShader = nullptr;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
HRESULT InitCamera();
HRESULT InitVertexShaders();
HRESULT InitPixelShaders();
HRESULT CreateBuffers();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Update(double deltaTime);
void Render();
void RenderScene();
void RenderToTexture();
void RenderModel();
bool SetShaderParameters(ursine::CFBXRenderDX11** currentModel, const UINT& mesh_index, const eLayout& layoutType);
UINT updateSpeed = 1;

const UINT	NUMBER_OF_MODELS = 2;

HRESULT InitApp();
void CleanupApp();
ursine::CFBXRenderDX11*	g_pFbxDX11[NUMBER_OF_MODELS];

// FBX file
char g_files[NUMBER_OF_MODELS][256] =
{
	//"Assets/Models/stanford_bunny.fbx",
	"Assets/Models/Plane.fbx",
	"Assets/Animations/Player/Player_Idle.fbx"
};

std::vector<XMMATRIX> skin_mat;

ID3D11BlendState*				g_pBlendState = nullptr;
ID3D11RasterizerState*			g_pRS = nullptr;
ID3D11Buffer*					g_pmtxBuffer = nullptr;
ID3D11Buffer*					g_pmtxPaletteBuffer = nullptr;
ID3D11Buffer*					g_plightBuffer = nullptr;
ID3D11VertexShader*				g_pvsLayout[8] = { nullptr, };
ID3D11ComputeShader*			g_pcsLayout[8] = { nullptr, };
ID3D11PixelShader*              g_ppsLayout[8] = { nullptr, };

//// Instancing
//bool	g_bInstancing = false;
//const uint32_t g_InstanceMAX = 32;
//ID3D11VertexShader*				g_pvsInstancing = nullptr;

// Shader Resource View - was implemented for instancing
struct SRVPerInstanceData
{
	XMMATRIX mWorld;
};
ID3D11Buffer*					g_pTransformStructuredBuffer = nullptr;
ID3D11ShaderResourceView*		g_pTransformSRV = nullptr;
HRESULT SetupTransformSRV();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	FAIL_CHECK( InitWindow(hInstance, nCmdShow) );

	if (FAILED( InitDevice() ))
	{
		CleanupApp();
		CleanupDevice();
		return 0;
	}

	FAIL_CHECK( InitCamera() );

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static std::clock_t start = std::clock();
			std::clock_t timedelta = std::clock() - start;
			float t_delta_per_msec = (timedelta * updateSpeed) / (float)(CLOCKS_PER_SEC);
			Update(t_delta_per_msec);

			// need to be reset on last frame's time
			if (t_delta_per_msec >= 1.f)
				start = std::clock();

			Render();
		}
	}

	CleanupApp();
	CleanupDevice();

	return (int)msg.wParam;
}

void Update(double deltaTime)
{
	for (DWORD i = 0; i < NUMBER_OF_MODELS; i++)
	{
		if (g_pFbxDX11[i])
			g_pFbxDX11[i]->Update(deltaTime);
	}
}

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	// clear out the window class for use
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.lpszClassName = "TutorialWindowClass";
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(
		"TutorialWindowClass",
		"Direct3D 11 FBX Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, hInstance, nullptr);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// driver type
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// feature lvl
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// scanline order, scaling unspecified
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// discard back buffer after swap
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// no additional option flag
	sd.Flags = 0;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr, //adaptor?
			g_driverType, // driver-type
			nullptr, // flag
			createDeviceFlags, // device flag
			featureLevels, // feature lvl array
			numFeatureLevels, // feature lvl count                        
			D3D11_SDK_VERSION, // sdk version
			&sd, // swapchain creator structure
			&g_pSwapChain, // created swapchain obj
			&g_pd3dDevice, // created device obj
			&g_featureLevel, // created feature lvl obj
			&g_pImmediateContext // created device context obj
			);
		if (SUCCEEDED(hr))
			break;
	}
	FAIL_CHECK(hr);

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(
		0, // back buffer index
		__uuidof(ID3D11Texture2D), // interface that access to back buffer
		(LPVOID*)&pBackBuffer);
	FAIL_CHECK(hr);

	hr = g_pd3dDevice->CreateRenderTargetView(
		pBackBuffer, // resource that view will access
		nullptr, // def of rendertargetview
		&g_pRenderTargetView);
	FAIL_CHECK(hr);

	pBackBuffer->Release();
	FAIL_CHECK(hr);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = SCREEN_WIDTH;
	descDepth.Height = SCREEN_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
	FAIL_CHECK(hr);
	
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	FAIL_CHECK(hr);

	// setting rendertargetview & depth-stencil buffer 
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	
	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC descDSS;
	ZeroMemory(&descDSS, sizeof(descDSS));

	// Set up the description of the stencil state.
	descDSS.DepthEnable = true;
	descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDSS.StencilEnable = true;
	descDSS.StencilReadMask = 0xFF;
	descDSS.StencilWriteMask = 0xFF;
	
	// Stencil operations if pixel is front-facing.
	descDSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// Stencil operations if pixel is back-facing.
	descDSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = g_pd3dDevice->CreateDepthStencilState(&descDSS, &g_pDepthStencilState);
	FAIL_CHECK(hr);

	// Create the depth disabled stencil state
	D3D11_DEPTH_STENCIL_DESC descDDisabledSS;
	ZeroMemory(&descDDisabledSS, sizeof(descDDisabledSS));
	descDDisabledSS.DepthEnable = false;
	descDDisabledSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDDisabledSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDDisabledSS.StencilEnable = true;
	descDDisabledSS.StencilReadMask = 0xFF;
	descDDisabledSS.StencilWriteMask = 0xFF;
	
	// Stencil operations if pixel is front-facing.
	descDDisabledSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDDisabledSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	descDDisabledSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDDisabledSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// Stencil operations if pixel is back-facing.
	descDDisabledSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDDisabledSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	descDDisabledSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDDisabledSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = g_pd3dDevice->CreateDepthStencilState(&descDDisabledSS, &g_pDepthDisabledStencilState);
	FAIL_CHECK(hr);

	// Setup the viewport - topleft(0,0), bottomright(1,1)
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, // number of vp will be set
		&vp);

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Init application
	FAIL_CHECK(InitApp());

	//// create shader resource view
	//hr = SetupTransformSRV(); 
	//if( FAILED( hr ) )
	//    return hr;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Init Camera
//--------------------------------------------------------------------------------------
HRESULT InitCamera()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR initPos;
	initPos.m128_f32[0] = 0.f;
	initPos.m128_f32[1] = 0.f;
	initPos.m128_f32[2] = -100.f;
	g_Camera.SetPosition(initPos);
	g_Camera.SetViewMatrix();

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 10000.0f);

	g_OrthoMatrix = XMMatrixOrthographicLH(width, height, 0.01f, 10000.0f);

	g_TRSMatrix = XMMatrixIdentity();

	return hr;
}

//--------------------------------------------------------------------------------------
// Init Shaders
//--------------------------------------------------------------------------------------
HRESULT InitVertexShaders()
{
	HRESULT hr = S_OK;

	// Compile the vertex shaders
	ID3DBlob
		*pVSBlobL0 = nullptr,
		*pVSBlobL1 = nullptr,
		*pVSBlobL2 = nullptr,
		*pVSBlobL3 = nullptr,
		*pVSBlobL4 = nullptr,
		*pVSBlobL5 = nullptr,
		*pVSBlobL6 = nullptr,
		*pVSBlobL7 = nullptr;// ,
		//*pVSBlobL8 = nullptr;

	for (UINT i = 0; i < 8; ++i)
	{
		std::string shaderName = "Shader/VertexShader/VertexShaderLayout";
		shaderName += (48 + i);
		shaderName += ".hlsl";

		ID3DBlob **targetVS = nullptr;
		switch (i)
		{
		case 0: targetVS = &pVSBlobL0; break;
		case 1: targetVS = &pVSBlobL1; break;
		case 2: targetVS = &pVSBlobL2; break;
		case 3: targetVS = &pVSBlobL3; break;
		case 4: targetVS = &pVSBlobL4; break;
		case 5: targetVS = &pVSBlobL5; break;
		case 6: targetVS = &pVSBlobL6; break;
		case 7: targetVS = &pVSBlobL7; break;
		//case 8: targetVS = &pVSBlobL8; break;
		}

		hr = CompileShaderFromFile(VERTEX_SHADER, shaderName.c_str(), "vs_main", "vs_5_0", &g_pd3dDevice, &(*targetVS), &g_pvsLayout[i]);
		shaderName += " cannot be compiled.  Please run this executable from the directory that contains the FX file.";
		FAIL_CHECK_WITH_MSG(hr, shaderName.c_str());
	}

	// Define the input layout
	// after load fbx successfully, then set the layout.
	// need to figure out which layout they are
	LAYOUT input_layout;
	for (UINT i = 0; i < NUMBER_OF_MODELS; ++i)
	{
		for (UINT j = 0; j < g_pFbxDX11[i]->GetMeshNodeCount(); ++j)
		{
			eLayout layout_type = g_pFbxDX11[i]->GetLayoutType(j);
			switch (layout_type)
			{
			case eLayout::LAYOUT0:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL0->GetBufferPointer(), pVSBlobL0->GetBufferSize(),
					input_layout.LAYOUT0, 2);
				break;

			case eLayout::LAYOUT1:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL1->GetBufferPointer(), pVSBlobL1->GetBufferSize(),
					input_layout.LAYOUT1, 3);
				break;

			case eLayout::LAYOUT2:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL2->GetBufferPointer(), pVSBlobL2->GetBufferSize(),
					input_layout.LAYOUT2, 4);
				break;

			case eLayout::LAYOUT3:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL3->GetBufferPointer(), pVSBlobL3->GetBufferSize(),
					input_layout.LAYOUT3, 5);
				break;

			case eLayout::LAYOUT4:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL4->GetBufferPointer(), pVSBlobL4->GetBufferSize(),
					input_layout.LAYOUT4, 4);
				break;

			case eLayout::LAYOUT5:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL5->GetBufferPointer(), pVSBlobL5->GetBufferSize(),
					input_layout.LAYOUT5, 5);
				break;

			case eLayout::LAYOUT6:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL6->GetBufferPointer(), pVSBlobL6->GetBufferSize(),
					input_layout.LAYOUT6, 6);
				break;

			case eLayout::LAYOUT7:
				hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
					pVSBlobL7->GetBufferPointer(), pVSBlobL7->GetBufferSize(),
					input_layout.LAYOUT7, 7);
				break;

			//case eLayout::LAYOUTT:
			//	hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
			//		pVSBlobL8->GetBufferPointer(), pVSBlobL8->GetBufferSize(),
			//		input_layout.LAYOUT_TEX, 2);
			//	break;
			}
		}
	}
	FAIL_CHECK(hr);

	SAFE_RELEASE(pVSBlobL0);
	SAFE_RELEASE(pVSBlobL1);
	SAFE_RELEASE(pVSBlobL2);
	SAFE_RELEASE(pVSBlobL3);
	SAFE_RELEASE(pVSBlobL4);
	SAFE_RELEASE(pVSBlobL5);
	SAFE_RELEASE(pVSBlobL6);
	SAFE_RELEASE(pVSBlobL7);

	return hr;
}

HRESULT InitPixelShaders()
{
	HRESULT hr = S_OK;

	// Compile the pixel shaders
	ID3DBlob
		*pPSBlobL0 = nullptr,
		*pPSBlobL1 = nullptr,
		*pPSBlobL2 = nullptr,
		*pPSBlobL3 = nullptr,
		*pPSBlobL4 = nullptr,
		*pPSBlobL5 = nullptr,
		*pPSBlobL6 = nullptr,
		*pPSBlobL7 = nullptr;

	for (UINT i = 0; i < 8; ++i)
	{
		std::string shaderName = "Shader/PixelShader/PixelShaderLayout";
		shaderName += (48 + i);
		shaderName += ".hlsl";

		ID3DBlob **targetPS = nullptr;
		switch (i)
		{
		case 0: targetPS = &pPSBlobL0; break;
		case 1: targetPS = &pPSBlobL1; break;
		case 2: targetPS = &pPSBlobL2; break;
		case 3: targetPS = &pPSBlobL3; break;
		case 4: targetPS = &pPSBlobL4; break;
		case 5: targetPS = &pPSBlobL5; break;
		case 6: targetPS = &pPSBlobL6; break;
		case 7: targetPS = &pPSBlobL7; break;
		}
		hr = CompileShaderFromFile(PIXEL_SHADER, shaderName.c_str(), "PS", "ps_5_0", &g_pd3dDevice, &(*targetPS), nullptr, &g_ppsLayout[i]);
		shaderName += " cannot be compiled.  Please run this executable from the directory that contains the FX file.";
		FAIL_CHECK_WITH_MSG(hr, shaderName.c_str());
	}
	FAIL_CHECK(hr);

	SAFE_RELEASE(pPSBlobL0);
	SAFE_RELEASE(pPSBlobL1);
	SAFE_RELEASE(pPSBlobL2);
	SAFE_RELEASE(pPSBlobL3);
	SAFE_RELEASE(pPSBlobL4);
	SAFE_RELEASE(pPSBlobL5);
	SAFE_RELEASE(pPSBlobL6);
	SAFE_RELEASE(pPSBlobL7);

	return hr;
}

//--------------------------------------------------------------------------------------
// Create Buffers
//--------------------------------------------------------------------------------------
HRESULT CreateBuffers()
{
	HRESULT hr = S_OK; 

	// Create Buffer - For Matrices
	D3D11_BUFFER_DESC mtxBufferDesc;
	ZeroMemory(&mtxBufferDesc, sizeof(mtxBufferDesc));
	mtxBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mtxBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	mtxBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mtxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&mtxBufferDesc, nullptr, &g_pmtxBuffer);
	FAIL_CHECK(hr);

	D3D11_BUFFER_DESC mtxPaletteBufferDesc;
	ZeroMemory(&mtxPaletteBufferDesc, sizeof(mtxPaletteBufferDesc));
	mtxPaletteBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mtxPaletteBufferDesc.ByteWidth = sizeof(PaletteBufferType);
	mtxPaletteBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mtxPaletteBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&mtxPaletteBufferDesc, nullptr, &g_pmtxPaletteBuffer);
	FAIL_CHECK(hr);

	// Create Buffer - For light
	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&lightBufferDesc, nullptr, &g_plightBuffer);
	FAIL_CHECK(hr);

	return hr;
}

//--------------------------------------------------------------------------------------
// Load models, Init buffers, Create layouts, Compile shaders
//--------------------------------------------------------------------------------------
HRESULT InitApp()
{
	HRESULT hr = S_OK;

	// create the render to texture obj
	g_pRenderTexture = new RenderTexture;// [MAX_RENDERTARGET];
	if (!g_pRenderTexture)
		return E_FAIL;
	// initialize the render to texture obj
	if (!g_pRenderTexture->Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		MessageBox(nullptr, "Could not initialize the render texture object.", "Error", MB_OK);
		return E_FAIL;
	}
	//for (UINT i = 0; i < MAX_RENDERTARGET; ++i)
	//{
	//	if (!g_pRenderTexture[i].Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT))
	//		return E_FAIL;
	//}

	// create debug window obj
	g_pDebugWindow = new DebugWindow;
	if (!g_pDebugWindow)
		return E_FAIL;

	// initialize the debug window obj.
	if (!g_pDebugWindow->Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT, 100, 100))
	{
		MessageBox(nullptr, "Could not initialize the debug window object.", "Error", MB_OK);
		return false;
	}
	
	// create texture class obj
	g_pTextureShader = new TextureShader;
	if (!g_pTextureShader)
		return E_FAIL;
	
	// initialize the debug window obj.
	if (!g_pTextureShader->Initialize(g_pd3dDevice, g_hWnd))
	{
		MessageBox(nullptr, "Could not initialize the debug window object.", "Error", MB_OK);
		return false;
	}

	// Load fbx model
	for (DWORD i = 0; i < NUMBER_OF_MODELS; ++i)
	{
		// this is the place where fbx file loaded
		g_pFbxDX11[i] = new ursine::CFBXRenderDX11;
		hr = g_pFbxDX11[i]->LoadFBX(g_files[i], g_pd3dDevice);
		FAIL_CHECK_WITH_MSG(hr, "Load FBX Error");
	}

	// Init Shaders
	FAIL_CHECK_WITH_MSG( InitVertexShaders(), "Vertex Shader initialize failed" );
	FAIL_CHECK_WITH_MSG( InitPixelShaders(), "Pixel Shader initialize failed" );

	// Create Buffers
	FAIL_CHECK_WITH_MSG( CreateBuffers(), "Buffer creation failed" );

	// rasterizer
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = FALSE;
	g_pd3dDevice->CreateRasterizerState(&rsDesc, &g_pRS);
	g_pImmediateContext->RSSetState(g_pRS);

	// blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;      ///tryed D3D11_BLEND_ONE ... (and others desperate combinations ... )
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;     ///tryed D3D11_BLEND_ONE ... (and others desperate combinations ... )
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pd3dDevice->CreateBlendState(&blendDesc, &g_pBlendState);

	return hr;
}

// Setup shader resource view
HRESULT SetupTransformSRV()
{
	HRESULT hr = S_OK;
	//const uint32_t count = g_InstanceMAX;
	const uint32_t stride = static_cast<uint32_t>(sizeof(SRVPerInstanceData));

	// Create StructuredBuffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = stride;// *count;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = stride;

	// create transformStructuredBuffer
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pTransformStructuredBuffer);
	FAIL_CHECK(hr);

	// Create ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = 1;// count;
	hr = g_pd3dDevice->CreateShaderResourceView(g_pTransformStructuredBuffer, &srvDesc, &g_pTransformSRV);
	FAIL_CHECK(hr);

	return hr;
}

// Clear application
void CleanupApp()
{
	SAFE_RELEASE(g_pTransformSRV);
	SAFE_RELEASE(g_pTransformStructuredBuffer);
	SAFE_RELEASE(g_pBlendState);

	for (UINT i = 0; i < NUMBER_OF_MODELS; ++i)
		SAFE_DELETE(g_pFbxDX11[i]);

	SAFE_RELEASE(g_pmtxBuffer);
	SAFE_RELEASE(g_pmtxPaletteBuffer);
	SAFE_RELEASE(g_plightBuffer);

	SAFE_RELEASE(g_pRS);
	//SAFE_RELEASE(g_pvsInstancing);
	for (UINT i = 0; i < 8; ++i)
	{
		SAFE_RELEASE(g_pvsLayout[i]);
		SAFE_RELEASE(g_ppsLayout[i]);
	}
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	SAFE_RELEASE(g_pDepthDisabledStencilState);
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pDepthStencil);
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pRenderTargetView);
	if (g_pTextureShader)
	{
		g_pTextureShader->Shutdown();
		SAFE_DELETE(g_pTextureShader);
	}
	if (g_pDebugWindow)
	{
		g_pDebugWindow->Shutdown();
		SAFE_DELETE(g_pDebugWindow);
		//for (UINT i = 0; i < MAX_RENDERTARGET; ++i)
		//	g_pRenderTexture[i].Shutdown();
		//SAFE_DELETE_ARRAY(g_pRenderTexture);
	}
	if (g_pRenderTexture)
	{
		g_pRenderTexture->Shutdown();
		SAFE_DELETE(g_pRenderTexture);
		//for (UINT i = 0; i < MAX_RENDERTARGET; ++i)
		//	g_pRenderTexture[i].Shutdown();
		//SAFE_DELETE_ARRAY(g_pRenderTexture);
	}
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pd3dDevice);
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static float rot_Angle[3] = { 0.0f, };
	static float tsl_dist[3] = { 0, };
	XMMATRIX tsl;
	XMMATRIX rot;
	XMMATRIX scl;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

		// camera control or model pos/rot control
	case WM_KEYDOWN:
		switch (wParam)
		{
			// left right back and forth
		case VK_LEFT:
			tsl_dist[0] -= 5.0f;
			break;
		case VK_RIGHT:
			tsl_dist[0] += 5.0f;
			break;
		case VK_UP:
			tsl_dist[2] += 5.0f;
			break;
		case VK_DOWN:
			tsl_dist[2] -= 5.0f;
			break;
	
			// x-Axis rot
		case 'w':
		case 'W':
			rot_Angle[0] += 0.1f;
			break;
		case 's':
		case 'S':
			rot_Angle[0] -= 0.1f;
			break;
			// y-Axis rot
		case 'a':
		case 'A':
			rot_Angle[1] += 0.1f;
			break;
		case 'd':
		case 'D':
			rot_Angle[1] -= 0.1f;
			break;
		}
		if (rot_Angle[0] >= 360.f)
			rot_Angle[0] = 0.0f;
		if (rot_Angle[1] >= 360.f)
			rot_Angle[1] = 0.0f;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	tsl = XMMatrixTranslation(tsl_dist[0], tsl_dist[1], tsl_dist[2]);	
	rot = XMMatrixRotationRollPitchYaw(rot_Angle[0], rot_Angle[1], rot_Angle[2]);
	g_TRSMatrix = rot * tsl;

	return 0;
}

//--------------------------------------------------------------------------------------
// Render the model
//--------------------------------------------------------------------------------------
void RenderModel()
{
	// for all model
	for (UINT mdl_idx = 0; mdl_idx < NUMBER_OF_MODELS; ++mdl_idx)
	{
		auto &currModel = g_pFbxDX11[mdl_idx];

		size_t meshnodeCnt = currModel->GetMeshNodeCount();

		// for all nodes
		for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
		{
			//////////////////////////////////////
			// sort by layout later
			//////////////////////////////////////
			eLayout layout_type = currModel->GetLayoutType(mn_idx);
			ID3D11VertexShader* pVS = nullptr;
			ID3D11PixelShader* pPS = nullptr;
			switch (layout_type)
			{
			case eLayout::NONE_LAYOUT:		continue;
			case eLayout::LAYOUT0:	pVS = g_pvsLayout[0];	pPS = g_ppsLayout[0]; break;
			case eLayout::LAYOUT1:	pVS = g_pvsLayout[1];	pPS = g_ppsLayout[1]; break;
			case eLayout::LAYOUT2:	pVS = g_pvsLayout[2];	pPS = g_ppsLayout[2]; break;
			case eLayout::LAYOUT3:	pVS = g_pvsLayout[3];	pPS = g_ppsLayout[3]; break;
			case eLayout::LAYOUT4:	pVS = g_pvsLayout[4];	pPS = g_ppsLayout[4]; break;
			case eLayout::LAYOUT5:	pVS = g_pvsLayout[5];	pPS = g_ppsLayout[5]; break;
			case eLayout::LAYOUT6:	pVS = g_pvsLayout[6];	pPS = g_ppsLayout[6]; break;
			case eLayout::LAYOUT7:	pVS = g_pvsLayout[7];	pPS = g_ppsLayout[7]; break;
			//case eLayout::LAYOUTT:	pVS = g_pvsLayout[8];	pPS = g_ppsLayout[8]; break;
			}

			g_pImmediateContext->VSSetShader(pVS, nullptr, 0);
			g_pImmediateContext->PSSetShader(pPS, nullptr, 0);

			// set shader parameters(mapping constant buffers, matrices, resources)
			SetShaderParameters(&currModel, mn_idx, layout_type);

			// render node
			currModel->RenderNode(g_pImmediateContext, mn_idx);

			// reset shader
			g_pImmediateContext->VSSetShader(nullptr, nullptr, 0);
			g_pImmediateContext->PSSetShader(nullptr, nullptr, 0);
		}
	}
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
		t += (float)XM_PI * 0.0125f;
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Pass1 - Render to texture
	RenderToTexture();

	// Pass2 - Render to back buffer
	RenderScene();

	// turn z buffer off
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthDisabledStencilState, 1);

	// render the debug window on 50x50 posittion
	g_pDebugWindow->Render(g_pImmediateContext, 50, 50);

	//D3D11_VIEWPORT viewportArray[16];
	//UINT outputVPCount = 16;
	//g_pImmediateContext->RSGetViewports(&outputVPCount, viewportArray);
	//
	//D3D11_VIEWPORT newViewport;
	//newViewport.Width = 50;
	//newViewport.Height = 50;
	//newViewport.MaxDepth = 1;
	//newViewport.MinDepth = 0;
	//newViewport.TopLeftX = 10;
	//newViewport.TopLeftY = 10;
	//
	//g_pImmediateContext->RSSetViewports(1, &newViewport);

	// Render the debug window using the texture shader.
	if (!g_pTextureShader->Render(g_pImmediateContext, g_pDebugWindow->GetIndexCount(), 
		g_World, 
		g_Camera.GetViewMatrix(),
		g_OrthoMatrix,
		g_pRenderTexture->GetShaderResourceView()))
	{
		MessageBox(nullptr, "failed to render debug window by using Texture Shader", "Error", MB_OK);
		return;
	}

	//
	//g_pImmediateContext->RSSetViewports(1, viewportArray);

	// turn z buffer on
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 1);
	
	// Present our back buffer to our front buffer
	g_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Render the texture
//--------------------------------------------------------------------------------------
void RenderScene()
{
	// Clear the back bufferw
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Clear the depth buffer to 1.0 (max depth)
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//// Set Blend Factors
	//float blendFactors[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	//g_pImmediateContext->RSSetState(g_pRS);
	//g_pImmediateContext->OMSetBlendState(g_pBlendState, blendFactors, 0xffffffff);
	//g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);
	
	RenderModel();
}

// seems like debug window texture is not painted
void RenderToTexture()
{
	g_pRenderTexture->SetRenderTarget(g_pImmediateContext, g_pDepthStencilView);
	//for (auto i = 0; i < MAX_RENDERTARGET; ++i)
	//{
	//	if (i == MAX_RENDERTARGET - 1)
	//		g_pRenderTexture[i].SetRenderTarget(g_pImmediateContext, g_pDepthStencilView);
	//	else
	//		g_pRenderTexture[i].SetRenderTarget(g_pImmediateContext, g_pDepthStencilView);
	//}

	// Clear the back buffer with Color rgba
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float Depth[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	g_pRenderTexture->ClearRenderTarget(g_pImmediateContext, g_pDepthStencilView, Blue);
	//for (auto i = 0; i < MAX_RENDERTARGET; ++i)
	//{
	//	if (i == MAX_RENDERTARGET - 1)
	//		g_pRenderTexture[i].ClearRenderTarget(g_pImmediateContext, g_pDepthStencilView, Depth);
	//	else
	//		g_pRenderTexture[i].ClearRenderTarget(g_pImmediateContext, g_pDepthStencilView, ClearColor);
	//}
	
	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}

//--------------------------------------------------------------------------------------
// Set Shader Parameters
//--------------------------------------------------------------------------------------
bool SetShaderParameters(ursine::CFBXRenderDX11** currentModel, const UINT& mesh_index, const eLayout& layoutType)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT hr;

	//--------------------------------------------------------------------------------------
	// Vertex Shader Parameters
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// setting matrices
	//--------------------------------------------------------------------------------------
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pmtxBuffer);
	// world, view, projection, WVP Matrices & material
	{
		// 이게 여기에 있어야만 하는지, 라이팅 셰이더를 이 이후에 그릴 때에 다시 이걸 세이더에 전달해줘야 하는지 궁금하군
		// 일단 라이팅 여기에다 넣어서 되는지 확인해.
		hr = g_pImmediateContext->Map(g_pmtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		FAIL_CHECK_BOOLEAN(hr);

		MatrixBufferType* mtxBuffer = (MatrixBufferType*)MappedResource.pData;

		// WVP
		mtxBuffer->mWorld = XMMatrixTranspose(g_TRSMatrix * g_World);
		mtxBuffer->mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
		mtxBuffer->mProj = XMMatrixTranspose(g_Projection);

		// xm matrix - row major
		// hlsl - column major
		// that's why we should transpose this
		mtxBuffer->mWVP = XMMatrixTranspose(g_TRSMatrix * g_World * g_Camera.GetViewMatrix() * g_Projection);

		// unmap constant buffer
		g_pImmediateContext->Unmap(g_pmtxBuffer, 0);
	}

	//--------------------------------------------------------------------------------------
	// matrix palette
	//--------------------------------------------------------------------------------------
	{
		if ((*currentModel)->IsSkinned())
			g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pmtxPaletteBuffer);	// setting matrix palettes

		if (eLayout::LAYOUT4 == layoutType || 
			eLayout::LAYOUT5 == layoutType ||
			eLayout::LAYOUT6 == layoutType || 
			eLayout::LAYOUT7 == layoutType)
		{
			hr = g_pImmediateContext->Map(g_pmtxPaletteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			PaletteBufferType* palBuffer = (PaletteBufferType*)MappedResource.pData;
			FAIL_CHECK_BOOLEAN(hr);

			(*currentModel)->UpdateMatPal(&palBuffer->matPal[0]);

			g_pImmediateContext->Unmap(g_pmtxPaletteBuffer, 0);
		}
	}

	if (g_pTransformSRV)	g_pImmediateContext->VSSetShaderResources(0, 1, &g_pTransformSRV);

	//--------------------------------------------------------------------------------------
	// Pixel Shader Parameters : materials & lights
	//--------------------------------------------------------------------------------------
	{
		//--------------------------------------------------------------------------------------
		// materials
		//--------------------------------------------------------------------------------------
		// should be changed to get specific materials according to specific material id 
		// to make this possible, we need to build up the structure of subsets
		{
			Material_Data material = (*currentModel)->GetNodeFbxMaterial(mesh_index);

			// set sampler
			if (material.pSampler)	g_pImmediateContext->PSSetSamplers(0, 1, &material.pSampler);
			//if (material.pSampler[0])	g_pImmediateContext->PSSetSamplers(0, 1, &material.pSampler[0]);
			//if (material.pSampler[1])	g_pImmediateContext->PSSetSamplers(1, 1, &material.pSampler[1]);

			// set shader resource view - for texture
			if (material.pSRV[0])		g_pImmediateContext->PSSetShaderResources(0, 1, &material.pSRV[0]);
			if (material.pSRV[1])		g_pImmediateContext->PSSetShaderResources(1, 1, &material.pSRV[1]);
			
			// set constant buffer for material
			if (material.pMaterialCb)
			{
				//g_pImmediateContext->UpdateSubresource(material.pMaterialCb, 0, nullptr, &material.mtrlConst, 0, 0);
				g_pImmediateContext->PSSetConstantBuffers(0, 1, &material.pMaterialCb);	// setting materials constant buffer

				hr = g_pImmediateContext->Map(material.pMaterialCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				MaterialBufferType* mtrlBuffer = (MaterialBufferType*)MappedResource.pData;
				FAIL_CHECK_BOOLEAN(hr);

				mtrlBuffer->ambient			= material.mtrlConst.ambient;
				mtrlBuffer->diffuse			= material.mtrlConst.diffuse;
				mtrlBuffer->specular		= material.mtrlConst.specular;
				mtrlBuffer->emissive		= material.mtrlConst.emissive;
				mtrlBuffer->shineness		= 1.0f;// = material.mtrlConst.shineness;
				mtrlBuffer->transparency	= 1.0f;// = material.mtrlConst.transparency;

				g_pImmediateContext->Unmap(material.pMaterialCb, 0);
			}
		}

		//--------------------------------------------------------------------------------------
		// light
		//--------------------------------------------------------------------------------------
		{
			g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_plightBuffer);		// setting lights
			
			hr = g_pImmediateContext->Map(g_plightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			LightBufferType* lightBuffer = (LightBufferType*)MappedResource.pData;
			FAIL_CHECK_BOOLEAN(hr);
			
			lightBuffer->ambientColor	= urColor::White;
			lightBuffer->diffuseColor	= urColor::White;
			lightBuffer->specularColor	= urColor::White;
			lightBuffer->emissiveColor	= urColor::White;
			lightBuffer->lightDirection = ursine::SVec3(0.f, 0.f, 1.f);			

			g_pImmediateContext->Unmap(g_plightBuffer, 0);
		}
	}

	return true;
}