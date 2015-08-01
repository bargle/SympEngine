//------------------------------------------------------------------------------//
//
// MODULE   : Randomizer.h
// 
// PURPOSE  : Randomizer - Definition
// 
// CREATED  : 12/22/2004
// 
//
// Code based on the Mersenne Twister Algorithm
// by: Makoto Matsumoto and Takuji Nishimura (alphabetical order) in 1996/1997
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/eindex.html
// 
// This particular implementation was based on code from:
// http://www.mcshaffry.com/GameCode/
//
// If you don't own a copy of "Game Coding Complete" by Mike McShaffry, 
// then go buy it!
//
//
//------------------------------------------------------------------------------//


#ifndef _RANDOMIZER_H_
#define _RANDOMIZER_H_

/* Period parameters */  
#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df   /* constant vector a */
#define CMATH_UPPER_MASK 0x80000000 /* most significant w-r bits */
#define CMATH_LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define CMATH_TEMPERING_MASK_B 0x9d2c5680
#define CMATH_TEMPERING_MASK_C 0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)  (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)  (y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)  (y << 15)
#define CMATH_TEMPERING_SHIFT_L(y)  (y >> 18)

class Randomizer
{
public:
	Randomizer();
	~Randomizer();

	void Init(unsigned int nSeed = 0);
	unsigned int GetRandom(int nLimit); //it returns a number between 0 and (limit - 1)
	void SetRandomSeed(unsigned int nSeed);

protected:

	unsigned int m_nSeed;

	unsigned long mt[CMATH_N]; /* the array for the state vector  */
	int mti; /* mti==N+1 means mt[N] is not initialized */
};

#endif