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

#ifndef _CFRUSTUM_H_
#define _CFRUSTUM_H_

#include "plane.h"
#include "bsphere.h"
#include "bbox.h"
#include "vectors.h"

class C_Frustum {
	private:
		C_Plane* front;
		C_Plane* back;
		C_Plane* left;
		C_Plane* right;
		C_Plane* top;
		C_Plane* bottom;

		C_Plane* frustumPlanes[6];

	public:

		C_Frustum(void);
		~C_Frustum(void);

		void calculateFrustum(void);

		bool pointInFrustum(const float x , const float y , const float z) const;
		bool pointInFrustum(const C_Vector3* point) const;

		bool sphereInFrustum(C_Vector3* point , const float radius) const;
		bool sphereInFrustum(C_BSphere* sphere) const;
		//Can tell if the SPHERE INTERSECTS with the frustum
		int sphereInFrustum2(C_BSphere* sphere) const;

		bool cubeInFrustum(const float x , const float y , const float z , const float size) const;
		bool cubeInFrustum(const C_BBox* box) const;
		//Can tell if the CUBE/BOX INTERSECTS with the frustum
		int cubeInFrustum2(const C_BBox* box) const;
};

#endif
