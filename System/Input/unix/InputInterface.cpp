#include "stdafx.h"
#include "InputInterface.h"

IInputInterface::IInputInterface()
{

}

IInputInterface::~IInputInterface()
{
	Term();
}

bool IInputInterface::Init(int hWnd)
{
	return true;
}

bool IInputInterface::Term()
{
	return true;
}

bool IInputInterface::Update(InputState& state)
{
	return true;
}
	