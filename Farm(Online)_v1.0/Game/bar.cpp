#include "bar.h"

CBar::CBar(CPoint2 &pos)
{
	r_bar.x=(Sint16)pos.x;
	r_bar.y=(Sint16)pos.y;
	r_bar.w=100;
	r_bar.h=20;	
}
void CBar::Init(SDL_Surface *p_bar)
{
	s_bar=p_bar;
	bEnable = false;
	duration = 0;
}

void CBar::Draw(CGameEngine *game)
{
	SDL_BlitSurface(s_bar, NULL, game->screen, &r_bar);
}

void CBar::SetRect(CPoint2 &pos)
{
	r_bar.x=(Sint16)pos.x;
	r_bar.y=(Sint16)pos.y;

	//�̵� ����
	if(r_bar.x<LEFTLINE)
		r_bar.x=LEFTLINE;
	if(r_bar.x > RIGHTLINE)
		r_bar.x=RIGHTLINE;	
}
void CBar::UseControlShock()
{
	timer.StartTick();
	bEnable = true;
}

//bar �������� �̵�
void CBar::MoveLeft()
{	
	double times;

	timer.GetTick();
	
	times = timer.GetElapedTime();
	duration += times;	
	if(duration >= 700.0f)		
	{
		bEnable = false;
		duration = 0;		
	}

	if(bEnable)              // ��Ʈ�� ��ũ
	{		
		r_bar.x+=7;			
	}
	else
	{		
		r_bar.x-=7;		
	}
	//�̵� ����
	if(r_bar.x<LEFTLINE)
		r_bar.x=LEFTLINE;
	if(r_bar.x > RIGHTLINE-100)
		r_bar.x=RIGHTLINE-100;	
}

//bar ���������� �̵�
void CBar::MoveRight()
{
	double times;

	timer.GetTick();
	
	times = timer.GetElapedTime();
	duration += times;	
	if(duration >= 700.0f)		
	{
		bEnable = false;
		duration = 0;		
	}
	if(bEnable)              // ��Ʈ�� ��ũ
	{		
		r_bar.x-=7;			
	}
	else
	{		
		r_bar.x+=7;		
	}
	//�̵� ����
	if(r_bar.x<LEFTLINE)
		r_bar.x=LEFTLINE;
	if(r_bar.x > RIGHTLINE-100)
		r_bar.x=RIGHTLINE-100;
}