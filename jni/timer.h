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

/**
 * Timer class.
 * A tipical use scenario would look like this:
 * C_Timer timer;
 * timer.Initialize();
 *
 * timer.Update();
 * float time = timer.GetDelta();
 *
 * // Do stuff...
 *
 * timer.Update();
 * float time2 = timer.GetDelta() / 1000.0f;
 */

#ifndef _C_TIMER_H_
#define _C_TIMER_H_

#include "globals.h"
#include <sys/time.h>
#include <time.h>

class C_Timer {
private:
	float timeFactor;				/// To convert the time received
	int64_t freq;					/// Timer frequency

	float delta, d0, d1;			/// Used to calculate delta
	timeval mmTimeStart;			/// Multimedia timer start value

public:
	C_Timer(void);					/// CTor
	void Initialize(void);			/// Initializes timer
	float GetTime(void);			/// Returns time elapsed since the initialization

	void Update(void);				/// Updates the delta variable

	/// Returns the delta variable
	inline float GetDelta(void) { return delta; }
};

#endif
