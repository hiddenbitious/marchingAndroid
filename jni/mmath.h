/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#ifndef __MATH_H__
#define __MATH_H__

#include "globals.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795f
#define DEGREES_TO_RADIANS 0.0174532925199432957692369076848861f
#define RADIANS_TO_DEGREES 57.2957795130823208767981548141052f

#define MAX(a,b) (( a > b ) ? a : b)
#define MIN(a,b) (( a < b ) ? a : b)

#define EPSILON 1.0e-4f
#define FLOAT_EQ(x,v) (((v - EPSILON)<=x) && (x <=( v + EPSILON)))

/// Numeric limts
#include <float.h>
//#	define FLT_MAX numeric_limits<float>::max()
//#	define FLT_MIN numeric_limits<float>::min()
//#	define INT_MAX numeric_limits<int>::min()
//#	define INT_MIN numeric_limits<int>::min()
//#	define UINT_MAX numeric_limits<unsigned int>::min()
//#	define UINT_MIN numeric_limits<unsigned int>::min()
#define SMALLEST_FLOAT FLT_MIN
#define GREATEST_FLOAT FLT_MAX

/// limits a value to low and high
#define LIMIT_RANGE(low, value, high)	{ if (value < low) value = low; else if(value > high) value = high;	}
#define UPPER_LIMIT (value , limit )	{ if ( value > limit ) value = limit; }
#define ABS(value)						((value) < 0.0f ? -1.0f * (value) : (value))

namespace math {
void Normalize(float* x , float* y , float* z);
float Magnitude(float x , float y , float z);
float PointToPointDistance(C_Vertex *p1 , C_Vertex *p2);
}

/// Code ripped from "OpenGL ES 2.0 Programming Guide"
void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ);
void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB);
void esMatrixLoadIdentity(ESMatrix *result);



#endif
