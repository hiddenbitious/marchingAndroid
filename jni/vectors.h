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

#ifndef _VECTORS_H_
#define _VECTORS_H_

#include "globals.h"

#include "quaternion.h"

class vector2 {
	public:
		float x , y;

		//Default constructor. Sets both values to 0;
		vector2(void);
		//Constructor
		vector2(float x , float y);

		//Sets vector's values
		void setVector(float x , float y);
		//Sets vector's values
		void setVector(vector2 vec);

		void translate(float x , float y);
		void translate(const vector2* vec);
		void translate(vector2 vec);

		static float Magnitude(const float x , const float y);
};

class C_Vector3 {
	public:
		float x , y , z;

		// Default constructor. Sets all values to 0;
		C_Vector3(void);
		// Constructor
		C_Vector3(float x , float y , float z);
		C_Vector3(C_Vertex* point);

		bool operator < (C_Vector3* vec) const;
		bool operator > (C_Vector3* vec) const;

		C_Vector3 operator + (C_Vector3);
		C_Vector3& operator += (C_Vector3);
		C_Vector3& operator += (const float);

		C_Vector3 operator - (C_Vector3);
		C_Vector3& operator -= (C_Vector3);
		C_Vector3& operator -= (const float);
		C_Vector3 operator - (void);

		// Will do x1*x2 y1*y2 z1*z2
		C_Vector3 operator * (C_Vector3);
		C_Vector3 operator * (float);

		// Sets vector's values
		void SetVector(const float x , const float y , const float z);
		// Sets vector's values
		void SetVector(const C_Vector3 vec);

		void Translate(const float x , const float y , const float z);
		void Translate(const C_Vector3* vec);
		void Translate(const C_Vector3 vec);

		// Rotates vector
		// This function is not needed. Use a quaternion to rotate a point instead
		void Rotate(float anglex , float angley , float anglez);

		// Rotates vector
		// This function is not needed. Use a quaternion to rotate a point instead
		void Rotate(C_Quaternion quat);

		// Returns magnitude of this vector
		float Magnitude(void);
		// Returns magnitude of vector
		static float Magnitude(const C_Vector3 vec);
		// Returns magnitude of vector
		static float Magnitude(const float x , const float y , const float z);
		// Returns distance between two points
		static float Magnitude(const C_Vector3 vec1 , const C_Vector3 vec2);
		// Return distance between two points
		static float Magnitude(float x1 , float y1 , float z1 , float x2 , float y2 , float z2);
		// Calculates the vector that 2 points form
		static C_Vector3 CalcVector(const C_Vector3* vec1 , const C_Vector3* vec2);
		// Calculates the cross product
		static C_Vector3 CrossProduct(const C_Vector3* vec1 , const C_Vector3* vec2 , const C_Vector3* vec3);
		static C_Vector3 CrossProduct(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3);
		static C_Vector3 CrossProduct2(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3);
		// Calculate the dot product
		static float  DotProduct(const C_Vector3* vec1 , const C_Vector3* vec2);
		static float DotProduct(const C_Vertex* v1 , const C_Vertex* v2);

		// Normalizes the vector
		void Normalize(void);
		// Does exaclty the same job with math::normalize
		static void Normalize(float* x , float* y , float* z);

		// Returns a vector with all it's members se to zero
		static C_Vector3 Zero(void);
};

#endif
