#pragma once

#include <stdint.h>
#include <string>
#include <windows.h>

#include "WindowClass.hpp"

class Window {
public:
	// Constructor with only class name, hInstance and window name
	Window(LPCWSTR windowClassName, LPCWSTR windowName, HINSTANCE hInstance) {
		m_NativeWindow = CreateWindowEx(0, windowClassName, windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
		if (!m_NativeWindow) {
			DWORD error = GetLastError();
			throw std::runtime_error("Failed to create window. Error code: " + std::to_string(error));
		}
	}

	// Constructor that are closer to `CreateWindowEx`
	Window(LPCWSTR windowClassName, LPCWSTR windowName, HINSTANCE hInstance, int x, int y, int width, int height, DWORD style, DWORD exStyle = 0) {
		m_NativeWindow = CreateWindowEx(exStyle, windowClassName, windowName, style, x, y, width, height, NULL, NULL, hInstance, NULL);
		if (!m_NativeWindow) {
			throw std::runtime_error("Failed to create window.");
		}
	}

	// Shows the window with `nCmdShow` as 10
	void Show() {
		ShowWindow(m_NativeWindow, (int)10);
	}

	// Shows the window based on `nCmdShow`
	void Show(int style) {
		ShowWindow(m_NativeWindow, style);
	}

	// Sets thw window procedure. If possible this should have been set in the WindowClass constructor
	void SetWindowProcedure(WNDPROC windowProc) {
		SetWindowLongPtr(m_NativeWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(windowProc));
	}

private:
	HWND m_NativeWindow;
};