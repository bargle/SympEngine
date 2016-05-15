#ifndef _INPUT_INTERFACE_H_
#define _INPUT_INTERFACE_H_

#define MAX_BUTTONS 6
struct InputState
{
	int X;
	int Y;
	unsigned char Button[MAX_BUTTONS];
};

class IInputInterface
{
public:
	IInputInterface();
	~IInputInterface();

	bool Init(int hWnd);
	bool Term();

	bool Update(InputState& state);
    
    void GrabMouse( bool bGrab ){}
    

protected:
};

#endif