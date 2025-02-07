
#pragma once

#include <windows.h>

class Game;

class Window
{
	HWND		hWnd;
	HDC			hDC;
	HGLRC		hRC;

	Game* game;

public:
	enum Error
	{
		REGISTERCLASS,
		CREATEWINDOW,
		GETDC,
		CHOOSEPIXELFORMAT,
		SETPIXELFORMAT,
		CREATERC,
		MAKECURRENTRC,

		NONE
	};

	static const wchar_t* GetErrorText(Error errorId);

	Window();
	~Window();

	void SetGame(Game* newGame) { game = newGame; }
	Game* GetGame() { return game; }

	Error Create(const wchar_t* className, const wchar_t* title, int width, int height);
	void Destroy(const wchar_t* className);
	void Init(int iW, int iH);
	void Show();
	void Hide();
	void Resize(int iW, int iH);
	void Draw();
	void Update();
};
