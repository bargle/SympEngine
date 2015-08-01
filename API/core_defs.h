#ifndef _CORE_DEFS_H_
#define _CORE_DEFS_H_

/*
#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
*/

// C-runtime includes
#include <assert.h>
#include <stdlib.h>

//Result codes //
#include "resultcodes.h"

// Data Types
#include "core_types.h"

//This should be moved the the math lib.
namespace {

float Lerp( float a, float b, float percentage ) {
	return ( a + ((b - a) * percentage) );
}

float Min( float a, float b ) {
	if ( a > b ) return b;
	
	return a;
}

float Max( float a, float b ) {
	if ( a > b ) return a;
	
	return b;
}

float Clamp( float min, float max, float value){
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

}

#endif
