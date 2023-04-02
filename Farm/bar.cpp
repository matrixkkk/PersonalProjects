#include "bar.h"

CBar::CBar(CPoint2 &pos)
{
	r_bar.x=pos.x;
	r_bar.y=pos.y;
	r_bar.w=100;
	r_bar.h=20;
	
	left=false;
	right=false;
	
	ControlS = 1;
}
void CBar::Init(SDL_Surface *p_bar)
{
	s_bar=p_bar;
	SDL_SetColorKey( p_bar, SDL_SRCCOLORKEY, SDL_MapRGB( p_bar->format, 255, 255, 255) );
}

void CBar::Draw(CGameEngine *game)
{
	SDL_BlitSurface(s_bar, NULL, game->screen, &r_bar);
}

void CBar::SetRect(CPoint2 &pos)
{
	r_bar.x=pos.x;
	r_bar.y=pos.y;

	//이동 제한
	if(r_bar.x<LEFTLINE)
		r_bar.x=LEFTLINE;
	if(r_bar.x > RIGHTLINE+100)
		r_bar.x=RIGHTLINE+100;
	//if(r_bar.y > 550 || r_bar.y < 550)
		//r_bar.y=550;
}

void CBar::SetLeft(bool lh)
{
	this->left=lh;
}
void CBar::SetRight(bool rh)
{
	this->right=rh;
}
void CBar::Move(int bar_state)   // 장애물을 위하여 Move 함수에 변수 추가
{
	if(bar_state == 0)
	{
		if(ControlS)              // 콘트롤 쇼크
		{
			if(left)
				r_bar.x-=6;
			else if(right)
				r_bar.x+=6;
		}
		else
		{
			if(left)
				r_bar.x+=6;
			else if(right)
				r_bar.x-=6;

			static int temp = 300;
			temp--;

			if(temp == 0)
			{
				ControlS = true;
				temp = 300;
			}
		}
		//이동 제한
		if(r_bar.x<LEFTLINE)
			r_bar.x=LEFTLINE;
		if(r_bar.x > RIGHTLINE+100)
			r_bar.x=RIGHTLINE+100;
		if(r_bar.y > 550 || r_bar.y < 550)
			r_bar.y=550;

	}
	else if(bar_state == 1)
	{
		if(left)
			r_bar.x-=2;
		else if(right)
			r_bar.x+=2;		
	}	
}