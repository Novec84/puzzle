#include "Application.h"

#include <time.h>

void Application::PrepareRandoms()
{
	srand((unsigned)time(NULL));
}

void Application::GameLoop(Window* window, int fps)
{
	MSG		msg;
	bool	bWork = true;
	int     sleepTime = 1000 / fps;

	while (bWork)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bWork = false;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			window->Update();
			window->Draw();
			Sleep(sleepTime);
		}
	}
}