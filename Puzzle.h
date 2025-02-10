#pragma once

#include "base\Game.h"

constexpr int MINROWS = 3;
constexpr int MAXROWS = 10;
constexpr int MINCOLUMNS = 3;
constexpr int MAXCOLUMNS = 10;

class Puzzle :public Game
{
	unsigned numbersFontId;

	int matrix[MAXROWS][MAXCOLUMNS];
	int rowCount;
	int columnCount;

	double matrixPositionX;
	double matrixPositionY;
	double cellWidth;
	double cellHeight;

	bool dayMode;
	bool drawGrid;

	void InitializeMatrix(int iRows, int iColumns);
	void LayoutMatrix(int iW, int iH);

	bool IsResolved();
public:
	Puzzle();
	virtual ~Puzzle();

	void SetDayMode(bool newDayMode) { dayMode = newDayMode; }
	void SetDrawGrid(bool newDrawGrid) { drawGrid = newDrawGrid; }

	virtual void Init(int iW, int iH) override;
	virtual void Resize(int iW, int iH) override;
	virtual void Draw() override;
	virtual void Update() override {};

	virtual void MouseLeftDown(int xPos, int yPos) override {}
	virtual void MouseLeftUp(int xPos, int yPos) override;
	virtual void MouseRightDown(int xPos, int yPos) override {};
	virtual void MouseRightUp(int xPos, int yPos) override {};
	virtual void MouseMove(int xPos, int yPos) override {};

	virtual void KeyDown(KeyCode keyCode) override {};
	virtual void KeyUp(KeyCode keyCode) override {};
};