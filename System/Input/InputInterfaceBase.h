#ifndef _INPUT_INTERFACE_BASE_H_
#define _INPUT_INTERFACE_BASE_H_

#ifdef _MSC_VER
	#include "win32/InputInterface.h"
#elif defined(__IOS)
    #include "ios/InputInterface.h"
#elif defined(__GNUC__)
	#include "unix/InputInterface.h"
#endif

#endif
