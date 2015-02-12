#include "Game.h"
#include "LoadTexture.h"

inline float random()
{
    return rand() / (float)RAND_MAX;
}

inline int RandomRange( int low, int high )
{
    int range = high - low;
    return (int) (random() * range) + low;
}

unsigned time_seed (void)
{
time_t timeval; /* Current time. */
unsigned char *ptr; /* Type punned pointed into timeval. */
unsigned seed; /* Generated seed. */
size_t i;

timeval = time (NULL);
ptr = (unsigned char *) &timeval;

seed = 0;
for (i = 0; i < sizeof timeval; i++)
seed = seed * (UCHAR_MAX + 2u) + ptr[i];

return seed;
}

CGame::CGame(void)
{

}

CGame::~CGame(void)
{
	Destroy();
}

void CGame::InitializeGame(){
	screen = NULL;
	level = 1;
	blockSpeed = 70 - 6*level;
	blockType = 0;
	shiftTime = 0;
	score = 0;
	holdingBlock = 0;
	isBlockFalling = false;

	tempTicker = 0;

	leftFlag = false;
	rightFlag = false;
	spaceFlag = false;
	rotationFlag = false;
	ctrlFlag = false;
	gameOver = false;
	blockFinalized = false;

	animationTrigger = false;
	shiftAnimationTrigger = false;
	rowAnimationTrigger = false;

	srand(time_seed());
	do{
		nextBlock = RandomRange(1, 8);
	}while(nextBlock == 3 || nextBlock == 4);	// Avoid initial Z or S
	
	//Initialize Grid coordinates
	for(int j = 0; j < GRID_ROWS + 2; j++){
		for(int i = 0; i < GRID_COLS + 2; i++){
			grid[j][i].x = MIN_X + i * 34 - 34;
			grid[j][i].y = MIN_Y + j * 34 - 34;
			grid[j][i].type = 0;
		}
	}

	// Initialize U shaped boundry around the well. 
	for(int i = 0; i < GRID_COLS + 2; i++){			// Initialize floor.
		grid[GRID_ROWS + 1][i].type = 8;
	}
	for(int i = 0; i < GRID_ROWS + 2; i++){		// Initialize side walls
		grid[i][0].type = 8;
		grid[i][GRID_COLS + 1].type = 8;
	}

	ResetRowsDeletedArray();

}

void CGame::Destroy(){
	glDeleteTextures( 1, &background );
	for(int i = 1; i < 8; i++){
		glDeleteTextures( 1, &(cellTexture[i]) );
	}
	
	for(int i = 0; i < 10; i++){
		glDeleteTextures( 1, &(scoreFont[i]) );
		glDeleteTextures( 1, &(levelFont[i]) );
		glDeleteTextures( 1, &(timeFont[i]) );
	}
	
	SDL_Quit();
}

bool CGame::InitGL(){
	//Set clear color 
	glClearColor( 0, 0, 0, 0 ); 
	//Set projection 
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity(); 
	glOrtho( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1 ); 
	//Initialize modelview matrix 
	glMatrixMode( GL_MODELVIEW ); 
	glLoadIdentity(); 
	glEnable( GL_TEXTURE_2D );

	glEnable(GL_BLEND);    // allow alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	//If there was any errors 
	if( glGetError() != GL_NO_ERROR ) { 
		MessageBox(0, "Failed to initialize OpenGL...", "CGame::InitGL", MB_OK | MB_ICONERROR);
		return false; 
	} //If everything initialized 
	return true; 
}

void CGame::PlayBackgroundMusic(){
#ifdef ENABLE_AUDIO
	Mix_PlayMusic( music, -1 );
#endif
}

int CGame::PlayMoveSound(){
#ifdef ENABLE_AUDIO
	return Mix_PlayChannel( -1, moveBlock, 0 );
#else
	return 1;
#endif
}

bool CGame::Initialize(CSoundManager * smPtr){
	InitializeGame();

	sm = smPtr;

	if(SDL_Init( SDL_INIT_EVERYTHING ) < 0){
		MessageBox(0, "CGame::Initialize SDL_Init", SDL_GetError(), MB_OK | MB_ICONERROR);
		return false;
	}

//	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // *new*
	//screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,  SDL_FULLSCREEN | SDL_OPENGL );
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,  SDL_OPENGL );
	if( screen == NULL ) { 
		MessageBox(0, "CGame::Initialize SDL_SetVideoMode", SDL_GetError(), MB_OK | MB_ICONERROR);
		return false; 
	} 
	//Initialize OpenGL 
	if( InitGL() == false ) { 
		return false; 
	} //Set caption 

	SDL_WM_SetCaption( "Space Blocks", NULL ); 

	if(LoadTextures() == false){
		MessageBox(0, "Failed to load textures...", "CGame::Initialize", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

// Loads texture AND Sound 
bool CGame::LoadTextures(){
	cellTexture[0] = -1;	// Invalid texture
	cellTexture[1] = LoadTexture("Media\\Artwork\\cells\\orangeCell.png");
	cellTexture[2] = LoadTexture("Media\\Artwork\\cells\\blueCell.png");
	cellTexture[3] = LoadTexture("Media\\Artwork\\cells\\redCell.png");
	cellTexture[4] = LoadTexture("Media\\Artwork\\cells\\greenCell.png");
	cellTexture[5] = LoadTexture("Media\\Artwork\\cells\\cyanCell.png");
	cellTexture[6] = LoadTexture("Media\\Artwork\\cells\\yellowCell.png");
	cellTexture[7] = LoadTexture("Media\\Artwork\\cells\\magentaCell.png");

	for(int i = 1; i < 8; i++){
		if(cellTexture[i] < 0){
			return false;
		}
	}
	background = LoadTexture("Media\\Artwork\\Game_screen\\game_bg.jpg");
	if(background < 0){
		return false;
	}

	// Load Score Font
	char t[100], t2[10];
	for(int i = 0; i < 10; i++){
		itoa(i, t2, 10);
		strcpy(t, "Media\\Artwork\\Game_screen\\sc_");
		strcat(t, t2);
		strcat(t, ".png");

		scoreFont[i] = LoadTexture(t);
		if(scoreFont[i] < 0){
			return false;
		}
    }

	// Load Time Font
	for(int i = 0; i < 10; i++){
		itoa(i, t2, 10);
		strcpy(t, "Media\\Artwork\\Game_screen\\t_");
		strcat(t, t2);
		strcat(t, ".png");

		timeFont[i] = LoadTexture(t);
		if(scoreFont[i] < 0){
			return false;
		}
    }

	// Load level font
	for(int i = 0; i < 10; i++){
		itoa(i, t2, 10);
		strcpy(t, "Media\\Artwork\\Game_screen\\lvl_");
		strcat(t, t2);
		strcat(t, ".png");

		levelFont[i] = LoadTexture(t);
		if(scoreFont[i] < 0){
			return false;
		}
    }

	gameOverScreen = LoadTexture("Media\\Artwork\\GameOver\\gameOver.jpg");
	if(gameOverScreen < 0){
		return false;
	}
	
	return true;
}

void CGame::DrawBackground(GLuint bck){
	glDisable(GL_BLEND);
	glBindTexture( GL_TEXTURE_2D, bck );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex3f( 0.f, 0.f, 0.0f );
	
		//Bottom-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex3f( (float)SCREEN_WIDTH, 0.f, 0.f );
	
		//Top-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex3f( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.f );
	
		//Top-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex3f( 0.f, (float)SCREEN_HEIGHT, 0.f );
	glEnd();
}

inline void CGame::DrawCell(int x, int y, int type, float opacity, int width, int height){
	glBindTexture( GL_TEXTURE_2D, cellTexture[type] );
	//Grid opacity:
	glColor4f( 1.0, 1.0, 1.0, opacity );

	glLoadIdentity();
	glTranslatef((float)x, (float)y, 0.0);
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex3f( 0.f, 0.f, 0.0f );
	
		//Bottom-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex3f( (float)width, 0.f, 0.f );
	
		//Top-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex3f( (float)width, (float)height, 0.f );
	
		//Top-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex3f( 0.f, (float)height, 0.f );
	glEnd();			
}

void CGame::DrawGrid(){
	for(int row = 0; row < GRID_ROWS; row++){
		for(int col = 0; col < GRID_COLS; col++){
			DrawCell(629 + col * 34 - 20, 164 + row * 34 - 20, row % 7 + 1, 0.25f, CELL_WIDTH, CELL_HEIGHT);
		}
	}
}

void CGame::DrawBlock(){
	float opacity = 1.0;
	for(int j = 0; j < 5; j++){
		for(int i = 0; i < 5; i++){
			if(blockList[currentBlock.type][(currentBlock.rotationCount)%4][j][i] != 0){
				if(currentBlock.y + j * 34 - 34 < 164 - 20) opacity = 0.5;
				else opacity = 1.0;
				DrawCell(currentBlock.x + i * 34, currentBlock.y + j * 34 - 34, currentBlock.type, opacity, CELL_WIDTH, CELL_HEIGHT);
			}
#ifdef __DEBUG__
			else{
				DrawCell(currentBlock.x + i * 34, currentBlock.y + j * 34 - 34, currentBlock.type, 0.3);
			}
#endif
		}
	}
}

void CGame::DrawBoardCells(){
	for(int row = 1; row < GRID_ROWS + 1; row++){
		for(int col = 1; col < GRID_COLS + 1; col++){
			if(grid[row][col].type != 0)
			{
				DrawCell(grid[row][col].x, grid[row][col].y, grid[row][col].type, grid[row][col].opacity, CELL_WIDTH, CELL_HEIGHT);
			}
		}
	}
}


#ifdef __DEBUG__
void CGame::DrawBorder(){
	for(int j = 0; j < GRID_ROWS + 2; j++){
		for(int i = 0; i < GRID_COLS + 2; i++){
			if(grid[j][i].type == 8)	// Draw only if it is a boundry cell
				DrawCell(grid[j][i].x, grid[j][i].y, 3, 1.0);	// draw RED boundry cells
		}
	}
}
#endif

void CGame::DrawDigit(GLuint digit, int x, int y, int width, int height){
	glBindTexture( GL_TEXTURE_2D, digit );
	//Grid opacity:
	glColor4f( 1.0, 1.0, 1.0, 1.0 );

	glLoadIdentity();
	glTranslatef((float)x, (float)y, 0.0);
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex3f( 0.f, 0.f, 0.0f );
	
		//Bottom-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex3f( (float)width, 0.f, 0.f );
	
		//Top-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex3f( (float)width, (float)height, 0.f );
	
		//Top-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex3f( 0.f, (float)height, 0.f );
	glEnd();

}

void CGame::DrawTime(){
	int nT = tempTicker/60;	// normalize time {60 FPS rememver ??}
	int h = nT / 3600;
	int m = (nT - h * 3600) / 60;
	int s = nT % 60;

	char st[12];
	sprintf(st, "%02d", h);
	for (int i = 0; i < 2; i++ )
	{
		DrawDigit(timeFont[st[i] - '0'], 765 - 5 + i * 20, 100 - 35, 49, 48);
	}
	sprintf(st, "%02d", m);
	for (int i = 0; i < 2; i++ )
	{
		DrawDigit(timeFont[st[i] - '0'], 765 + 50 - 5 + i * 20, 100 - 35, 49, 48);
	}
	sprintf(st, "%02d", s);
	for (int i = 0; i < 2; i++ )
	{
		DrawDigit(timeFont[st[i] - '0'], 765 + 100 - 5 + i * 20, 100 - 35, 49, 48);
	}
}

void CGame::DrawLevel(){
	char s[3];
	sprintf(s, "%02d", level);

	for (int i = 0; i < 2; i++ )
	{
		DrawDigit(levelFont[s[i] - '0'], 952 - 5 + i * 13, 141 - 25, 33, 33);
	}
}

void CGame::DrawScore(){
	char s[12];
	sprintf(s, "%010d", score);

	for (int i = 0; i < 10; i++ )
	{
		DrawDigit(scoreFont[s[i] - '0'], 697 - 5 + i*15, 141 - 25, 34, 33);
	}
}

void CGame::DrawGameOverScreen(){
	glLoadIdentity();

	// Draw Background
	DrawBackground(gameOverScreen);

	glEnable(GL_BLEND);

	char s[12];
	sprintf(s, "%010d", score);
	
	for (int i = 0; i < strlen(s); i++ )
	{
		DrawDigit(timeFont[s[i] - '0'], 697 - 45 + i*25, 350 + 141 - 25, 60, 60);
	}
}

void CGame::DrawNextPiece(){
	int width = CELL_WIDTH * 0.4;
	int height = CELL_HEIGHT * 0.4;
	for(int row = 0; row < 5; row++){
		for(int col = 0; col < 5; col++){
			if(blockList[nextBlock][0][row][col] != 0){
				DrawCell(976 + col * width/2, 150 + row * height/2, nextBlock, 1.0, width, height);
			}
		}
	}
}

void CGame::DrawHoldPiece(){
	if(holdingBlock != 0){
		int width = CELL_WIDTH * 0.4;
		int height = CELL_HEIGHT * 0.4;
		for(int row = 0; row < 5; row++){
			for(int col = 0; col < 5; col++){
				if(blockList[holdingBlock][0][row][col] != 0)
				{
					DrawCell(535 + col * width/2, 150 + row * height/2, holdingBlock, 1.0, width, height);
				}
			}
		}
	}
}

void CGame::Draw(){
	glLoadIdentity();

	// Draw Background
	DrawBackground(background);

	glEnable(GL_BLEND);

	//Draw Cell Grid
	DrawGrid();

	//Draw Falling Block
	if(isBlockFalling)	// draw only when the block is physically present, to avoid weird graphical glitches and flickering.
		DrawBlock();

	//Draw pile of cells on the board
	DrawBoardCells();

	DrawScore();
	DrawLevel();
	DrawTime();

	DrawNextPiece();
	DrawHoldPiece();

#ifdef __DEBUG__
	DrawBorder();
#endif
}

void CGame::ProcessInput(){
	static bool leftKeyPressed = false;
	static bool rightKeyPressed = false;
	static bool downKeyPressed = false;
	static bool upKeyPressed = false;
	static bool spaceKeyPressed = false;
	static bool ctrlKeyPressed = false;

	static int prevBlockSpeed = blockSpeed;

	Uint8 * keystates = SDL_GetKeyState( NULL );

	// Right Key pressed: key repitition properly handled
	if( keystates[ SDLK_RIGHT ] && !rightKeyPressed ) { 
		rightFlag = true;
		rightKeyPressed = true;
	}
	if(!keystates[ SDLK_RIGHT ]) { 
		rightKeyPressed = false;
	}

	// Left Key pressed: key repitition properly handled
	if( keystates[ SDLK_LEFT ] && !leftKeyPressed) { 
		leftFlag = true;
		leftKeyPressed = true;
	}
	if( !keystates[ SDLK_LEFT ]) { 
		leftKeyPressed = false;
	}

	// Down Key pressed: key repitition properly handled
	if( keystates[ SDLK_DOWN ] && !downKeyPressed){
		prevBlockSpeed = blockSpeed;
		blockSpeed = 5;				// Increase block descent speed
		downKeyPressed = true;
	}
	if( !keystates[ SDLK_DOWN ]) { 
		blockSpeed = prevBlockSpeed;
		downKeyPressed = false;
	}

	// UP Key pressed: Rotate Block
	if( keystates[ SDLK_UP ] && !upKeyPressed){
		rotationFlag = true;
		upKeyPressed = true;
	}
	if( !keystates[ SDLK_UP ]) { 
		upKeyPressed = false;
	}

	// SPACE Key pressed: Slam the block on the floor
	if( keystates[ SDLK_SPACE ] && !spaceKeyPressed){
		spaceKeyPressed = true;
		spaceFlag = true;
	}
	if( !keystates[ SDLK_SPACE ]) {
		spaceKeyPressed = false;
	}

	// CONTROL Key pressed: Slam the block on the floor
	if( keystates[ SDLK_LCTRL ] && !ctrlKeyPressed){
		ctrlKeyPressed = true;
		ctrlFlag = true;
	}
	if( !keystates[ SDLK_LCTRL ]) {
		ctrlKeyPressed = false;
	}

}

void CGame::FinalizeBlock(){
	int padX = (currentBlock.x - MIN_X) / 34;
	int padY = (currentBlock.y - MIN_Y) / 34;
	
	for(int row = 0; row < 5; row++){
		for(int col = 0; col < 5; col++){
			if(blockList[currentBlock.type][(currentBlock.rotationCount)%4][row][col] != 0){
				grid[row + padY][col + padX + 1].type = currentBlock.type;
				grid[row + padY][col + padX + 1].opacity = 1.0;
			}
		}
	}
	blockFinalized = true;
}

bool CGame::CheckCollision(){
	int padX = (currentBlock.x - MIN_X) / 34;
	int padY = (currentBlock.y - MIN_Y) / 34;
	
	for(int row = 0; row < 5; row++){
		for(int col = 0; col < 5; col++){
			if(blockList[currentBlock.type][(currentBlock.rotationCount)%4][row][col] != 0){
				if(grid[row + padY][col + padX + 1].type != 0){
					return true;
				}
			}
		}
	}
	return false;
}

void CGame::HandleCollision(){
	int padX = (currentBlock.x - MIN_X) / 34;
	int padY = (currentBlock.y - MIN_Y) / 34;

	if(leftFlag){
		sm->SendSoundEvent(CSoundManager::MOVEBLOCK);
		currentBlock.x -= 34;			// move block to the left
		if(CheckCollision() == true){
			currentBlock.x += 34;		// Roll Back: move block to the right
		}
		leftFlag = false;
	}

	else if(rightFlag){
		sm->SendSoundEvent(CSoundManager::MOVEBLOCK);
		currentBlock.x += 34;			// move block to the right
		if(CheckCollision() == true){
			currentBlock.x -= 34;		// Roll Back: move block to the left
		}
		rightFlag = false;
	}

	else if(rotationFlag){
		sm->SendSoundEvent(CSoundManager::ROTATE);
		currentBlock.rotationCount++;		// rotate 90 degrees clockwise
		if(CheckCollision() == true){
			currentBlock.rotationCount--;	// Roll Back: rotate anti-clockwise
		}
		rotationFlag = false;
	}

	else if(spaceFlag){
		sm->SendSoundEvent(CSoundManager::DROP);

		while(CheckCollision() == false){
			currentBlock.y += 34;
		}
		currentBlock.y -= 34;
		FinalizeBlock();	// landed onto something, Merge block in grid.
		spaceFlag = false;
		isBlockFalling = false;
	}

	else if(ctrlFlag){
		if(holdingBlock == 0){
			holdingBlock = currentBlock.type;
			isBlockFalling = false;
		}
		else{
			nextBlock = holdingBlock;
			holdingBlock = 0;
		}
		ctrlFlag = false;
	}

	else{		
		if(CheckCollision() == true){
			currentBlock.y -= 34;
			FinalizeBlock();	// landed onto something, Merge block in grid.
			isBlockFalling = false;
		}
	}
}

bool CGame::IsGameOver(){
	return gameOver;
}

void CGame::ResetRowsDeletedArray(){
	for(int i = 0; i < GRID_ROWS; i++){
		rowsDeleted[i] = false;
	}
}

int CGame::CheckPotentialDelRows(){
	int numberDeleted = 0;
	for(int row = 1; row < GRID_ROWS + 1; row++){
		bool isFull = true;
		// Check if a row is full... {Top to bottom approach}
		for(int col = 1; col < GRID_COLS + 1; col++){
			if(grid[row][col].type == 0){
				isFull = false;			// Empty cell detected, row not empty !
				break;					// break the loop!
			}
		}
		if(isFull == true){
			rowsDeleted[row - 1] = true;
			numberDeleted++;
			isBlockFalling = false;
		}
	}
	return numberDeleted;
}

int CGame::DeletePotentialRows(){
	int numberDeleted = 0;
	for(int row = 1; row < GRID_ROWS + 1; row++){
		bool isFull = true;
		// Check if a row is full... {Top to bottom approach}
		for(int col = 1; col < GRID_COLS + 1; col++){
			if(grid[row][col].type == 0){
				isFull = false;			// Empty cell detected, row not empty !
				break;					// break the loop!
			}
		}
		if(isFull == true){
			DeleteRow(row);
			numberDeleted++;
		}
	}
	return numberDeleted;
}

void CGame::DeleteRow(int row){
	// Going from Bottom to Top...
	for(int r = row; r >= 1; r--){
		// Shift everything down...
		for(int col = 1; col < GRID_COLS + 1; col++){
			grid[r][col].opacity = grid[r-1][col].opacity;
			grid[r][col].type = grid[r-1][col].type;
		}
	}
}

void CGame::UpdateScore(int linesDeleted){
	static int linesCleared = 0;
	// Scoring system:
	switch(linesDeleted){
	case 1:
		score += 100;
		break;
	case 2:
		score += 400;
		break;
	case 3:
		score += 800;
		break;
	case 4:
		score += 1600;
		break;
	default:
		break;
	}
	
	linesCleared += linesDeleted;		// Check whether to advance level ?
	if(linesCleared >= 10){
		level++;
		if(level > 20){
			level = 20;
		}
		else{
			blockSpeed = 125 - 6*level;
			//score *= level;
		}
		linesCleared = 0;
	}

	if(score > 99999999){
		score = 99999999;
	}
}

float F(float t, int i)
{
    float threshold = 0.5f;
    float m = (-1.0f / threshold);
    float shift = (1.0f - threshold) / 5.0f;
    return m * (t - threshold - shift * i);       // y = y0 + m*(x-x0)
}

float G(float x, int j) {
    return x*x*x;
}


// Actual deletion of the rows takes place in ShiftAnimation function, and NOT in this function. This
// is just an animation function {eventhough it is tampering with the physical grid structure which is
// very very bad. Graphics/animation/data all must be totally separate from each other}:
void CGame::DeleteAnimation(){
	if(rowAnimationTrigger){
		static float t = 0;
		const float duration = 1.0f / 3.0f;   // total animation sequence duration {animation length in seconds}

		for (int m = 0; m < 20; m++)
		{
			if(rowsDeleted[m] == true){
				for (int k = 4; k >= 0; k--)
				{
					float opacity = F(t, 4 - k);
					if(opacity < 0.0) opacity = 0.0;
					grid[m + 1][k + 1].opacity = opacity;
					grid[m + 1][10 - (k + 1) + 1].opacity = opacity;
				}
			}
		}

		t += 1.0f / ((float)FRAMES_PER_SECOND * duration);   // 1/(FPS * seconds)
		
		if(t > 1.0){
		// time over, take final actions...
			t = 0.0;
			//ResetRowsDeletedArray();
			shiftAnimationTrigger = true;
			rowAnimationTrigger = false;
		}
	}
}

void CGame::ResetGridCoordinates(){
	//Initialize Grid coordinates
	for(int j = 0; j < GRID_ROWS + 2; j++){
		for(int i = 0; i < GRID_COLS + 2; i++){
			grid[j][i].x = MIN_X + i * 34 - 34;
			grid[j][i].y = MIN_Y + j * 34 - 34;
		}
	}
}

// Actual deletion of the rows takes place in ShiftAnimation function, at the very end. The rows
// are physically deleted from the grid array.
void CGame::ShiftAnimation(){
	if(shiftAnimationTrigger)
	{
		static float t = 0;
		const float duration = 1.0f / 3.0f;   // total animation sequence duration {animation length in seconds}
		t += 1.0f / (60.0f * duration);   // 1/(FPS * seconds)

		/*
		for (int m = 0; m < 20; m++)
		{
			if(rowsDeleted[m] == true){
				for (int k = 4; k >= 0; k--)
				{
					float opacity = F(t, 4 - k);
					grid[m + 1][k + 1].opacity = opacity;
					grid[m + 1][10 - (k + 1) + 1].opacity = opacity;
				}
			}
		}
		*/
		if(t > 1.0)
		{
		// time over, take final actions...
			t = 0.0;
			DeletePotentialRows();
			animationTrigger = false;
			shiftAnimationTrigger = false;
			ResetRowsDeletedArray();
			//ResetGridCoordinates();
		}
	}
}

void CGame::UpdateLogic(){
	tempTicker++;

	if(!animationTrigger){
		if(isBlockFalling == false){
			currentBlock.type = nextBlock;
			nextBlock = RandomRange(1, 8);
			//nextBlock = 5;
			currentBlock.x = blockStartPositions[currentBlock.type][0];	// X
			currentBlock.y = blockStartPositions[currentBlock.type][1];	// Y
			isBlockFalling = true;
			currentBlock.rotationCount = 0;
			if(CheckCollision() == true)
			{
				gameOver = true;
			}
		}

		ProcessInput();

		if(tempTicker % blockSpeed == 0){
			currentBlock.y += 34;
		}

		HandleCollision();
		int linesDeleted = CheckPotentialDelRows();
		if(linesDeleted > 0){
			animationTrigger = true;
			rowAnimationTrigger = true;
			sm->SendSoundEvent(CSoundManager::DELETEROW);
		}

		UpdateScore(linesDeleted);
	}
	else{

		// ROW DELETION CODE:
		DeleteAnimation();

		// BLOCK SHIFT CODE:
		ShiftAnimation();
	}
}

/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/

CGameMenu::CGameMenu(void)
{
	ResetMenu();
}

void CGameMenu::ResetMenu()
{
	globalState = 0;
	localState = 0;
	choiceSelected = false;
	controlScreenActivated = false;
}

CGameMenu::~CGameMenu(void)
{
	DestroyMenu();
}

bool CGameMenu::InitializeMenu(CSoundManager * smPtr){

	background = LoadTexture("Media\\Artwork\\Menu\\menu_bck.jpg");
	if(background < 0){
		return false;
	}

	controls = LoadTexture("Media\\Artwork\\Controls\\ControlsBCK.jpg");
	if(controls < 0){
		return false;
	}

	menuOptions[0] = LoadTexture("Media\\Artwork\\Menu\\newGame.png");
	menuOptions[1] = LoadTexture("Media\\Artwork\\Menu\\controls.png");
	menuOptions[2] = LoadTexture("Media\\Artwork\\Menu\\exit.png");
	menuOptions[3] = LoadTexture("Media\\Artwork\\Menu\\newGame_selected.png");
	menuOptions[4] = LoadTexture("Media\\Artwork\\Menu\\controls_selected.png");
	menuOptions[5] = LoadTexture("Media\\Artwork\\Menu\\exit_selected.png");

	for(int i = 0; i < 6; i++){
		if(menuOptions[i] < 0){
			return false;
		}
	}

	sm = smPtr;

	return true;
}

void CGameMenu::DestroyMenu(){
	glDeleteTextures( 1, &background );
	glDeleteTextures( 1, &controls );
	for(int i = 0; i < 6; i++){
		glDeleteTextures( 1, &(menuOptions[i]) );
	}
}

void CGameMenu::DrawBackGround(GLuint bck){
	glLoadIdentity();
	glDisable(GL_BLEND);
	glBindTexture( GL_TEXTURE_2D, bck );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex3f( 0.f, 0.f, 0.0f );
	
		//Bottom-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex3f( (float)SCREEN_WIDTH, 0.f, 0.f );
	
		//Top-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex3f( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.f );
	
		//Top-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex3f( 0.f, (float)SCREEN_HEIGHT, 0.f );
	glEnd();

}

void CGameMenu::DrawOption(int index, int x, int y, int width, int height){
	glBindTexture( GL_TEXTURE_2D,  menuOptions[index]);
	//Grid opacity:
	glColor4f( 1.0, 1.0, 1.0, 1.0 );

	glLoadIdentity();
	glTranslatef((float)x, (float)y, 0.0);
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex3f( 0.f, 0.f, 0.0f );
	
		//Bottom-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex3f( (float)width, 0.f, 0.f );
	
		//Top-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex3f( (float)width, (float)height, 0.f );
	
		//Top-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex3f( 0.f, (float)height, 0.f );
	glEnd();
}

void CGameMenu::DrawHelpScreen(){
	DrawBackGround(controls);
}

void CGameMenu::DrawMenu(){
	if(controlScreenActivated == true)
	{
		DrawBackGround(controls);
	}
	else
	{
		DrawBackGround(background);
		glEnable(GL_BLEND);
		switch(localState){
		case 0:				// New game highlighted
			DrawOption(3, 629, 367 - 102, 341, 102);
			DrawOption(1, 629, 367, 341, 102);
			DrawOption(2, 629, 367 + 102, 341, 102);
			break;
		case 1:				// Controls option highlighted
			DrawOption(0, 629, 367 - 102, 341, 102);
			DrawOption(4, 629, 367, 341, 102);
			DrawOption(2, 629, 367 + 102, 341, 102);
			break;
		case 2:				// Exit highlighted
			DrawOption(0, 629, 367 - 102, 341, 102);
			DrawOption(1, 629, 367, 341, 102);
			DrawOption(5, 629, 367 + 102, 341, 102);
			break;
		default:
			break;
		}
	}
}

void CGameMenu::HandleInput(){
	static bool downKeyPressed = false;
	static bool upKeyPressed = false;
	static bool returnKeyPressed = false;

	Uint8 * keystates = SDL_GetKeyState( NULL );
	
	// Down Key pressed: key repitition properly handled
	if( keystates[ SDLK_DOWN ] && !downKeyPressed){
		downKeyPressed = true;
		localState = (localState + 1) % 3;
		globalState = 0;	// Still stuck in the menu... Common select already !

		sm->SendSoundEvent(CSoundManager::MOVEBLOCK);
	}
	if( !keystates[ SDLK_DOWN ]) { 
		downKeyPressed = false;
	}

	// UP Key pressed: key repitition properly handled
	if( keystates[ SDLK_UP ] && !upKeyPressed){
		upKeyPressed = true;
		localState = (localState + 2) % 3;
		globalState = 0;
		sm->SendSoundEvent(CSoundManager::MOVEBLOCK);
	}
	if( !keystates[ SDLK_UP ]) { 
		upKeyPressed = false;
	}

	if(controlScreenActivated == true){
		localState = 1;
	}
	
	// Return Key pressed: Take aooropriate action
	if( keystates[ SDLK_RETURN ] && !returnKeyPressed){
		returnKeyPressed = true;
		if(controlScreenActivated == false)
			controlScreenActivated = true;
		else controlScreenActivated = false;

		sm->SendSoundEvent(CSoundManager::DELETEROW);
		switch(localState){
		case 0:
			globalState = 1;	// start new game
			break;
		case 1:
			globalState = 0;	// Remain in the menu... Architecture changed somewhat for the Control help screen...
			break;
		case 2:
			globalState = 3;	// Exit !
			break;
		default:
			break;
		}
		
	}
	if( !keystates[ SDLK_RETURN ]) {
		returnKeyPressed = false;
	}
}

int CGameMenu::CurrentState(){
	return globalState;
}