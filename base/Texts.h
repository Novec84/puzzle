#pragma once

#include <Windows.h>

class Texts
{
	static HDC dc;
public:
	Texts() = delete;

	static void SetDC(HDC newDC) { dc = newDC; }
	static void Destroy();

	static unsigned CreateFont(const wchar_t* name, int height, bool bold, bool cursive, bool underscore, bool crossed);
	static void DestroyFont(unsigned fontId);

	static int GetTextHeight(unsigned fontId);
	static int GetTextWidth(unsigned fontId, const char* fmt, ...);
	static void DrawText(unsigned fontId, double x, double y, const char* fmt, ...);
};
