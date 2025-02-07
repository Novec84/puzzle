
#include "Font.h"

#include <gl/gl.h>

Font::Font()
	: hFont(NULL),
		fontBase(0),
		height(0)
{
}

bool Font::Create(HDC dc, const wchar_t* name, int h, bool bold, bool cursive, bool underscore, bool crossed)
{
	height = h;

	hFont = CreateFont(height,
		0,
		0,
		0,
		bold ? FW_BOLD : FW_NORMAL,
		cursive,
		underscore,
		crossed,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		name);
	if (!hFont)
		return false;
	SelectObject(dc, hFont);

	fontBase = glGenLists(256);
	if (!wglUseFontBitmaps(dc, 0, 255, fontBase))
		return false;

	GetCharABCWidths(dc, 0, 255, &glyphInfo[0]);

	return true;
}

void Font::Destroy()
{
	if (fontBase)
	{
		glDeleteLists(fontBase, 256);
		fontBase = 0;
	}

	if (hFont)
	{
		DeleteObject(hFont);
		hFont = NULL;
	}
}
