#include "collision.h"


void Check_Bar::operator ()(CBall &ball)
{
	if(bar.y >= ball.GetPos().y)
	{
		if(collision(bar,ball.GetRect())==true && ball.GetState()==true)	//충돌 한 경우!!
		{
			SDL_Rect tmp;
			int bar_center,point;		     //bar의 중심점.
			int degree,distance;			 //반사 각도,거리
				
			bar_center=bar.x+bar.w/2;		 //bar의 중심을 구하고
			tmp=ball.GetRect();				 //볼의 rect로 가져온후 ball의 중심을 구함
					
			point=tmp.x+tmp.w/2;
			distance=bar_center-point;			 //bar와 볼의 중심 사이의 거리를 구해서
			distance=abs(distance);
				
			//반사값 구하기
			if(distance>=40) //0도 
			{
				distance=40;
			}
				
			double x,y;
			degree=90-distance*2;		//중심과의 거리차에 따른 반사각 차이
				
			x=cos(R(degree));
			y=sin(R(degree));
			if((bar_center-point) > 0)
				x*=-1;
			ball.SetVector(CPoint2(x,-y));	

		}
	}
}

bool Check_Bar::operator ()(Citem *item)
{
	if(collision(bar,item->GetRect()))
		return true;
	else
		return false;
}
bool Check_Bar::operator ()(CBlock &block)
{
	if(collision(bar,block.GetRect()))
	{
		return true;
	}
	else 
		return false;
}