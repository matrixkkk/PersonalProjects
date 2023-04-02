#include <SDL/SDL.h>
#include "common.h"

CPoint3 transCoordinate(int a,int b)  //������ - ���� ��ǥ ��ȯ
{
	CPoint3 p;  //���� ���尪
	int x,y;   //x,y �ӽ� ���尪

	y=(int)b*pow(RATE,b); //y�� ������ ���� y ��ǥ ���
	x=(int)a*pow(RATE_w,b-100); //x�� ������ ���� x ��ǥ ���

	p=win_view(x,y); //������ - ����Ʈ ��ȯ

	return p;
}

CPoint3 win_view(int x,int y)
{
	CPoint3 p; //���� vlaue

	p.x=xvL+(x-xwL)*(xvR-xvL)/(xwR-xwL);
	p.y=yvB+(y-ywB)*(yvT-yvB)/(ywT-ywB);
	p.y-=66; //y ���� �̵�.

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

