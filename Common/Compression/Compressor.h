#ifndef _CCOMPRESSOR_H_
#define _CCOMPRESSOR_H_

#define CHUNK_FORMAT_VERSION 1
#define CHUNK_SIZE 64*1024

typedef struct t_CChunkFileHeader
{
	//char filename[256];
	unsigned int version;
	unsigned int reserved; //save 32 bits for later
}CChunkFileHeader;

class CCompressor
{
public:
	CCompressor();
	~CCompressor();

	bool CompressFile(const char* input_name, const char* ouput_name);
	bool UncompressFile(const char* input_name, const char* ouput_name);

	bool CompressBuffer(char *output_buffer, int &output_buffer_size, char* input_buffer, int input_buffer_size);
	bool UncompressBuffer(char *output_buffer, int &output_buffer_size, char* input_buffer, int input_buffer_size);

protected:

	int m_nBufferSize;
	char* m_Buffer;
	int m_nReadBufferSize;
	char* m_ReadBuffer;

};

#endif
