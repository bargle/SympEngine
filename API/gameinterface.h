#ifndef _IGAME_INTERFACE_H_
#define _IGAME_INTERFACE_H_

extern int InitializeGameInterface();
extern int ResizeGameWindow(int width, int height);
extern int UpdateGameInterface();
extern void MouseMove( int x, int y );
extern void MouseClick( int button, int state, int x, int y );
extern void KeyDown( int key );
extern void KeyUp( int key );

enum EMouseButton {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE
};

class IGameInterface {
public:
	virtual int OnStartup(SympSystem* pSystem) = 0;
	virtual void OnShutdown() = 0;
	virtual void OnUpdate(float elapsed) = 0;
	virtual void OnRender(float elapsed) = 0;
	virtual void OnResize(int nWidth, int nHeight) = 0;
	virtual void OnInputEvent()  = 0;
	virtual void OnKeyEvent(unsigned char key, int state, int x, int y) = 0;
	virtual void OnMouseClick(EMouseButton button, int state, int x, int y) = 0;
	virtual void OnMouseMove(int x, int y) = 0;

};

#endif

