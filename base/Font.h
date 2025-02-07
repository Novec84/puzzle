#pragma once

#include <Windows.h>

class Font
{
public:
	HFONT		hFont;
	unsigned	fontBase;
	ABC			glyphInfo[256];
	int			height;

	Font();

	bool Create(HDC dc, const wchar_t* name, int h, bool bold, bool cursive, bool underscore, bool crossed);
	void Destroy();
};
