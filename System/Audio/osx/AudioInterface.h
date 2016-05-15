#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <list>

//the data
class SoundBuffer {
public:
	SoundBuffer(){}
	virtual ~SoundBuffer(){}
	
	ALuint buffer;
};

//Controller
class SoundObject {
	SoundObject() : position( 0.0f, 0.0f, 0.0f ) {}
	virtual void Play(){}
	virtual void Stop(){}
	
	Vector3d position;
	ALuint source;
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
	typedef std::list<SoundBuffer*> SoundBufferList;
	SoundBufferList m_Buffers;
	typedef std::list<SoundObject*> SoundList;
	SoundList m_Sounds;
};

#endif