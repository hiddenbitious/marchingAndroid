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

#ifndef _BBOX_H_
#define _BBOX_H_

#include "bvolumes.h"

class C_BBox : C_BVolumes {
	private:

		C_Vector3 min;
		C_Vector3 max;

		C_Vector3 vertices[8];

		float MaxOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8);
		float MinOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8);

	public:

		void SetMin(const float x , const float y , const float z);
		void SetMin(const C_Vector3* vec);

		void SetMax(const float x , const float y , const float z);
		void SetMax(const C_Vector3* vec);

		void GetMax(C_Vector3* vec);
		void GetMin(C_Vector3* vec);

		void GetMax(C_Vertex* vec);
		void GetMin(C_Vertex* vec);

		void GetMax(float* x , float* y , float* z);
		void GetMin(float* x , float* y , float* z);

		void SetVertices(void);
		void GetVertices(C_Vector3 *verts) const;
		void GetVertices(C_Vertex *verts) const;

		void Draw(void);
		void Draw(float r , float g , float b);

		void Translate(const float x , const float y , const float z);
		void Translate(const C_Vector3* vec);

		void Rotate(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint);


		bool IsInside(const C_Vertex* point);

};

#endif
