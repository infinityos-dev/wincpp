#pragma once

#include <stdint.h>
#include <string>
#include <windows.h>

#include "WindowClass.hpp"

class Window {
public:
	Window(uint32_t width, uint32_t height, std::string title);
};