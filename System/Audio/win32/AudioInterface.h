#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <list>

class SoundBuffer {
public:
	SoundBuffer(){}
	virtual ~SoundBuffer(){}

	virtual void Play(){}
	virtual void Stop(){}

};

class IAudioInterface
{
public:
	IAudioInterface();
	virtual ~IAudioInterface();

	virtual GEN_RESULT	Init();
	virtual GEN_RESULT	Term();
	virtual GEN_RESULT	Update( float elapsed );
	
	virtual SoundBuffer*	CreateSoundBuffer( const char* filename );
	virtual SoundBuffer*	CreateSoundBuffer( const char* name, char *buffer, int buffer_size );
	virtual GEN_RESULT		DestroySoundBuffer( SoundBuffer* sound );
	
protected:
	typedef std::list<SoundBuffer*> SoundList;
	SoundList m_Sounds;
};

#endif