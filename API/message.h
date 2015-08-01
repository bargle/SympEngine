#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <vector>

#define CREATE_MESSAGE(x) CMessage *x = new CMessage;
#define CREATE_MESSAGE_RESERVE(x, n) CMessage *x = new CMessage(n);
#define RELEASE_MESSAGE(x) delete x;

class CMessage
{
public:
	CMessage():m_nPointer(0){}
	CMessage(int nReserve):m_nPointer(0){ m_Data.reserve(nReserve); }
	~CMessage(){ m_Data.clear(); }

	void SeekTo(int nSeek)
	{
		m_nPointer = nSeek;
	}

	void PushByte(uint8 nByte)
	{
		m_Data.push_back(nByte);
	}
	
	uint8 PopByte()
	{
		uint8 nByte = m_Data[m_nPointer]; 
		++m_nPointer;
		return nByte;
	}

	uint8 PeekByte()
	{
		return m_Data[m_nPointer];
	}

protected:
	typedef std::vector<uint8> MessageData;
	MessageData m_Data;

	uint32 m_nPointer;
};

#endif