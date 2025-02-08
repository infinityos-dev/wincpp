#pragma once

#include <stdint.h>
#include <string>
#include <windows.h>
#include "WindowClass.hpp"

class Window {
public:
    /**
     * @brief Constructor to create a window with a specified class name, window name, and instance handle.
     *
     * @param windowClassName The name of the window class.
     * @param windowName The name of the window.
     * @param hInstance The instance handle of the application.
     *
     * @throws std::runtime_error If window creation fails.
     */
    Window(LPCWSTR windowClassName, LPCWSTR windowName, HINSTANCE hInstance) {
        m_NativeWindow = CreateWindowEx(0, windowClassName, windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
        if (!m_NativeWindow) {
            DWORD error = GetLastError();
            throw std::runtime_error("Failed to create window. Error code: " + std::to_string(error));
        }
    }

    /**
     * @brief Constructor to create a window with more flexible parameters such as position, size, and styles.
     *
     * @param windowClassName The name of the window class.
     * @param windowName The name of the window.
     * @param hInstance The instance handle of the application.
     * @param x The x-position of the window.
     * @param y The y-position of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     * @param style The window style.
     * @param exStyle The extended window style (default is 0).
     *
     * @throws std::runtime_error If window creation fails.
     */
    Window(LPCWSTR windowClassName, LPCWSTR windowName, HINSTANCE hInstance, int x, int y, int width, int height, DWORD style, DWORD exStyle = 0) {
        m_NativeWindow = CreateWindowEx(exStyle, windowClassName, windowName, style, x, y, width, height, NULL, NULL, hInstance, NULL);
        if (!m_NativeWindow) {
            throw std::runtime_error("Failed to create window.");
        }
    }

    /**
     * @brief Destructor to destroy the window when it's no longer needed.
     */
    ~Window() {
        if (m_NativeWindow) {
            DestroyWindow(m_NativeWindow);
        }
    }

    /**
     * @brief Shows the window with the given style.
     *
     * @param style The show style (default is SW_SHOW).
     */
    void Show(int style = SW_SHOW) {
        ShowWindow(m_NativeWindow, style);
    }

    /**
     * @brief Hides the window.
     */
    void Hide() {
        ShowWindow(m_NativeWindow, SW_HIDE);
    }

    /**
     * @brief Sets the size of the window.
     *
     * @param width The new width of the window.
     * @param height The new height of the window.
     */
    void SetSize(int width, int height) {
        SetWindowPos(m_NativeWindow, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    }

    /**
     * @brief Retrieves the window style.
     *
     * @return The current window style.
     */
    DWORD GetStyle() const {
        return GetWindowLong(m_NativeWindow, GWL_STYLE);
    }

    /**
     * @brief Sets the window style.
     *
     * @param style The new window style.
     */
    void SetStyle(DWORD style) {
        SetWindowLong(m_NativeWindow, GWL_STYLE, style);
        SetWindowPos(m_NativeWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    /**
     * @brief Retrieves the extended window style.
     *
     * @return The current extended window style.
     */
    DWORD GetExStyle() const {
        return GetWindowLong(m_NativeWindow, GWL_EXSTYLE);
    }

    /**
     * @brief Sets the extended window style.
     *
     * @param exStyle The new extended window style.
     */
    void SetExStyle(DWORD exStyle) {
        SetWindowLong(m_NativeWindow, GWL_EXSTYLE, exStyle);
        SetWindowPos(m_NativeWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    /**
     * @brief Sets the window's icon.
     *
     * @param icon The icon handle to set as the window icon.
     */
    void SetIcon(HICON icon) {
        SendMessage(m_NativeWindow, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
        SendMessage(m_NativeWindow, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
    }

    /**
     * @brief Sets the cursor for the window.
     *
     * @param cursor The cursor handle to set.
     */
    void SetCursor(HCURSOR cursor) {
        SetClassLongPtr(m_NativeWindow, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
    }

    /**
     * @brief Checks if the window is visible.
     *
     * @return True if the window is visible, false otherwise.
     */
    bool IsVisible() const {
        return IsWindowVisible(m_NativeWindow) != 0;
    }

    /**
     * @brief Checks if the window is maximized.
     *
     * @return True if the window is maximized, false otherwise.
     */
    bool IsMaximized() const {
        return IsZoomed(m_NativeWindow) != 0;
    }

    /**
     * @brief Checks if the window is minimized.
     *
     * @return True if the window is minimized, false otherwise.
     */
    bool IsMinimized() const {
        return IsIconic(m_NativeWindow) != 0;
    }

    /**
     * @brief Sets the position of the window.
     *
     * @param x The new x-coordinate of the window.
     * @param y The new y-coordinate of the window.
     */
    void SetPosition(int x, int y) {
        SetWindowPos(m_NativeWindow, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    /**
     * @brief Maximizes the window.
     */
    void Maximize() {
        ShowWindow(m_NativeWindow, SW_MAXIMIZE);
    }

    /**
     * @brief Minimizes the window.
     */
    void Minimize() {
        ShowWindow(m_NativeWindow, SW_MINIMIZE);
    }

    /**
     * @brief Restores the window from minimized or maximized state.
     */
    void Restore() {
        ShowWindow(m_NativeWindow, SW_RESTORE);
    }

    /**
     * @brief Sets a custom window procedure for the window.
     *
     * @param windowProc The new window procedure to set.
     */
    void SetWindowProcedure(WNDPROC windowProc) {
        SetWindowLongPtr(m_NativeWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(windowProc));
    }

    /**
     * @brief Sets the transparency of the window.
     *
     * @param alpha The transparency level (0-255).
     */
    void SetTransparency(BYTE alpha) {
        SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_NativeWindow, 0, alpha, LWA_ALPHA);
    }

    /**
     * @brief Enables or disables the layered window style.
     *
     * @param layered True to enable, false to disable.
     */
    void SetLayered(bool layered) {
        if (layered) {
            SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
        }
        else {
            SetWindowLong(m_NativeWindow, GWL_EXSTYLE, GetWindowLong(m_NativeWindow, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        }
    }

    /**
     * @brief Sends a system command to the window.
     *
     * @param command The system command to send.
     */
    void SendSystemCommand(UINT command) {
        SendMessage(m_NativeWindow, WM_SYSCOMMAND, command, 0);
    }

    /**
     * @brief Sets focus to the window.
     */
    void SetFocus() {
        ::SetFocus(m_NativeWindow);
    }

    /**
     * @brief Checks if the window is the active window.
     *
     * @return True if the window is active, false otherwise.
     */
    bool IsActive() const {
        return GetForegroundWindow() == m_NativeWindow;
    }

    /**
     * @brief Sets the window menu.
     *
     * @param menu The menu handle to set.
     */
    void SetMenu(HMENU menu) {
        ::SetMenu(m_NativeWindow, menu);
    }

    /**
     * @brief Retrieves the window menu.
     *
     * @return The menu handle associated with the window.
     */
    HMENU GetMenu() const {
        return ::GetMenu(m_NativeWindow);
    }

    /**
     * @brief Sets the parent window for this window.
     *
     * @param parent The parent window handle.
     */
    void SetParent(HWND parent) {
        ::SetParent(m_NativeWindow, parent);
    }

    /**
     * @brief Sets the owner window for this window.
     *
     * @param owner The owner window handle.
     */
    void SetOwner(HWND owner) {
        SetWindowLongPtr(m_NativeWindow, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(owner));
    }

    /**
     * @brief Forces a redraw of the window.
     */
    void Redraw() {
        RedrawWindow(m_NativeWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    /**
     * @brief Invalidates the window, causing it to be redrawn.
     */
    void Invalidate() {
        InvalidateRect(m_NativeWindow, NULL, TRUE);
    }

    /**
     * @brief Copies the specified text to the clipboard.
     *
     * @param text The text to copy to the clipboard.
     */
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

    /**
     * @brief Pastes text from the clipboard.
     *
     * @return The text retrieved from the clipboard.
     */
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

    /**
     * @brief Sets a timer for the window.
     *
     * @param id The timer identifier.
     * @param elapse The timer interval in milliseconds.
     */
    void SetTimer(UINT_PTR id, UINT elapse) {
        ::SetTimer(m_NativeWindow, id, elapse, NULL);
    }

    /**
     * @brief Kills the specified timer for the window.
     *
     * @param id The timer identifier.
     */
    void KillTimer(UINT_PTR id) {
        ::KillTimer(m_NativeWindow, id);
    }

    /**
     * @brief Shows a context menu at the specified position.
     *
     * @param menu The menu handle.
     * @param x The x-coordinate for the menu.
     * @param y The y-coordinate for the menu.
     */
    void ShowContextMenu(HMENU menu, int x, int y) {
        TrackPopupMenu(menu, TPM_RIGHTBUTTON, x, y, 0, m_NativeWindow, NULL);
    }

    /**
     * @brief Retrieves the DPI (dots per inch) of the window.
     *
     * @return The DPI of the window.
     */
    UINT GetDPI() const {
        return GetDpiForWindow(m_NativeWindow);
    }

    /**
     * @brief Sets the DPI awareness for the window.
     */
    void SetDPIAwareness() {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    /**
     * @brief Sets the background color of the window.
     *
     * @param color The new background color.
     */
    void SetBackgroundColor(COLORREF color) {
        HBRUSH brush = CreateSolidBrush(color);
        SetClassLongPtr(m_NativeWindow, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush));
        InvalidateRect(m_NativeWindow, NULL, TRUE);
    }

    /**
     * @brief Brings the window to the top of the z-order.
     */
    void BringToTop() {
        SetWindowPos(m_NativeWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    /**
     * @brief Sends the window to the bottom of the z-order.
     */
    void SendToBottom() {
        SetWindowPos(m_NativeWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    /**
     * @brief Sets the title of the window.
     *
     * @param title The new title.
     */
    void SetTitle(LPCWSTR title) {
        SetWindowText(m_NativeWindow, title);
    }

    /**
     * @brief Retrieves the current title of the window.
     *
     * @return The current title of the window.
     */
    std::wstring GetTitle() const {
        wchar_t buffer[256];
        GetWindowText(m_NativeWindow, buffer, 256);
        return std::wstring(buffer);
    }

    /**
     * @brief Retrieves the window handle (HWND) of the window.
     *
     * @return The window handle.
     */
    HWND GetHandle() const {
        return m_NativeWindow;
    }

    /**
     * @brief Runs the default message loop for the window.
     */
    void RunDefaultMessageLoop() {
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    HWND m_NativeWindow; ///< The native window handle.
};
