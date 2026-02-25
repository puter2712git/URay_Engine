#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <windows.h>

namespace URay
{
	class InputManager
	{
	public:
		InputManager() = default;
		~InputManager() = default;

		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

	public:
		void Update();

		bool GetKey(int keyCode) const;
		bool GetKeyDown(int keyCode) const;
		bool GetKeyUp(int keyCode) const;

		bool GetMouseButton(int button) const;
		bool GetMouseButtonDown(int button) const;
		bool GetMouseButtonUp(int button) const;

		int GetMouseX() const { return _mouseX; }
		int GetMouseY() const { return _mouseY; }

	private:
		static constexpr int KEY_COUNT = 256;
		bool _keyStates[KEY_COUNT] = {};
		bool _prevKeyStates[KEY_COUNT] = {};

		static constexpr int MOUSE_BUTTON_COUNT = 3;
		bool _mouseStates[MOUSE_BUTTON_COUNT] = {};
		bool _prevMouseStates[MOUSE_BUTTON_COUNT] = {};

		int _mouseX = 0;
		int _mouseY = 0;
	};
}

#endif
