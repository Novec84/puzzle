#include <Windows.h>
#include <gl\GL.h>

#include "Puzzle.h"
#include "base\Texts.h"

constexpr int PADDING = 10.0;

Puzzle::Puzzle()
	: Game(),
		numbersFontId(0),
		rowCount(0),
		columnCount(0),
		matrixPositionX(0.0),
		matrixPositionY(0.0),
		cellWidth(0.0),
		cellHeight(0.0)
{

}

Puzzle::~Puzzle()
{

}

void Puzzle::InitializeMatrix(int iRows, int iColumns)
{
	if (iRows < 0)
		iRows = 0;
	if (iRows > MAXROWS)
		iRows = MAXROWS;
	rowCount = iRows;

	if (iColumns < 0)
		iColumns = 0;
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
	//TODO:
	matrixPositionX = PADDING;
	matrixPositionY = PADDING;
	cellWidth = 100.0;
	cellHeight = 100.0;
}

void Puzzle::Init(int iW, int iH)
{
	InitializeMatrix(5, 5);
	LayoutMatrix(iW, iH);

	numbersFontId = Texts::CreateFontW(L"Arial", 20, true, false, false, false);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(.5, .5, .5);

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
						glBegin(GL_LINE_STRIP);
							glVertex2d(0.0, 0.0);
							glVertex2d(cellWidth, 0.0);
							glVertex2d(cellWidth, cellHeight);
							glVertex2d(0.0, cellHeight);
							glVertex2d(0.0, 0.0);
						glEnd();

						//TODO:
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

}