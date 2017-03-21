/* ---------------------------------------------------------------------------
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** PlatformWindow.cpp
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** -------------------------------------------------------------------------*/
#include "PlatformWindow.h"
#include "MessageBox.h"
#include <iostream>

const unsigned int WINDOW_WIDTH_OFFSET = 0;
const unsigned int WINDOW_HEIGHT_OFFSET = 0;

bool PlatformWindow::mIsWindowCreated = false;

PlatformWindow::PlatformWindow() :
	PlatformWindow(WindowSettings())
{
}

PlatformWindow::PlatformWindow(const WindowSettings & gs) :
	mSettings(gs)
{
	SetTitle(mSettings.title);
	RegisterWindow();
}

PlatformWindow::~PlatformWindow()
{
	CloseWindow();
}

/////////////////////////////////////////////////////////////////////////////
void PlatformWindow::SetInstance(HINSTANCE hInstance)
{
	mInstance = hInstance;
}

void PlatformWindow::SetTitle(const std::string & name)
{
	mSettings.title = name;
	sTitle = name;

	if (mHwnd != nullptr)
		SetWindowText(mHwnd, mSettings.title.c_str());
}

const std::string & PlatformWindow::GetTitle() const
{
	return mSettings.title;
}

// TODO : fullscreen functionality not finalized
void PlatformWindow::SetFullscreen(bool state)
{
	mFullscreen = state;
	if (state)
	{
		DEVMODE dmScreenSettings;

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		mSettings.w = GetSystemMetrics(SM_CXSCREEN);
		mSettings.h = GetSystemMetrics(SM_CYSCREEN);
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = mSettings.w;
		dmScreenSettings.dmPelsHeight = mSettings.h;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		// Set the fullscreen mode style, clear menu if attached
		SetWindowLongPtr(mHwnd, GWL_STYLE, WS_POPUP | WS_EX_TOPMOST | WS_VISIBLE);

		// Change the display settings to full screen.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			std::cout << "FAIL" << std::endl;
			ChangeDisplaySettings(NULL, 0);
		}
		/* Set the position of the window to the top left corner. */
		SetPosition(0, 0);

		MoveWindow(mHwnd,
			mSettings.x,
			mSettings.y,
			static_cast<int>(dmScreenSettings.dmPelsWidth),
			static_cast<int>(dmScreenSettings.dmPelsHeight),
			TRUE);

		UpdateWindow(mHwnd);
		//GetClientRect(mHwnd, &ClientSize);
		/*This is required after SetWindowLong*/
		ShowWindow(mHwnd, SW_SHOWNORMAL);
		/*This sets my window to the front.*/
		SetForegroundWindow(mHwnd);
	}

	//The size passed to CreateWindow is the full size including the windows border and caption 
	//AdjustWindowRect will adjust the provided rect so that the client size of the window is the desired size
	/*
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);
	*/
}

bool PlatformWindow::GetFullscreen() const
{
	return mFullscreen;
}

void PlatformWindow::SetPosition(UINT x, UINT y)
{
	mSettings.x = x;
	mSettings.y = y;
}

UINT PlatformWindow::GetX() const
{
	return mSettings.x;
}

UINT PlatformWindow::GetY() const
{
	return mSettings.y;
}

UINT  PlatformWindow::GetWidth() const
{
	return mSettings.w;
}

UINT  PlatformWindow::GetHeight() const
{
	return mSettings.h;
}

bool PlatformWindow::IsCursorIn() const
{
	return mIsCursorIn;
}

WindowEvents & PlatformWindow::GetEventList()
{
	return mEventList;
}

WindowSettings & PlatformWindow::GetSettings()
{
	return mSettings;
}

/////////////////////////////////////////////////////////////////////////////

void PlatformWindow::OpenWindow()
{
	//RegisterWindow();
	mHwnd = CreateWindowEx(
		NULL,
		sTitle.c_str(),						// name of our registered class
		sTitle.c_str(),						// Window name/title
		mSettings.flags,				    // Style flags
		mSettings.x,					    // X position
		mSettings.y,					    // Y position
		mSettings.w + WINDOW_WIDTH_OFFSET,  // width of window
		mSettings.h + WINDOW_HEIGHT_OFFSET, // height of window
		NULL,                               // Parent window
		NULL,                               // Menu
		mInstance,                          // application instance handle
		this);								// pointer to window-creation data

	// Cursor of Arrow flags https://msdn.microsoft.com/en-us/library/windows/desktop/ms648391(v=vs.85).aspx
	mCurrentCursor = LoadCursor(NULL, IDC_ARROW);

	// Window flags  https://msdn.microsoft.com/en-us/library/windows/desktop/ms633548(v=vs.85).aspx
	ShowWindow(mHwnd, SW_SHOW);
	ShowCursor(true);

	SetFullscreen(false);

	if (mHwnd == nullptr)
		CreateMessageBox("Create Window failed.");
}

void PlatformWindow::CloseWindow()
{
	if (mHwnd != nullptr)
		DestroyWindow(mHwnd);

	mHwnd = nullptr;
}

void PlatformWindow::Update()
{
	ProcessWindowEvents();
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
void PlatformWindow::RegisterWindow()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = &PlatformWindow::WinProc;
	wcex.cbClsExtra = 0L;
	wcex.cbWndExtra = 0L;
	wcex.hInstance = mInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = sTitle.c_str();
	wcex.hIconSm = NULL;

	if (RegisterClassEx(&wcex) == false)
		CreateMessageBox("Register Window failed.");
}

void PlatformWindow::ProcessWindowEvents()
{
	if (mHwnd == nullptr)
		return;

	MSG msg;

	while (PeekMessage(&msg, mHwnd, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void PlatformWindow::ProcessEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (mHwnd == nullptr)
	//{
	//	//This is just for deleting warnings. It will be deleted after adding more features in windows system
	//	UNREFERENCED_PARAMETER(wParam);
	//	UNREFERENCED_PARAMETER(lParam);
	//	return;
	//}

	switch (msg)
	{
		// Window close event.
	case WM_CLOSE:
	{
		if (mEventList.mOnWindowClosed)
			mEventList.mOnWindowClosed();

		PostQuitMessage(0);

		CloseWindow();

		break;
	}

	// Set cursor event
	case WM_SETCURSOR:
	{
		break;
	}

	// Window gain focus event.
	case WM_SETFOCUS:
	{
		//if (mEventList.mOnWindowGainedFocus)
		//	mEventList.mOnWindowGainedFocus();

		break;
	}

	// Window lost focus event.
	case WM_KILLFOCUS:
	{
		//if (mEventList.mOnWindowLostFocus)
		//	mEventList.mOnWindowLostFocus();

		break;
	}

	// Sent after a window has been moved
	case WM_MOVE:
	{
		//if (mIsCursorIn == false)
		//{
		//	TRACKMOUSEEVENT tme;
		//	tme.cbSize = sizeof(TRACKMOUSEEVENT);
		//	tme.hwndTrack = mHwnd;
		//	tme.dwFlags = TME_LEAVE;
		//	TrackMouseEvent(&tme);
		//
		//	mIsCursorIn = true;
		//	if (mEventList.mOnWindowMouseEnter)
		//		mEventList.mOnWindowMouseEnter();
		//}
		//
		//if (mEventList.mOnWindowMouseMove)
		//	mEventList.mOnWindowMouseMove(LOWORD(lParam), HIWORD(lParam));

		break;
	}

	// Text event.
	case WM_CHAR:
	{
		//if (mEventList.mOnWindowKeyChar)
		//	mEventList.mOnWindowKeyChar(UINT(wParam));

		break;
	}

	// Key down event.
	// camera control or model pos/rot control
	case WM_KEYDOWN:
	{
		Key::KeyCode key = ConvertVirtualCode(wParam, lParam);

		if (mEventList.mOnWindowKeyDown)
			mEventList.mOnWindowKeyDown(key);
		
		//switch (wParam)
		//{
		//case VK_RETURN:
		//{
		//	//++renderMode;
		//	//if (renderMode >= RT_COUNT)
		//	//	renderMode = 1;
		//	//
		//	//g_SceneRenderer.RenderMode(g_pDeviceContext, renderMode);
		//}
		//break;
		//// xz translation
		//case VK_NUMPAD2:
		//	//tsl.z -= 5.0f;
		//	break;
		//case VK_NUMPAD8:
		//	//tsl.z += 5.0f;
		//	break;
		//case VK_NUMPAD4:
		//	//tsl.x -= 5.0f;
		//	break;
		//case VK_NUMPAD6:
		//	//tsl.x += 5.0f;
		//	break;
		//
		//// y translation
		//case 'w':
		//case 'W':
		//	tsl.y += 5.0f; break;
		//case 's':
		//case 'S':
		//	tsl.y -= 5.0f; break;
		//
		//	// initialize
		//case VK_NUMPAD5:
		//	tsl = InitTsl;
		//	rot = InitRot;
		//	scl = 1.0f;
		//	lrot = 0.f;
		//	//g_GlobalLight.Reset();
		//	//g_GlobalLight.Update();
		//	//for (auto& iter : g_LocalLights)
		//	//{
		//	//	iter.Reset();
		//	//	iter.Update();
		//	//}
		//	break;
		//
		//	// y-Axis rot
		//case VK_NUMPAD7:
		//	rot.y += 0.1f;
		//	if (rot.y > 360.f)
		//		rot.y = 0.1f;
		//	break;
		//case VK_NUMPAD9:
		//	rot.y -= 0.1f;
		//	if (rot.y < 0.f)
		//		rot.y = 359.9f;
		//	break;
		//
		// Number pad rotation "0" cw, "." ccw
		//case VK_NUMPAD0:
		//	lrot = 0.1f;
		//	//// 나중에 라이트도 자유롭게 transform 업뎃할 수 있게 만들자.
		//	////g_GlobalLight.SetPosition(tsl);
		//	//g_GlobalLight.SetRotation(lrot);
		//	//g_GlobalLight.Update();
		//	//for (auto& iter : g_LocalLights)
		//	//{
		//	//	iter.SetRotation(lrot);
		//	//	iter.Update();
		//	//}
		//	break;
		//case VK_DECIMAL:
		//	lrot = -0.1f;
		//	//// 나중에 라이트도 자유롭게 transform 업뎃할 수 있게 만들자.
		//	////g_GlobalLight.SetPosition(tsl);
		//	//g_GlobalLight.SetRotation(lrot);
		//	//g_GlobalLight.Update();
		//	//for (auto& iter : g_LocalLights)
		//	//{
		//	//	iter.SetRotation(lrot);
		//	//	iter.Update();
		//	//}
		//	break;
		//
		//	// Zoom in and out
		//case VK_ADD:
		//	scl += 0.1f;
		//	break;
		//case VK_SUBTRACT:
		//	scl -= 0.1f;
		//	if (scl <= 0.f)
		//		scl = 1.f;
		//	break;
		//
		//	// increase/decrease exponential constant
		//case VK_NUMPAD1:
		//	exponentialConst -= 5.f;
		//	break;
		//case VK_NUMPAD3:
		//	exponentialConst += 5.f;
		//	if (exponentialConst < 1.f) exponentialConst = 1.f;
		//	break;
		//}
		break;
	}
	case WM_SYSKEYDOWN:
	{
		//Key::KeyCode key = ConvertVirtualCode(wParam, lParam);
		//
		//if (mEventList.mOnWindowKeyDown)
		//	mEventList.mOnWindowKeyDown(key);

		break;
	}

	// Key up event.
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		//Key::KeyCode key = ConvertVirtualCode(wParam, lParam);
		//
		//if (mEventList.mOnWindowKeyUp)
		//	mEventList.mOnWindowKeyUp(key);
		//
		break;
	}

	// Mouse wheel event.
	case WM_MOUSEWHEEL:
	{
		//if (mEventList.mOnWindowMouseWheel)
		//	mEventList.mOnWindowMouseWheel(static_cast< short >(HIWORD(wParam)) / 120);

		break;
	}

	// Mouse move event, pretty cool stuff
	case WM_MOUSEMOVE:
	{
		//if (mEventList.mOnWindowMouseMove)
		//	mEventList.mOnWindowMouseMove(LOWORD(lParam), HIWORD(lParam));

		break;
	}

	// Left mouse button down event.
	case WM_LBUTTONDOWN:
	{
		//if (mEventList.mOnWindowMouseDown)
		//	mEventList.mOnWindowMouseDown(LOWORD(lParam), HIWORD(lParam), Mouse::Left);

		break;
	}

	// Left mouse button up event.
	case WM_LBUTTONUP:
	{
		//if (mEventList.mOnWindowMouseUp)
		//	mEventList.mOnWindowMouseUp(LOWORD(lParam), HIWORD(lParam), Mouse::Left);

		break;
	}

	// Left mouse button double click event.
	case WM_LBUTTONDBLCLK:
	{
		//if (mEventList.mOnWindowMouseDoubleClick)
		//	mEventList.mOnWindowMouseDoubleClick(LOWORD(lParam), HIWORD(lParam), Mouse::Left);

		break;
	}

	// Right mouse button down event.
	case WM_RBUTTONDOWN:
	{
		//if (mEventList.mOnWindowMouseDown)
		//	mEventList.mOnWindowMouseDown(LOWORD(lParam), HIWORD(lParam), Mouse::Right);

		break;
	}

	// Right mouse button up event.
	case WM_RBUTTONUP:
	{
		//if (mEventList.mOnWindowMouseUp)
		//	mEventList.mOnWindowMouseUp(LOWORD(lParam), HIWORD(lParam), Mouse::Right);

		break;
	}

	// Right mouse button double click event.
	case WM_RBUTTONDBLCLK:
	{
		//if (mEventList.mOnWindowMouseDoubleClick)
		//	mEventList.mOnWindowMouseDoubleClick(LOWORD(lParam), HIWORD(lParam), Mouse::Right);

		break;
	}

	// Middle mouse button down event.
	case WM_MBUTTONDOWN:
	{
		//if (mEventList.mOnWindowMouseDown)
		//	mEventList.mOnWindowMouseDown(LOWORD(lParam), HIWORD(lParam), Mouse::Middle);

		break;
	}

	// Middle mouse button up event.
	case WM_MBUTTONUP:
	{
		//if (mEventList.mOnWindowMouseUp)
		//	mEventList.mOnWindowMouseUp(LOWORD(lParam), HIWORD(lParam), Mouse::Middle);

		break;
	}

	// Middle mouse button double click event.
	case WM_MBUTTONDBLCLK:
	{
		//if (mEventList.mOnWindowMouseDoubleClick)
		//	mEventList.mOnWindowMouseDoubleClick(LOWORD(lParam), HIWORD(lParam), Mouse::Middle);

		break;
	}

	// Posted to a window when the cursor leaves the client area of the window
	case WM_MOUSELEAVE:
	{
		//if (mEventList.mOnWindowMouseLeave)
		//	mEventList.mOnWindowMouseLeave();

		mIsCursorIn = false;

		break;
	}

	//// When window resize
	//case WM_SIZE:
	//{
	//	if (wParam != SIZE_MINIMIZED )//&& mHwnd)
	//	{
	//		RECT rect;
	//		GetClientRect(mHwnd, &rect);
	//		mSettings.w = rect.right - rect.left;
	//		mSettings.h = rect.bottom - rect.top;
	//		break;
	//	}
	//}

	}
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK PlatformWindow::WinProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// associate handle and window instance when the creation message is received
	PlatformWindow * window = nullptr;
	if (msg == WM_CREATE)
	{
		// Get PlatformWindow instance 
		// last argument of CreateWindow
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		LPMDICREATESTRUCT pMDICreateStruct = reinterpret_cast<LPMDICREATESTRUCT>(pCreateStruct->lpCreateParams);
		window = reinterpret_cast<PlatformWindow*>(pMDICreateStruct->lParam);
		window = reinterpret_cast<PlatformWindow*>(pCreateStruct->lpCreateParams);

		// Set as the "user data" parameter of the window
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)window);
		mIsWindowCreated = true;
	}

	// 핸들을 받아오는 걸 잘못했을 가능성이 가장 높다.
	// Get the PlatformWindow instance corresponding to the window handle
	window = reinterpret_cast<PlatformWindow *>(GetWindowLongPtr(handle, GWLP_USERDATA));
	
	if (window && mIsWindowCreated)
		window->ProcessEvents(msg, wParam, lParam);

	return DefWindowProc(handle, msg, wParam, lParam);
}