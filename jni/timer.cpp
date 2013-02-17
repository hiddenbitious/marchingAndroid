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

#include "timer.h"

C_Timer::C_Timer () {
}

void C_Timer::Initialize(void)
{
	d0 = d1 = delta = 0.0f;
	freq = 1000.0f;
	timeFactor = 1.0f / freq;
	gettimeofday(&mmTimeStart, NULL);
}

float C_Timer::GetTime(void)
{
	timeval currentTime;
	float elapsedTime;

	gettimeofday(&currentTime, NULL);

	elapsedTime = (currentTime.tv_sec - mmTimeStart.tv_sec) * freq;      // sec to ms
    elapsedTime += (currentTime.tv_usec - mmTimeStart.tv_usec) * timeFactor;   // us to ms

	return elapsedTime;
}

void C_Timer::Update(void)
{
	d1 = GetTime();
	delta = d1 - d0;
	d0 = d1;
}
