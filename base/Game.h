#pragma once

#include "KeyCode.h"

class Game
{
public:
	Game() {}
	virtual ~Game() {}

	virtual void Init(int iW, int iH) = 0;
	virtual void Resize(int iW, int iH) = 0;
	virtual void Draw() = 0;
	virtual void Update() = 0;

	virtual void MouseLeftDown(int xPos, int yPos) = 0;
	virtual void MouseLeftUp(int xPos, int yPos) = 0;
	virtual void MouseRightDown(int xPos, int yPos) = 0;
	virtual void MouseRightUp(int xPos, int yPos) = 0;
	virtual void MouseMove(int xPos, int yPos) = 0;

	virtual void KeyDown(KeyCode keyCode) = 0;
	virtual void KeyUp(KeyCode keyCode) = 0;
};