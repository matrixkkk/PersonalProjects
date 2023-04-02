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
 
class CPoint2 //3 ��ǥ ü�� 
{
public:
	float x; //x��ǥ
	float y; //y��ǥ

	CPoint2();
	CPoint2(float _x,float _y) { x=_x; y=_y; }

};

void SetSDLRect(SDL_Rect &rect,int x,int y,int w,int h);	//RECT �ʱ�ȭ �Լ�
bool PointInRect(const SDL_Rect &rect,const CPoint2 &p);	//���� �ٿ�� �ڽ� �浹 üũ 
CPoint2 Reflect_vector(CPoint2 &N,CPoint2 &D_vector);			//�ݻ� ���� ���ϴ� �Լ�.
bool collision( SDL_Rect &A,SDL_Rect &B);					//�ٿ���ڽ� üũ

#endif
