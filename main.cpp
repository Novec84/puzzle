#include "base\Application.h"
#include "base\Window.h"
#include "Puzzle.h"

constexpr auto CLASSNAME = L"puzzle-window";
constexpr auto WINDOWTITLE = L"Puzzle";
constexpr int WINDOWWIDTH = 1024;
constexpr int WINDOWHEIGHT = 768;
constexpr int FPS = 20;

int main()
{
	Application		application;
	Window			window;
	Window::Error	error;
	Puzzle			game;

	application.PrepareRandoms();

	error = window.Create(CLASSNAME, WINDOWTITLE, WINDOWWIDTH, WINDOWHEIGHT);
	if (error != Window::NONE)
	{
		MessageBox(NULL, Window::GetErrorText(error), L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	window.SetGame(&game);

	window.Init(WINDOWWIDTH, WINDOWHEIGHT);
	window.Show();

	application.GameLoop(&window, FPS);

	window.Hide();
	window.Deinit();
	window.Destroy(CLASSNAME);

	return 0;
}