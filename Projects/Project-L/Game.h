#ifndef _GAME_H_
#define _GAME_H_

#include "gameinterface.h"
#include "VideoInterface3D.h"
class SympSystem;

enum movement_e {
	MOVE_NONE = 0,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_JUMP,
	MOVE_JET,
};

class MyGame : public IGameInterface{
public: 
	MyGame();
	~MyGame();

	virtual int OnStartup(SympSystem* pSystem);
	virtual void OnShutdown();
	virtual void OnUpdate(float elapsed);
	virtual void OnRender(float elapsed);
	virtual void OnResize(int nWidth, int nHeight);
	virtual void OnInputEvent();
	virtual void OnKeyEvent(unsigned char key, int state, int x, int y);
	virtual void OnMouseClick(EMouseButton button, int state, int x, int y);
	virtual void OnMouseMove(int x, int y);
	
	Prim_t* CreateBox( const char* tex, float scale );
	
protected:
	int			m_Yaw;
	int			m_Pitch;
	ViewPort	m_ViewPort;
	HFONT2D		m_hFont;
	float		m_Elapsed;
};

#endif
