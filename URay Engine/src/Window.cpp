#include "Window.h"

namespace URay
{
	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	void Window::Create(const wchar_t* title, int width, int height, HINSTANCE hInstance)
	{
		WCHAR windowClass[] = L"WindowClass";
		WNDCLASS wndClass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, windowClass };
		RegisterClass(&wndClass);

		_hWnd = CreateWindowEx(0, windowClass, title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			nullptr, nullptr, hInstance, nullptr);
	}

	void Window::Release()
	{
		if (_hWnd)
		{
			DestroyWindow(_hWnd);
			_hWnd = nullptr;
		}
	}
}