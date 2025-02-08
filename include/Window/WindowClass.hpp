#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <functional>
#include <windows.h>
#include <stdexcept> // For std::runtime_error

class WindowClass {
public:
    // Default constructor
    WindowClass() : m_NativeClass{ 0 } {}

    // Constructor with all parameters (no default values)
    WindowClass(WNDPROC procedure, HINSTANCE hInstance, HBRUSH background, LPCWSTR className)
    {
        Initialize(procedure, hInstance, background, className);
    }

    // Constructor without background brush (defaults to COLOR_WINDOW + 1)
    WindowClass(WNDPROC procedure, HINSTANCE hInstance, LPCWSTR className)
    {
        Initialize(procedure, hInstance, (HBRUSH)(COLOR_WINDOW + 1), className);
    }

    // Constructor without class name (auto-generates a unique name)
    WindowClass(WNDPROC procedure, HINSTANCE hInstance, HBRUSH background)
    {
        // Generate a unique class name using hashing
        std::size_t hashValue = std::hash<void*>{}(reinterpret_cast<void*>(procedure)) ^
            std::hash<void*>{}(reinterpret_cast<void*>(hInstance)) ^
            std::hash<void*>{}(reinterpret_cast<void*>(background));

        // Convert hash to a wide string
        std::wstringstream wss;
        wss << L"Class" << hashValue;
        std::wstring classNameStr = wss.str();

        // Assign the string's c_str() (which remains valid in this scope)
        LPCWSTR className = _wcsdup(classNameStr.c_str());
        if (!className) {
            throw std::bad_alloc(); // Handle memory allocation failure
        }

        Initialize(procedure, hInstance, background, className);
    }

    // Destructor
    ~WindowClass() {
        Unregister();
        if (m_NativeClass.lpszClassName) {
            free((void*)m_NativeClass.lpszClassName);
        }
    }

    // Delete copy constructor and copy assignment operator
    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    // Move constructor
    WindowClass(WindowClass&& other) noexcept {
        m_NativeClass = other.m_NativeClass;
        other.m_NativeClass.lpszClassName = nullptr; // Prevent double-free
    }

    // Move assignment operator
    WindowClass& operator=(WindowClass&& other) noexcept {
        if (this != &other) {
            Unregister();
            if (m_NativeClass.lpszClassName) {
                free((void*)m_NativeClass.lpszClassName);
            }
            m_NativeClass = other.m_NativeClass;
            other.m_NativeClass.lpszClassName = nullptr; // Prevent double-free
        }
        return *this;
    }

    operator WNDCLASS() {
        return m_NativeClass;
    }

    // Register the window class
    bool Register() {
        if (!RegisterClass(&m_NativeClass)) {
            // Handle error (e.g., log or throw an exception)
            return false;
        }
        return true;
    }

    // Unregister the window class
    void Unregister() {
        if (m_NativeClass.lpszClassName) {
            UnregisterClass(m_NativeClass.lpszClassName, m_NativeClass.hInstance);
        }
    }

    // Get the class name
    LPCWSTR GetClassName() const {
        return m_NativeClass.lpszClassName;
    }

private:
    // Helper function to initialize the WNDCLASS structure
    void Initialize(WNDPROC procedure, HINSTANCE hInstance, HBRUSH background, LPCWSTR className) {
        m_NativeClass = { 0 }; // Zero-initialize the structure
        m_NativeClass.lpfnWndProc = procedure;
        m_NativeClass.hInstance = hInstance;
        m_NativeClass.hbrBackground = background;
        m_NativeClass.lpszClassName = className;

        // Set default values for other WNDCLASS members
        m_NativeClass.style = CS_HREDRAW | CS_VREDRAW; // Example style
        m_NativeClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // Default cursor
        m_NativeClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // Default icon
    }

    WNDCLASS m_NativeClass;
};