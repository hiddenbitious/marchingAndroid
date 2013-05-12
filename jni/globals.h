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

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define MAX_THREADS			3

//#define ENABLE_LOGGING
//#define ENABLE_TIMING

#ifdef ENABLE_TIMING
#	ifndef ENABLE_LOGGING
#		define ENABLE_LOGGING
#	endif
#endif

#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

#ifdef ENABLE_LOGGING
#	define 	LOG_TAG		"marchingJNI"
#	define  LOGI(...)	__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#	define  LOGE(...)	__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#	define  LOGI(...)
#	define  LOGE(...)
#endif

//#define FUN_ENTRY	LOGI("%s:%s\n", __FILE__, __FUNCTION__);
#define FUN_ENTRY

typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned short USHORT;

//Very simple struct to hold vertex information
typedef struct {
	float x , y , z;
} C_Vertex;

//Very simple struct to hold normal information
typedef struct {
	float nx , ny , nz;
} C_Normal;

//Very simple struct to hold texture coordinate information
typedef struct {
	float u , v;
} C_TexCoord;

//Very simple struct to hold color information
typedef struct {
	float r , g , b , a;
} C_Color;

//Very simple struct to hold index information
typedef struct {
	UINT p0 , p1 , p2;
} C_TriIndices;

typedef struct {
    GLfloat   m[4][4];
} ESMatrix;

extern ESMatrix globalModelviewMatrix, globalProjectionMatrix;

class C_3DSReader;
class C_Camera;
class C_Quaternion;
class vector2;
class C_Vector3;

void DumpMatrix(ESMatrix *matrix);

extern const float gridCenter;

#endif
