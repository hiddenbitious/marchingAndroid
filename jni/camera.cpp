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

#include "camera.h"
#include <string.h>

// Here are the fonts:
//void *glutFonts[7] = {
//	GLUT_BITMAP_9_BY_15,
//	GLUT_BITMAP_8_BY_13,
//	GLUT_BITMAP_TIMES_ROMAN_10,
//	GLUT_BITMAP_TIMES_ROMAN_24,
//	GLUT_BITMAP_HELVETICA_10,
//	GLUT_BITMAP_HELVETICA_12,
//	GLUT_BITMAP_HELVETICA_18
//};

C_Camera::C_Camera(void)
{
	position = C_Vector3::Zero();
	position.z = 0.0f;

	lookAt = C_Vector3::Zero();
	lookAt.z = -1.0f;

	up = C_Vector3::Zero();
	up.y = 1.0;

	rotationQuaternion.Rotate(0.0 , 0.0 , 0.0);
	xVec.SetVector(1.0 , 0.0 , 0.0);

	zNear = 0.5f;
	zFar = 500.0f;

	updateFrustum = true;

	rotation = translation = true;
}

void C_Camera::Look(void)
{
//	_UpdateCamera(&position , &lookAt , &up);

//	C_Quaternion revQ;
//	rotationQuaternion.GetReverseQuat(&revQ);
//	revQ.QuaternionToMatrix16(rotMatrix);
	rotationQuaternion.QuaternionToMatrix16(&ESrotMatrix);

	if(rotation) rotation = false;
	if(translation) translation = false;

	//Rotate and then translate.
//	glLoadIdentity();
//	glMultMatrixf (rotMatrix);
//	glTranslatef(-position.x, -position.y, -position.z);

	esMatrixMultiply(&globalModelviewMatrix, &ESrotMatrix, &globalModelviewMatrix);
	esTranslate(&globalModelviewMatrix, -position.x, -position.y, -position.z);

	/*
		//glMultMatrixf ( rotMatrix );
		rotMatrix[12] = -position.x;
		rotMatrix[13] = -position.y;
		rotMatrix[14] = -position.z;
		glLoadMatrixf ( rotMatrix );
	*/

	if(updateFrustum) {
		frustum->calculateFrustum();
		updateFrustum = false;
	}
}

void C_Camera::Rotate(const float xRotation , const float yRotation)
{
	C_Quaternion qq , qy;

	qy.Rotate(0.0 , yRotation , 0.0);
	qy.RotatePoint(&xVec);

	qq.AxisAngleToQuaternion(xVec.x , xVec.y , xVec.z , xRotation);
	qq.Mult(&qy);

	qq.RotatePoint(&lookAt , &position);
	qq.RotatePoint(&up);

	rotationQuaternion.Mult(&qq);

	updateFrustum = true;
	rotation = true;
}

void C_Camera::Move(const float x , const float y , const float z)
{
	position.Translate(x , y , z);
	lookAt.Translate(x , y , z);

	updateFrustum = true;
	translation = true;
}


void C_Camera::Move(const float speed)
{
	C_Vector3 moveVec;

	moveVec = C_Vector3::CalcVector(&position , &lookAt);
	moveVec.Normalize();

	Move(moveVec.x * speed , moveVec.y * speed , moveVec.z * speed);

	updateFrustum = true;
	translation = true;
}


void C_Camera::MoveUp(const float speed)
{
	position.y += speed;
	lookAt.y += speed;
}


void C_Camera::MoveDown(const float speed)
{
	position.y -= speed;
	lookAt.y -= speed;
}


void C_Camera::StrafeLeft(const float speed)
{
	float revSpeed = -speed;
	C_Vector3 moveVec , tempUp;

	tempUp = position;
	tempUp.Translate(&up);
	moveVec = C_Vector3::CrossProduct(&tempUp , &position , &lookAt);
	moveVec.Normalize();

	Move(moveVec.x * revSpeed , moveVec.y * revSpeed , moveVec.z * revSpeed);

	updateFrustum = true;
	translation = true;
}


void C_Camera::StrafeRight(const float speed)
{
	C_Vector3 moveVec , tempUp;

	tempUp = position;
	tempUp.Translate(&up);
	moveVec = C_Vector3::CrossProduct(&tempUp , &position , &lookAt);
	moveVec.Normalize();

	Move(moveVec.x * speed , moveVec.y * speed , moveVec.z * speed);

	updateFrustum = true;
	translation = true;
}


void C_Camera::setProjection(int w , int h)
{
	this->w = w;
	this->h = h;

	glViewport(0, 0, w, h);

	esMatrixLoadIdentity(&globalProjectionMatrix);
	esPerspective(&globalProjectionMatrix, fov, float(w) / float(h), zNear, zFar);
}

#if 0
void C_Camera::PrintText(int x , int y , float r , float g , float b, float a , const char *string , ...)
{
	if(!string || !strlen(string)) { return; }

	bool blending , texturing , lighting;
	blending = texturing = lighting = false;

	char text[256];
	va_list va;
	va_start(va , string);
	vsprintf(text , string , va);
	va_end(va);

	if(glIsEnabled(GL_TEXTURE_2D)) { texturing = true; }
	if(glIsEnabled(GL_BLEND)) { blending = true; }
	if(glIsEnabled(GL_LIGHTING)) { lighting = true; }

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f , this->w , 0.0f , this->h , -1.0f , 1.0f);

	glColor4f(r , g , b , a);
	glRasterPos2i(x , this->h - y);
	int i = 0;
	while(text[i]) {
		glutBitmapCharacter(glutFonts[6] , text[i]);
		i++;
	}

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if(!blending) { glDisable(GL_BLEND); }
	if(texturing) { glEnable(GL_TEXTURE_2D); }
	if(lighting) { glEnable(GL_LIGHTING); }
}
#endif
