#include "ball.h"

CBall::CBall(CPoint2 &pos,SDL_Surface *p_sur)
{
	this->pos.x=pos.x;
	this->pos.y=pos.y;
	r_ball.x=pos.x-10;
	r_ball.y=pos.y-10;
	r_ball.w=20;
	r_ball.h=20;

	state= 0;		//상태를 0 게임 시작전으로 초기화.
	view = 1;       //출력 함수를 보이도록.
	
	speed=5;			//기본 속도를 5로 해준다.
	vec.x=cos(R(45));			//기본벡터 설정
	vec.y=-sin(R(45));

	s_ball=p_sur;		//로딩한 이미지의 포인터를 넘겨준다.
	SDL_SetColorKey( s_ball, SDL_SRCCOLORKEY, SDL_MapRGB( s_ball->format, 255, 255, 0) );
	id=rand()%100000;	
}

void CBall::Draw()
{
	if(view)
	{
		//볼의 이미지를 그린다.
		SDL_Surface		*screen=SDL_GetVideoSurface();			//디스플레이 메모리의 주소를 가져온다.
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
		
		//볼 이동 제한
		
		if(pos.x > 643)		//우측 라인 안넘어가게 하기.
		{
			pos.x=643;
			this->vec.x*=-1;	//x방향 벡터를 반대로
		}
		if(pos.x < 155)     //좌측 라인 안넘어 가게 하기
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
	else if(state == 0)	//게임 시작 전 
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
	this->BarPos=barpos;		//bar의 위치를 저장.
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
	id=rand()%1000000;			//ID값 리셋
}


void CBall::SetItemVector(int i_ball)    // Ball의 각도를 처리해주는 함수
{
	float temp = 0;
	int x=1,y=1;

//	cout << "(befor) vec.y : " << vec.y << endl;
//	cout << "(befor) vec.x : " << vec.x << endl;

	if(vec.x < 0)       // -와 +로 인하여 temp값이 변하는것을 막기위하여
	{
		x = -1;
		vec.x *= -1;
	}
	if(vec.y < 0)
	{
		y = -1;
		vec.y *= -1;
	}

	if(i_ball == 0)    // 0과 1로 예각과 둔각으로 구분
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

