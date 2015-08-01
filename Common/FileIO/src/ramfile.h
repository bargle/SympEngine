#ifndef _RAM_FILE_H_
#define _RAM_FILE_H_

#include "basefile.h"
#include <stdio.h>
#include <vector>

class RamFile : public BaseFile
{
public:
	RamFile();
	virtual ~RamFile();

	virtual GEN_RESULT Open(const char* szFilename, EFileFlags eFlags){ return RESULT_ERROR; }
	virtual GEN_RESULT Open(uint8* pData, int nSize);
	virtual void Close(){ delete this; }
	virtual GEN_RESULT Read(void* pBuffer, int nBytes);
	virtual GEN_RESULT Write(void* pBuffer, int nBytes){ return RESULT_ERROR; }
	virtual GEN_RESULT SeekTo(long nOffset);
	virtual int GetSize(){ return m_nFileSize; }

protected:

	int		m_nFilePointer;
	int		m_nFileSize;
	uint8*		m_FileData;

};

#endif
