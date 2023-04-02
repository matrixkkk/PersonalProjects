#ifndef _COMMON_H_

#define _COMMON_H_

#include <math.h>
#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <functional>
#include <deque>
#include <time.h>
#include <stdlib.h>


#define LEFTLINE 145
#define RIGHTLINE 450

using namespace std;

#define R(x) x*3.141592/180
 
class CPoint2 //3 좌표 체계 
{
public:
	float x; //x좌표
	float y; //y좌표

	CPoint2();
	CPoint2(float _x,float _y) { x=_x; y=_y; }

};

void SetSDLRect(SDL_Rect &rect,int x,int y,int w,int h);	//RECT 초기화 함수
bool PointInRect(const SDL_Rect &rect,const CPoint2 &p);	//점과 바운드 박스 충돌 체크 
CPoint2 Reflect_vector(CPoint2 &N,CPoint2 &D_vector);			//반사 벡터 구하는 함수.
bool collision( SDL_Rect &A,SDL_Rect &B);					//바운딩박스 체크

#endif
