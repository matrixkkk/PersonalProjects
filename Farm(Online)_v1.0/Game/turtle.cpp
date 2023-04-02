#include "turtle.h"


void CTurtle::Init(SDL_Surface *pScreen)
{
	//�� ǥ�� ������ ����
	screen = pScreen;

	//�ִϸ��̼� ��Ʈ ����
	SDL_Surface *temp = SDL_LoadBMP("../../Game/image/turtle.bmp");
	animationSheet = SDL_DisplayFormat(temp);
	SDL_SetColorKey( animationSheet, SDL_SRCCOLORKEY, SDL_MapRGB( animationSheet->format, 0, 0, 255) );
	
	alive = false;
	frame = 0;
	animationFrame = 0;
	duration = 0;
	movement = 2;

	SetSDLRect(posRect,18,511,100,60);		//�ź����� �ʱ� ��ġ
}

//�ź��� �׸���
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

//�ź����� �̵�
void CTurtle::Move()
{
	double times = 0;

	if(!alive)		//��� ���� ���� ���
		return;
	timer.GetTick();	
	
	//�ź��� �̵�~~
	posRect.x += movement;
	if(posRect.x > RIGHTLINE-100 || posRect.x < LEFTLINE)
	{
		movement *= -1;
	}
	
	times = timer.GetElapedTime();		//�ð��� ������

	duration += times;	
	if(duration >= 1500.0f)		
	{
		alive = false;
		duration = 0;
		frame = 0;
		ball_id.clear();			//����� ���̵�� �� �Ұ�
	}

	frame++;
	if(frame >= 30)		//30�������� �Ѿ� ����
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
	timer.StartTick();	//�ð� üũ ���� 
} 