#pragma once

#include <windows.h>

#include "Window.h"

class Application
{
public:
	void PrepareRandoms();
	void GameLoop(Window* window, int fps);
};
