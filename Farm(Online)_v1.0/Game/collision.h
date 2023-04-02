#ifndef COLLISION_H_
#define COLLISION_H_

#include "ball.h"
#include "item.h"
#include "block.h"
#include "SDL/SDL.h"

//공과 Bar을 충돌 체크를 위한 함수 객체
class Check_Bar
{
private:
	SDL_Rect	bar;
	Mix_Chunk	*soundEffect;

public:
	Check_Bar(SDL_Rect &b,Mix_Chunk *effect):bar(b),soundEffect(effect){
	}
	//공과 bar의 충돌 체크
	void operator ()(CBall &ball);  //야 이 함수는 ball을 원소로 하는 컨테이너에 쓰이는 함수 객체고
	//bar와 아이템 충돌 체크
	bool operator() (Citem *item);	//
	bool operator()	(CBlock &block);	// 그냥 함수 객체 재활용
};

//장애물 과 충동 체크 하는 함수 객체
class obstacle
{
private:
	SDL_Rect ob;
public:
	obstacle(SDL_Rect &o): ob(o){
	}
	bool operator () (CBall &ball)   //이거 써라
	{
		if( collision(ob,ball.GetRect()))
		{
			return true;
		}
		else 
			return false; //이거 써라 오키? ㅇㅇㅇ
	}
};


#endif
