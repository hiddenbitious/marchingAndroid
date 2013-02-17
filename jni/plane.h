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

#ifndef _CPLANE_H_
#define _CPLANE_H_

#include "mmath.h"
#include "vectors.h"
//#include <vector>

using namespace std;

class C_Plane {
	public:

		float a , b , c , d;

		C_Plane(float a , float b , float c , float d);
		C_Plane(const C_Vector3* vec1 , const C_Vector3* vec2 , const C_Vector3* vec3);
		C_Plane(const C_Vertex* vec1 , const C_Vertex* vec2 , const C_Vertex* vec3);
		C_Plane(void);

		void normalizePlane(void);
		void setPlane(float , float , float , float);
		void setPlane(C_Plane* plane);

		void getPlaneNormal(float* x , float* y , float* z);
		C_Vertex getPlaneNormal(void);

		float GetPlaneConstant(void);

		C_Vertex pointOnPlane;

//		vector<C_Vertex> points;
		C_Vertex points[3];

		void Draw(void);

		// These two functions works for normalized plane equations (so that a*a + b*b + c*c == 1)
		// Dot product + d
		inline float distanceFromPoint(const C_Vector3* point)
		{ return (a * point->x + b * point->y + c * point->z + d); }

		inline float distanceFromPoint(const C_Vertex* point)
		{ return (a * point->x + b * point->y + c * point->z + d); }

};

#endif
