#ifndef _INPUT_LISTENER_H_
#define _INPUT_LISTENER_H_

class IInputListener
{
public:
	virtual void OnLButtonUp(int x, int y, uint32 keyFlags)						= 0;
	virtual void OnLButtonDown(bool bDoubleClick, int x, int y, uint32 keyFlags) 	= 0;
	virtual void OnRButtonUp(int x, int y, uint32 keyFlags)						= 0;
	virtual void OnRButtonDown(bool bDoubleClick, int x, int y, uint32 keyFlags) 	= 0;
	virtual void OnMouseMove(int x, int y, uint32 keyFlags)						= 0;
};

#endif