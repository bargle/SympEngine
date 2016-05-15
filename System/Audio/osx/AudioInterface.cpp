#include "core_defs.h"
#include "AudioInterface.h"

IAudioInterface::IAudioInterface(){
}

IAudioInterface::~IAudioInterface(){
}

GEN_RESULT		IAudioInterface::Init() { 
    //InitAudioInterface();
    //InitOpenAL();
    return RESULT_OK; 
}

GEN_RESULT		IAudioInterface::Term() { 
    //TermOpenAL();
    //TermAudioInterface();
    return RESULT_OK; 
}

GEN_RESULT		IAudioInterface::Update( float elapsed ) { 
    return RESULT_ERROR; 
}

SoundBuffer*	IAudioInterface::CreateSoundBuffer( const char* filename ) { 
    return NULL; 
}

SoundBuffer*	IAudioInterface::CreateSoundBuffer( const char* name, char *buffer, int buffer_size ) { 
    return NULL; 
}

GEN_RESULT		IAudioInterface::DestroySoundBuffer( SoundBuffer* sound ) { 
    return RESULT_ERROR; 
}
