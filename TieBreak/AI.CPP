#include "AI.h"
#include <windows.h>

CAi::CAi()
{
	x=0;	//이동해야할 x좌표
	y=0;	//이동해야할 y좌표
	action=0;
}

void CAi::AIaction(CBall *ball,CPlayer *user,CPlayer *com)
{
	CPoint3 BallPos;	//볼의 위치 저장
	CPoint3 Original_spot;
	CPoint3 tmp_v;
	SDL_Rect ComPos;	//com의 위치 저장.
	ball_physics tmp;

	BallPos=ball->GetBallPos();		//볼의 위치 얻어 옴
	if(ball->GetBallDir()==1) //유저가 친 상태
	{
		//x=BallPos.x;
		tmp=ball->GetBallPhysics();
		Original_spot=ball->Getoriginal_spot();
		tmp_v.y=tmp.v*cos(R(tmp.height_degree))*3.3f;
		tmp_v.x=tmp_v.y/tan(R(-tmp.width_degree));
		x=Original_spot.x+tmp_v.x*FPM;
	}
	else //com이 친 상태
	{
		x=0;
	}
	ComPos=com->getRect();	//com의 위치를 얻어 옴

	if( check_collision( com->getBoundingBox(), ball->GetBallShadowBounceBox() ) )
	{
	    com->Swing(BallPos.x);
	}
	else
        com->Stand();


	if(ComPos.x+10>x)	//공의 위치가 자기 보다 작을 경우.
	{
		action=1;	//1 은 좌측 이동
		com->SetLeft(true);
		com->LeftWalk();
		com->SetRight(false);
	}
	else if(ComPos.x+90<x)
	{
		action=2;
		com->SetRight(true);
		com->RightWalk();
		com->SetLeft(false);
	}
	else
	{
		com->SetLeft(false);
		com->SetRight(false);
	}
}
