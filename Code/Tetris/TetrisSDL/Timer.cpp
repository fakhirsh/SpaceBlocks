#include "Timer.h"


CTimer::CTimer(void)
{
	startTicks = 0;
	pauseTicks = 0;
	started = false;
	paused = false;
}


CTimer::~CTimer(void)
{
}

void CTimer::Start(){
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
}

void CTimer::Stop(){
	started = false;
	paused = false;
}

void CTimer::Pause(){
	if(started == true && paused == false){
		paused = true;
		pauseTicks = SDL_GetTicks() - startTicks;
	}
}

void CTimer::UnPause(){
	 //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pauseTicks;

        //Reset the paused ticks
        pauseTicks = 0;
    }
}

// Get current timer ticks
int CTimer::GetTicks(){
	if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pauseTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
	}
	return 0;
}

//state checking
bool CTimer::IsStarted(){
	return started;
}

bool CTimer::IsPaused(){
	return paused;
}