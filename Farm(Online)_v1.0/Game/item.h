#ifndef _ITEM_H_
#define _ITEM_H_

#include "ball.h"
#include "bar.h"
#include "common.h"
#include "turtle.h"

class Citem
{
protected:
	SDL_Rect		r_item;		//아이템의 위치
	SDL_Rect		sheet_rect;	//시트 상의 위치
	SDL_Surface		*item_sheet;
	SDL_Surface		*screen;

	Mix_Chunk		*soundEffect;
	
	bool store;     //저장 가능한 아이템인지 아닌지
	int itemNum;	//아이템 넘버
	

public:
	Citem(SDL_Rect item_pos,SDL_Surface *sheet);
	~Citem();
	SDL_Rect GetRect()			{ return r_item; }
	int		 GetStore()			{ return store;	 }
	int		 GetItemNum()		{ return itemNum;}

	void SetRect(int x,int y)   { r_item.x=x,r_item.y=y;}

	virtual void UseItem()=0;
	void Update();
	void Draw();
};


///---볼 증가------------------------------------------- 
class AddBall : public Citem
{
	int idx;			//증가하는 공의 갯수
	deque<CBall> *balls;
	int *topLine;
public:
	
	//*ball 추가할 컨테이너 포인터,idx 증가할 공의 개수
	AddBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball,int idx,int *line);
	
	void UseItem();
	
};
///----------------------------------------------------
class Bone : public Citem
{
	int	 *scores;		//수정할 점수
	
	deque<CBall> *balls;
public:
	//*ball 추가된 공을 모두 삭제하기 위한 볼 컨테이너, pscore 삭제될 점수
	Bone(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball,int *pscore);
	
	void UseItem();
};
//-------------------------------------------------------
class PlusPoint : public Citem
{
	int *scores;		//수정할 점수	
	int point;		// 더 할 점수
public:
	PlusPoint(SDL_Rect item_pos,SDL_Surface *sheet,int *score,int point);
	
	void UseItem();

};
//----------------------------------------------------------------
class Laser : public Citem
{

	bool	*lasers;
	CBar	*bar;
	CPoint2 *pos;
public:
	Laser(SDL_Rect item_pos,SDL_Surface *sheet,bool *laser,CPoint2 *pos,CBar *bar);
	~Laser();
	void UseItem();
};




///---볼 각도-------------------------------------------
class RawBall : public Citem      // 볼 각도 감소 함수
{
	deque<CBall> *balls;
public:

	RawBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

class HighBall : public Citem     // 볼각도 증가 함수
{
	deque<CBall> *balls;
public:

	HighBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};


///---볼 속도-------------------------------------------
class SPupBall : public Citem      // 공속도 증가 함수
{
	deque<CBall> *balls;

public:

	SPupBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};
class SPdownBall : public Citem    // 공속도 감소 함수
{	
	deque<CBall> *balls;
public:

	SPdownBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

///---볼 상태(멈춤, 안보이게)----------------------------
class SPstopBall : public Citem    // 공을 정지 시키는 함수
{
	deque<CBall> *balls;
public:

	SPstopBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};
class SPhideBall : public Citem    // 공을 안보이게 하는 함수
{
	deque<CBall> *balls;
public:

	SPhideBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

///---컨트롤 쇼크 ----------------------------------------

class ControlBar : public Citem    // 컨트롤을 반대로 해주는 함수
{
	CBar *players;
public:

	ControlBar(SDL_Rect item_pos,SDL_Surface *sheet,CBar *player) ;

	void UseItem();  
};

class Nbar_block : public Citem
{
	CTurtle *pTurtle;

public:
	Nbar_block(SDL_Rect item_pos,SDL_Surface *sheet,CTurtle *turtle);

	void UseItem();
};


#endif
