#ifndef _RESULT_CODES_H_
#define _RESULT_CODES_H_ 

enum EErrorCodes
{
	RESULT_OK = 0,
	RESULT_ERROR,
	RESULT_HANDLED,
	RESULT_FINISHED,

	RESULT_TRUE = 0,
	RESULT_FALSE = 1
};
typedef EErrorCodes GEN_RESULT;

#endif
