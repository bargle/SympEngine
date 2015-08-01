#ifndef _INPUT_INTERFACE_H_
#define _INPUT_INTERFACE_H_

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

	bool Init(int hWnd);
	bool Term();

	bool Update(InputState& state);

protected:
};

#endif