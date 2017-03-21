#pragma once

#include "Core/GameComponent.h"
#include "Core/IUpdatable.h"
#include "Core/Event.h"
#include "KeyBind.h"
#include "Math/Vec2.h"

#include <vector>
#include <array>

namespace Input
{
	enum Action
	{
		Triggered = 0,
		Pressed,
		Released,

		Count
	};
}

class InputManager : public GameComponent/*, public IUpdatable*/
{
public:
	typedef Event< void() >								InputEvent;
	typedef InputEvent::Delegate						InputDelegate;
	typedef std::vector< InputEvent * >					InputEventList;
	typedef std::vector< InputEventList >				InputActionEventList;

	typedef Event< void(int, int) >						MouseEvent;
	typedef MouseEvent::Delegate						MouseDelegate;
	typedef std::vector< MouseEvent * >					MouseEventList;
	typedef std::vector< MouseEventList >				MouseActionEventList;
	typedef Event< void(int) >							MouseWheelEvent;
	typedef MouseWheelEvent::Delegate					MouseWheelDelegate;

	typedef Event< void(Key::KeyCode) >					KeyCodeEvent;
	typedef KeyCodeEvent::Delegate						KeyCodeDelegate;
	typedef Event< void(int, int, Mouse::Button) >		MouseCodeEvent;
	typedef MouseCodeEvent::Delegate					MouseCodeDelegate;

	typedef std::array< bool, Key::Count >				KeyPressedList;
	typedef std::array< bool, Mouse::Count >			MousePressedList;

	ursine::Vec2 GetMousePosition() const { return mMousePosition; }

public:
	// Constructor
	InputManager();

	// Destructor
	~InputManager();

	// Initialise
	void Initialize() override;

	// Shutdown
	void Shutdown() override;

	// Update
	//void Update( f32 dt ) override;

	// Add Key Event
	void AddKeyEvent(Key::KeyCode key, Input::Action action, const InputDelegate & idelegate);

	// Remove Key Event
	void RemoveKeyEvent(Key::KeyCode key, Input::Action action, const InputDelegate & idelegate);

	// Add Mouse Event
	void AddMouseEvent(Mouse::Button button, Input::Action action, const MouseDelegate & mdelegate);

	// Remove Mouse Event
	void RemoveMouseEvent(Mouse::Button button, Input::Action action, const MouseDelegate & mdelegate);

	// Add Mouse Double Click Event
	void AddMouseDoubleClickEvent(Mouse::Button button, const MouseDelegate & mdelegate);

	// Remove Mouse Double Click Event
	void RemoveMouseDoubleClickEvent(Mouse::Button button, const MouseDelegate & mdelegate);

	// Add Mouse Move Event
	void AddMouseMoveEvent(const MouseDelegate & mdelegate);

	// Remove Mouse Move Event
	void RemoveMouseMoveEvent(const MouseDelegate & mdelegate);

	// Add Mouse Wheel Event
	void AddMouseWheelEvent(const MouseWheelDelegate & mdelegate);

	// Remove Mouse Wheel Event
	void RemoveMouseWheelEvent(const MouseWheelDelegate & mdelegate);

public:
	/*
		Add Key down event (Note: Key event handles key down as well,
								  this features is only for APIs that
								  need access to key callback!)
	*/
	void AddKeyDownEvent(const KeyCodeDelegate & kdelegate);
	void RemoveKeyDownEvent(const KeyCodeDelegate & kdelegate);

	/*
		Add Key up event (Note: Key event handles key down as well,
								this features is only for APIs that
								need access to key callback!)
	*/
	void AddKeyUpEvent(const KeyCodeDelegate & kdelegate);
	void RemoveKeyUpEvent(const KeyCodeDelegate & kdelegate);

	/*
		Add Key triggered event (Note: Key event handles key down as well,
									   this features is only for APIs that
									   need access to key callback!)
	*/
	void AddKeyTriggeredEvent(const KeyCodeDelegate & kdelegate);
	void RemoveKeyTriggeredEvent(const KeyCodeDelegate & kdelegate);

	/*
		Add Mouse down event (Note: Key event handles key down as well,
									this features is only for APIs that
									need access to key callback!)
	*/
	void AddMouseDownEvent(const MouseCodeDelegate & mdelegate);
	void RemoveMouseDownEvent(const MouseCodeDelegate & mdelegate);

	/*
		Add Mouse up event (Note: Key event handles key down as well,
								  this features is only for APIs that
								  need access to key callback!)
	*/
	void AddMouseUpEvent(const MouseCodeDelegate & mdelegate);
	void RemoveMouseUpEvent(const MouseCodeDelegate & mdelegate);

	/*
		Add Mouse triggered event (Note: Key event handles key down as well,
										 this features is only for APIs that
										 need access to key callback!)
	*/
	void AddMouseTriggeredEvent(const MouseCodeDelegate & mdelegate);
	void RemoveMouseTriggeredEvent(const MouseCodeDelegate & mdelegate);

private:
	// Input Callbacks

	// Key down callback
	void OnKeyDownCallback(Key::KeyCode code);

	// Key up callback
	void OnKeyUpCallback(Key::KeyCode code);

	// Mouse down callback
	void OnMouseDownCallback(int x, int y, Mouse::Button button);

	// Mouse up callback
	void OnMouseUpCallback(int x, int y, Mouse::Button button);

	// Mouse double click callback
	void OnMouseDoubleClickCallback(int x, int y, Mouse::Button button);

	// Mouse move callback
	void MouseMoveCallback(int x, int y);

private:
	// Do not need mutex as every event has their own
	KeyPressedList mKeyPressed;				//!< Current key pressed
	KeyPressedList mPrevKeyPressed;			//!< Previous key pressed
	MousePressedList mMousePressed;			//!< Current mouse pressed
	MousePressedList mPrevMousePressed;		//!< Previous mouse pressed

	InputActionEventList mKeyEvents;		//!< Key Events
	MouseActionEventList mMouseEvents;		//!< Mouse Events
	MouseEventList mMouseDblClickEvents;	//!< Mouse Double Click Events
	MouseEvent mMouseMoveEvent;				//!< Mouse Move Event
	MouseWheelEvent mMouseWheelEvent;		//!< Mouse Wheel Event

	KeyCodeEvent mKeyUpEvent;				//!< Key up event
	KeyCodeEvent mKeyDownEvent;				//!< Key down event
	KeyCodeEvent mKeyTriggeredEvent;		//!< Key triggered event
	MouseCodeEvent mMouseUpEvent;			//!< Mouse up event
	MouseCodeEvent mMouseDownEvent;			//!< Mouse down event
	MouseCodeEvent mMouseTriggeredEvent;	//!< Mouse triggered event

	ursine::Vec2 mMousePosition;			//!< Mouse screen pos
};

#include "Typing.h"
DECLARE_TYPE(InputManager, InputManager);