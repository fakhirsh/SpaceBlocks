#include "SoundManager.h"


CSoundManager::CSoundManager(void)
{
	events[0] = false;
	events[1] = false;
	events[2] = false;
	events[3] = false;
	// Initialize and Load Sound related stuff...
	Initialize(22050, MIX_DEFAULT_FORMAT, 2, 512);
	LoadContent();
}


CSoundManager::~CSoundManager(void)
{
	UnLoad();
	Free();
}


// Initializes sound manager
bool CSoundManager::Initialize(int frequency, Uint16 format, int channels, int chunksize)
{
	music = NULL;
	for(int i = 0; i < 4; i++){
		effects[i] = NULL;
	}
	
	if( Mix_OpenAudio( frequency, format, channels, chunksize ) == -1 ) { 
		return false; 
	} 
	Mix_VolumeMusic(70);

	return true;
}

// Loads all the sound files into memory
bool CSoundManager::LoadContent(void)
{
	music = Mix_LoadMUS( "Media\\Sound\\BCK.wav" );
    //If there was a problem loading the music
    if( music == NULL )
    {
        return false;
    }

    //Load the sound effects
    effects[MOVEBLOCK] = Mix_LoadWAV( "Media\\Sound\\moveBlock.wav" );
	if(effects[MOVEBLOCK] == NULL){
		return false;
	}

	effects[DROP] = Mix_LoadWAV( "Media\\Sound\\button-11.wav" );
	if(effects[DROP] == NULL){
		return false;
	}

    effects[DELETEROW] = Mix_LoadWAV( "Media\\Sound\\deleteRow.wav" );
	if(effects[DELETEROW] == NULL){
		return false;
	}



    //If there was a problem loading the sound effects
    
	return true;
}


void CSoundManager::UnLoad(void)
{
	for(int i = 0; i < 4; i++){
		Mix_FreeChunk( effects[i] ); 
	}
	Mix_FreeMusic( music ); 
}


void CSoundManager::Free(void)
{
	Mix_CloseAudio();	// Free audio
}


int CSoundManager::PlayMusic(void)
{
	//If there is no music playing
    if( Mix_PlayingMusic() == 0 )
    {
        //Play the music
        return Mix_PlayMusic( music, -1 );
    }
	return 1;
}


void CSoundManager::StopMusic(void)
{
	Mix_HaltMusic();
}

int CSoundManager::PlayEffect(int effect)
{
	switch(effect){
	case MOVEBLOCK:
		// Player pressed Left or Right keys
		if( Mix_PlayChannel( -1, effects[0], 0 ) == -1 ) { 
			return 0; 
		} 
		break;
	default:
		return 0;
	}
	return 1;
}

int CSoundManager::HandleSoundEvents(){

	if(events[MOVEBLOCK] == true){
		if( Mix_PlayChannel( -1, effects[0], 0 ) == -1 ) { 
			return 0; 
		} 
	}
	if(events[ROTATE] == true){
		if( Mix_PlayChannel( -1, effects[0], 0 ) == -1 ) { 
			return 0; 
		} 
	}

	// Play only if the row is NOT being deleted. If row is being deleted, then play ONLY row
	// deletion sound
	if(events[DROP] == true && events[DELETEROW] == false){	
		if( Mix_PlayChannel( -1, effects[DROP], 0 ) == -1 ) { 
			return 0; 
		} 
	}

	if(events[DELETEROW] == true){
		if( Mix_PlayChannel( -1, effects[DELETEROW], 0 ) == -1 ) { 
			return 0; 
		}
	}

	events[MOVEBLOCK] = false;
	events[ROTATE] = false;
	events[DROP] = false;
	events[3] = false;

	return 1;
}

void CSoundManager::SendSoundEvent(int e){
	events[e] = true;
}
