#include "common.h"


bool PointInRect(const SDL_Rect &rect,const CPoint2 &p)
{
	int left,right,top,bottom;

	left=rect.x;
	right=rect.x+rect.w;
	top=rect.y;
	bottom=rect.y+rect.h;

	if(p.x<left)	return false;
	if(p.x>right)	return false;
	if(p.y<top)		return false;
	if(p.y>bottom)	return false;

	return true;

}//포인트와 바운딩 박스 충돌 체크

void SetSDLRect(SDL_Rect &rect,int x,int y,int w,int h)
{
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
}//바운딩 박스 초기화



CPoint2::CPoint2()
{
	this->x=0;
	this->y=0;
}


CPoint2 Reflect_vector(CPoint2 &N,CPoint2 &D_vector) //반사 벡터 구하는 함수 
{
	double DN_inner_product,NN_inner_product; //D와 N의 내적 N과 N의 내적
	double product;
	CPoint2 temp;
	D_vector.x*=-1,D_vector.y*=-1;
	DN_inner_product=(double)(D_vector.x*N.x+D_vector.y*N.y);
	NN_inner_product=(double)(N.x*N.x+N.y*N.y);
	product=(double)(DN_inner_product/NN_inner_product)*2;
	N.x=(float)product*N.x;
	N.y=(float)product*N.y;
	temp.x=N.x-D_vector.x;
	temp.y=N.y-D_vector.y;
	return temp;
}

// 바운딩 박스 충돌체크
bool collision( SDL_Rect &A,SDL_Rect &B)
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


//윈도우-뷰포트 변환
void win_viewTransFrom(SRECT winPort,SRECT viewPort,SDL_Rect sourcePort,SDL_Rect *desPort)
{
	int xvL,xvR;		//뷰포트 x 최소,최대
	int yvT,yvB;		//뷰포트 y 최소,최대
	int xwL,xwR;		//윈도우 포트 x
	int ywT,ywB;
	
	//원본 좌표
	int src_xL,src_xR;
	int src_yT,src_yB;

	//변환된 좌표
	int des_xL,des_xR;
	int des_yT,des_yB;

	//원본 좌표 설정
	src_xL = sourcePort.x;
	src_xR = sourcePort.x + sourcePort.w;
	src_yT = sourcePort.y;
	src_yB = sourcePort.y + sourcePort.h;

	//뷰포트 값 설정
	xvL = viewPort.left;
	xvR = viewPort.right;
	yvT = viewPort.top;
	yvB = viewPort.bottom;
	
	//윈도우 포트 값 설정
	xwL = winPort.left;
	xwR = winPort.right;
	ywT = winPort.top;
	ywB = winPort.bottom;

	
	//윈도우 - 뷰 포트 변환
	
	des_xL = xvL+(src_xL - xwL)*(xvR-xvL)/(xwR-xwL);
	des_xR = xvL+(src_xR - xwL)*(xvR-xvL)/(xwR-xwL);
	des_yB = yvB+(src_yB - ywB)*(yvT-yvB)/(ywT-ywB);
	des_yT = yvB+(src_yT - ywB)*(yvT-yvB)/(ywT-ywB);

	desPort->x = des_xL;
	desPort->w = des_xR - des_xL;
	desPort->y = des_yT;
	desPort->h = des_yB - des_yT;

}