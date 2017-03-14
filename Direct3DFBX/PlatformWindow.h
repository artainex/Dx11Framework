/* ---------------------------------------------------------------------------
** 2017 DigiPen Institute of Technology, All Rights Reserved.
**
** PlatformWindow.h
**
** Author:
** - Park Hyung Jun - park.hyungjun@digipen.edu
**
** Contributors:
** -------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <windows.h>
#include <functional>

#include "KeyBind.h"

struct WindowEvents
{
private:
	typedef std::function<void()>									WindowEvent;
	typedef std::function<void(UINT, UINT, UINT, UINT, unsigned short, float) >	WindowSizeEvent;
	typedef std::function<void(bool, bool)>							WindowFullscreenEvent;

	typedef std::function<void(UINT)>								WindowCharEvent;
	typedef std::function<void(Key::KeyCode)>						WindowKeyEvent;

	typedef std::function<void(int, int)>							WindowMouseMoveEvent;
	typedef std::function<void(int, int, Mouse::Button)>			WindowMouseButtonEvent;
	typedef std::function<void(int)>								WindowMouseWheelEvent;

public:
	typedef WindowEvent					WindowCreatedEvent;
	typedef WindowEvent					WindowDestroyedEvent;
	typedef WindowEvent					WindowClosedEvent;
	typedef WindowEvent					WindowLostFocusEvent;
	typedef WindowEvent					WindowGainedFocusEvent;
	typedef WindowEvent					WindowMouseEnterEvent;
	typedef WindowEvent					WindowMouseLeaveEvent;
	typedef WindowMouseButtonEvent		WindowMouseUpEvent;
	typedef WindowMouseButtonEvent		WindowMouseDownEvent;
	typedef WindowMouseButtonEvent		WindowMouseDblClickEvent;
	typedef WindowKeyEvent				WindowKeyUpEvent;
	typedef WindowKeyEvent				WindowKeyDownEvent;

public:
	WindowSizeEvent						mOnWindowResized;				// Triggered when window is resized.
	WindowCreatedEvent					mOnWindowCreated;				// Triggered when window is created.
	WindowDestroyedEvent				mOnWindowDestroyed;				// Triggered when window is destroyed.
	WindowClosedEvent					mOnWindowClosed;				// Triggered when window is closed.
	WindowLostFocusEvent				mOnWindowLostFocus;				// Triggered when window is loses focus.
	WindowGainedFocusEvent				mOnWindowGainedFocus;			// Triggered when window is gains focus.
	WindowFullscreenEvent				mOnWindowFullscreen;			// Triggered when window toggles fullscreen mode.

	WindowKeyDownEvent					mOnWindowKeyDown;				// Triggered when a key is pressed down.
	WindowKeyUpEvent					mOnWindowKeyUp;					// Triggered when a key is released.
	WindowCharEvent						mOnWindowKeyChar;				// Triggered when a key character is pressed.

	WindowMouseMoveEvent				mOnWindowMouseMove;				// Triggered when mouse is moved.
	WindowMouseDblClickEvent			mOnWindowMouseDoubleClick;		// Trigger when mouse is double clicked.
	WindowMouseDownEvent				mOnWindowMouseDown;				// Triggered when mouse button is pressed down.
	WindowMouseUpEvent					mOnWindowMouseUp;				// Triggered when mouse button is released.
	WindowMouseWheelEvent				mOnWindowMouseWheel;			// Triggered when mouse wheel is moved.

	WindowMouseEnterEvent				mOnWindowMouseEnter;			// Triggered when mouse enters the window.
	WindowMouseLeaveEvent				mOnWindowMouseLeave;			// Triggered when mouse leaves the window.
};

struct WindowSettings
{
	std::string title = "FBX_Loader";

	UINT x = 0;
	UINT y = 0;

	UINT w = 800;
	UINT h = 600;

	UINT min_w = 800;
	UINT min_h = 600;

	UINT max_w = 1920;
	UINT max_h = 1080;

	UINT flags = WS_OVERLAPPEDWINDOW;

};

class PlatformWindow
{
public:
	PlatformWindow();
	PlatformWindow(const WindowSettings & gs);
	~PlatformWindow();

	void SetInstance(HINSTANCE hInstance);
	void SetTitle(const std::string &name);
	const std::string & GetTitle() const;

	void SetFullscreen(bool state);
	bool GetFullscreen() const;

	void SetPosition(UINT x, UINT y);
	UINT GetX() const;
	UINT GetY() const;

	UINT  GetWidth() const;
	UINT  GetHeight() const;

	bool IsCursorIn() const;

	void OpenWindow();
	void CloseWindow();

	void Update();

	WindowEvents & GetEventList();
	WindowSettings & GetSettings();

private:
	void RegisterWindow();
	void ProcessWindowEvents();
	void ProcessEvents(UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

	// events
	WindowEvents	mEventList;

	// fullscreen
	bool			mFullscreen = false;

	// window attributes
	WindowSettings	mSettings = {};

	// cursor
	bool			mIsCursorIn = true;
	HCURSOR			mCurrentCursor = nullptr;
	HICON			mIcon = nullptr;

	// windows data
	HINSTANCE		mInstance = GetModuleHandle(NULL);
	HWND			mHwnd = nullptr;
	std::string		sTitle;
	static bool		mIsWindowCreated;

	friend class Graphics;
};