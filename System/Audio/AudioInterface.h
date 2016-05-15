 
#ifndef _AUDIO_INTERFACE_BASE_H_
#define _AUDIO_INTERFACE_BASE_H_

#ifdef _MSC_VER
	#include "win32/AudioInterface.h"
#elif defined ( __IOS )
	#include "ios/AudioInterface.h"
#elif defined(__GNUC__)
	#include "unix/AudioInterface.h"
	#include "unix/AudioInterface_fmod.h"
#elif defined (__APPLE__ && __MACH__ )
	#include "osx/AudioInterface.h"
#endif

#endif
