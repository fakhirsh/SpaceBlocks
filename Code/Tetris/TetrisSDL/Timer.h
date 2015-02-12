#ifndef __CTIMER_H_
#define __CTIMER_H_

#include "SDL.h"

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	void Start();
	void Stop();
	void Pause();
	void UnPause();

	// Get current timer ticks
	int GetTicks();

	//state checking
	bool IsStarted();
	bool IsPaused();

private:
	int startTicks, pauseTicks;
	bool started, paused;
};

#endif