#ifndef BALL_H_
#define BALL_H_

#include "common.h"

class CBall
{
private:
	SDL_Surface		*s_ball;	//볼의 이미지를 가져올 표면
	SDL_Rect		 r_ball;	//볼의 Rect
	SDL_Rect		 BarPos;	//bar 의 위치

	CPoint2			 pos;		//볼의 위치. 중앙.
	CPoint2			 vec;	    //볼의 벡터
	
	int			speed;			//볼의 스피드
	int			id;				//충돌 확인을 위한 ID 값
	int 		state;			//볼의 상태 0: 게임 시작전 1: 게임진행중.   // int 로 변환

	int         view;           //볼의 출력여부값
	
	int			*minCollisionLine;	//볼의 충돌 최소 y라인
public:
	CBall() { }
	CBall(CPoint2 &pos,SDL_Surface *p_sur,int *minLine=NULL);

	void Draw();
	void Move();
	void SetState(int stat);				//볼의 상태를 바꿈.
	void SetSpeed(CPoint2 &des);			//블럭에 부딫힌 방향.
	void SetBarPos(SDL_Rect	 &barpos); 
	void SetVector(CPoint2 &des);
	void SetID(int ids);					//id를 셋한다.
	void SetScore(int scores);				//점수를 셋한다.
	void SetCollisionLine(int *line);

	void InverseVec(CPoint2 &p);
	void ID_Reset();							//id를 랜덤으로 리셋한다.

	void SetItemVector(int i_ball);         //볼의 각도를 조정하는 함수
	
	int     &GetView()      { return view;  }  //볼의 출력 여부를 리턴
	int     &GetState()		{ return state; }  //볼의 상태를 리턴   // bool 함수를 int 로 변경
	int		 GetID()        { return id;	}  //볼의 ID값을 리턴 한다.
	int		&GetSpeed()		{ return speed; }	//볼의 속도를 리턴
	CPoint2  GetPos()		{ return pos;   }  //볼의 중심 리턴
	CPoint2  GetVec()		{ return vec;	}  //볼의 벡터 리턴
	SDL_Rect GetRect()	    { return r_ball;} 

	SDL_Surface* GetSurface() { return s_ball; }
	
};

#endif
