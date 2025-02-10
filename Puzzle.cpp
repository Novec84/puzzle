#include <Windows.h>
#include <gl\GL.h>

#include "Puzzle.h"
#include "base\Texts.h"

constexpr double PADDING = 10.0;

Puzzle::Puzzle()
	: Game(),
		numbersFontId(0),
		rowCount(0),
		columnCount(0),
		matrixPositionX(0.0),
		matrixPositionY(0.0),
		cellWidth(0.0),
		cellHeight(0.0),
		dayMode(false),
		drawGrid(true)
{

}

Puzzle::~Puzzle()
{

}

void Puzzle::InitializeMatrix(int iRows, int iColumns)
{
	if (iRows < MINROWS)
		iRows = MINROWS;
	if (iRows > MAXROWS)
		iRows = MAXROWS;
	rowCount = iRows;

	if (iColumns < MINCOLUMNS)
		iColumns = MINCOLUMNS;
	if (iColumns > MAXCOLUMNS)
		iColumns = MAXCOLUMNS;
	columnCount = iColumns;

	int index = 0;
	int row, column;
	for (row = 0; row < rowCount; row++)
		for (column = 0; column < columnCount; column++)
			matrix[row][column] = ++index;

	if ((rowCount > 0) && (columnCount > 0))
		matrix[rowCount - 1][columnCount - 1] = 0;
}

void Puzzle::LayoutMatrix(int iW, int iH)
{
	//TODO: image
	matrixPositionX = PADDING;
	matrixPositionY = PADDING;
	cellWidth = (iW - 2 * PADDING) / columnCount;
	cellHeight = (iH - 2 * PADDING) / rowCount;

	int numbersHeight = ((int)((cellWidth < cellHeight) ? cellWidth : cellHeight)) / 2;
	if ((!numbersFontId) || (Texts::GetTextHeight(numbersFontId) != numbersHeight))
	{
		if (numbersFontId)
			Texts::DestroyFont(numbersFontId);
		numbersFontId = Texts::CreateFont(L"Arial", numbersHeight, true, false, false, false);
	}
}

void Puzzle::Init(int iW, int iH)
{
	InitializeMatrix(5, 5);
	LayoutMatrix(iW, iH);
}

void Puzzle::Resize(int iW, int iH)
{
	LayoutMatrix(iW, iH);

	glViewport(0, 0, iW, iH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, iW, iH, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Puzzle::Draw()
{
	if (dayMode)
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glColor3d(0.0, 0.0, 0.0);
	}
	else
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glColor3d(1.0, 1.0, 1.0);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
		glTranslated(matrixPositionX, matrixPositionY, 0.0);
		for (int row = 0; row < rowCount; row++)
		{
			glPushMatrix();
				for (int column = 0; column < columnCount; column++)
				{
					if (matrix[row][column] == 0)
					{
						glBegin(GL_POLYGON);
							glVertex2d(0.0, 0.0);
							glVertex2d(cellWidth, 0.0);
							glVertex2d(cellWidth, cellHeight);
							glVertex2d(0.0, cellHeight);
						glEnd();
					}
					else
					{
						if (drawGrid)
						{
							glBegin(GL_LINE_STRIP);
								glVertex2d(0.0, 0.0);
								glVertex2d(cellWidth, 0.0);
								glVertex2d(cellWidth, cellHeight);
								glVertex2d(0.0, cellHeight);
								glVertex2d(0.0, 0.0);
							glEnd();
						}

						//TODO: image
						double textHeight = Texts::GetTextHeight(numbersFontId);
						Texts::DrawTextW(
							numbersFontId,
							(cellWidth - Texts::GetTextWidth(numbersFontId, "%d", matrix[row][column])) / 2.0,
							(cellHeight + Texts::GetTextHeight(numbersFontId)) / 2.0,
							"%d", matrix[row][column]);

					}
					glTranslated(cellWidth, 0.0, 0.0);
				}
			glPopMatrix();
			glTranslated(0.0, cellHeight, 0.0);
		}
	glPopMatrix();
}

void Puzzle::MouseLeftUp(int xPos, int yPos)
{
	int cellX = (int)((xPos - matrixPositionX) / cellWidth);
	int cellY = (int)((yPos - matrixPositionY) / cellHeight);

	if ((cellX < 0) || (cellX >= columnCount) || (cellY < 0) || (cellY >= rowCount))
		return;

	if ((cellX > 0) && (matrix[cellY][cellX - 1] == 0))
	{
		matrix[cellY][cellX - 1] = matrix[cellY][cellX];
		matrix[cellY][cellX] = 0;
	}
	if ((cellX < columnCount - 1) && (matrix[cellY][cellX + 1] == 0))
	{
		matrix[cellY][cellX + 1] = matrix[cellY][cellX];
		matrix[cellY][cellX] = 0;
	}

	if ((cellY > 0) && (matrix[cellY - 1][cellX] == 0))
	{
		matrix[cellY - 1][cellX] = matrix[cellY][cellX];
		matrix[cellY][cellX] = 0;
	}
	if ((cellY < rowCount - 1) && (matrix[cellY + 1][cellX] == 0))
	{
		matrix[cellY + 1][cellX] = matrix[cellY][cellX];
		matrix[cellY][cellX] = 0;
	}
}