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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "globals.h"
#include "vectors.h"
#include "quaternion.h"
#include "frustum.h"

class C_Camera {
	private:
//		float rotMatrix[16];
		ESMatrix ESrotMatrix;

		//Where camera is looking at
		C_Vector3 lookAt;
		//Up vector
		C_Vector3 up;

		//Holds the camera's rotation.
		C_Quaternion rotationQuaternion;
		C_Vector3 xVec;

		//Set true when the frustun needs to be updated
		bool updateFrustum;

		bool rotation , translation;

		int w , h;

	public:
		C_Frustum* frustum;

		//Camera coordinates
		C_Vector3 position;

		C_Camera(void);

		float zNear , zFar , fov;

		void Look(void);
		void setProjection(int w , int h);

		void Rotate(const float x , const float y);
		void Move(const float speed);
		void Move(const float x , const float y , const float z);
		void MoveUp(const float speed);
		void MoveDown(const float speed);
		void StrafeLeft(const float speed);
		void StrafeRight(const float speed);

		void PrintText(int x , int y , float r , float g , float b , float a , const char *text  , ...);

		inline C_Vector3 GetPosition(void) const { return position; }

};

#endif
