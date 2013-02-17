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

#include "bsphere.h"
#include <GL/glut.h>


C_BSphere::C_BSphere(void)
{
	radius = 0.0f;
	position = C_Vector3::Zero();
}


void C_BSphere::SetRadius(float _radius)
{
	radius = _radius;
}


void C_BSphere::SetPosition(const C_Vector3 _position)
{
	position.SetVector(_position);
}


void C_BSphere::SetPosition(const float x , const float y , const float z)
{
	position.SetVector(x , y , z);
}


void C_BSphere::Draw(void)
{
	glColor3f(1.0f , 1.0f , 1.0f);
	glPushMatrix();
	glTranslatef(position.x , position.y , position.z);
	glutWireSphere(radius , 50 , 50);
	glPopMatrix();
}


void C_BSphere::Draw(const float r , const float g , const float b)
{
	glColor3f(r , g , b);
	glPushMatrix();
	glTranslatef(position.x , position.y , position.z);
	glutWireSphere(radius , 25 , 25);
	glPopMatrix();
}


void C_BSphere::Translate(const float x, const float y, const float z)
{
	position.Translate(x , y , z);
}


void C_BSphere::Translate(const C_Vector3 *vec)
{
	position.Translate(vec);
}


void C_BSphere::Rotate(const float anglex , const float angley , const float anglez , const C_Vector3 *rotPoint)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	tempQuat.RotatePoint(&position , rotPoint);
}


void C_BSphere::Rotate(const C_Quaternion *quat , const C_Vector3 *rotPoint)
{
	quat->RotatePoint(&position , rotPoint);
}
