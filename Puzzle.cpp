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
		drawGrid(DEFAULTDRAWGRID),
		solutionMode(false)
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
	double paddedSurfaceWidth = surfaceWidth - 2 * PADDING;
	double paddedSurfaceHeight = surfaceHeight - 2 * PADDING;
	if (textureId)
	{
		double horizontalRatio = paddedSurfaceWidth / textureWidth;
		double verticalRatio = paddedSurfaceHeight / textureHeight;
		double ratio = (horizontalRatio < verticalRatio) ? horizontalRatio : verticalRatio;
		double scaledWidth = textureWidth * ratio;
		double scaledHeight = textureHeight * ratio;
		matrixPositionX = PADDING + (paddedSurfaceWidth - scaledWidth) / 2;
		matrixPositionY = PADDING + (paddedSurfaceHeight - scaledHeight) / 2;
		cellWidth = scaledWidth / columnCount;
		cellHeight = scaledHeight / rowCount;
	}
	else
	{
		matrixPositionX = PADDING;
		matrixPositionY = PADDING;
		cellWidth = paddedSurfaceWidth / columnCount;
		cellHeight = paddedSurfaceHeight / rowCount;
	}

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

void Puzzle::SetTexture()
{
	OPENFILENAMEA openFileName;
	char selectedFileName[256] = { 0 };

	ZeroMemory(&openFileName, sizeof(openFileName));
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.hwndOwner = NULL;
	openFileName.lpstrFile = selectedFileName;
	openFileName.nMaxFile = sizeof(selectedFileName);
	openFileName.lpstrFilter = "All\0*.*\0BMP\0*.bmp\0JPG\0*.jpg\0PNG\0*.png\0";
	openFileName.nFilterIndex = 1;
	openFileName.lpstrFileTitle = NULL;
	openFileName.nMaxFileTitle = 0;
	openFileName.lpstrInitialDir = NULL;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileNameA(&openFileName))
 		return;

	int channels;
	unsigned char* image = stbi_load(selectedFileName, &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
	if (!image)
		return;

	DropTexture();

 	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
}

void Puzzle::DropTexture()
{
	if (textureId)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
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
	DropTexture();
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
					if (textureId)
					{
						double textureX = (solutionMode ? column : ((matrix[row][column] - 1) % rowCount)) * textureCellWidth;
						double textureY = (solutionMode ? row : ((matrix[row][column] - 1) / rowCount)) * textureCellHeight;
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
						int number = solutionMode ? row*columnCount + column + 1 : matrix[row][column];
						glColor3d(color[0], color[1], color[2]);
						double textHeight = Texts::GetTextHeight(numbersFontId);
						Texts::DrawTextW(
							numbersFontId,
							(cellWidth - Texts::GetTextWidth(numbersFontId, "%d", number)) / 2.0,
							(cellHeight + Texts::GetTextHeight(numbersFontId)) / 2.0,
							"%d", number);
					}
					if ((!solutionMode) && (matrix[row][column] == 0))
					{
						glColor3d(color[0], color[1], color[2]);
						glBegin(GL_POLYGON);
							glVertex2d(0.0, 0.0);
							glVertex2d(cellWidth, 0.0);
							glVertex2d(cellWidth, cellHeight);
							glVertex2d(0.0, cellHeight);
						glEnd();
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
	case SPACE:
		solutionMode = true;
		break;
	case KEY_G:
		drawGrid = !drawGrid;
		break;
	case KEY_M:
		dayMode = !dayMode;
		break;
	case KEY_N:
		DropTexture();
		LayoutMatrix();
		break;
	case KEY_I:
		SetTexture();
		LayoutMatrix();
		break;
	default:
		break;
	}
}

void Puzzle::KeyUp(KeyCode keyCode)
{
	switch (keyCode)
	{
	case SPACE:
		solutionMode = false;
		break;
	default:
		break;
	}
}