#ifndef TURTLE_H_
#define TURTLE_H_

//#include "common.h"
#include "CTimer.h"

//장애물 거북이 : 
class CTurtle
{
	SDL_Surface		*animationSheet;		//애니메이션 시트
	SDL_Surface		*screen;				//그리기 위한 주 표면
	
	SDL_Rect		posRect;				//거북이의 위치
	
	bool			alive;					//true 살아 있음 false 죽어 있음
	int				frame;					
	int				animationFrame;			//애니메이션 프레임
	int				movement;				//이동방향
	double			duration;				//지속시간
	
	CTimer timer;							//타이머 클래스
	
public:
	void Init(SDL_Surface *pScreen);					//초기화
	void Move();										//거북이 이동
	void Draw();
	void CleanUp();	
	
	SDL_Rect	GetRect()			{ return posRect; }
	bool		GetLive()			{ return alive;	  }
	
	void		SetLive(bool live);  

	vector<int>		ball_id;				//충돌한 볼 ID 컨테이너
};

#endif
