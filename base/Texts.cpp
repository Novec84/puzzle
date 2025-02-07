
#include "Texts.h"
#include "Font.h"

#include <gl/gl.h>

#include <map>
#include <cstdarg>

typedef std::map<unsigned, Font*> FontStoreType;

HDC Texts::dc;
FontStoreType fonts;

void Texts::Destroy()
{
	for (auto const& it : fonts)
	{
		Font* font = it.second;
		font->Destroy();
		delete font;
	}
	fonts.clear();
}

//L"Arial";L"Courier New";
unsigned Texts::CreateFont(const wchar_t* name, int height, bool bold, bool cursive, bool underscore, bool crossed)
{
	Font* font = new Font();
	if (!font->Create(dc, name, height, bold, cursive, underscore, crossed))
	{
		font->Destroy();
		delete font;
		return 0;
	}
	unsigned fontId = fonts.empty() ? 0 : fonts.rbegin()->first;
	fontId++;
	fonts[fontId] = font;
	return fontId;
}

void Texts::DestroyFont(unsigned fontId)
{
	FontStoreType::iterator it = fonts.find(fontId);
	if (it == fonts.end())
		return;
	Font* font = it->second;
	font->Destroy();
	delete font;
	fonts.erase(it);
}

int Texts::GetTextHeight(unsigned fontId)
{
	return (fonts.find(fontId) == fonts.end()) ? 0 : fonts[fontId]->height;
}

int Texts::GetTextWidth(unsigned fontId, const char* fmt, ...)
{
	if (fonts.find(fontId) == fonts.end())
		return 0;
	Font* font = fonts[fontId];

	char text[256];
	std::va_list args;
	va_start(args, fmt);
	vsnprintf(text, 256, fmt, args);
	va_end(args);

	int w = 0;
	for (int i = 0; i < strlen(text); i++)
		w += font->glyphInfo[text[i]].abcA + font->glyphInfo[text[i]].abcB + font->glyphInfo[text[i]].abcC;

	return w;
}

void Texts::DrawText(unsigned fontId, double x, double y, const char* fmt, ...)
{
	if (fonts.find(fontId) == fonts.end())
		return;
	Font* font = fonts[fontId];

	char text[256];
	std::va_list args;
	va_start(args, fmt);
	vsnprintf(text, 256, fmt, args);
	va_end(args);

	glRasterPos2d(x, y);
	glPushAttrib(GL_LIST_BIT);
	glListBase(font->fontBase);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}
