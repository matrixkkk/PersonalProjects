//-----------------------------------------------------------------------------
// File: CTimer.cpp
//
// Desc: This class handles all timing functionality. This includes counting
//       the number of frames per second, to scaling vectors and values
//       relative to the time that has passed since the previous frame.
//
// Copyright (c) 1997-2005 GameInstitute.com. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CTimer Specific Includes
//-----------------------------------------------------------------------------
#include "CTimer.h"


CTimer::CTimer()
{	
	elapsedTime = 0;
}

void CTimer::StartTick()
{
	QueryPerformanceFrequency(&freq); 
    time(&start);
	// 시간 측정 시작
	QueryPerformanceCounter(&Start);
}

void CTimer::GetTick()
{
	time(&finish);
  
	 //시간 측정 끝
	QueryPerformanceCounter(&End);
	difftime(start, finish);
	elapsedTime = (double)(End.QuadPart-Start.QuadPart)/freq.QuadPart;
}

double CTimer::GetElapedTime()
{
	return elapsedTime;
}