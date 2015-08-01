#ifndef _AUDIO_INTERFACE_FMOD_H_
#define _AUDIO_INTERFACE_FMOD_H_

#include "AudioInterface.h"

class SoundBuffer_fmod : public SoundBuffer {
public:
	SoundBuffer_fmod():fmod_sound(NULL){}
	virtual ~SoundBuffer_fmod(){
		Term();
	}
	
	void Term();
	virtual void Play();
	virtual void Stop();
	
	void*      fmod_sound;
};

class IAudioInterface_fmod : public IAudioInterface
{
public:
	IAudioInterface_fmod();
	virtual ~IAudioInterface_fmod();

	virtual GEN_RESULT	Init();
	virtual GEN_RESULT	Term();
	virtual GEN_RESULT	Update( float elapsed );

	virtual SoundBuffer*	CreateSoundBuffer( const char* filename );
	virtual SoundBuffer*	CreateSoundBuffer( const char* name, char *buffer, int buffer_size );
	virtual GEN_RESULT		DestroySoundBuffer( SoundBuffer* sound );
};

#endif