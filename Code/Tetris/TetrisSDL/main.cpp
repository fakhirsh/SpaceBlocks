#include <string>
#include "Timer.h"
#include "Game.h"

using namespace std;

int main( int argc, char* args[] ) { 
	bool quit = false;
	SDL_Event ev;

	CTimer fps;
	CSoundManager sm;

	CGame game;
	if(game.Initialize(&sm) == false){
		return 1;
	}
	if(game.gameMenu.InitializeMenu(&sm) == false){
		MessageBox(0, "Failed to load menu textures...", "main()", MB_OK | MB_ICONERROR);
		return 1;
	}
	
	int state = 0;	// initially start with the menu
	int gameOver = false;
	
	sm.PlayMusic();

	while(!quit){
		fps.Start();
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT){
				quit = true;
			}
			if(ev.key.keysym.sym == SDLK_ESCAPE){
				quit = true;
			}

			if(ev.key.keysym.sym == SDLK_RETURN && gameOver == true){
				state = 0;
				gameOver = false;
				game.gameMenu.ResetMenu();
				SDL_Delay(500);
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );
		switch(state){
		case 0:
			game.gameMenu.DrawMenu();
			game.gameMenu.HandleInput();
			state = game.gameMenu.CurrentState();
			break;
		case 1:	// New Game
			game.InitializeGame();
			state = 4;
			break;
		case 4:	// Game running
			// Perform Logic Functions
			game.UpdateLogic();
			if(game.IsGameOver() == true){
				state = 5;		// Yep, the game is over !!!
			}
			else{
				game.Draw();
			}
			break;
		case 5:	// Game Over... Show End screen and score.
			game.DrawGameOverScreen();
			gameOver = true;
			break;
		case 2:	//	Controls screen
			break;
		case 3:	// Exit
			quit = true;
			break;
		}

		sm.HandleSoundEvents();

		// Update screen
		SDL_GL_SwapBuffers();

		// Synchronize FPS
		if(fps.GetTicks() < 1000 / FRAMES_PER_SECOND){
			SDL_Delay((1000/FRAMES_PER_SECOND) - fps.GetTicks());
	    }
	}

	return 0; 
}