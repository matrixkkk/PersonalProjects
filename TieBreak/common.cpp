#include <SDL/SDL.h>
#include "common.h"

CPoint3 transCoordinate(int a,int b)  //물리적 - 논리적 좌표 변환
{
	CPoint3 p;  //리턴 저장값
	int x,y;   //x,y 임시 저장값

	y=(int)b*pow(RATE,b); //y축 비율에 따른 y 좌표 축소
	x=(int)a*pow(RATE_w,b-100); //x축 비율에 따른 x 좌표 축소

	p=win_view(x,y); //윈도우 - 뷰포트 변환

	return p;
}

CPoint3 win_view(int x,int y)
{
	CPoint3 p; //리턴 vlaue

	p.x=xvL+(x-xwL)*(xvR-xvL)/(xwR-xwL);
	p.y=yvB+(y-ywB)*(yvT-yvB)/(ywT-ywB);
	p.y-=66; //y 값만 이동.

	return p;
}

void SetSDLRect(SDL_Rect *rect,int x,int y,int w,int h)
{
	rect->x=x;
	rect->y=y;
	rect->w=w;
	rect->h=h;
}


bool check_collision( SDL_Rect A, SDL_Rect B )
{
	int leftA,leftB;
	int rightA,rightB;
	int topA,topB;
	int bottomA,bottomB;

	leftA=A.x;
	rightA=A.x+A.w;
	topA=A.y;
	bottomA=A.y+A.h;

	leftB=B.x;
	rightB=B.x+B.w;
	topB=B.y;
	bottomB=B.y+B.h;

	if(bottomA < topB) return false;
	if(topA > bottomB) return false;
	if(rightA < leftB) return false;
	if(leftA > rightB) return false;

	return true;
}

