#ifndef BALL_H
#define BALL_H

#include <SDL/SDL.h>
#include "common.h"
//#include "sprite.h"
#include "Player.h"

class CBall
{
	SDL_Surface* image;  //볼 이미지를 그릴 표면
	SDL_Surface* shadow; //그림자 이미지를 그릴 표면

	CPoint3 ball_position; //볼의 위치 좌표
	CPoint3 shadow_position; //그림자의 위치 좌표
	CPoint3 shadow_vector; //그림자의 벡터
	CPoint3 original_spot; //시작 지점
	SDL_Rect ball; //볼
	SDL_Rect ball_Bound;
    CPoint3 SkillPos;
	//SDL_Rect ball_BoundShadow;
	int r;		   // 볼의 반지름
	ball_physics quant;
	float Rapid; //공의 회전
	float time; //시간의 변위
	int BallDir;	//공의 방향  0:방향 없음 1: 1p->2p  2: 2p->1p
	bool serveok;   //서비스를 치는게 가능해 짐.
	bool Service;   //true : 서비스 볼 false : 리턴 볼.
	int bounce;
	bool Skill;
public:
	CBall::CBall();
	CBall::~CBall();
	void init(int x,int y,int z); //볼의 위치 초기화
	void draw();
	void get_v(ball_physics fm);//float v,int height_degree,int width_degree); //공이 힘을 받을 때
	void ball_movement(); //볼의 이동
	void Ball_collision( CPlayer *user, CPlayer *com, int state );	//볼과 플레이어와의 충돌 체크
	int GetBallDir() { return BallDir;}		 //볼의 방향 리턴
	CPoint3 GetBallPos(){ return ball_position;} //볼의 위치 리턴
	bool GetServiceState(){ return Service;}	 //서비스 상태의 공인지 체크
	void SetService(bool n){ Service=n;}
	void SetServeok(bool n){ serveok=n;}
	void Clear(){ ball.x=3000; bounce=0;}					//볼의 잔상 없임
	int GetBounce(){ return bounce; }	//볼의 바운드 수 리턴
	SDL_Rect GetBallShadowBounceBox() { return ball_Bound;} //쉐도우 바운딩 박스 리턴
	SDL_Rect GetBallBounceBox() { return ball;}
	void SetTime(float t){ time=t; } //시간 셋팅(추가)
	void SetBounce() { bounce++;}
	ball_physics GetBallPhysics() { return quant; }		//공의 물리량 리턴.(추가)
	CPoint3 Getoriginal_spot() { return original_spot;} //공의 초기 위치 리턴(추가)
	void BounceReset(){ bounce=0; } //바운스 초기화 (추가)


};

#endif
