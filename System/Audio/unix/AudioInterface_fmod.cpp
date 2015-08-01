#include "core_defs.h"
#include "AudioInterface_fmod.h"

#include "../../../3rdParty/fmod/api/inc/fmod.hpp"
#include "../../../3rdParty/fmod/api/inc/fmod_errors.h"

namespace {
    FMOD::System     *fmod_system = NULL;
    FMOD::Channel    *fmod_channel = NULL;
}

void SoundBuffer_fmod::Term() {
	if ( fmod_sound ) {
		((FMOD::Sound*)fmod_sound)->release();
		fmod_sound = NULL;
	}
}

void SoundBuffer_fmod::Play() {
	if ( fmod_system == NULL ){
		return;
	}

	if ( fmod_sound == NULL ) {
		return;
	}

	FMOD::Sound* pSound = ((FMOD::Sound*)fmod_sound);
	FMOD_RESULT result = fmod_system->playSound(FMOD_CHANNEL_FREE, pSound, 0, &fmod_channel);
	if ( result != FMOD_OK ) {
		//do something... TODO: need to return a result.
		printf("can not play\n");
	}
}

void SoundBuffer_fmod::Stop() {
}

IAudioInterface_fmod::IAudioInterface_fmod() {
	Init();
}

IAudioInterface_fmod::~IAudioInterface_fmod() {
	Term();
}

GEN_RESULT	IAudioInterface_fmod::Init() { 

	FMOD_RESULT result = FMOD::System_Create(&fmod_system);

    result = fmod_system->setOutput(FMOD_OUTPUTTYPE_ALSA);
    if ( result != FMOD_OK ) {
    	return RESULT_ERROR;
    }

    result = fmod_system->init(32, FMOD_INIT_NORMAL, 0);
	if ( result != FMOD_OK ) {
    	return RESULT_ERROR;
    }

	return (result == FMOD_OK) ? RESULT_OK : RESULT_ERROR;
}

GEN_RESULT	IAudioInterface_fmod::Term() { 
	if ( fmod_system == NULL ) {
		return RESULT_ERROR;
	}

    FMOD_RESULT result = fmod_system->close();
    if ( result != FMOD_OK ) {
    	return RESULT_ERROR;
    }

    result = fmod_system->release();
    fmod_system = NULL;

	return (result == FMOD_OK) ? RESULT_OK : RESULT_ERROR;
}

GEN_RESULT	IAudioInterface_fmod::Update( float elapsed ) {
	if ( fmod_system == NULL ) {
		return RESULT_ERROR;
	}
	
	fmod_system->update();

	return RESULT_OK;
}

SoundBuffer*	IAudioInterface_fmod::CreateSoundBuffer( const char* filename ) {

	if ( fmod_system == NULL ) {
		return NULL;
	}

	SoundBuffer_fmod* sound = new SoundBuffer_fmod;
	if ( sound == NULL ) {
		return NULL;
	}

	FMOD::Sound *fmod_sound;
	FMOD_RESULT result = fmod_system->createSound( filename, FMOD_SOFTWARE, 0, &fmod_sound );
    if ( result != FMOD_OK ) {
    	delete sound;
    	return NULL;
    }

    //store the pointer
    sound->fmod_sound = (void*)fmod_sound;

    //TODO: add a FLAGS param
    result = fmod_sound->setMode(FMOD_LOOP_OFF);
    if ( result != FMOD_OK ) {
    	delete sound;
    	return NULL;
    }

	return sound;
}

SoundBuffer*	IAudioInterface_fmod::CreateSoundBuffer( const char* name, char *buffer, int buffer_size ) { 
	return NULL; 
}

GEN_RESULT	IAudioInterface_fmod::DestroySoundBuffer( SoundBuffer* sound ) { 
	if ( sound == NULL ) {
		return RESULT_ERROR;
	}

	delete sound;

	return RESULT_OK; 
}

