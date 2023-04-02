#include "turtle.h"


void CTurtle::Init(SDL_Surface *pScreen)
{
	//주 표면 포인터 저장
	screen = pScreen;

	//애니메이션 시트 저장
	SDL_Surface *temp = SDL_LoadBMP("../../Game/image/turtle.bmp");
	animationSheet = SDL_DisplayFormat(temp);
	SDL_SetColorKey( animationSheet, SDL_SRCCOLORKEY, SDL_MapRGB( animationSheet->format, 0, 0, 255) );
	
	alive = false;
	frame = 0;
	animationFrame = 0;
	duration = 0;
	movement = 2;

	SetSDLRect(posRect,18,511,100,60);		//거북이의 초기 위치
}

//거북이 그리기
void CTurtle::Draw()
{
	if(!alive)
		return;

	
	SDL_Rect tmpRect;
	tmpRect.x = 100 * animationFrame;
	tmpRect.y = 0;
	tmpRect.w = 100;
	tmpRect.h = 60;
	if(movement < 0)
	{
		tmpRect.y = 60;
	}

	SDL_BlitSurface(animationSheet,&tmpRect,screen,&posRect);
}

//거북이의 이동
void CTurtle::Move()
{
	double times = 0;

	if(!alive)		//살아 있지 않은 경우
		return;
	timer.GetTick();	
	
	//거북이 이동~~
	posRect.x += movement;
	if(posRect.x > RIGHTLINE-100 || posRect.x < LEFTLINE)
	{
		movement *= -1;
	}
	
	times = timer.GetElapedTime();		//시간을 가져옴

	duration += times;	
	if(duration >= 1500.0f)		
	{
		alive = false;
		duration = 0;
		frame = 0;
		ball_id.clear();			//저장된 아이디들 다 소거
	}

	frame++;
	if(frame >= 30)		//30프레임이 넘어 가면
	{
		animationFrame++;
		animationFrame = animationFrame%3;
	}
	frame = frame%30;
}

void CTurtle::CleanUp()
{
	SDL_FreeSurface(animationSheet);
}

void CTurtle::SetLive(bool live)
{
	alive = live; 
	timer.StartTick();	//시간 체크 시작 
} 