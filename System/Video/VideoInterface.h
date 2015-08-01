#ifndef _VIDEO_INTERFACE_BASE_H_
#define _VIDEO_INTERFACE_BASE_H_

#ifdef _MSC_VER
	#include "win32/VideoInterface.h"
	#include "win32/VideoInterface_MMX.h"
	#include "win32/VideoInterface_SSE.h"
#elif defined(__GNUC__)
	#include "unix/VideoInterface.h"
#endif

#endif
