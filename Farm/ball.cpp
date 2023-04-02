#include "ball.h"

CBall::CBall(CPoint2 &pos,SDL_Surface *p_sur)
{
	this->pos.x=pos.x;
	this->pos.y=pos.y;
	r_ball.x=pos.x-10;
	r_ball.y=pos.y-10;
	r_ball.w=20;
	r_ball.h=20;

	state= 0;		//���¸� 0 ���� ���������� �ʱ�ȭ.
	view = 1;       //��� �Լ��� ���̵���.
	
	speed=5;			//�⺻ �ӵ��� 5�� ���ش�.
	vec.x=cos(R(45));			//�⺻���� ����
	vec.y=-sin(R(45));

	s_ball=p_sur;		//�ε��� �̹����� �����͸� �Ѱ��ش�.
	SDL_SetColorKey( s_ball, SDL_SRCCOLORKEY, SDL_MapRGB( s_ball->format, 255, 255, 0) );
	id=rand()%100000;	
}

void CBall::Draw()
{
	if(view)
	{
		//���� �̹����� �׸���.
		SDL_Surface		*screen=SDL_GetVideoSurface();			//���÷��� �޸��� �ּҸ� �����´�.
		SDL_BlitSurface(s_ball, NULL,screen, &r_ball);
	}
	else
	{
		static int temp = 60;
		temp--;

		if(temp == 0)
		{
			view = 1;
			temp = 60;
		}
	}
}

void CBall::Move()
{	
	if(state == 1)
	{
		float vec_x,vec_y;
		vec_x=vec.x*speed;
		vec_y=vec.y*speed;

		this->pos.x+=vec_x;
		this->pos.y+=vec_y;

		r_ball.x=pos.x-10;
		r_ball.y=pos.y-10;
		
		//�� �̵� ����
		
		if(pos.x > 643)		//���� ���� �ȳѾ�� �ϱ�.
		{
			pos.x=643;
			this->vec.x*=-1;	//x���� ���͸� �ݴ��
		}
		if(pos.x < 155)     //���� ���� �ȳѾ� ���� �ϱ�
		{
			pos.x=155;
			this->vec.x*=-1;
		}
		if(pos.y < 10)
		{
			pos.y=10;
			this->vec.y*=-1;
		}
		
		
	}
	else if(state == 0)	//���� ���� �� 
	{
		r_ball.x=BarPos.x+40;
		pos.x=r_ball.x+10;
		pos.y=r_ball.y+10;
	}
	else
	{
		static int temp = 160;
		temp--;

		if(temp == 0)
		{
			state = 1;
			temp = 160;
		}
	}
}

void CBall::SetBarPos(SDL_Rect &barpos)
{
	this->BarPos=barpos;		//bar�� ��ġ�� ����.
}
void CBall::SetState(int stat)
{
	state=stat;
}
void CBall::SetID(int ids)
{
	id=ids;
}

void CBall::SetVector(CPoint2 &des)
{
	vec.x=des.x;
	vec.y=des.y;
}

void CBall::InverseVec(CPoint2 &p)
{
	vec.x*=p.x;
	vec.y*=p.y;
}

void CBall::ID_Reset()
{
	id=rand()%1000000;			//ID�� ����
}


void CBall::SetItemVector(int i_ball)    // Ball�� ������ ó�����ִ� �Լ�
{
	float temp = 0;
	int x=1,y=1;

//	cout << "(befor) vec.y : " << vec.y << endl;
//	cout << "(befor) vec.x : " << vec.x << endl;

	if(vec.x < 0)       // -�� +�� ���Ͽ� temp���� ���ϴ°��� �������Ͽ�
	{
		x = -1;
		vec.x *= -1;
	}
	if(vec.y < 0)
	{
		y = -1;
		vec.y *= -1;
	}

	if(i_ball == 0)    // 0�� 1�� ������ �а����� ����
	{
		temp = vec.x + vec.y;
		vec.y = temp/6;
		vec.x = (temp/6)*5;

		vec.y *= y;
		vec.x *= x;
	}
	else if(i_ball == 1)
	{
		temp = vec.x + vec.y;
		vec.x = temp/6;
		vec.y = (temp/6)*5;

		vec.y *= y;
		vec.x *= x;
	}
}

