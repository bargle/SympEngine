#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef _MSC_VER
	#include <windows.h>
#endif

namespace Platform {

	void sleep(long seconds, long milliseconds);
	int createWindow(char *title, int width, int height, int bpp, bool fullscreen);
	int closeWindow();
	void swapBuffers();
	void pump();
	void grabMouse( bool grab );
	void ungrabMouse();
	void toggleGrabMouse();
	bool getActive();
	void setActive( bool active );
	
	//TODO: this should not be here... should be in a platform-specific header.
#ifdef _MSC_VER
	HWND getWindowHandle();
#else
	void* getWindowHandle();
#endif
}

#endif
