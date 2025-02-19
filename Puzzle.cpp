#include <Windows.h>
#include <gl\GL.h>

#include "Puzzle.h"
#include "base\Texts.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

constexpr int DEFAULTROWS = 5;
constexpr int DEFAULTCOLUMNS = 5;
constexpr bool DEFAULTDAYMODE = false;
constexpr bool DEFAULTDRAWGRID = true;

constexpr double PADDING = 10.0;

enum StepDirection
{
	INVALID,

	LEFT,
	RIGHT,
	UP,
	DOWN
};

Puzzle::Puzzle()
	: Game(),
		numbersFontId(0),
		rowCount(0),
		columnCount(0),
		surfaceWidth(0.0),
		surfaceHeight(0.0),
		matrixPositionX(0.0),
		matrixPositionY(0.0),
		cellWidth(0.0),
		cellHeight(0.0),
		textureId(0),
		textureWidth(0),
		textureHeight(0),
		dayMode(DEFAULTDAYMODE),
		drawGrid(DEFAULTDRAWGRID)
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

bool Puzzle::GetCurrentPosition(int& iRow, int& iColumn)
{
	int row, column;
	for(row = 0;row< rowCount;row++)
		for(column = 0;column < columnCount;column++)
			if (matrix[row][column] == 0)
			{
				iRow = row;
				iColumn = column;
				return true;
			}
	return false;
}

void Puzzle::LayoutMatrix()
{
	//TODO: image
	matrixPositionX = PADDING;
	matrixPositionY = PADDING;
	cellWidth = (surfaceWidth - 2 * PADDING) / columnCount;
	cellHeight = (surfaceHeight - 2 * PADDING) / rowCount;

	int numbersHeight = ((int)((cellWidth < cellHeight) ? cellWidth : cellHeight)) / 2;
	if ((!numbersFontId) || (Texts::GetTextHeight(numbersFontId) != numbersHeight))
	{
		if (numbersFontId)
			Texts::DestroyFont(numbersFontId);
		numbersFontId = Texts::CreateFont(L"Arial", numbersHeight, true, false, false, false);
	}
}

bool Puzzle::IsResolved()
{
	int index = 0;
	int row, column;
	for(row = 0; row < rowCount; row++)
		for (column = 0; column < columnCount; column++)
			if ((row == rowCount - 1) && (column == columnCount - 1))
			{
				if (matrix[row][column] != 0)
					return false;
			}
			else
			{
				if (matrix[row][column] != ++index)
					return false;
			}
	return true;
}

void Puzzle::Shuffle(int steps)
{
	StepDirection possibleSteps[4];
	int possibleStepsCount;
	int row, column;
	if (!GetCurrentPosition(row, column))
		return;
	StepDirection forbiddenStep = StepDirection::INVALID;
	while (steps > 0)
	{
		steps--;
		
		possibleStepsCount = 0;
		if ((column > 0) && (forbiddenStep != StepDirection::LEFT))
			possibleSteps[possibleStepsCount++] = StepDirection::LEFT;
		if ((column < columnCount - 1) && (forbiddenStep != StepDirection::RIGHT))
			possibleSteps[possibleStepsCount++] = StepDirection::RIGHT;
		if ((row > 0) && (forbiddenStep != StepDirection::UP))
			possibleSteps[possibleStepsCount++] = StepDirection::UP;
		if ((row < rowCount - 1) && (forbiddenStep != StepDirection::DOWN))
			possibleSteps[possibleStepsCount++] = StepDirection::DOWN;
		
		if (possibleStepsCount == 0)
			return;
		int nextStep = rand() % possibleStepsCount;
		switch (possibleSteps[nextStep])
		{
		case StepDirection::LEFT:
			matrix[row][column] = matrix[row][column - 1];
			column--;
			matrix[row][column] = 0;
			forbiddenStep = StepDirection::RIGHT;
			break;
		case StepDirection::RIGHT:
			matrix[row][column] = matrix[row][column + 1];
			column++;
			matrix[row][column] = 0;
			forbiddenStep = StepDirection::LEFT;
			break;
		case StepDirection::UP:
			matrix[row][column] = matrix[row - 1][column];
			row--;
			matrix[row][column] = 0;
			forbiddenStep = StepDirection::DOWN;
			break;
		case StepDirection::DOWN:
			matrix[row][column] = matrix[row + 1][column];
			row++;
			matrix[row][column] = 0;
			forbiddenStep = StepDirection::UP;
			break;
		default:
			break;
		}
	}
}

void Puzzle::PrepareTexture(const char* fileName)
{
	if (textureId)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}

	int channels;
	unsigned char* image = stbi_load(fileName, &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
	if (!image)
		return;

 	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
}

void Puzzle::Init(int iW, int iH)
{
	surfaceWidth = iW;
	surfaceHeight = iH;
	InitializeMatrix(DEFAULTROWS, DEFAULTCOLUMNS);
	Shuffle(rowCount*columnCount*2);
	LayoutMatrix();
}

void Puzzle::Deinit()
{
	if (textureId)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

void Puzzle::Resize(int iW, int iH)
{
	surfaceWidth = iW;
	surfaceHeight = iH;
	LayoutMatrix();

	glViewport(0, 0, iW, iH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, iW, iH, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Puzzle::Draw()
{
	double textureCellWidth = 1.0 / columnCount;
	double textureCellHeight = 1.0 / rowCount;
	double color[3];
	if (dayMode)
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		color[0] = color[1] = color[2] = 0.0;
	}
	else
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		color[0] = color[1] = color[2] = 1.0;
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
						glColor3d(color[0], color[1], color[2]);
						glBegin(GL_POLYGON);
							glVertex2d(0.0, 0.0);
							glVertex2d(cellWidth, 0.0);
							glVertex2d(cellWidth, cellHeight);
							glVertex2d(0.0, cellHeight);
						glEnd();
					}
					else
					{
						if (textureId)
						{
							double textureX = ((matrix[row][column] - 1) % rowCount) * textureCellWidth;
							double textureY = ((matrix[row][column] - 1) / rowCount) * textureCellHeight;
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, textureId);
							glColor3d(1.0, 1.0, 1.0);
							glBegin(GL_QUADS);
								glTexCoord2d(textureX, textureY);
								glVertex2d(0.0, 0.0);
								glTexCoord2d(textureX + textureCellWidth, textureY);
								glVertex2d(cellWidth, 0.0);
								glTexCoord2d(textureX + textureCellWidth, textureY + textureCellHeight);
								glVertex2d(cellWidth, cellHeight);
								glTexCoord2d(textureX, textureY + textureCellHeight);
								glVertex2d(0.0, cellHeight);
							glEnd();
							glDisable(GL_TEXTURE_2D);
						}

						if (drawGrid)
						{
							glColor3d(color[0], color[1], color[2]);
							glBegin(GL_LINE_STRIP);
								glVertex2d(0.0, 0.0);
								glVertex2d(cellWidth, 0.0);
								glVertex2d(cellWidth, cellHeight);
								glVertex2d(0.0, cellHeight);
								glVertex2d(0.0, 0.0);
							glEnd();
						}

						if (!textureId)
						{
							glColor3d(color[0], color[1], color[2]);
							double textHeight = Texts::GetTextHeight(numbersFontId);
							Texts::DrawTextW(
								numbersFontId,
								(cellWidth - Texts::GetTextWidth(numbersFontId, "%d", matrix[row][column])) / 2.0,
								(cellHeight + Texts::GetTextHeight(numbersFontId)) / 2.0,
								"%d", matrix[row][column]);
						}
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
	int row = (int)((yPos - matrixPositionY) / cellHeight);
	int column = (int)((xPos - matrixPositionX) / cellWidth);

	if ((column < 0) || (column >= columnCount) || (row < 0) || (row >= rowCount))
		return;

	if ((column > 0) && (matrix[row][column - 1] == 0))
	{
		matrix[row][column - 1] = matrix[row][column];
		matrix[row][column] = 0;
	}
	if ((column < columnCount - 1) && (matrix[row][column + 1] == 0))
	{
		matrix[row][column + 1] = matrix[row][column];
		matrix[row][column] = 0;
	}

	if ((row > 0) && (matrix[row - 1][column] == 0))
	{
		matrix[row - 1][column] = matrix[row][column];
		matrix[row][column] = 0;
	}
	if ((row < rowCount - 1) && (matrix[row + 1][column] == 0))
	{
		matrix[row + 1][column] = matrix[row][column];
		matrix[row][column] = 0;
	}
}

void Puzzle::KeyDown(KeyCode keyCode)
{
	int row, column;
	if (!GetCurrentPosition(row, column))
		return;
	switch (keyCode)
	{
	case KeyCode::ARROWLEFT:
		if (column > 0)
		{
			matrix[row][column] = matrix[row][column - 1];
			matrix[row][column - 1] = 0;
		}
		break;
	case KeyCode::ARROWRIGHT:
		if (column < columnCount - 1)
		{
			matrix[row][column] = matrix[row][column + 1];
			matrix[row][column + 1] = 0;
		}
		break;
	case KeyCode::ARROWUP:
		if (row > 0)
		{
			matrix[row][column] = matrix[row - 1][column];
			matrix[row - 1][column] = 0;
		}
		break;
	case KeyCode::ARROWDOWN:
		if (row < rowCount - 1)
		{
			matrix[row][column] = matrix[row + 1][column];
			matrix[row + 1][column] = 0;
		}
		break;
	case KEY_G:
		drawGrid = !drawGrid;
		break;
	case KEY_M:
		dayMode = !dayMode;
		break;
	default:
		break;
	}

}