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

#include "vectors.h"

vector2::vector2(void) : x((float)0) , y((float)0) {}
vector2::vector2(float _x , float _y) : x(_x) , y(_y) {}

void vector2::setVector(float _x , float _y)
{
	x = _x;
	y = _y;
}

void vector2::setVector(vector2 vec)
{
	x = vec.x;
	y = vec.y;
}

void vector2::translate(float _x , float _y)
{
	x += _x;
	y += _y;
}

void vector2::translate(vector2 vec)
{
	x += vec.x;
	y += vec.y;
}

void vector2::translate(const vector2 *vec)
{
	x += vec->x;
	y += vec->y;
}

float vector2::Magnitude(const float x , const float y)
{
	return (sqrt(x * x + y * y));
}



C_Vector3::C_Vector3(void) : x(0.0f) , y(0.0f) , z(0.0f) {}
C_Vector3::C_Vector3(float _x , float _y , float _z) : x(_x) , y(_y) , z(_z) {}
C_Vector3::C_Vector3(C_Vertex* point)
{
	x = point->x;
	y = point->y;
	z = point->z;
}


C_Vector3 C_Vector3::operator + (C_Vector3 vec)
{
	C_Vector3 temp;
	temp.x = x + vec.x;
	temp.y = y + vec.y;
	temp.z = z + vec.z;

	return temp;
}


C_Vector3& C_Vector3::operator += (const float val)
{
	x += val;
	y += val;
	z += val;

	return *this;
}


C_Vector3& C_Vector3::operator += (C_Vector3 vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}


C_Vector3 C_Vector3::operator - (C_Vector3 vec)
{
	C_Vector3 temp;
	temp.x = x - vec.x;
	temp.y = y - vec.y;
	temp.z = z - vec.z;

	return temp;
}


C_Vector3& C_Vector3::operator -= (const float val)
{
	x -= val;
	y -= val;
	z -= val;

	return *this;
}


C_Vector3& C_Vector3::operator -= (C_Vector3 vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}


C_Vector3 C_Vector3::operator - (void)
{
	C_Vector3 temp;
	temp.x = -x;
	temp.y = -y;
	temp.z = -z;

	return temp;
}


bool C_Vector3::operator < (C_Vector3* vec) const
{
	if(x <= vec->x && y <= vec->y && z <= vec->z) {
		return true;
	}

	return false;
}


bool C_Vector3::operator > (C_Vector3* vec) const
{
	if(x >= vec->x && y >= vec->y && z >= vec->z) {
		return true;
	}

	return false;
}


C_Vector3 C_Vector3::operator * (C_Vector3 vec)
{
	C_Vector3 temp;
	temp.x = x * vec.x;
	temp.y = y * vec.y;
	temp.z = z * vec.z;

	return temp;
}


C_Vector3 C_Vector3::operator * (float n)
{
	C_Vector3 temp;
	temp.x = x * n;
	temp.y = y * n;
	temp.z = z * n;

	return temp;
}


void C_Vector3::SetVector(float _x , float _y , float _z)
{
	x = _x;
	y = _y;
	z = _z;
}


void C_Vector3::SetVector(const C_Vector3 vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}


void C_Vector3::Translate(const float _x , const float _y , const float _z)
{
	x += _x;
	y += _y;
	z += _z;
}


void C_Vector3::Translate(const C_Vector3 *vec)
{
	x += vec->x;
	y += vec->y;
	z += vec->z;
}


void C_Vector3::Translate(const C_Vector3 vec)
{
	x += vec.x;
	y += vec.y;
	y += vec.z;
}


float C_Vector3::Magnitude(void)
{
	return (sqrt(x * x + y * y + z * z));
}

float C_Vector3::Magnitude(const C_Vector3 vec)
{
	return (sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}


float C_Vector3::Magnitude(const float x , const float y , const float z)
{
	return (sqrt(x * x + y * y + z * z));
}


float C_Vector3::Magnitude(const C_Vector3 vec1 , const C_Vector3 vec2)
{
	return (sqrt((vec1.x - vec2.x) * (vec1.x - vec2.x) + (vec1.y - vec2.y) * (vec1.y - vec2.y) + (vec1.z - vec2.z) * (vec1.z - vec2.z)));
}


float C_Vector3::Magnitude(float x1 , float y1 , float z1 , float x2 , float y2 , float z2)
{
	return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
}


C_Vector3 C_Vector3::CalcVector(const C_Vector3* vec1 , const C_Vector3* vec2)
{
	C_Vector3 tempVec;

	tempVec.SetVector(vec2->x - vec1->x , vec2->y - vec1->y , vec2->z - vec1->z);

	return tempVec;
}


C_Vector3 C_Vector3::CrossProduct(const C_Vector3* v1 , const C_Vector3* v2 , const C_Vector3* v3)
{
	C_Vector3 tempVec;

	tempVec.x = (v1->y - v2->y) * (v2->z - v3->z)  - (v2->y - v3->y) * (v1->z - v2->z);
	tempVec.y = (v2->x - v3->x) * (v1->z - v2->z)  - (v1->x - v2->x) * (v2->z - v3->z);
	tempVec.z = (v1->x - v2->x) * (v2->y - v3->y)  - (v2->x - v3->x) * (v1->y - v2->y);

	return tempVec;
}


C_Vector3 C_Vector3::CrossProduct(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3)
{
	C_Vector3 tempVec;

	tempVec.x = (v1->y - v2->y) * (v2->z - v3->z)  - (v2->y - v3->y) * (v1->z - v2->z);
	tempVec.y = (v2->x - v3->x) * (v1->z - v2->z)  - (v1->x - v2->x) * (v2->z - v3->z);
	tempVec.z = (v1->x - v2->x) * (v2->y - v3->y)  - (v2->x - v3->x) * (v1->y - v2->y);

	return tempVec;
}


C_Vector3 C_Vector3::CrossProduct2(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3)
{
	C_Vector3 tempVec;

	tempVec.x = (v1->y - v2->y) * (v3->z - v2->z)  - (v3->y - v2->y) * (v1->z - v2->z);
	tempVec.y = (v3->x - v2->x) * (v1->z - v2->z)  - (v1->x - v2->x) * (v3->z - v2->z);
	tempVec.z = (v1->x - v2->x) * (v3->y - v2->y)  - (v3->x - v2->x) * (v1->y - v2->y);

	return tempVec;
}


float C_Vector3::DotProduct(const C_Vector3* vec1 , const C_Vector3* vec2)
{
	return (vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z);
}


float C_Vector3::DotProduct(const C_Vertex* v1 , const C_Vertex* v2)
{
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}


void C_Vector3::Normalize(void)
{
	float lenght = C_Vector3::Magnitude(x , y , z);

	if(lenght) {
		x /= lenght;
		y /= lenght;
		z /= lenght;
	}
}


void C_Vector3::Normalize(float* x , float* y , float* z)
{
	float lenght = C_Vector3::Magnitude(*x , *y , *z);

	if(lenght) {
		*x /= lenght;
		*y /= lenght;
		*z /= lenght;
	}
}


void C_Vector3::Rotate(C_Quaternion quat)
{
	quat.RotatePoint(this);
}


void C_Vector3::Rotate(float anglex , float angley , float anglez)
{
	C_Quaternion quat;
	quat.EulerToQuat(anglex , angley , anglez);
	quat.RotatePoint(this);
}


C_Vector3 C_Vector3::Zero(void)
{
	C_Vector3 temp;
	return temp;
}
