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

#define JNI_COMPATIBLE
//#define ENABLE_LOGGING
//#define ENABLE_TIMING

#ifdef JNI_COMPATIBLE
#	include <jni.h>
#	include <android/log.h>
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>

#	ifdef ENABLE_LOGGING
#		define 	LOG_TAG		"marchingJNI"
#		define  LOGI(...)	__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#		define  LOGE(...)	__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#	else
#		define  LOGI(...)
#		define  LOGE(...)
#	endif
#else
#	define  LOGI 		printf
#	define  LOGE		printf

#	include <stdint.h>
#	include <string>
#	include <limits>
#	include <assert.h>
#	include <GL/glew.h>
#endif

#define FUN_ENTRY	LOGI("%s:%s\n", __FILE__, __FUNCTION__);

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

#endif
