#include "../platform.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/extensions/xf86vmode.h>

//HACK
extern void cb_resize( int Width, int Height );
extern void cb_mousemove(int x, int y);
extern void cb_keyboard_dn( unsigned char key, int x, int y);
extern void cb_keyboard_up( unsigned char key, int x, int y);
extern void cb_mouseclick( int button, int state, int x, int y);

namespace Platform {
	bool bGrabMouse = false;
	bool bActive = true;
	
	struct timespec time_sleep;

	void sleep(long seconds, long milliseconds){
			assert(milliseconds < 1000);
			time_sleep.tv_sec = seconds;
			time_sleep.tv_nsec =  milliseconds * 1000000; //convert nanoseconds to milliseconds.
			nanosleep (&time_sleep, NULL);
	}

	typedef struct {
		Display *dpy;
		int screen;
		Window win;
		GLXContext ctx;
		XSetWindowAttributes attr;
		bool fs;
		XF86VidModeModeInfo deskMode;
		int x, y;
		unsigned int width, height;
		unsigned int depth;    
	} GLWindow;

	typedef struct {
		int width;
		int height;
		unsigned char *data;
	} textureImage;
	
	/* attributes for a single buffered visual in RGBA format with at least
	 * 4 bits per color and a 16 bit depth buffer */
	static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4,
		GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, 16,
		None};
	
	/* attributes for a double buffered visual in RGBA format with at least
	 * 4 bits per color and a 16 bit depth buffer */
	static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
		GLX_RED_SIZE, 4, 
		GLX_GREEN_SIZE, 4, 
		GLX_BLUE_SIZE, 4, 
		GLX_DEPTH_SIZE, 16,
		None };
	
	GLWindow GLWin;
	Bool done;
	
	int createWindow(char *title, int width, int height, int bpp, bool fullscreen) {
		XVisualInfo *vi;
		Colormap cmap;
		int dpyWidth, dpyHeight;
		int i;
		int glxMajorVersion, glxMinorVersion;
		int vidModeMajorVersion, vidModeMinorVersion;
		XF86VidModeModeInfo **modes;
		int modeNum;
		int bestMode;
		Atom wmDelete;
		Window winDummy;
		unsigned int borderDummy;
		
		GLWin.fs = fullscreen;
		/* set best mode to current */
		bestMode = 0;
		/* get a connection */
		GLWin.dpy = XOpenDisplay(0);
		GLWin.screen = DefaultScreen(GLWin.dpy);
		XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
			&vidModeMinorVersion);
		printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
			vidModeMinorVersion);
		XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
		/* save desktop-resolution before switching modes */
		GLWin.deskMode = *modes[0];
		/* look for mode with requested resolution */
		for (i = 0; i < modeNum; i++)
		{
			if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
			{
				bestMode = i;
			}
		}
		/* get an appropriate visual */
		vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
		if (vi == NULL)
		{
			vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
			//GLWin.doubleBuffered = False;
			printf("Only Singlebuffered Visual!\n");
		}
		else
		{
			//GLWin.doubleBuffered = True;
			printf("Got Doublebuffered Visual!\n");
		}
		glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
		printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
		/* create a GLX context */
		GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
		/* create a color map */
		cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
			vi->visual, AllocNone);
		GLWin.attr.colormap = cmap;
		GLWin.attr.border_pixel = 0;
	
		if (GLWin.fs)
		{
			XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
			XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
			dpyWidth = modes[bestMode]->hdisplay;
			dpyHeight = modes[bestMode]->vdisplay;
			printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
			XFree(modes);
		
			/* create a fullscreen window */
			GLWin.attr.override_redirect = True;
			GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
				StructureNotifyMask;
			GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
				0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
				CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
				&GLWin.attr);
			XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
			XMapRaised(GLWin.dpy, GLWin.win);
			XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
				GrabModeAsync, CurrentTime);
			XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
				GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
		}
		else
		{
			/* create a window in window mode*/
			GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask | SubstructureNotifyMask;
			GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
			/* only set window title and handle wm_delete_events if in windowed mode */
			wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
			XSetStandardProperties(GLWin.dpy, GLWin.win, title,
				title, None, NULL, 0, NULL);
			XMapRaised(GLWin.dpy, GLWin.win);
		}       
		/* connect the glx-context to the window */
		glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
		XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y, &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
		printf("Depth %d\n", GLWin.depth);
		if (glXIsDirect(GLWin.dpy, GLWin.ctx)) {
			printf("Congrats, you have Direct Rendering!\n");
		} else	{
			printf("Sorry, no Direct Rendering possible!\n");
		}

		if( !GLWin.fs ) {
			XSelectInput(GLWin.dpy, GLWin.win, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | StructureNotifyMask );
		}
		
		cb_resize( GLWin.width, GLWin.height );
		 //resizeGLScene(GLWin.width, GLWin.height);
		/*
		if (!initGL())
		{
			printf("Could not initialize OpenGL.\nAborting...\n");
			return -1;
		}
		*/
		return 0;
	}

	int closeWindow() {
		if (GLWin.ctx)
		{
			if (!glXMakeCurrent(GLWin.dpy, None, NULL))
			{
				printf("Could not release drawing context.\n");
				//return -1;
			}
			glXDestroyContext(GLWin.dpy, GLWin.ctx);
			GLWin.ctx = NULL;
		}
		/* switch back to original desktop resolution if we were in fs */
		if (GLWin.fs)
		{
			XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
			XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
		}
		XCloseDisplay(GLWin.dpy);
		return 0;
	}
	
	void swapBuffers() {
		glXSwapBuffers(GLWin.dpy, GLWin.win);
	}
	
	void* getWindowHandle() {
		return NULL; //for now...
	}
	
	void pump() {
		XEvent event;
		KeySym key;
		
        /* handle the events in the queue */
        while (XPending(GLWin.dpy) > 0)
        {
            XNextEvent(GLWin.dpy, &event);
            switch (event.type)
            {
            	case Expose: {
	                if (event.xexpose.count != 0)
	                    break;
                    //drawGLScene();
         	    }    break;
                case ConfigureNotify: {
                    /* call resizeGLScene only if our window-size changed */
                    if ((event.xconfigure.width != GLWin.width) || (event.xconfigure.height != GLWin.height)) {
                        GLWin.width = event.xconfigure.width;
                        GLWin.height = event.xconfigure.height;
                        //resizeGLScene(event.xconfigure.width,
                            //event.xconfigure.height);
                        cb_resize( GLWin.width, GLWin.height );
                    }
                } break;
                /* exit in case of a mouse button press */
                case ButtonPress:     
                    printf("ButtonPress...%d\n", event.xbutton.button);
                    cb_mouseclick( event.xbutton.button, 1, event.xbutton.x, event.xbutton.y);
                    break;
                case ButtonRelease:     
                    printf("ButtonRelease...%d\n", event.xbutton.button);
                    cb_mouseclick( event.xbutton.button, 0, event.xbutton.x, event.xbutton.y);
                    break;
                case KeyPress:
                    key = XLookupKeysym(&event.xkey, 0);
                    cb_keyboard_dn(key, 0, 0);
                    //printf("KeyPress...[%d | %c](%d)(%c)\n", key, key, event.xkey, event.xkey);
                    break;
                case KeyRelease: {
                	//Disable auto repeat.
                	bool is_retriggered = false;
					if (XEventsQueued(GLWin.dpy, QueuedAfterReading)) {
						XEvent nev;
						XPeekEvent(GLWin.dpy, &nev);
						if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode) {
							 // delete retriggered KeyPress event
							 XNextEvent(GLWin.dpy, &event);
							 is_retriggered = true;
						   }
						}
						if( !is_retriggered ) {
						   key = XLookupKeysym(&event.xkey, 0);
						   cb_keyboard_up(key, 0, 0);
						   //printf("KeyRelease...[%d | %c](%d)(%c)\n", key, key, event.xkey, event.xkey);
						}
                } break;
                case MotionNotify:
                	//printf("MotionNotify...(%d %d)\n", event.xmotion.x, event.xmotion.y);
                	cb_mousemove( event.xmotion.x, event.xmotion.y );
                   	break;
                case ClientMessage: {    
                    if ( *XGetAtomName(GLWin.dpy, event.xclient.message_type) == *"WM_PROTOCOLS") {
                        printf("ClientMessage...\n");
                        bActive = false;
                    }
                }break;
                default:
                    break;
            }
        }
	}

	void grabMouse() {
		XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
	}

	void ungrabMouse() {
		XUngrabPointer(GLWin.dpy, CurrentTime);
	}
	
	void toggleGrabMouse() {
		bGrabMouse = !bGrabMouse;
		if ( bGrabMouse ) {
			grabMouse();
		} else {
			ungrabMouse();
		}
	}

	bool getActive() {
		return bActive;
	}
	
	void setActive( bool active ) {
		bActive = active;
	}
}

