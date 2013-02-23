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

#include "plane.h"

C_Plane::C_Plane(float _a , float _b , float _c , float _d)
{
	a = _a;
	b = _b;
	c = _c;
	d = _d;
}


C_Plane::C_Plane(const C_Vector3* vec1 , const C_Vector3* vec2 , const C_Vector3* vec3)
{
	C_Vector3 temp = C_Vector3::CrossProduct(vec1 , vec2 , vec3);
	a = temp.x;
	b = temp.y;
	c = temp.z;

	d = -C_Vector3::DotProduct(&temp , vec1);

	pointOnPlane.x = vec1->x;
	pointOnPlane.y = vec1->y;
	pointOnPlane.z = vec1->z;
}


C_Plane::C_Plane(const C_Vertex* vec1 , const C_Vertex* vec2 , const C_Vertex* vec3)
{
	// Calculate the plane's normal vector
	C_Vector3 temp = C_Vector3::CrossProduct2(vec1 , vec2 , vec3);
	a = temp.x;
	b = temp.y;
	c = temp.z;

	C_Vertex vert;
	vert.x = temp.x; vert.y = temp.y; vert.z = temp.z;

	d = -C_Vector3::DotProduct(&vert , vec1);

	this->normalizePlane();

	pointOnPlane.x = vec1->x;
	pointOnPlane.y = vec1->y;
	pointOnPlane.z = vec1->z;

	points[0] = *vec1;
	points[1] = *vec2;
	points[2] = *vec3;
}


C_Plane::C_Plane(void)
{
	a = 0.0f;
	b = 0.0f;
	c = 0.0f;
	d = 0.0f;

	pointOnPlane.x = 0.0f;
	pointOnPlane.y = 0.0f;
	pointOnPlane.x = 0.0f;
}


void C_Plane::normalizePlane(void)
{
	float magn = math::Magnitude(a , b , c);

	a /= magn;
	b /= magn;
	c /= magn;
	d /= magn;
}


void C_Plane::setPlane(float _a , float _b , float _c , float _d)
{
	a = _a;
	b = _b;
	c = _c;
	d = _d;
}


void C_Plane::setPlane(C_Plane* plane)
{
	a = plane->a;
	b = plane->b;
	c = plane->c;
	d = plane->d;

	pointOnPlane.x = plane->pointOnPlane.x;
	pointOnPlane.y = plane->pointOnPlane.y;
	pointOnPlane.z = plane->pointOnPlane.z;

//	if(points.size()) {
		points[0] = plane->points[0];
		points[1] = plane->points[1];
		points[2] = plane->points[2];
//	} else {
//		points.push_back(plane->points[0]);
//		points.push_back(plane->points[1]);
//		points.push_back(plane->points[2]);
//	}
}


void C_Plane::getPlaneNormal(float* x , float* y , float* z)
{
	*x = a;
	*y = b;
	*z = c;
}


C_Vertex C_Plane::getPlaneNormal(void)
{
	C_Vertex point;

	point.x = a;
	point.y = b;
	point.z = c;

	return point;
}


float C_Plane::GetPlaneConstant(void)
{
	return d;
}

#if 0
void C_Plane::Draw(void)
{
	glDisable(GL_TRIANGLES);
	glColor3f(.5 , 0.5 , 0.0);

	glBegin(GL_TRIANGLES);
	glVertex3f(points[0].x , points[0].y , points[0].z);
	glVertex3f(points[1].x , points[1].y , points[1].z);
	glVertex3f(points[2].x , points[2].y , points[2].z);
	glEnd();

	glColor3f(1.0 , 1.0 , 1.0);
	glEnable(GL_LIGHTING);
}
#endif
