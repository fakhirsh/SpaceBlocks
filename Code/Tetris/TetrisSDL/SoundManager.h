#ifndef __SOUNDMANAGER_H_
#define __SOUNDMANAGER_H_

#include "SDL_mixer.h"

class CSoundManager
{
public:
	enum { MOVEBLOCK = 0, ROTATE, DROP, DELETEROW};

	CSoundManager(void);
	~CSoundManager(void);
	bool Initialize(int frequency, Uint16 format, int channels, int chunksize);
	bool LoadContent(void);
	void UnLoad(void);
	void Free(void);
	int PlayMusic(void);
	void StopMusic(void);
	int PlayEffect(int effect);

	int HandleSoundEvents();
	void SendSoundEvent(int e);

private:
	Mix_Music * music;
	Mix_Chunk * effects[4];
	bool events[4];
};

#endif