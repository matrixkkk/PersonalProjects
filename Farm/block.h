#ifndef BLOCK_H_
#define BLOCK_H_

#include "common.h"
#include "gameengine.h"
#include "ball.h"

//블럭의 정보를 나타내는 클래스
class CBlock
{
private:	
	SDL_Rect		rect;		//블럭의 위치
	SDL_Rect		sheet_rect;	//시트의 위치
	SDL_Rect		animation_sheet;		//애니메이션 시트

	int	kind;					//블럭의 종류. 0 : 닭 1:개 2:돼지 3.소 4.양 5.사자왕 6.안부셔지는 벽돌.
	bool state;					//블럭의 유효 상태를 나타내는 변수 있음/없음
	bool block_break;		    //블럭이 파괴된 상태인지 나타냄
	
	int frame;					//애니메이션 프레임
	int frame_delay;			//플레임 딜레이	

public:

	CBlock(const CPoint2 &pos);

	bool GetState()			{ return state; }
	bool Getbreak()			{ return block_break; }
	int  GetKind()          { return kind;  }

	void SetState(bool s)	 { state=s;  } 
	void SetSheet(int sheet);
	void Setbreak(bool br)   { block_break=br; }
	void SetKind(int _kind)	 { kind=_kind; }
	void update();

	SDL_Rect GetSheetRect() { return sheet_rect; }
	SDL_Rect GetAniRect()	{ return animation_sheet; }
	SDL_Rect GetRect()		{ return rect; }	
};


//함수객체
class BlockDraw
{
private:
	SDL_Surface		*block;
	SDL_Surface		*screen;
	SDL_Surface     *ani;		//애니메이션 표면
	
public:
	BlockDraw(SDL_Surface * b,SDL_Surface *a)
	{
		block=b;
		ani=a;
	}
	void operator ()(CBlock &b)
	{
		if(b.GetState())
		{
			screen = SDL_GetVideoSurface();						//주 화면의 메모리 포인터를 가져온다.
			if(b.Getbreak())
			{
				SDL_BlitSurface(ani,&b.GetAniRect(),screen,&b.GetRect());		//블럭 파괴 애니메이션 그리기
			}
			else
				SDL_BlitSurface(block, &b.GetSheetRect(), screen, &b.GetRect());		//블럭 그리기
		}
	}
};
//블럭과 충돌된 볼을 찾는 함수 객체
class Find_ball
{
	SDL_Rect block;
public:
	Find_ball(SDL_Rect &b): block(b){
	}
	bool operator () (CBall &ball)
	{
		if(collision(ball.GetRect(),block))
		{
			return true;
		}
		else
			return false;
	}
};



#endif
