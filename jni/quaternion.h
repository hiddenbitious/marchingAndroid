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

#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "globals.h"
#include "mmath.h"
#include "vectors.h"

#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}

class C_Quaternion {
	public:

		float x, y, z, a;

		C_Quaternion();
		C_Quaternion(C_Vector3 point);

		// (*this*) * quat
		void Mult(const C_Quaternion* quat);
		//q1*q2
		C_Quaternion* Mult(const C_Quaternion* q1 , const C_Quaternion* q2);
		//each member
		C_Quaternion* Mult(float x , float y , float z , float a);

		void Identity(void);

		void CreateXquaternion(float angle);
		void CreateYquaternion(float angle);
		void CreateZquaternion(float angle);

		void EulerToQuat(const float x, const float y, const float z);
		void AxisAngleToQuaternion(float x , float y , float z , float a);

		void QuaternionToAxisAngle(float& x , float& y , float& z , float& a);
		void QuaternionToMatrix9(float*);
		//Just the same as quaternionToMatrix16 ()...but this can go directly into glMultMatrixf ()
		void QuaternionToMatrix16(float*);
		void QuaternionToMatrix16(ESMatrix *);

		//Rotate the current quaternion
		void Rotate(float angleX , float angleY , float angleZ);
		void Rotate(float angleX , float angleY);

		//Rotates the given point using *this* quaternion as a rotation quaternion
		void RotatePoint(C_Vector3* point) const;
		void RotatePoint(C_Vector3* p1 , const C_Vector3* rotationPoint) const;
		void RotatePoint(C_Vector3* point , float rotX , float rotY , float rotZ) const;

		//normilize quaternion
		void Normalize(void);
		void Reverse(void);

		float Magnitude(void);
		float Magnitude(C_Quaternion*);

		void CopyQuaternion(C_Quaternion* quat) const;

		bool HoldsRotation(void);

		float GetQuaternionX(void);
		float GetQuaternionY(void);
		float GetQuaternionZ(void);
		float GetQuaternionAngle(void);

		void GetReverseQuat(C_Quaternion* quat);
		void SetQuaternion(const C_Quaternion* quat);
		void SetQuaternion(float angle , float x , float y , float z);

		void SetQuaternionAxis(float x , float y , float z);
		void SetQuaternionAxis(C_Vector3* vector);

		void SetQuaternionX(float x);
		void SetQuaternionY(float y);
		void SetQuaternionZ(float z);
		void SetQuaternionAngle(float angle);
};

#endif
