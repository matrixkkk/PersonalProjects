#include "collision.h"


void Check_Bar::operator ()(CBall &ball)
{
	if(bar.y >= ball.GetPos().y)
	{
		if(collision(bar,ball.GetRect())==true && ball.GetState()==true)	//�浹 �� ���!!
		{
			SDL_Rect tmp;
			int bar_center,point;		     //bar�� �߽���.
			int degree,distance;			 //�ݻ� ����,�Ÿ�
				
			bar_center=bar.x+bar.w/2;		 //bar�� �߽��� ���ϰ�
			tmp=ball.GetRect();				 //���� rect�� �������� ball�� �߽��� ����
					
			point=tmp.x+tmp.w/2;
			distance=bar_center-point;			 //bar�� ���� �߽� ������ �Ÿ��� ���ؼ�
			distance=abs(distance);
				
			//�ݻ簪 ���ϱ�
			if(distance>=40) //0�� 
			{
				distance=40;
			}
				
			double x,y;
			degree=90-distance*2;		//�߽ɰ��� �Ÿ����� ���� �ݻ簢 ����
				
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