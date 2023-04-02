//-----------------------------------------------------------------------------
// File: CTimer.h
//
// Desc: This class handles all timing functionality. This includes counting
//       the number of frames per second, to scaling vectors and values
//       relative to the time that has passed since the previous frame.
//
// Copyright (c) 1997-2005 GameInstitute.com. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _CTIMER_H_
#define _CTIMER_H_

#include "common.h"

//-----------------------------------------------------------------------------
// Definitions, Macros & Constants
//-----------------------------------------------------------------------------
const ULONG MAX_SAMPLE_COUNT = 1; // Maximum frame time sample count

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CTimer (Class)
// Desc : Game Timer class, queries performance hardware if available, and 
//        calculates all the various values required for frame rate based
//        vector / value scaling.
//-----------------------------------------------------------------------------
class CTimer
{
private:
    LARGE_INTEGER freq, Start, End;
	time_t start, finish;

	double elapsedTime;

public:
	CTimer();

	void   StartTick();		//시간 측정 시작
	void   GetTick();		//시간 측정
	double GetElapedTime();	//경과 시간 리턴
};

#endif // _CTIMER_H_