
#include "../platform.h"

namespace Platform {

	void sleep(long seconds, long milliseconds) {} //stub
	int createWindow(char *title, int width, int height, int bpp, bool fullscreen) { return 0; }
	int closeWindow() { return 0; }
	void swapBuffers() {}
	void pump() { //do our work here...
	}
	void grabMouse() {}
	void ungrabMouse() {}
	void toggleGrabMouse() {}
	bool getActive() { return true; }
	void setActive( bool active ) {}

	void* getWindowHandle() { return (void*)0; }
}
