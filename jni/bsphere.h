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

#ifndef _BSPHERE_H_
#define _BSPHERE_H_

#include "bvolumes.h"
#include "vectors.h"

class C_BSphere : C_BVolumes {
	public:

		C_BSphere();

		//Draws the bounding sphere for debugging reasons.
		void Draw(void);
		void Draw(const float r , const float g , const float b);

		void Translate(const float x , const float y , const float z);
		void Translate(const C_Vector3* vec);

		void Rotate(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint);
		void Rotate(const C_Quaternion* quat , const C_Vector3* rotPoint);

		//Sets the sphere's radius
		void SetRadius(float radius);

		//Set's the sphere's position
		void SetPosition(const C_Vector3 position);
		void SetPosition(const float x , const float y , const float z);

		inline C_Vector3 GetPosition(void) { return position; }
		inline float GetRadius(void) { return radius; }

	private:
		//Sphere's radius
		float radius;
		//Sphere's position/center
		C_Vector3 position;
};

#endif
