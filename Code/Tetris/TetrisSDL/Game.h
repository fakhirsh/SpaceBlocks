#ifndef __CGAME_H_
#define __CGAME_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"
#include "SDL_opengl.h" 
#include "SDL_image.h" 

#include "SoundManager.h"

using namespace std;

//#define __DEBUG__
//#define ENABLE_AUDIO
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
const int SCREEN_BPP = 32;

const int FRAMES_PER_SECOND = 60;

const int GRID_ROWS = 20;
const int GRID_COLS = 10;

const int CELL_WIDTH = 73;
const int CELL_HEIGHT = 73;

const int GRID_X = 629;
const int GRID_Y = 164;

const int MIN_Y = 164 + 0*34 - 20;
const int MAX_Y = 164 + 19*34 - 20;

const int MIN_X = 629 + 0*34 - 20;
const int MAX_X = 629 + 9*34 - 20;

struct CELL{
	CELL(){
		x = 0; y = 0;
		opacity = 0.0;
		type = 0;
	}
	int x, y;
	float opacity;
	int type;	//	0 = EMPTY, 1 = blue, 2 = cyan, 3 = green, 4 = magenta, 5 = orange, 6 = red, 7 = yellow
};

const int blockStartPositions[8][2] =	{
										{0, 0},			// NULL	
										{629 + 3*34 - 20, 164 - 1*34 - 20},	// L
										{629 + 3*34 - 20, 164 - 1*34 - 20},	// J
										{629 + 3*34 - 20, 164 - 1*34 - 20},	// Z
										{629 + 3*34 - 20, 164 - 1*34 - 20},	// S
										{629 + 2*34 - 20, 164 - 1*34 - 20},	// I
										{629 + 2*34 - 20, 164 - 1*34 - 20},	// O
										{629 + 3*34 - 20, 164 - 1*34 - 20}	// T
										};

const int blockList[8][4][5][5] = {
									// NULL block at position 0
								{
								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}}
								},

								// L shaped block at position 1 with 4 rotations
								{
								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,1,0},
								{0,1,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,1,1,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,1,0},
								{0,1,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,1,1,0},
								{0,0,0,0,0}}
								},

								// J shaped block at position 2
								{
								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,1,0},
								{0,0,0,1,0},
								{0,0,0,0,0}},
								
								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,1,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,1,0,0,0},
								{0,1,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,1,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,0,0,0}}
								},
								
								// Z shaped block at position 3
								{
								{{0,0,0,0,0},
								{0,1,1,0,0},
								{0,0,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,1,0},
								{0,0,1,1,0},
								{0,0,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,0,0},
								{0,0,1,1,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,1,1,0,0},
								{0,1,0,0,0},
								{0,0,0,0,0}}
								},

								// S shaped block at position 4
								{
								{{0,0,0,0,0},
								{0,0,1,1,0},
								{0,1,1,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,0,1,1,0},
								{0,0,0,1,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,1,1,0},
								{0,1,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,1,0,0,0},
								{0,1,1,0,0},
								{0,0,1,0,0},
								{0,0,0,0,0}}
								},

								// I shaped block at position 5
								{
								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,1,1},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{1,1,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,1,0,0},
								{0,0,0,0,0}}
								},

								// O shaped block at position 6
								{
								{{0,0,0,0,0},
								{0,0,1,1,0},
								{0,0,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},
								
								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,0,1,1,0},
								{0,0,1,1,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,0,0},
								{0,1,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,1,1,0,0},
								{0,1,1,0,0},
								{0,0,0,0,0},
								{0,0,0,0,0}}
								},

								// T shaped block at position 7
								{
								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,1,1,1,0},
								{0,0,0,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,0,1,1,0},
								{0,0,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,0,0,0},
								{0,1,1,1,0},
								{0,0,1,0,0},
								{0,0,0,0,0}},

								{{0,0,0,0,0},
								{0,0,1,0,0},
								{0,1,1,0,0},
								{0,0,1,0,0},
								{0,0,0,0,0}}}};

struct BLOCK{
	BLOCK(){
		x = y = type = rotationCount = 0;	//  block corner coordinates
	}
	int x, y;
	int type;	// 0 = invalid; 1 = L; 2 = J; 3 = Z; 4 = S; 5 = I; 6 = o; 7 = T; 8 = Boundry wall or floor
	unsigned long rotationCount;			// Contains clockwise rotation status for the 4 dimensional array
};

class CGameMenu
{
public:
	CGameMenu(void);
	~CGameMenu(void);

	bool InitializeMenu(CSoundManager * smPtr);
	void DestroyMenu();

	void DrawBackGround(GLuint bck);
	void DrawHelpScreen();
	void DrawOption(int index, int x, int y, int width, int height);
	void DrawMenu();

	void ResetMenu();

	void HandleInput();
	int CurrentState();

	CSoundManager * sm;

private:
	int globalState;
	int localState;
	bool choiceSelected;
	bool controlScreenActivated;	// check whether to display the control screen or the normal menu
	GLuint background;
	GLuint controls;
	GLuint menuOptions[6];
};


class CGame
{
public:
	CGame(void);
	~CGame(void);
	bool Initialize(CSoundManager * smPtr);
	void Draw();
	void DrawGameOverScreen();
	void UpdateLogic();
	bool IsGameOver();

	void InitializeGame();		// initialize all the data in order to get separated from the constructor
	void Destroy();				// Destroys all the data in order to get separated from the destructor

	void PlayBackgroundMusic();
	int PlayMoveSound();

	CGameMenu gameMenu;

	//Pointer to the global SoundManager object.
	CSoundManager * sm;

private:
	bool InitGL();
	bool LoadTextures();

	// Drawing Routines:
	void DrawBackground(GLuint bck);
	void DrawGrid();
	void DrawBlock();
	void DrawBoardCells();
	inline void DrawCell(int x, int y, int type, float opacity, int width, int height);
	void DrawTime();
	void DrawLevel();
	void DrawScore();
	void DrawNextPiece();
	void DrawHoldPiece();
	void DrawDigit(GLuint digit, int x, int y, int width, int height);
	//void DrawHoldingPiece();
	
	//void DrawBorder();

	// Logic Functions
	void FinalizeBlock();
	void ProcessInput();
	void HandleCollision();
	bool CheckCollision();
	int DeletePotentialRows();	// Delete any possible row
	int CheckPotentialDelRows();	// check for any possible row delitions. used in row delition animation.
	void DeleteRow(int row);	// Delete ith row
	void UpdateScore(int linesDeleted);
	void ResetRowsDeletedArray();
	void DeleteAnimation();
	void ShiftAnimation();
	void ResetGridCoordinates();

	// Data:
	CELL grid[GRID_ROWS + 2][GRID_COLS+2];	// Draw a U shaped boundry around the well for efficient collision detection.
	SDL_Surface * screen;

	GLuint cellTexture[8];
	GLuint background;
	GLuint timeFont[10];
	GLuint scoreFont[10];
	GLuint levelFont[10];
	GLuint gameOverScreen;

	int level;
	int blockSpeed;		// Speed of the falling block
	int blockType;
	int shiftTime;		// Time allowed to properly place the block either left or right AFTER it has landed
	int score;
	int nextBlock;
	int holdingBlock;	

	bool isBlockFalling;	// Check if a block is already falling.

	//Uint8 * keystates;		// Pointer for monitoring key states;

	unsigned long int tempTicker;			// increments every frame. tempTicker=60 means 1 sec passed {60 FPS}.

	BLOCK currentBlock;		// current falling block

	bool leftFlag;
	bool rightFlag;			
	bool spaceFlag;
	bool rotationFlag;
	bool ctrlFlag;			// Control key press, for holding the block.
	bool blockFinalized;	// Has the block completely settled down?
	bool gameOver;
	bool animationTrigger;		// indicates whether to start the row deletion animation
	bool rowAnimationTrigger;	// trigger row animation. This varuable is necessary to ensure that the row animation dowsn't execure more than once. 
	bool shiftAnimationTrigger;	// trigger shift animation right after the row animation has ended !
	bool rowsDeleted[GRID_ROWS];	// contains which of the rows are to be deleted

};


#endif