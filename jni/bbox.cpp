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

#include "bbox.h"

void C_BBox::SetMax(const float x , const float y , const float z)
{
	max.x = x;
	max.y = y;
	max.z = z;
}

void C_BBox::SetMax(const C_Vector3 *vec)
{
	max.SetVector(*vec);
}

void C_BBox::SetMin(const float x , const float y , const float z)
{
	min.x = x;
	min.y = y;
	min.z = z;
}

void C_BBox::SetMin(const C_Vector3 *vec)
{
	min.SetVector(*vec);
}

void C_BBox::SetVertices(void)
{
	vertices[0].SetVector(max.x, min.y, min.z);
	vertices[1].SetVector(max.x, min.y, max.z);
	vertices[2].SetVector(min.x, min.y, max.z);
	vertices[3].SetVector(min.x, min.y, min.z);

	vertices[4].SetVector(max.x, max.y, min.z);
	vertices[5].SetVector(max.x, max.y, max.z);
	vertices[6].SetVector(min.x, max.y, max.z);
	vertices[7].SetVector(min.x, max.y, min.z);
}

void C_BBox::GetVertices(C_Vector3* _vertices) const
{
	if(_vertices == NULL) { return; }

	_vertices[0].SetVector(max.x, min.y, min.z);
	_vertices[1].SetVector(max.x, min.y, max.z);
	_vertices[2].SetVector(min.x, min.y, max.z);
	_vertices[3].SetVector(min.x, min.y, min.z);

	_vertices[4].SetVector(max.x, max.y, min.z);
	_vertices[5].SetVector(max.x, max.y, max.z);
	_vertices[6].SetVector(min.x, max.y, max.z);
	_vertices[7].SetVector(min.x, max.y, min.z);
}

void C_BBox::GetVertices(C_Vertex* _vertices) const
{
	if(_vertices == NULL) { return; }

	_vertices[0].x = max.x;		_vertices[0].y = min.y;		_vertices[0].z = min.z;
	_vertices[1].x = max.x;		_vertices[1].y = min.y;		_vertices[1].z = max.z;
	_vertices[2].x = min.x;		_vertices[2].y = min.y;		_vertices[2].z = max.z;
	_vertices[3].x = min.x;		_vertices[3].y = min.y;		_vertices[3].z = min.z;

	_vertices[4].x = max.x;		_vertices[4].y = max.y;		_vertices[4].z = min.z;
	_vertices[5].x = max.x;		_vertices[5].y = max.y;		_vertices[5].z = max.z;
	_vertices[6].x = min.x;		_vertices[6].y = max.y;		_vertices[6].z = max.z;
	_vertices[7].x = min.x;		_vertices[7].y = max.y;		_vertices[7].z = min.z;
}

void C_BBox::GetMax(C_Vector3* vec)
{
	vec->SetVector(max);
}

void C_BBox::GetMin(C_Vector3* vec)
{
	vec->SetVector(min);
}

void C_BBox::GetMax(C_Vertex* vec)
{
	vec->x = max.x;
	vec->y = max.y;
	vec->z = max.z;
}

void C_BBox::GetMin(C_Vertex* vec)
{
	vec->x = min.x;
	vec->y = min.y;
	vec->z = min.z;
}

void C_BBox::GetMax(float* x , float* y , float* z)
{
	*x = max.x;
	*y = max.y;
	*z = max.z;
}

void C_BBox::GetMin(float* x , float* y , float* z)
{
	*x = min.x;
	*y = min.y;
	*z = min.z;
}


void C_BBox::Draw(void)
{
	/// Not implemended for gles 2
}

void C_BBox::Draw(float r , float g , float b)
{
	/// Not implemended for gles 2
}

void C_BBox::Translate(const float x, const float y, const float z)
{
	min.Translate(x , y , z);
	max.Translate(x , y , z);

	vertices[0].Translate(x , y , z);
	vertices[1].Translate(x , y , z);
	vertices[2].Translate(x , y , z);
	vertices[3].Translate(x , y , z);
	vertices[4].Translate(x , y , z);
	vertices[5].Translate(x , y , z);
	vertices[6].Translate(x , y , z);
	vertices[7].Translate(x , y , z);
}

void C_BBox::Translate(const C_Vector3 *vec)
{
	min.Translate(vec);
	max.Translate(vec);

	vertices[0].Translate(vec);
	vertices[1].Translate(vec);
	vertices[2].Translate(vec);
	vertices[3].Translate(vec);
	vertices[4].Translate(vec);
	vertices[5].Translate(vec);
	vertices[6].Translate(vec);
	vertices[7].Translate(vec);
}

void C_BBox::Rotate(const float anglex , const float angley , const float anglez , const C_Vector3 *rotPoint)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	tempQuat.RotatePoint(&vertices[0] , rotPoint);
	tempQuat.RotatePoint(&vertices[1] , rotPoint);
	tempQuat.RotatePoint(&vertices[2] , rotPoint);
	tempQuat.RotatePoint(&vertices[3] , rotPoint);
	tempQuat.RotatePoint(&vertices[4] , rotPoint);
	tempQuat.RotatePoint(&vertices[5] , rotPoint);
	tempQuat.RotatePoint(&vertices[6] , rotPoint);
	tempQuat.RotatePoint(&vertices[7] , rotPoint);

	max.x = MaxOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	max.y = MaxOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	max.z = MaxOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);

	min.x = MinOfEight(vertices[0].x , vertices[1].x , vertices[2].x , vertices[3].x , vertices[4].x , vertices[5].x , vertices[6].x , vertices[7].x);
	min.y = MinOfEight(vertices[0].y , vertices[1].y , vertices[2].y , vertices[3].y , vertices[4].y , vertices[5].y , vertices[6].y , vertices[7].y);
	min.z = MinOfEight(vertices[0].z , vertices[1].z , vertices[2].z , vertices[3].z , vertices[4].z , vertices[5].z , vertices[6].z , vertices[7].z);
}

float C_BBox::MaxOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8)
{
	float max = n1;

	if(n2 > max) { max = n2; }
	if(n3 > max) { max = n3; }
	if(n4 > max) { max = n4; }
	if(n5 > max) { max = n5; }
	if(n6 > max) { max = n6; }
	if(n7 > max) { max = n7; }
	if(n8 > max) { max = n8; }

	return max;
}

float C_BBox::MinOfEight(float n1 , float n2 , float n3 , float n4 , float n5 , float n6 , float n7 , float n8)
{
	float min = n1;

	if(n2 < min) { min = n2; }
	if(n3 < min) { min = n3; }
	if(n4 < min) { min = n4; }
	if(n5 < min) { min = n5; }
	if(n6 < min) { min = n6; }
	if(n7 < min) { min = n7; }
	if(n8 < min) { min = n8; }

	return min;
}

bool C_BBox::IsInside(const C_Vertex* p)
{
//	if ( p->x >= min.x && p->x <= max.x && p->y >= min.y && p->y <= max.y && p->z >= min.z && p->z <= max.z )
//		 return true;
	if(p->x < min.x || p->x > max.x) {
		return false;
	}
	if(p->y < min.y || p->y > max.y) {
		return false;
	}
	if(p->z < min.z || p->z > max.z) {
		return false;
	}

	return true;
}
