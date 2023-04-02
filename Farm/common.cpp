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

}//����Ʈ�� �ٿ�� �ڽ� �浹 üũ

void SetSDLRect(SDL_Rect &rect,int x,int y,int w,int h)
{
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
}//�ٿ�� �ڽ� �ʱ�ȭ



CPoint2::CPoint2()
{
	this->x=0;
	this->y=0;
}


CPoint2 Reflect_vector(CPoint2 &N,CPoint2 &D_vector) //�ݻ� ���� ���ϴ� �Լ� 
{
	double DN_inner_product,NN_inner_product; //D�� N�� ���� N�� N�� ����
	double product;
	CPoint2 temp;
	D_vector.x*=-1,D_vector.y*=-1;
	DN_inner_product=(double)(D_vector.x*N.x+D_vector.y*N.y);
	NN_inner_product=(double)(N.x*N.x+N.y*N.y);
	product=(double)(DN_inner_product/NN_inner_product)*2;
	N.x=product*N.x;
	N.y=product*N.y;
	temp.x=N.x-D_vector.x;
	temp.y=N.y-D_vector.y;
	return temp;
}

// �ٿ�� �ڽ� �浹üũ
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

