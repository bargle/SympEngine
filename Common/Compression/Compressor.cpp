#include "stdafx.h"
#include "Compressor.h"
#include "zlib.h"

CCompressor::CCompressor():
m_nBufferSize(0),
m_Buffer(NULL),
m_nReadBufferSize(CHUNK_SIZE),
m_ReadBuffer(NULL)
{

}

CCompressor::~CCompressor()
{
	if(m_Buffer != NULL)
	{
		delete [] m_Buffer;
	}

	if(m_ReadBuffer != NULL)
	{
		delete [] m_ReadBuffer;
	}
}

bool CCompressor::CompressFile(const char* input_name, const char* ouput_name)
{

	if(input_name == NULL)
		return false;

	CChunkFileHeader header;
	memset(&header, 0, sizeof(CChunkFileHeader));
	header.version = CHUNK_FORMAT_VERSION;

	FILE *fp_in = fopen(input_name, "rb");
	FILE *fp_out = fopen(ouput_name, "wb");

	if(!fp_in || !fp_out)
	{
		if(fp_in)
			fclose(fp_in);

		if(fp_out)
			fclose(fp_out);

		return false;
	}

	//ok keep going
	//calc buffer size we need for compression
	int nNewBufferSize = CHUNK_SIZE + (int)(((float)CHUNK_SIZE * 0.1f) + 12.0f);

	if(m_nBufferSize < nNewBufferSize)
	{
		m_nBufferSize = nNewBufferSize;

		if(m_Buffer != NULL)
		{
			delete [] m_Buffer;
			m_Buffer = NULL;
		}
	}

	if(m_Buffer == NULL)
	{
		m_Buffer = new char[m_nBufferSize];
		if(m_Buffer == NULL)
		{
			return false;
		}
	}

	if(m_ReadBuffer == NULL)
	{
		m_ReadBuffer = new char[m_nReadBufferSize];
		if(m_ReadBuffer == NULL)
		{
			return false;
		}
	}

	//we have our buffer, now write out the header
	fwrite(&header, sizeof(CChunkFileHeader), 1, fp_out);

	bool bKeepGoing = true;
	while(bKeepGoing)
	{
		int nCount = (int)fread(m_ReadBuffer, 1, CHUNK_SIZE, fp_in);
		if(nCount > 0)
		{
			uLongf unDestLen = m_nBufferSize;
			uLong unSrcLen = nCount;

			int nResult = compress((Bytef*)m_Buffer, &unDestLen, (Bytef*)m_ReadBuffer, unSrcLen);
			if(nResult != Z_OK)
			{
				//error				
				break;
			}

			fwrite(&nCount, sizeof(unsigned int), 1, fp_out); //uncompressed
			fwrite(&unDestLen, sizeof(unsigned int), 1, fp_out); //compressed size
			fwrite(m_Buffer, unDestLen, 1, fp_out); //chunk
		}
		else
		{
			bKeepGoing = false;
			fclose(fp_in);
			fclose(fp_out);
		}
	}
	return true;
}

bool CCompressor::UncompressFile(const char* input_name, const char* ouput_name)
{
	FILE *fp_in = fopen(input_name, "rb");
	FILE *fp_out = fopen(ouput_name, "wb");

	if(!fp_in || !fp_out)
	{
		if(fp_in)
			fclose(fp_in);

		if(fp_out)
			fclose(fp_out);

		return false;
	}

	//set up buffers

	int nNewBufferSize = CHUNK_SIZE;

	if(m_nBufferSize < nNewBufferSize)
	{
		m_nBufferSize = nNewBufferSize;

		if(m_Buffer != NULL)
		{
			delete [] m_Buffer;
			m_Buffer = NULL;
		}
	}

	if(m_Buffer == NULL)
	{
		m_Buffer = new char[m_nBufferSize];
		if(m_Buffer == NULL)
		{
			return false;
		}
	}

	if(m_ReadBuffer == NULL)
	{
		m_ReadBuffer = new char[m_nReadBufferSize];
		if(m_ReadBuffer == NULL)
		{
			return false;
		}
	}

	//read our file header
	CChunkFileHeader header;
	memset(&header, 0, sizeof(CChunkFileHeader));
	if(fread(&header, sizeof(CChunkFileHeader), 1, fp_in) <= 0)
	{
		return false;
	}

	//now read until we're all done
	bool bKeepGoing = true;
	while(bKeepGoing)
	{
		int nUncompressedSize = 0;
		int nCompressedSize = 0;

		//read uncompressed size
		if(fread(&nUncompressedSize, sizeof(int), 1, fp_in) <= 0)
		{
			//we're done here
			break;
		}
		
		//read compressed size
		fread(&nCompressedSize, sizeof(int), 1, fp_in);

		//now read the chunk
		fread(m_ReadBuffer, 1, nCompressedSize, fp_in);

		//uncompress it
		uLongf nDestLen = nUncompressedSize;
		uLong nSrcLen = nCompressedSize;
		int nResult = uncompress((Bytef*)m_Buffer, &nDestLen, (Bytef*)m_ReadBuffer, nSrcLen);
		if(nResult != Z_OK)
		{
			break;
		}

		fwrite(m_Buffer, 1, nDestLen, fp_out);
	}

	fclose(fp_in);
	fclose(fp_out);

	return true;
}

bool CCompressor::CompressBuffer(char *output_buffer, int &output_buffer_size, char* input_buffer, int input_buffer_size)
{
	//uncompress a chunk
	uLongf nDestLen = output_buffer_size;
	uLong nSrcLen = input_buffer_size;
	int nResult = compress((Bytef*)output_buffer, &nDestLen, (Bytef*)input_buffer, nSrcLen);

	if(nResult != Z_OK)
	{
		output_buffer_size = 0;
		return false;
	}

	output_buffer_size = nDestLen;
	return true;
}

bool CCompressor::UncompressBuffer(char *output_buffer, int &output_buffer_size, char* input_buffer, int input_buffer_size)
{
	int nInputBufferPointer = 0;
	int nOutputBufferPointer = 0;

	//read our file header
	CChunkFileHeader header;
	memset(&header, 0, sizeof(CChunkFileHeader));

	memcpy(&header, &input_buffer[nInputBufferPointer], sizeof(CChunkFileHeader));
	nInputBufferPointer += sizeof(CChunkFileHeader);

	bool bKeepGoing = true;
	while(bKeepGoing)
	{
		int nUncompressedSize = 0;
		int nCompressedSize = 0;

		if(nInputBufferPointer >= input_buffer_size)
		{
			break;
		}

		//read uncompressed size
		memcpy(&nUncompressedSize, &input_buffer[nInputBufferPointer], sizeof(int));
		nInputBufferPointer += sizeof(int);

		//read compressed size
		memcpy(&nCompressedSize, &input_buffer[nInputBufferPointer], sizeof(int));
		nInputBufferPointer += sizeof(int);

		//uncompress a chunk
		uLongf nDestLen = nUncompressedSize;
		uLong nSrcLen = nCompressedSize;
		int nResult = uncompress((Bytef*)&output_buffer[nOutputBufferPointer], &nDestLen, (Bytef*)&input_buffer[nInputBufferPointer], nSrcLen);
			
		if(nResult != Z_OK)
		{
			break;
		}

		nInputBufferPointer += nCompressedSize;
		nOutputBufferPointer += nUncompressedSize;

		if(nOutputBufferPointer >= output_buffer_size)
		{
			//we're about to go out of bounds, or already have...
			break;
		}
	}

	output_buffer_size = nOutputBufferPointer;

	return true;
}
