// *********************************************************************************************************************
/// 
/// @file 		FBXLoaderSample.cpp
/// @brief		FBX Loader tester application
/// 
/// @author 	Park Hyung Jun
/// @date 		09/03/2015
/// 
// *********************************************************************************************************************

#include "Setting.h"

//--------------------------------------------------------------------------------------
// TODO list
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
HRESULT InitCamera();
HRESULT InitLight();
HRESULT InitVertexShaders();
HRESULT InitPixelShaders();
HRESULT CreateGeometryBuffers();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Update(double deltaTime);
void SetRenderTarget();
void ClearRenderTarget();
void RenderScene();
void GeometryPass();
void LightPass();
void DepthPass(ursine::Light& light);
void Render();
void RenderLightModel();
void RenderGeometryModel();
void RenderGeometryModelDepth(const ursine::Light& light);
void SetMatrix();
bool SetGeometryShaderParameters(ursine::FBXModel** currentModel, const UINT& mesh_index, const eLayout& layoutType);
bool SetLightShaderParameters(ursine::FBXModel** currentModel, const ursine::Light& light, const UINT& mesh_index);

void SetZBuffer(bool on);
void SetAlphaBlend();
void SetNoBlend();
void SetAdditiveBlend();
void SetFrontFaceCull(bool on);
void SetBackFaceCull(bool on);

HRESULT InitApp();
HRESULT InitModel();
void CleanupApp();
Models g_Models;

// Shader Resource View - was implemented for instancing
struct SRVPerInstanceData
{
	XMMATRIX mWorld;
};
HRESULT SetupTransformSRV();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	FAIL_CHECK(InitWindow(hInstance, nCmdShow));

	if (FAILED(InitDevice()))
	{
		CleanupApp();
		CleanupDevice();
		return 0;
	}

	FAIL_CHECK(InitLight());
	FAIL_CHECK(InitCamera());

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
			static float timeElapsed = 0.f;

			static std::clock_t start = std::clock();
			std::clock_t timedelta = std::clock() - start;
			float t_delta_per_msec = (timedelta * updateSpeed) / (float)(CLOCKS_PER_SEC);

			Update(t_delta_per_msec);

			// need to be reset on last frame's time
			// should change this if delta time goes over the last frame, set it as 0
			if (t_delta_per_msec >= 2.f)
				start = std::clock();

			clock_t beginFrame = std::clock();
			Render();
			clock_t endFrame = std::clock();

			timeElapsed += (endFrame - beginFrame) / (float)(CLOCKS_PER_SEC);
			++frame;
			if (timeElapsed >= 1.0f)
			{
				timeElapsed = 0.f;
				frame_per_sec = frame;
				frame = 0;
			}
		}
	}

	CleanupApp();
	CleanupDevice();

	return (int)msg.wParam;
}

void Update(double deltaTime)
{
	for (auto& iter : g_Models["GeoModel"])
	{
		iter->Update(deltaTime);
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
			&g_pDeviceContext // created device context obj
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
	SetRenderTarget();

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC descDSS;
	ZeroMemory(&descDSS, sizeof(descDSS));
	
	// Set up the description of the stencil state.
	descDSS.DepthEnable = true;
	descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDSS.StencilEnable = false;
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
	descDDisabledSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDDisabledSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDDisabledSS.StencilEnable = false;
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
	g_pDeviceContext->RSSetViewports(1, // number of vp will be set
		&vp);

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Init application
	FAIL_CHECK(InitApp());

	// create shader resource view
	if (g_bInstancing)
	{
		hr = SetupTransformSRV();
		FAIL_CHECK(hr);
	}

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
	g_Camera.SetPosition(XMFLOAT3(0.f, 0.f, -100.f));
	g_Camera.SetRotation(XMFLOAT3(0.f, 0.f, 0.f));
	g_Camera.SetLookAt(XMFLOAT3(0.f, 0.f, 1.f));
	g_Camera.Update();
	g_ScreenView = g_Camera.GetViewMatrix();

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / (float)height, SCREEN_NEAR, SCREEN_FAR);
	g_OrthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, SCREEN_NEAR, SCREEN_FAR);

	return hr;
}

//--------------------------------------------------------------------------------------
// Init Light
//--------------------------------------------------------------------------------------
HRESULT InitLight()
{
	// Init ambienet light
	g_AmbientLight.SetColor(urColor::Blue.TOXMFLOAT4());
	g_AmbientLight.SetType(ursine::LightType::LIGHT_AMBIENT);

	// Init global light
	g_GlobalLight.Initialize(
		urColor::Red.TOXMFLOAT4(), 
		ursine::LightType::LIGHT_GLOBALDIRECTIONAL,
		XMFLOAT3(-1.f, -1.f, 0.f),
		XMFLOAT3(50.f, 50.f, 0.f)
		);

	// Init local lights for diffuse and specular
	for (UINT i = 0; i < MAX_LIGHT; ++i)
	{
		double angle = i * (360.0 / MAX_LIGHT) * (XM_PI / 180.0);
		XMFLOAT3 pos = XMFLOAT3(400.f * cos(angle), 0.f, 400.f * sin(angle));
		XMFLOAT3 dir = XMFLOAT3(-pos.x, -pos.y, -pos.z);
		g_LocalLights[i].Initialize(
			urColor::White.TOXMFLOAT4(),
			ursine::LightType::LIGHT_LOCALDIRECTIONAL,
			dir, pos);
	}

	return S_OK;
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
		*pVSBlobL7 = nullptr;

	// geometry shaders
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout0.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL0, &g_pVShader[0]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout0.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout1.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL1, &g_pVShader[1]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout1.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout2.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL2, &g_pVShader[2]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout2.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout3.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL3, &g_pVShader[3]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout3.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout4.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL4, &g_pVShader[4]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout4.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout5.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL5, &g_pVShader[5]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout5.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout6.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL6, &g_pVShader[6]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout6.hlsl compile failed.");
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/VertexShaderLayout7.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &pVSBlobL7, &g_pVShader[7]);
	FAIL_CHECK_WITH_MSG(hr, "VertexShaderLayout7.hlsl compile failed.");

	// instancing vertex shader
	ID3DBlob *instanceVS = nullptr;
	hr = CompileShaderFromFile(VERTEX_SHADER, "Shader/InstancingVS.hlsl", "vs_main", "vs_5_0", &g_pd3dDevice, &instanceVS, &g_pvsInstancing);
	FAIL_CHECK_WITH_MSG(hr, "InstancingVS compile failed");

	// Define the input layout
	// after load fbx successfully, then set the layout.
	// need to figure out which layout they are
	LAYOUT input_layout;
	for(auto& iter : g_Models["GeoModel"])
	{
		for (UINT j = 0; j < iter->GetMeshNodeCount(); ++j)
		{
			eLayout layout_type = iter->GetLayoutType(j);
			switch (layout_type)
			{
			case eLayout::LAYOUT0:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL0->GetBufferPointer(), pVSBlobL0->GetBufferSize(),
					input_layout.LAYOUT0, 2);
				break;

			case eLayout::LAYOUT1:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL1->GetBufferPointer(), pVSBlobL1->GetBufferSize(),
					input_layout.LAYOUT1, 3);
				break;

			case eLayout::LAYOUT2:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL2->GetBufferPointer(), pVSBlobL2->GetBufferSize(),
					input_layout.LAYOUT2, 4);
				break;

			case eLayout::LAYOUT3:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL3->GetBufferPointer(), pVSBlobL3->GetBufferSize(),
					input_layout.LAYOUT3, 5);
				break;

			case eLayout::LAYOUT4:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL4->GetBufferPointer(), pVSBlobL4->GetBufferSize(),
					input_layout.LAYOUT4, 4);
				break;

			case eLayout::LAYOUT5:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL5->GetBufferPointer(), pVSBlobL5->GetBufferSize(),
					input_layout.LAYOUT5, 5);
				break;

			case eLayout::LAYOUT6:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL6->GetBufferPointer(), pVSBlobL6->GetBufferSize(),
					input_layout.LAYOUT6, 6);
				break;

			case eLayout::LAYOUT7:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL7->GetBufferPointer(), pVSBlobL7->GetBufferSize(),
					input_layout.LAYOUT7, 7);
				break;
			}
		FAIL_CHECK(hr);
		}
	}

	for (auto& iter : g_Models["LightModel"])
	{
		for (UINT j = 0; j < iter->GetMeshNodeCount(); ++j)
		{
			eLayout layout_type = iter->GetLayoutType(j);
			switch (layout_type)
			{
			case eLayout::LAYOUT0:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL0->GetBufferPointer(), pVSBlobL0->GetBufferSize(),
					input_layout.LAYOUT0, 2);
				break;

			case eLayout::LAYOUT1:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL1->GetBufferPointer(), pVSBlobL1->GetBufferSize(),
					input_layout.LAYOUT1, 3);
				break;

			case eLayout::LAYOUT2:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL2->GetBufferPointer(), pVSBlobL2->GetBufferSize(),
					input_layout.LAYOUT2, 4);
				break;

			case eLayout::LAYOUT3:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL3->GetBufferPointer(), pVSBlobL3->GetBufferSize(),
					input_layout.LAYOUT3, 5);
				break;

			case eLayout::LAYOUT4:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL4->GetBufferPointer(), pVSBlobL4->GetBufferSize(),
					input_layout.LAYOUT4, 4);
				break;

			case eLayout::LAYOUT5:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL5->GetBufferPointer(), pVSBlobL5->GetBufferSize(),
					input_layout.LAYOUT5, 5);
				break;

			case eLayout::LAYOUT6:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL6->GetBufferPointer(), pVSBlobL6->GetBufferSize(),
					input_layout.LAYOUT6, 6);
				break;

			case eLayout::LAYOUT7:
				hr = iter->CreateInputLayout(g_pd3dDevice,
					pVSBlobL7->GetBufferPointer(), pVSBlobL7->GetBufferSize(),
					input_layout.LAYOUT7, 7);
				break;
			}
			FAIL_CHECK(hr);
		}
	}

	SAFE_RELEASE(pVSBlobL0);
	SAFE_RELEASE(pVSBlobL1);
	SAFE_RELEASE(pVSBlobL2);
	SAFE_RELEASE(pVSBlobL3);
	SAFE_RELEASE(pVSBlobL4);
	SAFE_RELEASE(pVSBlobL5);
	SAFE_RELEASE(pVSBlobL6);
	SAFE_RELEASE(pVSBlobL7);
	SAFE_RELEASE(instanceVS);

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

	// geometry shaders
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout0.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL0, nullptr, &g_pPShader[0]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout0.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout1.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL1, nullptr, &g_pPShader[1]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout1.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout2.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL2, nullptr, &g_pPShader[2]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout2.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout3.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL3, nullptr, &g_pPShader[3]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout3.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout4.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL4, nullptr, &g_pPShader[4]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout4.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout5.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL5, nullptr, &g_pPShader[5]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout5.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout6.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL6, nullptr, &g_pPShader[6]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout6.hlsl compile failed.");
	hr = CompileShaderFromFile(PIXEL_SHADER, "Shader/PixelShaderLayout7.hlsl", "PS", "ps_5_0", &g_pd3dDevice, &pPSBlobL7, nullptr, &g_pPShader[7]);
	FAIL_CHECK_WITH_MSG(hr, "PixelShaderLayout7.hlsl compile failed.");

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
// CreateGeometryBuffers - Constant
//--------------------------------------------------------------------------------------
HRESULT CreateGeometryBuffers()
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

	return hr;
}

//--------------------------------------------------------------------------------------
// Load models
//--------------------------------------------------------------------------------------
HRESULT InitModel()
{
	HRESULT hr = S_OK;

	// 이거 순서가 반대여야 g_Model에 푸시백하기 쉬울 거야

	// Load fbx models in here
	// this is the place where fbx file loaded
	ursine::FBXModel* sphere = new ursine::FBXModel;
	hr = sphere->LoadFBX("Assets/Models/sphere.fbx", g_pd3dDevice);
	FAIL_CHECK_WITH_MSG(hr, "sphere.fbx load fail");

	ursine::FBXModel* model = new ursine::FBXModel;
	hr = model->LoadFBX("Assets/Models/dragonplane.fbx", g_pd3dDevice);
	FAIL_CHECK_WITH_MSG(hr, "dragonplane.fbx load fail");

	g_Models["LightModel"].push_back(sphere);
	//g_Models["GeoModel"].push_back(sphere);
	g_Models["GeoModel"].push_back(model);

	return hr;
}

//--------------------------------------------------------------------------------------
// Init buffers, Create layouts, Compile shaders
//--------------------------------------------------------------------------------------
HRESULT InitApp()
{
	HRESULT hr = S_OK;

	// Load fbx models in here
	// this is the place where fbx file loadeds
	FAIL_CHECK(hr = InitModel());

	// create the multi render target which covers Full Screen Quad
	FAIL_CHECK_WITH_MSG(g_GBufferRenderTarget.Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT), "Could not initialize the G-buffer render target.");

	// create the multi render target which covers Full Screen Quad
	FAIL_CHECK_WITH_MSG(g_SceneBufferRenderTarget.Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT), "Could not initialize the G-buffer render target.");

	// create the multi render target which covers Full Screen Quad
	FAIL_CHECK_WITH_MSG(g_DepthBufferRenderTarget.Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT), "Could not initialize the G-buffer render target.");

	// create render target which covers Full Screen Quad
	FAIL_CHECK_WITH_MSG(g_DebugWindow.Initialize(g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT), "Could not initialize the final render target.");

	// create scene renderer
	FAIL_CHECK_WITH_MSG(g_SceneRenderer.Initialize(g_pd3dDevice, g_hWnd), "Could not initialize the scene renderer object.");
	
	// Init Shaders
	FAIL_CHECK_WITH_MSG(InitVertexShaders(), "Vertex Shader initialize failed");
	FAIL_CHECK_WITH_MSG(InitPixelShaders(), "Pixel Shader initialize failed");

	// Create Buffers
	FAIL_CHECK_WITH_MSG(CreateGeometryBuffers(), "Geometry const buffer creation failed");
	
	// Init Light Shader
	FAIL_CHECK_BOOLEAN_WITH_MSG(g_LightShader.Initialize(g_pd3dDevice, g_hWnd), "Light Shader initialize failed");

	// Init Depth Shader
	FAIL_CHECK_BOOLEAN_WITH_MSG(g_DepthShader.Initialize(g_pd3dDevice, g_hWnd), "Depth Shader initialize failed");

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_RasterState);
	FAIL_CHECK(hr);

	// Now set the rasterizer state.
	g_pDeviceContext->RSSetState(g_RasterState);

	// Setup a raster description which turns on front face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_FrontCullRasterState);
	FAIL_CHECK(hr);

	// Setup a raster description which turns off back face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_NoCullRasterState);
	FAIL_CHECK(hr);

	// Blend state
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = g_pd3dDevice->CreateBlendState(&blendStateDescription, &g_AlphaEnableBlendingState);
	FAIL_CHECK(hr);

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the second blend state using the description.
	hr = g_pd3dDevice->CreateBlendState(&blendStateDescription, &g_AlphaDisableBlendingState);
	FAIL_CHECK(hr);

	// Create a secondary alpha blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = g_pd3dDevice->CreateBlendState(&blendStateDescription, &g_AlphaAdditiveBlendState);
	FAIL_CHECK(hr);
	
#if DEBUG
#else
	//// SpriteBatch
	//g_pSpriteBatch = new DirectX::SpriteBatch(g_pDeviceContext);
	//
	//// SpriteFont
	//g_pFont = new DirectX::SpriteFont(g_pd3dDevice, L"Assets\\Arial.spritefont");
#endif

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

	if (g_bInstancing)
	{
		// create transformStructuredBuffer
		hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pTransformStructuredBuffer);
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
	}

	return hr;
}

//--------------------------------------------------------------------------------------
// Clean up applications
//--------------------------------------------------------------------------------------
void CleanupApp()
{
#if DEBUG
#else
	SAFE_DELETE(g_pSpriteBatch);
	SAFE_DELETE(g_pFont);
#endif

	// release views, states
	SAFE_RELEASE(g_AlphaAdditiveBlendState);
	SAFE_RELEASE(g_AlphaDisableBlendingState);
	SAFE_RELEASE(g_AlphaEnableBlendingState);
	SAFE_RELEASE(g_NoCullRasterState);
	SAFE_RELEASE(g_FrontCullRasterState);
	SAFE_RELEASE(g_RasterState);
	SAFE_RELEASE(g_pDepthDisabledStencilState);
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencil);
	SAFE_RELEASE(g_pRenderTargetView);

	g_SceneRenderer.Shutdown();
	g_DepthShader.Shutdown();
	g_LightShader.Shutdown();
	g_DebugWindow.Shutdown();
	g_GBufferRenderTarget.Shutdown();
	g_DepthBufferRenderTarget.Shutdown();
	g_SceneBufferRenderTarget.Shutdown();
	
	for (auto& iter : g_Models["GeoModel"])
		iter->Release();
	for (auto& iter : g_Models["LightModel"])
		iter->Release();
	g_Models.clear();

	SAFE_RELEASE(g_pmtxBuffer);
	SAFE_RELEASE(g_pmtxPaletteBuffer);
	
	for (UINT i = 0; i < 9; ++i)
	{
		SAFE_RELEASE(g_pVShader[i]);
		SAFE_RELEASE(g_pPShader[i]);
	}

	SAFE_RELEASE(g_pTransformSRV);
	SAFE_RELEASE(g_pTransformStructuredBuffer);
	SAFE_RELEASE(g_pvsInstancing);
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pDeviceContext) 
		g_pDeviceContext->ClearState();
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pd3dDevice);
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static UINT renderMode = 1;

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
		case VK_RETURN:
		{
			//++renderMode;
			//if (renderMode >= RT_COUNT)
			//	renderMode = 1;
			//
			//g_SceneRenderer.RenderMode(g_pDeviceContext, renderMode);
		}
			break;
			// xz translation
		case VK_NUMPAD2: tsl.z -= 5.0f; break;
		case VK_NUMPAD8: tsl.z += 5.0f; break;
		case VK_NUMPAD4: tsl.x -= 5.0f; break;
		case VK_NUMPAD6: tsl.x += 5.0f; break;

			// y translation
		case 'w':
		case 'W':
			tsl.y += 5.0f; break;
		case 's':
		case 'S':
			tsl.y -= 5.0f; break;

			// initialize
		case VK_NUMPAD5:
			tsl = InitTsl;
			rot = InitRot;
			scl = 1.0f;

			// resetting elements of lights
			lrot = 0.f;
			g_GlobalLight.Reset();
			break;

			// y-Axis rot
		case VK_NUMPAD7: rot.y += 0.1f; break;
		case VK_NUMPAD9: rot.y -= 0.1f; break;

			// Number pad rotation "0" cw, "." ccw
		case VK_NUMPAD0: lrot = 0.1f; break;
		case VK_DECIMAL: lrot = -0.1f; break;

			// Zoom in and out
		case VK_ADD: scl += 0.1f; break;
		case VK_SUBTRACT: scl -= 0.1f; break;
		}

		if (rot.y >= 360.f) rot.y = 0.0f;
		if (scl <= 0.f) scl = 1.f;

		g_translMatrix = XMMatrixTranslation(tsl.x, tsl.y, tsl.z);
		g_RotationMatrix = XMMatrixRotationY(rot.y);
		g_ScaleMatrix = XMMatrixScaling(scl, scl, scl);

		g_World = g_ScaleMatrix * g_RotationMatrix * g_translMatrix;
		
		// 일단 당장 섀도우가 사라지는 건 프러스텀(디렉션)을 체인지 해주지 않기 때문
		//// light transformation update 일단 글로벌만
		g_GlobalLight.SetRotation(lrot);
		g_GlobalLight.Update();
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
// Set render target
//--------------------------------------------------------------------------------------
void SetRenderTarget()
{
	// Set render target to back buffer
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}

//--------------------------------------------------------------------------------------
// Clear render target
//--------------------------------------------------------------------------------------
void ClearRenderTarget()
{
	// Clear the render target
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Clear the depth buffer to 1.0 (max depth)
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

//--------------------------------------------------------------------------------------
// Render the texture
//--------------------------------------------------------------------------------------
void RenderScene()
{
	// Clear the back buffer and depth buffer to 1.0 (max depth)
	ClearRenderTarget();

	g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilState, 1);

	RenderLightModel();		// light objects should comes first - foward rendering
	RenderGeometryModel();
}

// Geometry Pass for deferred shading
void GeometryPass()
{
	// Clear the back buffer with Color rgba
	g_GBufferRenderTarget.ClearRenderTarget(g_pDeviceContext, g_pDepthStencilView, ClearColor);

	// set G-buffer as render target
	g_GBufferRenderTarget.SetRenderTarget(g_pDeviceContext, g_pDepthStencilView);

	RenderLightModel();		// render light objects (should comes first)
	RenderGeometryModel();	// render models
}

// Light Pass for deferred shading
void LightPass()
{
	float lightNear = 0.1f;
	float lightFar = 500.f;

	// set scene buffer as render target
	g_SceneBufferRenderTarget.ClearRenderTarget(g_pDeviceContext, g_pDepthStencilView, ClearColor);
	g_SceneBufferRenderTarget.SetRenderTarget(g_pDeviceContext, g_pDepthStencilView);
	
	// Ambient Light
	{
		SetNoBlend();
		SetZBuffer(false);
		{
			// draw debug window(same resolution as screen)
			g_DebugWindow.Render(g_pDeviceContext, 0, 0);

			g_AmbientLight.GenerateShadowView();
			g_AmbientLight.GenerateShadowProjection(XM_PIDIV4, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, lightNear, lightFar);

			if (!g_LightShader.Render(g_pDeviceContext,
				g_DebugWindow.GetIndexCount(),
				g_ScreenRTMatrix, g_ScreenView, g_OrthoMatrix,
				g_GBufferRenderTarget.GetShaderResourceViews(),
				nullptr,
				g_AmbientLight,
				g_Camera.GetPosition()))
			{
				MessageBox(nullptr, "failed to render debug window by using Texture Shader", "Error", MB_OK);
				return;
			}
	
			// release rendertarget textures after use
			g_GBufferRenderTarget.ReleaseRenderTarget(g_pDeviceContext);
		}
	}

	// Global light
	{
		// create depth for global light	
		DepthPass(g_GlobalLight);
	
		SetAdditiveBlend();
		SetZBuffer(false);
		{
			g_DebugWindow.Render(g_pDeviceContext, 0, 0);
	
			if (!g_LightShader.Render(g_pDeviceContext,
				g_DebugWindow.GetIndexCount(),
				g_ScreenRTMatrix, g_ScreenView, g_OrthoMatrix,
				g_GBufferRenderTarget.GetShaderResourceViews(),
				g_DepthBufferRenderTarget.GetShaderResourceView(),
				g_GlobalLight,
				g_Camera.GetPosition()))
			{
				MessageBox(nullptr, "failed to render debug window by using Texture Shader", "Error", MB_OK);
				return;
			}
	
			// release rendertarget textures after use
			g_GBufferRenderTarget.ReleaseRenderTarget(g_pDeviceContext);
			g_DepthBufferRenderTarget.ReleaseRenderTarget(g_pDeviceContext);
		}
	}
	
	// local lights
	{
		SetAdditiveBlend();
		SetZBuffer(false);
		SetBackFaceCull(true);
		{
			g_DebugWindow.Render(g_pDeviceContext, 0, 0);
	
			for (UINT i = 0; i < MAX_LIGHT; ++i)
			{
				if (!g_LightShader.Render(g_pDeviceContext,
					g_DebugWindow.GetIndexCount(),
					g_ScreenRTMatrix, g_ScreenView, g_OrthoMatrix,
					g_GBufferRenderTarget.GetShaderResourceViews(),
					nullptr,
					g_LocalLights[i],
					g_Camera.GetPosition()))
				{
					MessageBox(nullptr, "failed to render debug window by using Texture Shader", "Error", MB_OK);
					return;
				}
			}
	
			// release rendertarget textures after use
			g_GBufferRenderTarget.ReleaseRenderTarget(g_pDeviceContext);
		}
	}

	// reset ZBuffer and No blending
	SetZBuffer(true);
	SetNoBlend();
}

// Depth Pass for deferred shading
void DepthPass(ursine::Light& light)
{
	float lightNear = 0.1f;
	float lightFar = 500.f;

	light.GenerateShadowView();
	light.GenerateShadowProjection(XM_PIDIV4, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, lightNear, lightFar);

	g_DepthBufferRenderTarget.ClearRenderTarget(g_pDeviceContext, g_pDepthStencilView, ClearColor);
	g_DepthBufferRenderTarget.SetRenderTarget(g_pDeviceContext, g_pDepthStencilView);

	SetNoBlend();
	SetZBuffer(true);
	SetBackFaceCull(true);

	// redner geometry
	RenderGeometryModelDepth(light);

	// reset back face cull
	SetBackFaceCull(false);

	// Reset rendertarget as scene buffer render target
	g_SceneBufferRenderTarget.SetRenderTarget(g_pDeviceContext, g_pDepthStencilView);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	// Pass1 - G Buffer Rendering. Render to texture
	// pos, norm, diff+transparency, spec+shineness, depth
	// Render Scene to the texture(render target)
	GeometryPass();
	
	// Pass2 - Lighting Rendering
	// output to screen or final render target with viewport set to its width height
	// set blending off, depth testing off,
	// set ambi light amount
	// draw FSQ to invoke a pixel shader at all pixels
	  // VS : pass the FSQ's vertex straight through. no transformation
	  // PS : compute and output ambient light
	LightPass();

	// Final Result Rendering
	{
		// Set backbuffer as render target and clear
		ClearRenderTarget();
		SetRenderTarget();
	
		// Quad Renderer(left right top bottom)
		g_DebugWindow.Render(g_pDeviceContext, 0, 0);
		
		if (!g_SceneRenderer.Render(g_pDeviceContext,
			g_DebugWindow.GetIndexCount(),
			g_ScreenRTMatrix, g_ScreenView, g_OrthoMatrix,
			g_SceneBufferRenderTarget.GetShaderResourceView()
			))
		{
			MessageBox(nullptr, "failed to render debug window by using Texture Shader", "Error", MB_OK);
			return;
		}

		g_SceneBufferRenderTarget.ReleaseRenderTarget(g_pDeviceContext);
	}

#if DEBUG
#else
	//// Draw Text
	//WCHAR wstr[512];
	//g_pSpriteBatch->Begin();
	//
	//// fps
	//g_pFont->DrawString(g_pSpriteBatch, L"FPS", XMFLOAT2(0, 0), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//swprintf_s(wstr, L"%d", frame_per_sec);
	//g_pFont->DrawString(g_pSpriteBatch, wstr, XMFLOAT2(0, 16), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//
	//// number of lights
	//g_pFont->DrawString(g_pSpriteBatch, L"Number of Lights", XMFLOAT2(45, 0), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//swprintf_s(wstr, L"%d", 2 + MAX_LIGHT);
	//g_pFont->DrawString(g_pSpriteBatch, wstr, XMFLOAT2(45, 16), DirectX::Colors::Yellow, 0, XMFLOAT2(0, 0), 0.5f);
	//
	//g_pSpriteBatch->End();
#endif

	// Present our back buffer to our front buffer
	g_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Render the light model
//--------------------------------------------------------------------------------------
void RenderLightModel()
{
	// light model(sphere)
	if (g_Models["LightModel"].empty())
		return;

	// for all lights
	// for all mesh nodes
	g_pDeviceContext->VSSetShader(g_pVShader[1], nullptr, 0);
	g_pDeviceContext->PSSetShader(g_pPShader[1], nullptr, 0);

	for (auto& iter : g_Models["LightModel"])
	{
		// mesh nodes count
		size_t meshnodeCnt = iter->GetMeshNodeCount();

		for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
		{
			auto& currMesh = iter->GetMeshNode(mn_idx);
			ursine::Light* light = nullptr;

			// ambient light
			{
				light = &g_AmbientLight;
				// set shader parameters(mapping constant buffers, matrices, resources)
				currMesh.SetMeshColor(light->GetColor());
				SetLightShaderParameters(&iter, (*light), mn_idx);
				//// Don't need to draw ambient light
				//iter->RenderNode(g_pDeviceContext, mn_idx);
			}

			// global light
			{
				light = &g_GlobalLight;
				// set shader parameters(mapping constant buffers, matrices, resources)
				currMesh.SetMeshColor(light->GetColor());
				SetLightShaderParameters(&iter, (*light), mn_idx);
				iter->RenderNode(g_pDeviceContext, mn_idx);
			}

			// local lights
			{
				for (UINT loc = 0; loc < MAX_LIGHT; ++loc)
				{
					light = &g_LocalLights[loc];
					// set shader parameters(mapping constant buffers, matrices, resources)
					currMesh.SetMeshColor(light->GetColor());
					SetLightShaderParameters(&iter, (*light), mn_idx);
					iter->RenderNode(g_pDeviceContext, mn_idx);
				}
			}
		}
	}

	// reset shader
	g_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
	g_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
}

//--------------------------------------------------------------------------------------
// Render the geometry model
//--------------------------------------------------------------------------------------
void RenderGeometryModel()
{
	// geo model
	if (g_Models["GeoModel"].empty())
		return;

	// for all model
	for( auto& iter : g_Models["GeoModel"])
	{
		// mesh nodes count
		size_t meshnodeCnt = iter->GetMeshNodeCount();

		// for all mesh nodes
		for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
		{
			//////////////////////////////////////
			// sort by layout later
			//////////////////////////////////////
			eLayout layout_type = iter->GetLayoutType(mn_idx);
			ID3D11VertexShader* pVS = nullptr;
			ID3D11PixelShader* pPS = nullptr;
			switch (layout_type)
			{
			case eLayout::NONE_LAYOUT:		continue;
			case eLayout::LAYOUT0:	pVS = g_pVShader[0];	pPS = g_pPShader[0]; break;
			case eLayout::LAYOUT1:
			{
				if (g_bInstancing)
					pVS = g_pvsInstancing;
				else
				{
					pVS = g_pVShader[1]; 
					pPS = g_pPShader[1];
				}
			}
			break;
			case eLayout::LAYOUT2:	pVS = g_pVShader[2];	pPS = g_pPShader[2]; break;
			case eLayout::LAYOUT3:	pVS = g_pVShader[3];	pPS = g_pPShader[3]; break;
			case eLayout::LAYOUT4:	pVS = g_pVShader[4];	pPS = g_pPShader[4]; break;
			case eLayout::LAYOUT5:	pVS = g_pVShader[5];	pPS = g_pPShader[5]; break;
			case eLayout::LAYOUT6:	pVS = g_pVShader[6];	pPS = g_pPShader[6]; break;
			case eLayout::LAYOUT7:	pVS = g_pVShader[7];	pPS = g_pPShader[7]; break;
			}

			g_pDeviceContext->VSSetShader(pVS, nullptr, 0);
			g_pDeviceContext->PSSetShader(pPS, nullptr, 0);

			// set shader parameters(mapping constant buffers, matrices, resources)
			SetGeometryShaderParameters(&iter, mn_idx, layout_type);

			iter->RenderNode(g_pDeviceContext, mn_idx);

			// reset shader
			g_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
			g_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
		}
	}
}

//--------------------------------------------------------------------------------------
// Render the geometry model's depth
//--------------------------------------------------------------------------------------
void RenderGeometryModelDepth(const ursine::Light& light)
{
	// geo model
	if (g_Models["GeoModel"].empty())
		return;

	// for all model
	for (auto& iter : g_Models["GeoModel"])
	{
		// mesh nodes count
		size_t meshnodeCnt = iter->GetMeshNodeCount();

		// for all mesh nodes
		for (UINT mn_idx = 0; mn_idx < meshnodeCnt; ++mn_idx)
		{
			//////////////////////////////////////
			// sort by layout later
			//////////////////////////////////////
			g_pDeviceContext->VSSetShader(g_DepthShader.GetVertexShader(), nullptr, 0);
			g_pDeviceContext->PSSetShader(g_DepthShader.GetPixelShader(), nullptr, 0);

			// set shader parameters(mapping constant buffers, matrices, resources)
			g_DepthShader.SetShaderParameters(g_pDeviceContext,
				g_World,
				light.GetShadowView(),
				light.GetShadowProjection());

			iter->RenderNodeDepth(g_pDeviceContext, mn_idx, g_DepthShader.GetDepthLayout());

			// reset shader 
			g_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
			g_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
		}
	}
}

//--------------------------------------------------------------------------------------
// Set Matrices for instancing
//--------------------------------------------------------------------------------------
void SetMatrix()
{
	HRESULT hr = S_OK;
	const UINT count = g_InstanceMAX;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hr = g_pDeviceContext->Map(g_pTransformStructuredBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	SRVPerInstanceData*	pSrvInstanceData = (SRVPerInstanceData*)MappedResource.pData;

	for (UINT i = 0; i < count; i++)
	{
		XMFLOAT3 pos = g_LocalLights[i].GetPosition();
		pSrvInstanceData[i].mWorld = XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

	g_pDeviceContext->Unmap(g_pTransformStructuredBuffer, 0);
}

//--------------------------------------------------------------------------------------
// Set Shader Parameters for geometry
//--------------------------------------------------------------------------------------
bool SetGeometryShaderParameters(ursine::FBXModel** currentModel, const UINT& mesh_index, const eLayout& layoutType)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT hr;

	//--------------------------------------------------------------------------------------
	// Vertex Shader Parameters
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// setting matrices
	//--------------------------------------------------------------------------------------
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pmtxBuffer);
	// world, view, projection, WVP Matrices & material
	{
		hr = g_pDeviceContext->Map(g_pmtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		FAIL_CHECK_BOOLEAN(hr);

		MatrixBufferType* mtxBuffer = (MatrixBufferType*)MappedResource.pData;

		// WVP
		mtxBuffer->mWorld = XMMatrixTranspose(g_World);
		mtxBuffer->mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
		mtxBuffer->mProj = XMMatrixTranspose(g_Projection);

		// xm matrix - row major
		// hlsl - column major
		// that's why we should transpose this
		mtxBuffer->mWVP = XMMatrixTranspose(
			g_World * g_Camera.GetViewMatrix() * g_Projection);

		// unmap constant buffer
		g_pDeviceContext->Unmap(g_pmtxBuffer, 0);
	}

	//if(g_bInstancing)
	//	SetMatrix();

	//--------------------------------------------------------------------------------------
	// matrix palette
	//--------------------------------------------------------------------------------------
	{
		if ((*currentModel)->IsSkinned())
		{
			g_pDeviceContext->VSSetConstantBuffers(1, 1, &g_pmtxPaletteBuffer);	// setting matrix palettes

			if (eLayout::LAYOUT4 == layoutType ||
				eLayout::LAYOUT5 == layoutType ||
				eLayout::LAYOUT6 == layoutType ||
				eLayout::LAYOUT7 == layoutType)
			{
				hr = g_pDeviceContext->Map(g_pmtxPaletteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				PaletteBufferType* palBuffer = (PaletteBufferType*)MappedResource.pData;
				FAIL_CHECK_BOOLEAN(hr);

				(*currentModel)->UpdateMatPal(&palBuffer->matPal[0]);

				g_pDeviceContext->Unmap(g_pmtxPaletteBuffer, 0);
			}
		}
	}

	//--------------------------------------------------------------------------------------
	// SRV for instancing
	//--------------------------------------------------------------------------------------
	//if (g_pTransformSRV && g_bInstancing)	
	//	g_pImmediateContext->VSSetShaderResources(0, 1, &g_pTransformSRV);

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
			if (material.pSampler)	g_pDeviceContext->PSSetSamplers(0, 1, &material.pSampler);

			// set shader resource view - for texture
			if (material.pSRV[0])		g_pDeviceContext->PSSetShaderResources(0, 1, &material.pSRV[0]);
			if (material.pSRV[1])		g_pDeviceContext->PSSetShaderResources(1, 1, &material.pSRV[1]);

			// set constant buffer for material
			if (material.pMaterialCb)
			{
				//g_pImmediateContext->UpdateSubresource(material.pMaterialCb, 0, nullptr, &material.mtrlConst, 0, 0);
				g_pDeviceContext->PSSetConstantBuffers(0, 1, &material.pMaterialCb);	// setting materials constant buffer

				hr = g_pDeviceContext->Map(material.pMaterialCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				MaterialBufferType* mtrlBuffer = (MaterialBufferType*)MappedResource.pData;
				FAIL_CHECK_BOOLEAN(hr);

				mtrlBuffer->shineness = material.mtrlConst.shineness;
				mtrlBuffer->transparency = material.mtrlConst.transparency;
				mtrlBuffer->ambient = material.mtrlConst.ambient;
				mtrlBuffer->diffuse = material.mtrlConst.diffuse;
				mtrlBuffer->specular = material.mtrlConst.specular;
				mtrlBuffer->emissive = material.mtrlConst.emissive;

				g_pDeviceContext->Unmap(material.pMaterialCb, 0);
			}
		}

		//--------------------------------------------------------------------------------------
		// mesh color
		//--------------------------------------------------------------------------------------
		{
			auto& currMesh = (*currentModel)->GetMeshNode(mesh_index);
			g_pDeviceContext->PSSetConstantBuffers(1, 1, &currMesh.m_meshColorBuffer);
			hr = g_pDeviceContext->Map(currMesh.m_meshColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			MeshBufferType* meshBuffer = (MeshBufferType*)MappedResource.pData;
			FAIL_CHECK_BOOLEAN(hr);
		
			meshBuffer->color = currMesh.GetMeshColor();
		
			g_pDeviceContext->Unmap(currMesh.m_meshColorBuffer, 0);
		}	
	}

	return true;
}

//--------------------------------------------------------------------------------------
// Set Shader Parameters for geometry
//--------------------------------------------------------------------------------------
bool SetLightShaderParameters(ursine::FBXModel** currentModel, const ursine::Light& light, const UINT& mesh_index)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	HRESULT hr;

	//--------------------------------------------------------------------------------------
	// Vertex Shader Parameters
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// setting matrices
	//--------------------------------------------------------------------------------------
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pmtxBuffer);
	// world, view, projection, WVP Matrices & material
	{
		// 이게 여기에 있어야만 하는지, 라이팅 셰이더를 이 이후에 그릴 때에 다시 이걸 세이더에 전달해줘야 하는지 궁금하군
		// 일단 라이팅 여기에다 넣어서 되는지 확인해.
		hr = g_pDeviceContext->Map(g_pmtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		FAIL_CHECK_BOOLEAN(hr);

		MatrixBufferType* mtxBuffer = (MatrixBufferType*)MappedResource.pData;

		XMMATRIX world = light.GetTransformation() * g_World;

		// WVP
		mtxBuffer->mWorld = XMMatrixTranspose(world);
		mtxBuffer->mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
		mtxBuffer->mProj = XMMatrixTranspose(g_Projection);

		// xm matrix - row major
		// hlsl - column major
		// that's why we should transpose this
		mtxBuffer->mWVP = XMMatrixTranspose(world * g_Camera.GetViewMatrix() * g_Projection);

		// unmap constant buffer
		g_pDeviceContext->Unmap(g_pmtxBuffer, 0);
	}
	
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
			if (material.pSampler)	g_pDeviceContext->PSSetSamplers(0, 1, &material.pSampler);

			// set shader resource view - for texture
			if (material.pSRV[0])	g_pDeviceContext->PSSetShaderResources(0, 1, &material.pSRV[0]);

			// set constant buffer for material
			if (material.pMaterialCb)
			{
				//g_pImmediateContext->UpdateSubresource(material.pMaterialCb, 0, nullptr, &material.mtrlConst, 0, 0);
				g_pDeviceContext->PSSetConstantBuffers(0, 1, &material.pMaterialCb);	// setting materials constant buffer

				hr = g_pDeviceContext->Map(material.pMaterialCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				MaterialBufferType* mtrlBuffer = (MaterialBufferType*)MappedResource.pData;
				FAIL_CHECK_BOOLEAN(hr);

				mtrlBuffer->shineness = material.mtrlConst.shineness;
				mtrlBuffer->transparency = material.mtrlConst.transparency;
				mtrlBuffer->ambient = material.mtrlConst.ambient;
				mtrlBuffer->diffuse = material.mtrlConst.diffuse;
				mtrlBuffer->specular = material.mtrlConst.specular;
				mtrlBuffer->emissive = material.mtrlConst.emissive;

				g_pDeviceContext->Unmap(material.pMaterialCb, 0);
			}
		}

		//--------------------------------------------------------------------------------------
		// mesh color
		//--------------------------------------------------------------------------------------
		{
			auto& currMesh = (*currentModel)->GetMeshNode(mesh_index);
			g_pDeviceContext->PSSetConstantBuffers(1, 1, &currMesh.m_meshColorBuffer);
			hr = g_pDeviceContext->Map(currMesh.m_meshColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			MeshBufferType* meshBuffer = (MeshBufferType*)MappedResource.pData;
			FAIL_CHECK_BOOLEAN(hr);

			meshBuffer->color = currMesh.GetMeshColor();

			g_pDeviceContext->Unmap(currMesh.m_meshColorBuffer, 0);
		}
	}

	return true;
}

void SetZBuffer(bool on)
{
	if (on)
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilState, 1);
	else
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthDisabledStencilState, 1);
}

void SetAlphaBlend()
{
	// Set blend factor
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Turn on the alpha blending.
	g_pDeviceContext->OMSetBlendState(g_AlphaEnableBlendingState, blendFactors, 0xffffffff);

	return;
}

void SetNoBlend()
{
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Turn off the alpha blending.
	g_pDeviceContext->OMSetBlendState(g_AlphaDisableBlendingState, blendFactors, 0xffffffff);

	return;
}

void SetAdditiveBlend()
{
	// Set blend factor
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Turn on the alpha blending.
	g_pDeviceContext->OMSetBlendState(g_AlphaAdditiveBlendState, blendFactors, 0xffffffff);
	return;
}

void SetFrontFaceCull(bool on)
{
	// Set the culling rasterizer state.
	if (on)
		g_pDeviceContext->RSSetState(g_FrontCullRasterState);
	else
		g_pDeviceContext->RSSetState(g_NoCullRasterState);
}

void SetBackFaceCull(bool on)
{
	// Set the culling rasterizer state.
	if (on)
		g_pDeviceContext->RSSetState(g_RasterState);
	else
		g_pDeviceContext->RSSetState(g_NoCullRasterState);
}