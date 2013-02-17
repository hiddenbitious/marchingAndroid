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

#include "quaternion.h"

C_Quaternion::C_Quaternion()
//This is the identity quaternion
	: x(0.0) , y(0.0) , z(0.0) , a(1.0)
{ }


C_Quaternion::C_Quaternion(C_Vector3 point)
	: x(point.x) , y(point.y) , z(point.z) , a(0.0)
{ }


void C_Quaternion::CreateXquaternion(float angle)
{
	a = (float) cos(angle / 2.0 * DEGREES_TO_RADIANS);
	x = (float) sin(angle / 2.0 * DEGREES_TO_RADIANS);
	y = 0.0;
	z = 0.0;
}


void C_Quaternion::CreateYquaternion(float angle)
{
	a = (float) cos(angle / 2.0 * DEGREES_TO_RADIANS);
	x = 0.0;
	y = (float) sin(angle / 2.0 * DEGREES_TO_RADIANS);
	z = 0.0;
}


void C_Quaternion::CreateZquaternion(float angle)
{
	a = (float) cos(angle / 2.0 * DEGREES_TO_RADIANS);
	x = 0.0;
	y = 0.0;
	z = (float) sin(angle / 2.0 * DEGREES_TO_RADIANS);
}


void C_Quaternion::Reverse(void)
{
	this->x = -x;
	this->y = -y;
	this->z = -z;
}


void C_Quaternion::Normalize(void)
{
	float magn = sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->a * this->a);

	if(magn) {
		this->x /= magn;
		this->y /= magn;
		this->z /= magn;
		this->a /= magn;

		LIMIT_RANGE(-1.0 , this->x , 1.0);
		LIMIT_RANGE(-1.0 , this->y , 1.0);
		LIMIT_RANGE(-1.0 , this->z , 1.0);
		LIMIT_RANGE(-1.0 , this->a , 1.0);
	}
}


void C_Quaternion::Identity(void)
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	a = 1.0;
}


// quat * (*this*)
void C_Quaternion::Mult(const C_Quaternion* quat)
{
	float _a = quat->a * this->a - quat->x * this->x - quat->y * this->y - quat->z * this->z;

	float _x = quat->a * this->x + quat->x * this->a + quat->y * this->z - quat->z * this->y;
	float _y = quat->a * this->y + quat->y * this->a + quat->z * this->x - quat->x * this->z;
	float _z = quat->a * this->z + quat->z * this->a + quat->x * this->y - quat->y * this->x;

	this->a = _a;
	this->x = _x;
	this->y = _y;
	this->z = _z;

	this->Normalize();
}


C_Quaternion* C_Quaternion::Mult(const C_Quaternion* q1 , const C_Quaternion* q2)
{
	C_Quaternion* q3 = new C_Quaternion();

	q3->a = q1->a * q2->a - (q1->x * q2->x + q1->y * q2->y + q1->z * q2->z);
	q3->x = q1->a * q2->x + q2->a * q1->x + q1->y * q2->z - q1->z * q2->y;
	q3->y = q1->a * q2->y + q2->a * q1->y + q1->z * q2->x - q1->x * q2->z;
	q3->z = q1->a * q2->z + q2->a * q1->z + q1->x * q2->y - q1->y * q2->x;

	q3->Normalize();

	return q3;
}


C_Quaternion* C_Quaternion::Mult(float x , float y , float z , float a)
{
	C_Quaternion* tempQuat = new C_Quaternion();

	tempQuat->SetQuaternion(this->a * a , this->x * x, this->y * y , this->z * z);
	tempQuat->Normalize();

	return tempQuat;
}


void C_Quaternion::QuaternionToAxisAngle(float& _x , float& _y , float& _z , float& _angle)
{
	_angle = 2.0f * (float) acos(this->a) * RADIANS_TO_DEGREES;

	//Both calculations will bear the same result...
//	float tmp = sqrt ( 1.0f - (float) sin ( this->a / 2.0f ) );
	float tmp = sqrt(x * x + y * y + z * z);

	if(tmp) {
		_x = this->x / tmp;
		_y = this->y / tmp;
		_z = this->z / tmp;

		math::Normalize(&_x , &_y , &_z);
	}
	// 0 rotation...don't care about the rotation axis
	else {
		_x = 1.0f;
		_y = 0.0f;
		_z = 0.0f;
		_angle = 0.0f;
	}
}


void C_Quaternion::QuaternionToMatrix9(float* matrix)
{
	matrix[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	matrix[1] = 2.0f * x * y + 2.0f * a * z;
	matrix[2] = 2.0f * x * z - 2.0f * a * y;

	matrix[3] = 2.0f * x * y - 2.0f * a * z;
	matrix[4] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	matrix[5] = 2.0f * y * z + 2.0f * a * x;

	matrix[6] = 2.0f * x * z + 2.0f * a * y;
	matrix[7] = 2.0f * y * z - 2.0f * a * x;
	matrix[8] = 1.0f - 2.0f * x * x - 2.0f * y * y;
}


void C_Quaternion::QuaternionToMatrix16(float* matrix)
{
	// Original code
	/*
		matrix[0] = 1.0f - 2*y*y - 2*z*z;
		matrix[1] = 2*x*y + 2*a*z;
		matrix[2] = 2*x*z - 2*a*y;
	//	matrix[3] = 0.0;

		matrix[4] = 2*x*y - 2*a*z;
		matrix[5] = 1.0f - 2*x*x -2*z*z;
		matrix[6] = 2*y*z + 2*a*x;
	//	matrix[7] = 0.0;

		matrix[8] = 2*x*z + 2*a*y;
		matrix[9] = 2*y*z - 2*a*x;
		matrix[10] = 1.0f - 2*x*x - 2*y*y;
	//	matrix[11] = 0.0;

		matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0f;
		matrix[15] = 1.0f;
	*/

	float zz2 	= 2.0f * z * z;
	float yy2 	= 2.0f * y * y;
	float xx2 	= 2.0f * x * x;

	float ax2 	= 2.0f * a * x;
	float ay2 	= 2.0f * a * y;
	float az2 	= 2.0f * a * z;

	float xy2 	= 2.0f * x * y;
	float xz2 	= 2.0f * x * z;
	float yz2 	= 2.0f * y * z;

	matrix[0]	= 1.0f - yy2 - zz2;
	matrix[1]	= xy2 + az2;
	matrix[2]	= xz2 - ay2;

	matrix[4]	= xy2 - az2;
	matrix[5]	= 1.0f - xx2 - zz2;
	matrix[6]	= yz2 + ax2;

	matrix[8]	= xz2 + ay2;
	matrix[9]	= yz2 - ax2;
	matrix[10]	= 1.0f - xx2 - yy2;

	matrix[3]	= matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0f;
	matrix[15]	= 1.0f;
}

void C_Quaternion::QuaternionToMatrix16(ESMatrix *matrix)
{
	float zz2 	= 2.0f * z * z;
	float yy2 	= 2.0f * y * y;
	float xx2 	= 2.0f * x * x;

	float ax2 	= 2.0f * a * x;
	float ay2 	= 2.0f * a * y;
	float az2 	= 2.0f * a * z;

	float xy2 	= 2.0f * x * y;
	float xz2 	= 2.0f * x * z;
	float yz2 	= 2.0f * y * z;

	matrix->m[0][0]	= 1.0f - yy2 - zz2;
	matrix->m[1][0]	= xy2 + az2;
	matrix->m[2][0]	= xz2 - ay2;

	matrix->m[0][1]	= xy2 - az2;
	matrix->m[1][1]	= 1.0f - xx2 - zz2;
	matrix->m[2][1]	= yz2 + ax2;

	matrix->m[0][2]	= xz2 + ay2;
	matrix->m[1][2]	= yz2 - ax2;
	matrix->m[2][2]	= 1.0f - xx2 - yy2;

	matrix->m[3][0]	= matrix->m[3][1] = matrix->m[3][2] = 0.0f;
	matrix->m[0][3] = matrix->m[1][3] = matrix->m[2][3] = 0.0f;
	matrix->m[3][3]	= 1.0f;
}


void C_Quaternion::Rotate(float angleX , float angleY , float angleZ)
{
	C_Quaternion q1;
	C_Quaternion q2;
	C_Quaternion q3;

	q1.CreateXquaternion(angleX);
	q2.CreateYquaternion(angleY);
	q3.CreateZquaternion(angleZ);

	q1.Mult(&q2);
	q1.Mult(&q3);

	this->Mult(&q1);
	/*	this->mult ( &q1 );
		this->mult ( &q2 );
		this->mult ( &q3 );
	*/
}


void C_Quaternion::Rotate(float angleX , float angleY)
{
	C_Quaternion q1;
	C_Quaternion q2;

	q1.CreateXquaternion(angleX);
	q2.CreateYquaternion(angleY);

	//(q1*q2*q3)
	q1.Mult(&q2);

	//(*this*) * (q1*q2*q3)
	this->Mult(&q1);
}


void C_Quaternion::EulerToQuat(const float x, const float y, const float z)
{
	double	ex, ey, ez;
	double	cr, cp, cy, sr, sp, sy, cpcy, spsy;

	ex = x * DEGREES_TO_RADIANS / 2.0;
	ey = y * DEGREES_TO_RADIANS / 2.0;
	ez = z * DEGREES_TO_RADIANS / 2.0;

	cr = cos(ex);
	cp = cos(ey);
	cy = cos(ez);

	sr = sin(ex);
	sp = sin(ey);
	sy = sin(ez);

	cpcy = cp * cy;
	spsy = sp * sy;

	this->a = float(cr * cpcy + sr * spsy);

	this->x = float(sr * cpcy - cr * spsy);
	this->y = float(cr * sp * cy + sr * cp * sy);
	this->z = float(cr * cp * sy - sr * sp * cy);

	this->Normalize();

	return;
}


void C_Quaternion::AxisAngleToQuaternion(float x , float y , float z , float a)
{
	math::Normalize(&x , &y , &z);
	this->x = x * (float) sin(a * DEGREES_TO_RADIANS / 2.0f);
	this->y = y * (float) sin(a * DEGREES_TO_RADIANS / 2.0f);
	this->z = z * (float) sin(a * DEGREES_TO_RADIANS / 2.0f);

	this->a = (float) cos(a * DEGREES_TO_RADIANS / 2.0f);
}


void C_Quaternion::RotatePoint(C_Vector3* p1) const
{
	float newX = a * a * p1->x + 2 * y * a * p1->z - 2 * z * a * p1->y + x * x * p1->x + 2 * y * x * p1->y + 2 * z * x * p1->z - z * z * p1->x - y * y * p1->x;
	float newY = 2 * x * y * p1->x + y * y * p1->y + 2 * z * y * p1->z + 2 * a * z * p1->x - z * z * p1->y + a * a * p1->y - 2 * x * a * p1->z - x * x * p1->y;
	float newZ = 2 * x * z * p1->x + 2 * y * z * p1->y + z * z * p1->z - 2 * a * y * p1->x - y * y * p1->z + 2 * a * x * p1->y - x * x * p1->z + a * a * p1->z;

	p1->SetVector(newX , newY , newZ);
}


void C_Quaternion::RotatePoint(C_Vector3* p1 , const C_Vector3* rotationPoint) const
{
	C_Vector3 tmp;

	tmp.x = p1->x - rotationPoint->x;
	tmp.y = p1->y - rotationPoint->y;
	tmp.z = p1->z - rotationPoint->z;

	RotatePoint(&tmp);
	tmp.Translate(rotationPoint);

	p1->SetVector(tmp);
}


void C_Quaternion::RotatePoint(C_Vector3* p1 , float rotX , float rotY , float rotZ) const
{
	C_Vector3 tmp;

	tmp.x = p1->x - rotX;
	tmp.y = p1->y - rotY;
	tmp.z = p1->z - rotZ;

	this->RotatePoint(&tmp);
	tmp.Translate(rotX , rotY , rotZ);

	p1->SetVector(tmp);
}


float C_Quaternion::Magnitude(void)
{
	return float(sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->a * this->a));
}


float C_Quaternion::Magnitude(C_Quaternion* quat)
{
	return float(sqrt(quat->x * quat->x + quat->y * quat->y + quat->z * quat->z + quat->a * quat->a));
}


void C_Quaternion::CopyQuaternion(C_Quaternion* quat) const
{
	if(!quat) { quat = new C_Quaternion(); }

	quat->a = this->a;
	quat->x = this->x; quat->y = this->y; quat->z = this->z;
}


bool C_Quaternion::HoldsRotation(void)
{
	if(!this->x && !this->y && !this->z) {
		return false;
	}

	return true;
}


void C_Quaternion::GetReverseQuat(C_Quaternion* quat)
{
	quat->x = -this->x;
	quat->y = -this->y;
	quat->z = -this->z;

	quat->a = this->a;
}


void C_Quaternion::SetQuaternion(const C_Quaternion* quat)
{
	this->x = quat->x;
	this->y = quat->y;
	this->z = quat->z;

	this->a = quat->a;
}


void C_Quaternion::SetQuaternion(float angle , float x , float y , float z)
{
	this->a = angle;

	this->x = x;
	this->y = y;
	this->z = z;
}


void C_Quaternion::SetQuaternionAxis(float x , float y , float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


void C_Quaternion::SetQuaternionAxis(C_Vector3* vector)
{
	this->x = vector->x;
	this->y = vector->y;
	this->z = vector->z;
}

void C_Quaternion::SetQuaternionAngle(float angle)
{ this->a = (float) cos(angle / 2.0f * DEGREES_TO_RADIANS); }

float C_Quaternion::GetQuaternionX(void)
{ return this->x; }

float C_Quaternion::GetQuaternionY(void)
{ return this->y; }

float C_Quaternion::GetQuaternionZ(void)
{ return this->z; }

float C_Quaternion::GetQuaternionAngle(void)
{ return this->a; }

void C_Quaternion::SetQuaternionX(float x)
{ this->x = x; }

void C_Quaternion::SetQuaternionY(float y)
{ this->y = y; }

void C_Quaternion::SetQuaternionZ(float z)
{ this->z = z; }
