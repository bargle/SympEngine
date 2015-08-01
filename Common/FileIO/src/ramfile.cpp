#include "stdafx.h"
#include "ramfile.h"

RamFile::RamFile():
m_nFilePointer(0),
m_nFileSize(0),
m_FileData(NULL)
{
}

RamFile::~RamFile()
{
}

GEN_RESULT RamFile::Open(uint8* pData, int nSize)
{
	m_FileData = pData;
	m_nFileSize = nSize;
	m_nFilePointer = 0;

	return RESULT_OK;
}

GEN_RESULT RamFile::Read(void* pBuffer, int nBytes)
{
	if((m_nFilePointer + nBytes) > m_nFileSize)
	{
		return RESULT_ERROR;
	}

	memcpy(pBuffer, &m_FileData[m_nFilePointer], nBytes);

	m_nFilePointer += nBytes;

	return RESULT_OK;
}


GEN_RESULT RamFile::SeekTo(long nOffset)
{
	m_nFilePointer = nOffset;

	return RESULT_OK;
}


