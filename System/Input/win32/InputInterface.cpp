#include "stdafx.h"
#include "InputInterface.h"
#include "InputListener.h"
#include "../Platform/platform.h"

namespace
{
	struct t_EnumJoystickCallbackPointers
	{
		LPDIRECTINPUT8 pDirectInput;
		LPDIRECTINPUTDEVICE8 pJoystick;
	};
	
	IInputListener*	g_pInputListener = NULL;
}

BOOL CALLBACK    EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK    EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
									VOID* pContext )
{
	HRESULT hr;

	t_EnumJoystickCallbackPointers* pointers = (t_EnumJoystickCallbackPointers*)pContext;

	// Obtain an interface to the enumerated joystick.
	hr = pointers->pDirectInput->CreateDevice( pdidInstance->guidInstance, &pointers->pJoystick, NULL );

	// If it failed, then we can't use this joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED(hr) ) 
		return DIENUM_CONTINUE;

	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_STOP;
}




//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
								  VOID* pContext )
{
	LPDIRECTINPUTDEVICE8 pJoystick = (LPDIRECTINPUTDEVICE8)pContext;

	static int nSliderCount = 0;  // Number of returned slider controls
	static int nPOVCount = 0;     // Number of returned POV controls

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg; 
		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin              = -1000; 
		diprg.lMax              = +1000; 

		// Set the range for the axis
		if( FAILED( pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
			return DIENUM_STOP;

	}


	return DIENUM_CONTINUE;
}

WNDPROC				g_pfnMainWndProc = NULL;
LRESULT CALLBACK	HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL HookWindow(HWND hWnd)
{
	g_pfnMainWndProc = (WNDPROC)(GetWindowLong(hWnd, GWL_WNDPROC));

	if(!g_pfnMainWndProc)
	{
		return FALSE;
	}

	if(!SetWindowLong(hWnd, GWL_WNDPROC, (LONG)HookedWindowProc))
	{
		return FALSE;
	}

	return TRUE;
}

void UnhookWindow(HWND hWnd)
{
	if(g_pfnMainWndProc && hWnd)
	{
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)g_pfnMainWndProc);
		//g_hWnd = NULL;
		g_pfnMainWndProc = NULL;
	}
}

LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!g_pfnMainWndProc || !hWnd)
	{
		return FALSE;
	}

	switch(uMsg)
	{
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, IInputInterface::OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, IInputInterface::OnLButtonUp);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, IInputInterface::OnLButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, IInputInterface::OnRButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, IInputInterface::OnRButtonDown);

	default:
		//return FALSE;
		break;
	}

	return(CallWindowProc(g_pfnMainWndProc,hWnd,uMsg,wParam,lParam));
}

IInputInterface::IInputInterface():
m_pDirectInput(NULL),
m_pJoystick(NULL),
m_hWnd(NULL)
{

}

IInputInterface::~IInputInterface()
{
	Term();
}

bool IInputInterface::Init(HWND hWnd)
{
	HRESULT hr;
	
	m_hWnd = hWnd;
	
	HookWindow(m_hWnd);

#if 0
	t_EnumJoystickCallbackPointers pointers;
	memset(&pointers, 0, sizeof(t_EnumJoystickCallbackPointers));

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	// Create a DInput object
	if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (VOID**)&m_pDirectInput, NULL ) ) )
	{
		return false;
	}


	pointers.pDirectInput = m_pDirectInput;

	// Look for a simple joystick we can use for this sample program.
	if( FAILED( hr = m_pDirectInput->EnumDevices( DI8DEVCLASS_GAMECTRL, 
		EnumJoysticksCallback,
		&pointers, DIEDFL_ATTACHEDONLY ) ) )
	{
		return false;
	}

	m_pJoystick = pointers.pJoystick;

	// Make sure we got a joystick
	if( NULL == m_pJoystick )
	{
		MessageBox( NULL, TEXT("Joystick not found. The sample will now exit."),  
			TEXT("DirectInput Sample"), 
			MB_ICONERROR | MB_OK );
		//EndDialog( hDlg, 0 );
		return false;
	}

	// Set the data format to "simple joystick" - a predefined data format 
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		return false;
	}

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	if( FAILED( hr = m_pJoystick->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
	{
		return false;
	}

	// Enumerate the joystick objects. The callback function enabled user
	// interface elements for objects that are found, and sets the min/max
	// values property for discovered axes.
	if( FAILED( hr = m_pJoystick->EnumObjects( EnumObjectsCallback, 
		(VOID*)m_pJoystick, DIDFT_ALL ) ) )
	{
		return false;
	}
	#endif

	return true;
}

bool IInputInterface::Term()
{
	// Release any DirectInput objects.
	if( m_pJoystick )
	{
		m_pJoystick->Unacquire();
		m_pJoystick->Release();
		m_pJoystick = NULL;
	}

	if( m_pDirectInput )
	{
		m_pDirectInput->Release();
		m_pDirectInput = NULL;
	}
	
	UnhookWindow(m_hWnd);
	
	return true;
}

bool IInputInterface::Update(InputState& state)
{
	return false;
#if 0	
	HRESULT     hr;
	//TCHAR       strText[512]; // Device state text
	DIJOYSTATE2 js;           // DInput joystick state 
	//TCHAR*      str;

	if( NULL == m_pJoystick ) 
	{
		return false;
	}

	// Poll the device to read the current state
	hr = m_pJoystick->Poll(); 
	if( FAILED(hr) )  
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = m_pJoystick->Acquire();
		while( hr == DIERR_INPUTLOST ) 
		{
			hr = m_pJoystick->Acquire();
		}

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return false; 
	}

	// Get the input's device state
	if( FAILED( hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
	{
		return false; // The device should have been acquired during the Poll()
	}

	state.X = js.lX;
	state.Y = js.lY;
	for(int i  = 0; i< MAX_BUTTONS; ++i )
	{
		state.Button[i] = js.rgbButtons[i];
	}
#endif

	return true;
}

void IInputInterface::GrabMouse( bool grab )
{
	Platform::toggleGrabMouse();
}

void IInputInterface::SetListener(IInputListener* pListener)
{
	g_pInputListener = pListener;
}
	
void IInputInterface::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	if(!g_pInputListener)
		return;

	g_pInputListener->OnLButtonUp(x, y, (uint32)keyFlags);

}

void IInputInterface::OnLButtonDown(HWND hWnd, BOOL bDoubleClick, int x, int y, UINT keyFlags)
{
	if(!g_pInputListener)
		return;

	g_pInputListener->OnLButtonDown((bool)bDoubleClick, x, y, (uint32)keyFlags);

}

void IInputInterface::OnRButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	if(!g_pInputListener)
		return;

	g_pInputListener->OnRButtonUp(x, y, (uint32)keyFlags);
}

void IInputInterface::OnRButtonDown(HWND hWnd, BOOL bDoubleClick, int x, int y, UINT keyFlags)
{
	if(!g_pInputListener)
		return;

	g_pInputListener->OnRButtonDown((bool)bDoubleClick, x, y, (uint32)keyFlags);
}

void IInputInterface::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	if(!g_pInputListener)
		return;
	
	g_pInputListener->OnMouseMove(x, y, (uint32)keyFlags);
}