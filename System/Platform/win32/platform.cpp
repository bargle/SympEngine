#include "../platform.h"
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <time.h>
#include "core_defs.h"
#include "system.h"
#include "gameinterface.h"

namespace {
	HDC			g_hDC=NULL;				// Private GDI Device Context
	HGLRC		g_hRC=NULL;				// Permanent Rendering Context
	HWND		g_hWnd=NULL;			// Holds Our Window Handle
	HINSTANCE	g_hInstance = NULL;		// Holds The Instance Of The Application
	BOOL		g_active = FALSE;
	bool		g_grabMouse = false;
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG msg;

	//set up basics,

	//call into GameInterface.. allow game interface to create window.
	if ( InitializeGameInterface() ) {
		return -1;
	}

	while ( 1 ) {									// Loop That Runs While done=FALSE 
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			if (msg.message==WM_QUIT) {
				break;
			} else {
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		} else {
			if ( g_active ) {
				//
				UpdateGameInterface();
			} else {
				SwapBuffers(g_hDC);					// Swap Buffers (Double Buffering)
			}
			
		}
	}

	Platform::closeWindow();
	return (int)(msg.wParam);
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg) {									// Check For Windows Messages
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			// LoWord Can Be WA_INACTIVE, WA_ACTIVE, WA_CLICKACTIVE,
			// The High-Order Word Specifies The Minimized State Of The Window Being Activated Or Deactivated.
			// A NonZero Value Indicates The Window Is Minimized.
			if ((LOWORD(wParam) != WA_INACTIVE) && !((BOOL)HIWORD(wParam)))
				g_active=TRUE;						// Program Is Active
			else
				g_active=FALSE;						// Program Is No Longer Active

			return 0;								// Return To The Message Loop
		}
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		case WM_SIZE: {
			ResizeGameWindow(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
					  /*
		case WM_MOUSEMOVE: {
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 
			MouseMove( xPos, yPos );
			return 0;
		}
		*/
		case WM_KEYDOWN:
		{
			KeyDown( (int)wParam );

			return 0;
		}
		case WM_KEYUP:
		{
			KeyUp( (int)wParam );

			return 0;
		}
		case WM_RBUTTONDOWN:
			MouseClick( 1, 1, 0, 0 );
			break;
		case WM_RBUTTONUP:
			MouseClick( 1, 0, 0, 0 );
			break;
		case WM_INPUT: 
		{
			UINT dwSize = 40;
			static BYTE lpb[40];
    
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
							lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
			RAWINPUT* raw = (RAWINPUT*)lpb;
    
			if (raw->header.dwType == RIM_TYPEMOUSE) 
			{
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;

				if ( g_grabMouse )
				{
					MouseMove( xPosRelative, yPosRelative );
					SetCursorPos( 100, 100 );
				}
			} 
			break;
		}
		default:
			break;
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

namespace Platform {
	void sleep(long seconds, long milliseconds){
			Sleep( (seconds * 1000) + milliseconds );
	}

	int createWindow(char *title, int width, int height, int bpp, bool fullscreen) {

		int		PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left=(long)0;			// Set Left Value To 0
		WindowRect.right=(long)width;		// Set Right Value To Requested Width
		WindowRect.top=(long)0;				// Set Top Value To 0
		WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

		//fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

		g_hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Window Data
		wc.cbWndExtra		= 0;									// No Extra Window Data
		wc.hInstance		= g_hInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL;									// No Background Required For GL
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= "OpenGL";								// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;											// Return FALSE
		}

		//Windowed mode...
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

		// Create The Window
		if (!(g_hWnd=CreateWindowEx(dwExStyle,							// Extended Style For The Window
									"OpenGL",							// Class Name
									title,								// Window Title
									dwStyle |							// Defined Window Style
									WS_CLIPSIBLINGS |					// Required Window Style
									WS_CLIPCHILDREN,					// Required Window Style
									0, 0,								// Window Position
									WindowRect.right-WindowRect.left,	// Calculate Window Width
									WindowRect.bottom-WindowRect.top,	// Calculate Window Height
									NULL,								// No Parent Window
									NULL,								// No Menu
									g_hInstance,						// Instance
									NULL)))								// Dont Pass Anything To WM_CREATE
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bpp,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};
		
		if (!(g_hDC=GetDC(g_hWnd)))							// Did We Get A Device Context?
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		if (!(PixelFormat=ChoosePixelFormat(g_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		if(!SetPixelFormat(g_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		if (!(g_hRC=wglCreateContext(g_hDC)))				// Are We Able To Get A Rendering Context?
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		if(!wglMakeCurrent(g_hDC,g_hRC))					// Try To Activate The Rendering Context
		{
			closeWindow();								// Reset The Display
			MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}

		ShowWindow(g_hWnd,SW_SHOW);						// Show The Window
		SetForegroundWindow(g_hWnd);						// Slightly Higher Priority
		SetFocus(g_hWnd);									// Sets Keyboard Focus To The Window

		//TODO: implement GL initializers
		{
		#ifndef HID_USAGE_PAGE_GENERIC
		#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
		#endif
		#ifndef HID_USAGE_GENERIC_MOUSE
		#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
		#endif

		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_INPUTSINK;   
		Rid[0].hwndTarget = g_hWnd;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]) );
		}
#if 0
		ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

		if (!InitGL())									// Initialize Our Newly Created GL Window
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return -1;								// Return FALSE
		}
#endif

		return 0;
	}

	int closeWindow() { 
		if (g_hRC)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(g_hRC))						// Are We Able To Delete The RC?
			{
				MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
			g_hRC=NULL;										// Set RC To NULL
		}

		if (g_hDC && !ReleaseDC(g_hWnd,g_hDC))					// Are We Able To Release The DC
		{
			MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			g_hDC=NULL;										// Set DC To NULL
		}

		if (g_hWnd && !DestroyWindow(g_hWnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			g_hWnd=NULL;										// Set hWnd To NULL
		}

		if (!UnregisterClass("OpenGL",g_hInstance))			// Are We Able To Unregister Class
		{
			MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			g_hInstance=NULL;									// Set hInstance To NULL
		}

		return 0; 
	}

	void swapBuffers() {
		if ( g_hDC ) {
			SwapBuffers(g_hDC);
		}
	}

	HWND getWindowHandle() {
		return g_hWnd;
	}

	void pump(){}

	void toggleGrabMouse()
	{
		g_grabMouse = !g_grabMouse;

		if ( g_grabMouse )
		{
			ShowCursor( false );
		} else
		{
			ShowCursor( true );
		}

	}

	void grabMouse( bool grab )
	{
		g_grabMouse = grab;

		if ( g_grabMouse )
		{
			ShowCursor( false );
		} else
		{
			ShowCursor( true );
		}
	}

	bool getActive() { return true; }
	void setActive( bool active ) {}

}