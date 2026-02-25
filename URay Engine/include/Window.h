#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

namespace URay
{
	class Window
	{
	public:
		Window() = default;
		~Window() = default;

	public:
		void Create(const wchar_t* title, int width, int height, HINSTANCE hInstance);
		void Release();

		HWND GetHandle() const { return _hWnd; }

	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND _hWnd;
	};
}

#endif
