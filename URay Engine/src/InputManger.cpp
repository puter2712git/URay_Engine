#include "InputManager.h"

namespace URay
{
	void InputManager::Update()
	{
		for (int i = 0; i < KEY_COUNT; ++i)
		{
			_prevKeyStates[i] = _keyStates[i];
		}
		for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
		{
			_prevMouseStates[i] = _mouseStates[i];
		}

		for (int i = 0; i < KEY_COUNT; ++i)
		{
			_keyStates[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
		}

		_mouseStates[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
		_mouseStates[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
		_mouseStates[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

		POINT cursorPos;
		if (GetCursorPos(&cursorPos))
		{
			_mouseX = cursorPos.x;
			_mouseY = cursorPos.y;
		}
	}

	bool InputManager::GetKey(int keyCode) const
	{
		if (keyCode < 0 || keyCode >= KEY_COUNT)
		{
			return false;
		}
		return _keyStates[keyCode];
	}

	bool InputManager::GetKeyDown(int keyCode) const
	{
		if (keyCode < 0 || keyCode >= KEY_COUNT)
		{
			return false;
		}
		return _keyStates[keyCode] && !_prevKeyStates[keyCode];
	}

	bool InputManager::GetKeyUp(int keyCode) const
	{
		if (keyCode < 0 || keyCode >= KEY_COUNT)
		{
			return false;
		}
		return !_keyStates[keyCode] && _prevKeyStates[keyCode];
	}

	bool InputManager::GetMouseButton(int button) const
	{
		if (button < 0 || button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}
		return _mouseStates[button];
	}

	bool InputManager::GetMouseButtonDown(int button) const
	{
		if (button < 0 || button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}
		return _mouseStates[button] && !_prevMouseStates[button];
	}

	bool InputManager::GetMouseButtonUp(int button) const
	{
		if (button < 0 || button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}
		return !_mouseStates[button] && _prevMouseStates[button];
	}
}