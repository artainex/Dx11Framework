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
#include <LightClass.h>
#include "CFBXRendererDX11.h"

using namespace DirectX;
using namespace ursine::FBX_DATA;
typedef ursine::Color urColor;

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

#define SAFE_RELEASE(pt) if( nullptr != pt )\
{ pt->Release(); pt = nullptr; }			\

#define SAFE_DELETE(pt) if( nullptr != pt )	\
{ delete pt; pt = nullptr; }				\

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11DepthStencilState*			g_pDepthStencilState = NULL;

XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Update(double deltaTime);
void Render();
bool SetShaderParameters(ursine::CFBXRenderDX11** currentModel, const UINT& mesh_index, const eLayout& layoutType);
UINT updateSpeed = 1;

const UINT	NUMBER_OF_MODELS = 1;

HRESULT InitApp();
void CleanupApp();
ursine::CFBXRenderDX11*	g_pFbxDX11[NUMBER_OF_MODELS];

// FBX file
char g_files[NUMBER_OF_MODELS][256] =
{
	//"Assets/Models/stanford_bunny.fbx"
	"Assets/Animations/Player/Player_Idle.fbx"
};

std::vector<XMMATRIX> skin_mat;

struct MatrixBufferType
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProj;
	XMMATRIX mWVP;
};

struct PaletteBufferType
{
	XMMATRIX matPal[96];
};

struct LightBufferType
{
	// For now, try use phong model, use ursine LightClass if I understand HDR or more (this class doesn't have HDR)
	urColor diffuseColor;
	urColor ambientColor;
	urColor specularColor;
	urColor emissiveColor;
	ursine::SVec3 lightDirection;
	float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
};

ID3D11BlendState*				g_pBlendState = nullptr;
ID3D11RasterizerState*			g_pRS = nullptr;
ID3D11Buffer*					g_pmtxBuffer = nullptr;
ID3D11Buffer*					g_pmtxPaletteBuffer = nullptr;
ID3D11Buffer*					g_plightBuffer = nullptr;
ID3D11VertexShader*				g_pvsStatic = nullptr;
ID3D11VertexShader*             g_pvsSkinned = nullptr;
ID3D11PixelShader*              g_pps = nullptr;

// Instancing
bool	g_bInstancing = false;
const uint32_t g_InstanceMAX = 32;
ID3D11VertexShader*				g_pvsInstancing = nullptr;

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

//--------------------------------------------------------------------------------------
// task list
// 1. Deferred Rendering Pipeline
// 2. 노멀 맵핑이랑 퐁 라이팅 적용해두기
// 3. G-buffer란 무엇인가? 셰도우 맵핑 적용할 준비를 할 것.
// 4. 여러 오브젝트를 로딩할 수 있는가? 인스턴싱이 아니라.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupApp();
		CleanupDevice();
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.lpszClassName = "TutorialWindowClass";
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(
		"TutorialWindowClass",
		"Direct3D 11 FBX Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

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
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, //adaptor?
			g_driverType, // driver-type
			NULL, // flag
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
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(
		0, // back buffer index
		__uuidof(ID3D11Texture2D), // interface that access to back buffer
		(LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(
		pBackBuffer, // resource that view will access
		NULL, // def of rendertargetview
		&g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))  return hr;

	// setting rendertargetview & depth-stencil buffer 
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC descDSS;
	ZeroMemory(&descDSS, sizeof(descDSS));
	descDSS.DepthEnable = TRUE;
	descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDSS.StencilEnable = FALSE;
	hr = g_pd3dDevice->CreateDepthStencilState(&descDSS, &g_pDepthStencilState);

	// Setup the viewport - topleft(0,0), bottomright(1,1)
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, // number of vp will be set
		&vp);

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Init application
	hr = InitApp();
	if (FAILED(hr))
		return hr;

	//// create shader resource view
	//hr = SetupTransformSRV(); 
	//if( FAILED( hr ) )
	//    return hr;

	return S_OK;
}

HRESULT InitApp()
{
	HRESULT hr = S_OK;

	for (DWORD i = 0; i < NUMBER_OF_MODELS; ++i)
	{
		// this is the place where fbx file loaded
		g_pFbxDX11[i] = new ursine::CFBXRenderDX11;
		hr = g_pFbxDX11[i]->LoadFBX(g_files[i], g_pd3dDevice);
		FAIL_CHECK_WITH_MSG(hr, "Load FBX Error");
	}

	// Compile the vertex shader
	ID3DBlob* pVSBlobStatic = NULL, *pVSBlobSkinned = NULL, *pVSBlobInstancing = NULL;
	hr = CompileShaderFromFile("simpleRenderVSStatic.hlsl", "vs_main", "vs_5_0", &pVSBlobStatic);
	FAIL_CHECK_WITH_MSG(hr, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.");

	// Create the vertex shader - static
	hr = g_pd3dDevice->CreateVertexShader(pVSBlobStatic->GetBufferPointer(), pVSBlobStatic->GetBufferSize(), NULL, &g_pvsStatic);
	if (FAILED(hr))
	{
		pVSBlobStatic->Release();
		return hr;
	}

	// Compile the vertex shader
	hr = CompileShaderFromFile("simpleRenderVSSkinned.hlsl", "vs_main", "vs_5_0", &pVSBlobSkinned);
	FAIL_CHECK_WITH_MSG(hr, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.");

	// Create the vertex shader - Skinned
	hr = g_pd3dDevice->CreateVertexShader(pVSBlobSkinned->GetBufferPointer(), pVSBlobSkinned->GetBufferSize(), NULL, &g_pvsSkinned);
	if (FAILED(hr))
	{
		pVSBlobSkinned->Release();
		return hr;
	}

	// Compile the vertex shader
	hr = CompileShaderFromFile("simpleRenderInstancingVS.hlsl", "vs_main", "vs_5_0", &pVSBlobInstancing);
	FAIL_CHECK_WITH_MSG(hr, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.");

	// Create the vertex shader - Instance
	hr = g_pd3dDevice->CreateVertexShader(pVSBlobInstancing->GetBufferPointer(), pVSBlobInstancing->GetBufferSize(), NULL, &g_pvsInstancing);
	if (FAILED(hr))
	{
		pVSBlobInstancing->Release();
		return hr;
	}

	// Define the input layout	
	// Todo: InputLayout
	// after load fbx successfully, then set the layout.
	// need to figure out which layout they are
	LAYOUT input_layout;
	for (UINT i = 0; i < NUMBER_OF_MODELS; ++i)
	{
		eLayout layout_type = g_pFbxDX11[i]->GetLayoutType(i);
		switch (layout_type)
		{
			// static mesh - currently instancing, but don't need actually
		case eLayout::STATIC:
			hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
				pVSBlobStatic->GetBufferPointer(),
				pVSBlobStatic->GetBufferSize(),
				input_layout.STATIC_LAYOUT,
				3);
			break;

			// skinned mesh
		case eLayout::SKINNED:
			hr = g_pFbxDX11[i]->CreateInputLayout(g_pd3dDevice,
				pVSBlobSkinned->GetBufferPointer(),
				pVSBlobSkinned->GetBufferSize(),
				input_layout.SKINNED_LAYOUT,
				5);
			break;
		}
	}

	if (pVSBlobStatic) pVSBlobStatic->Release();
	if (pVSBlobSkinned) pVSBlobSkinned->Release();
	if (pVSBlobInstancing) pVSBlobInstancing->Release();
	FAIL_CHECK(hr);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile("simpleRenderPS.hlsl", "PS", "ps_5_0", &pPSBlob);
	FAIL_CHECK_WITH_MSG(hr, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.");

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pps);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Create Constant Buffer - For Matrices
	D3D11_BUFFER_DESC mtxBufferDesc;
	ZeroMemory(&mtxBufferDesc, sizeof(mtxBufferDesc));
	mtxBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mtxBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	mtxBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mtxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&mtxBufferDesc, NULL, &g_pmtxBuffer);
	FAIL_CHECK(hr);

	D3D11_BUFFER_DESC mtxPaletteBufferDesc;
	ZeroMemory(&mtxPaletteBufferDesc, sizeof(mtxPaletteBufferDesc));
	mtxPaletteBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mtxPaletteBufferDesc.ByteWidth = sizeof(PaletteBufferType);
	mtxPaletteBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mtxPaletteBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&mtxPaletteBufferDesc, NULL, &g_pmtxPaletteBuffer);
	FAIL_CHECK(hr);

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer(&lightBufferDesc, NULL, &g_plightBuffer);
	FAIL_CHECK(hr);

	//
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = FALSE;
	g_pd3dDevice->CreateRasterizerState(&rsDesc, &g_pRS);
	g_pImmediateContext->RSSetState(g_pRS);

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
	const uint32_t count = g_InstanceMAX;
	const uint32_t stride = static_cast<uint32_t>(sizeof(SRVPerInstanceData));

	// Create StructuredBuffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = stride * count;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = stride;

	// create transformStructuredBuffer
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pTransformStructuredBuffer);
	FAIL_CHECK(hr);

	// Create ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = count;
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
	SAFE_RELEASE(g_pvsInstancing);
	SAFE_RELEASE(g_pvsSkinned);
	SAFE_RELEASE(g_pvsStatic);
	SAFE_RELEASE(g_pps);
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pDepthStencil);
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pRenderTargetView);
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

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 15.f, -50.f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 15.f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 10000.0f);

	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Clear the back buffer
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Clear the depth buffer to 1.0 (max depth)
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Set Blend Factors
	float blendFactors[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	g_pImmediateContext->RSSetState(g_pRS);
	g_pImmediateContext->OMSetBlendState(g_pBlendState, blendFactors, 0xffffffff);
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState, 0);

	// Pass1
	// for all model
	for (UINT mdl_idx = 0; mdl_idx < NUMBER_OF_MODELS; ++mdl_idx)
	{
		auto &currModel = g_pFbxDX11[mdl_idx];

		size_t meshnodeCnt = currModel->GetMeshNodeCount();

		// for all nodes
		for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
		{
			//auto &currMesh = g_pFbxDX11[mdl_idx]->GetMeshNode(mn_idx);

			//////////////////////////////////////
			// sort by layout later
			//////////////////////////////////////
			eLayout layout_type = currModel->GetLayoutType(mn_idx);
			ID3D11VertexShader* pVS = nullptr;
			switch (layout_type)
			{
			case eLayout::NONE:		continue;
			case eLayout::STATIC:	pVS = g_pvsStatic;	break;
			case eLayout::SKINNED:	pVS = g_pvsSkinned;	break;
			}

			g_pImmediateContext->VSSetShader(pVS, NULL, 0);
			g_pImmediateContext->PSSetShader(g_pps, NULL, 0);

			// set shader parameters(mapping constant buffers, matrices, resources)
			SetShaderParameters(&currModel, mn_idx, layout_type);

			// render node
			currModel->RenderNode(g_pImmediateContext, mn_idx);

			// reset shader
			g_pImmediateContext->VSSetShader(NULL, NULL, 0);
			g_pImmediateContext->PSSetShader(NULL, NULL, 0);
		}
	}

	// Present our back buffer to our front buffer
	g_pSwapChain->Present(0, 0);
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
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pmtxBuffer);				// setting matrices
	
	if ((*currentModel)->IsSkinned())
		g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pmtxPaletteBuffer);	// setting matrix palettes

	// world, view, projection, WVP Matrices & material
	{
		// 이게 여기에 있어야만 하는지, 라이팅 셰이더를 이 이후에 그릴 때에 다시 이걸 세이더에 전달해줘야 하는지 궁금하군
		// 일단 라이팅 여기에다 넣어서 되는지 확인해.
		hr = g_pImmediateContext->Map(g_pmtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		FAIL_CHECK_BOOLEAN(hr);

		MatrixBufferType* mtxBuffer = (MatrixBufferType*)MappedResource.pData;

		// WVP
		mtxBuffer->mWorld = XMMatrixTranspose(g_World);
		mtxBuffer->mView = XMMatrixTranspose(g_View);
		mtxBuffer->mProj = XMMatrixTranspose(g_Projection);

		// xm matrix - row major
		// hlsl - column major
		// that's why we should transpose this
		mtxBuffer->mWVP = XMMatrixTranspose(g_World * g_View * g_Projection);

		// unmap constant buffer
		g_pImmediateContext->Unmap(g_pmtxBuffer, 0);
	}

	// matrix palette
	if (eLayout::SKINNED == layoutType)
	{
		hr = g_pImmediateContext->Map(g_pmtxPaletteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		PaletteBufferType* palBuffer = (PaletteBufferType*)MappedResource.pData;
		FAIL_CHECK_BOOLEAN(hr);

		(*currentModel)->UpdateMatPal(&palBuffer->matPal[0]);

		g_pImmediateContext->Unmap(g_pmtxPaletteBuffer, 0);
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
		Material_Data material = (*currentModel)->GetNodeFbxMaterial(mesh_index);
		if (material.pSRV)		g_pImmediateContext->PSSetShaderResources(0, 1, &material.pSRV);

		// set constant buffer for material
		if (material.pMaterialCb)
		{
			g_pImmediateContext->UpdateSubresource(material.pMaterialCb, 0, NULL, &material.materialConst, 0, 0);
			g_pImmediateContext->PSSetConstantBuffers(0, 1, &material.pMaterialCb);	// setting materials
		}

		// set sampler
		if (material.pSampler)	g_pImmediateContext->PSSetSamplers(0, 1, &material.pSampler);

		//--------------------------------------------------------------------------------------
		// light
		//--------------------------------------------------------------------------------------
		{
			g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_plightBuffer);		// setting lights
			
			hr = g_pImmediateContext->Map(g_plightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			LightBufferType* lightBuffer = (LightBufferType*)MappedResource.pData;
			FAIL_CHECK_BOOLEAN(hr);
			
			lightBuffer->ambientColor	= urColor::Red;
			lightBuffer->diffuseColor	= urColor::Red;
			lightBuffer->specularColor	= urColor::Red;
			lightBuffer->emissiveColor	= urColor::Red;
			lightBuffer->lightDirection = ursine::SVec3(0.f, 0.f, -1.f);			

			g_pImmediateContext->Unmap(g_plightBuffer, 0);
		}
	}

	return true;
}