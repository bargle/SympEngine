#ifndef _INPUT_INTERFACE_H_
#define _INPUT_INTERFACE_H_

class IInputListener;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define MAX_BUTTONS 6
struct InputState
{
	int32 X;
	int32 Y;
	uint8 Button[MAX_BUTTONS];
};

class IInputInterface
{
public:
	IInputInterface();
	~IInputInterface();

	bool Init(HWND hWnd);
	bool Term();

	bool Update(InputState& state);
	void SetListener(IInputListener* pListener);
	void GrabMouse( bool grab );
	
	static void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	static void OnLButtonDown(HWND hWnd, BOOL bDoubleClick, int x, int y, UINT keyFlags);
	static void OnRButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	static void OnRButtonDown(HWND hWnd, BOOL bDoubleClick, int x, int y, UINT keyFlags);
	static void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);	

protected:

	LPDIRECTINPUT8       	m_pDirectInput;
	LPDIRECTINPUTDEVICE8 	m_pJoystick;
	HWND					m_hWnd;
};

#endif