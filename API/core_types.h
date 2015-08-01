#ifndef _CORE_TYPES_H_
#define _CORE_TYPES_H_

#include <math.h>
#include <stdio.h>

typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef signed long		int32;
typedef signed short	int16;
typedef signed char		int8;

#define DEG_TO_RAD(x) (x * 0.0174532925)

//FIXME: get rid of the operator overloads.. They are the devil. (well, maybe keep some overloads for compares)

class Vector2d
{
public:
	Vector2d(){}
	Vector2d(float _x, float _y){x = _x; y = _y;}
	Vector2d(int _x, int _y){x = (float)_x; y = (float)_y;}
	void Set(float _x, float _y){x = _x; y = _y;}
	void Normalize()
	{
		float f = sqrtf( (x*x) + (y*y) );
		x /= f;
		y /= f;
	}
	Vector2d& operator*=(float f)
	{
		x *= f;
		y *= f;
		return *this;
	}

	Vector2d& operator=(float f)
	{
		x = f;
		y = f;
		return *this;
	}

	Vector2d& operator=(const Vector2d& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	Vector2d operator-(Vector2d& vVec)
	{
		Vector2d op;
		op.x = x - vVec.x;
		op.y = y - vVec.y;
		return op;
	}

	Vector2d& operator+=(Vector2d& vVec)
	{
		x += vVec.x;
		y += vVec.y;
		return *this;
	}

	inline float Distance( Vector2d& vVec )
	{		
		//BAD
		//return sqrtf(DistanceSqr(*this - vVec)); //this actually modifies the data...
		
		//This is also terrible.. 
		Vector2d temp( vVec.x - x, vVec.y - y );
		return sqrtf(DistanceSqr( temp ));
	}

	inline float DistanceSqr( Vector2d& vVec )
	{
		return (vVec.x * vVec.x) + (vVec.y * vVec.y);
	}

	bool operator==(Vector2d& rhs)
	{
		if( (fabsf(rhs.x - x) < 0.001f) && (fabsf(rhs.y - y) < 0.001f) )
		{
			return true;
		}

		return false;
	}

	bool CloseTo(Vector2d& rhs, float fLimit = 0.5f)
	{
		if( (fabsf(rhs.x - x) < fLimit) && (fabsf(rhs.y - y) < fLimit) )
		{
			return true;
		}

		return false;
	}

	float x,y;
};

class Vector3d
{
public:
	Vector3d():x(0.0f), y(0.0f), z(0.0f){}
	Vector3d(float _x, float _y, float _z){x = _x; y = _y; z = _z;}
	Vector3d(int _x, int _y, int _z){x = (float)_x; y = (float)_y; z = (float)_z;}
	void Set(float _x, float _y, float _z){x = _x; y = _y; z = _z;}
	void Set(Vector3d& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	void Normalize()
	{
		float f = sqrtf( (x*x) + (y*y) + (z*z) );
		x /= f;
		y /= f;
		z /= f;
	}
	
	void Transform(float** matrix)
	{
		x = (x * matrix[0][0]) + (y * matrix[1][0]) + (z * matrix[2][0]) + matrix[3][0];
		y = (x * matrix[0][1]) + (y * matrix[1][1]) + (z * matrix[2][1]) + matrix[3][1];
		z = (x * matrix[0][2]) + (y * matrix[1][2]) + (z * matrix[2][2]) + matrix[3][2];
	}
	
	float Length() {
		return sqrt( (x*x) + (y*y) + (z*z) );
	}
	
	float LengthSqr() {
		return ((x*x) + (y*y) + (z*z));
	}
	
	float Dot( Vector3d &vec ) {
		return (x*vec.x) + (y*vec.y) + (z*vec.z);
	}

	bool operator!=( const Vector3d& rhs) const {
		if( (fabsf(rhs.x - x) < 0.001f) && (fabsf(rhs.y - y) < 0.001f) && (fabsf(rhs.z - z) < 0.001f) )
		{
			return false;
		}

		return true;
	}

	Vector3d& operator=(const Vector3d& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	void Print()
	{
		printf("<%f %f %f>", x, y, z);
	}

	void Println()
	{
		printf("<%f %f %f>\n", x, y, z);
	}

	float x, y, z;
};

class Vector4d {
public:
	Vector4d():x(0.0f), y(0.0f), z(0.0f), w(0.0f){}
	Vector4d(float _x, float _y, float _z, float _w){ x = _x; y = _y; z = _z; w = _w; }
	Vector4d(int _x, int _y, int _z, int _w){x = (float)_x; y = (float)_y; z = (float)_z; w = (float)_w; }
	void Set(float _x, float _y, float _z, float _w){x = _x; y = _y; z = _z; w = _w;}
	void Set(Vector4d& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
	}

	void Normalize() {
		float f = sqrtf( (x*x) + (y*y) + (z*z) + (w*w) );
		x /= f;
		y /= f;
		z /= f;
		w /= f;
	}

	float x;
	float y;
	float z;
	float w;
};

class ColorRGBA
{
public:
	ColorRGBA(){}
	ColorRGBA(float _r, float _g, float _b, float _a){ r = _r; g = _g; b = _b; a = _a; }
	ColorRGBA(int _r, int _g, int _b, int _a){r = (float)_r; g = (float)_g; b = (float)_b; a = (float)_a;}
	void Set(float _r, float _g, float _b, float _a){ r = _r; g = _g; b = _b; a = _a; }
	float r, g, b, a;
	
	void NormalizeRGB() {
		float f = sqrtf( (r*r) + (g*g) + (b*b) );
		r /= f;
		g /= f;
		b /= f;
	}
};

class Line2d
{
public:
	Line2d(){}
	Line2d(float _x1, float _y1, float _x2, float _y2){x1 = _x1; y1 = _y1; x2 = _x2; y2 = _y2;}

	float x1,y1, x2,y2;
};

class VectorInt2d
{
public:
	VectorInt2d(){}
	VectorInt2d(int _x, int _y){x = _x; y = _y;}
	int x,y;
};

class Rect2D
{
public:
	Rect2D(){}
	Rect2D(float _x, float _y, float _x2, float _y2){x = _x; y = _y; x2 = _x2; y2 = _y2;}
	float x,y,x2,y2;
};

class RectInt2D
{
public:
	RectInt2D(){}
	RectInt2D(int _x, int _y, int _x2, int _y2){x = _x; y = _y; x2 = _x2; y2 = _y2;}
	RectInt2D& operator=(const RectInt2D& rRect){ x=rRect.x; y=rRect.y; x2=rRect.x2; y2=rRect.y2; return *this;}
	int x,y,x2,y2;
};

#include "matrix.h"

template< class T, int SIZE >
class StaticArray {
public:
	StaticArray() {}
	~StaticArray() {}
	
	T& operator[]( const int idx ){
		if ( idx < 0 || idx >= SIZE ) {
			//TODO handle error case.
			return data[0];
		}
		return data[ idx ];
	}
	int GetCount(){
		return SIZE;
	}
	
private:
	T data[ SIZE ];
};

#endif
