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

	// Destroys the window
	~Window() {
		if (m_NativeWindow) {
			DestroyWindow(m_NativeWindow);
		}
	}

	// Shows the window with the given style
	void Show(int style = SW_SHOW) {
		ShowWindow(m_NativeWindow, style);
	}

	// Hides the window
	void Hide() {
		ShowWindow(m_NativeWindow, SW_HIDE);
	}

	void SetSize(int width, int height) {
		SetWindowPos(m_NativeWindow, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	}

	DWORD GetStyle() const {
		return GetWindowLong(m_NativeWindow, GWL_STYLE);
	}

	void SetStyle(DWORD style) {
		SetWindowLong(m_NativeWindow, GWL_STYLE, style);
		SetWindowPos(m_NativeWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	DWORD GetExStyle() const {
		return GetWindowLong(m_NativeWindow, GWL_EXSTYLE);
	}

	void SetExStyle(DWORD exStyle) {
		SetWindowLong(m_NativeWindow, GWL_EXSTYLE, exStyle);
		SetWindowPos(m_NativeWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void SetIcon(HICON icon) {
		SendMessage(m_NativeWindow, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
		SendMessage(m_NativeWindow, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
	}

	void SetCursor(HCURSOR cursor) {
		SetClassLongPtr(m_NativeWindow, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
	}


	bool IsVisible() const {
		return IsWindowVisible(m_NativeWindow) != 0;
	}

	bool IsMaximized() const {
		return IsZoomed(m_NativeWindow) != 0;
	}

	bool IsMinimized() const {
		return IsIconic(m_NativeWindow) != 0;
	}


	void SetPosition(int x, int y) {
		SetWindowPos(m_NativeWindow, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Maximize() {
		ShowWindow(m_NativeWindow, SW_MAXIMIZE);
	}

	void Minimize() {
		ShowWindow(m_NativeWindow, SW_MINIMIZE);
	}

	void Restore() {
		ShowWindow(m_NativeWindow, SW_RESTORE);
	}

	// Sets thw window procedure. If possible this should have been set in the WindowClass constructor
	void SetWindowProcedure(WNDPROC windowProc) {
		SetWindowLongPtr(m_NativeWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(windowProc));
	}

	void SetTransparency(BYTE alpha) {
		SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(m_NativeWindow, 0, alpha, LWA_ALPHA);
	}

	void SetLayered(bool layered) {
		if (layered) {
			SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
		}
		else {
			SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		}
	}

	void SendSystemCommand(UINT command) {
		SendMessage(m_NativeWindow, WM_SYSCOMMAND, command, 0);
	}

	void SetFocus() {
		::SetFocus(m_NativeWindow);
	}

	bool IsActive() const {
		return GetForegroundWindow() == m_NativeWindow;
	}

	void SetMenu(HMENU menu) {
		::SetMenu(m_NativeWindow, menu);
	}

	HMENU GetMenu() const {
		return ::GetMenu(m_NativeWindow);
	}

	void SetParent(HWND parent) {
		::SetParent(m_NativeWindow, parent);
	}

	void SetOwner(HWND owner) {
		SetWindowLongPtr(m_NativeWindow, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(owner));
	}

	void Redraw() {
		RedrawWindow(m_NativeWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	void Invalidate() {
		InvalidateRect(m_NativeWindow, NULL, TRUE);
	}

	void CopyToClipboard(const std::wstring& text) {
		if (OpenClipboard(m_NativeWindow)) {
			EmptyClipboard();
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
			if (hGlobal) {
				wchar_t* pGlobal = static_cast<wchar_t*>(GlobalLock(hGlobal));
				if (pGlobal) {
					wcscpy_s(pGlobal, text.size() + 1, text.c_str());
					GlobalUnlock(hGlobal);
					SetClipboardData(CF_UNICODETEXT, hGlobal);
				}
			}
			CloseClipboard();
		}
	}

	std::wstring PasteFromClipboard() {
		std::wstring result;
		if (OpenClipboard(m_NativeWindow)) {
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			if (hData) {
				wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hData));
				if (pText) {
					result = pText;
					GlobalUnlock(hData);
				}
			}
			CloseClipboard();
		}
		return result;
	}

	void SetTimer(UINT_PTR id, UINT elapse) {
		::SetTimer(m_NativeWindow, id, elapse, NULL);
	}

	void KillTimer(UINT_PTR id) {
		::KillTimer(m_NativeWindow, id);
	}

	void ShowContextMenu(HMENU menu, int x, int y) {
		TrackPopupMenu(menu, TPM_RIGHTBUTTON, x, y, 0, m_NativeWindow, NULL);
	}

	UINT GetDPI() const {
		return GetDpiForWindow(m_NativeWindow);
	}

	void SetDPIAwareness() {
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}

	void SetBackgroundColor(COLORREF color) {
		HBRUSH brush = CreateSolidBrush(color);
		SetClassLongPtr(m_NativeWindow, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush));
		InvalidateRect(m_NativeWindow, NULL, TRUE);
	}

	void BringToTop() {
		SetWindowPos(m_NativeWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	void SendToBottom() {
		SetWindowPos(m_NativeWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	void SetTitle(LPCWSTR title) {
		SetWindowText(m_NativeWindow, title);
	}

	std::wstring GetTitle() const {
		wchar_t buffer[256];
		GetWindowText(m_NativeWindow, buffer, 256);
		return std::wstring(buffer);
	}

	// Get the HWND handle
	HWND GetHandle() const {
		return m_NativeWindow;
	}

	// Runs the default message loop
	void RunDefaultMessageLoop() {
		MSG msg = {};
		while (GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

private:
	HWND m_NativeWindow;
};