#ifndef _ITEM_H_
#define _ITEM_H_

#include "ball.h"
#include "bar.h"
#include "common.h"

class Citem
{
protected:
	SDL_Rect		r_item;		//아이템의 위치
	SDL_Rect		sheet_rect;	//시트 상의 위치
	SDL_Surface		*item_sheet;
	SDL_Surface		*screen;

	Mix_Chunk		*ball_high;
	Mix_Chunk		*ball_law;
	Mix_Chunk		*ball_spup;
	Mix_Chunk		*ball_spdown;
	Mix_Chunk		*ball_stop;
	Mix_Chunk		*ball_hide;
	Mix_Chunk		*key_revers;
	Mix_Chunk		*ball_die;
	Mix_Chunk		*ball_saperate;
	Mix_Chunk		*ball_point;
	Mix_Chunk		*key_raiser;
	Mix_Chunk       *ball_bar;

	int kind;			//아이템의 종류
	int own;			//볼의 주인 1:1p 0:2p   2:중립
public:
	Citem(SDL_Rect item_pos,SDL_Surface *sheet,int _own,int _kind);
	
	SDL_Rect GetRect()			{ return r_item; }
	int		 GetOwn()			{ return own;	 }

	void SetRect(int x,int y)   { r_item.x=x,r_item.y=y;}
	void SetOwn(int wn)			{ own=wn;				}

	virtual void UseItem()=0;
	void Update();
	void Draw();
};


///---볼 증가------------------------------------------- 
class AddBall : public Citem
{
	int idx;			//증가하는 공의 갯수
	deque<CBall> *balls1,*balls2;
public:
	
	AddBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;
	
	void UseItem();
	
};
///----------------------------------------------------
class Bone : public Citem
{
	int	 *scores1,*scores2;		//수정할 점수
	int  player;	//대상 플레이
	deque<CBall> *balls1,*balls2;
public:
	Bone(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int *pscore1,int *pscore2);
	
	void UseItem();
};
//-------------------------------------------------------
class PlusPoint : public Citem
{
	int *scores1,*scores2;		//수정할 점수	
	int point;		// 더 할 점수
public:
	PlusPoint(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,int *score1,int *score2,int point);
	
	void UseItem();

};
//----------------------------------------------------------------
class Laser : public Citem
{

	bool	*lasers;
	CBar	*bar;
	CPoint2 *pos;
public:
	Laser(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,bool *laser,CPoint2 *pos,CBar *bar);
	~Laser();
	void UseItem();
};




///---볼 각도-------------------------------------------
class RawBall : public Citem      // 볼각도 감소 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	RawBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

class HighBall : public Citem     // 볼각도 증가 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	HighBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};


///---볼 속도-------------------------------------------
class SPupBall : public Citem      // 공속도 증가 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPupBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};
class SPdownBall : public Citem    // 공속도 감소 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPdownBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

///---볼 상태(멈춤, 안보이게)----------------------------
class SPstopBall : public Citem    // 공을 정지 시키는 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPstopBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};
class SPhideBall : public Citem    // 공을 안보이게 하는 함수
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPhideBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

///---컨트롤 쇼크 ----------------------------------------

class ControlBar : public Citem    // 컨트롤을 반대로 해주는 함수
{
	CBar *players1,*players2;
public:

	ControlBar(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,CBar *player1,CBar *player2) ;

	void UseItem();  
};

///---장애물 함수 -----------------------------------------
/*
class Nbar_block : public Citem  // 장애물을 생성해주는 함수
{
	bool *nbars;
public:
	Nbar_block(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind, bool *nbar, int idx) ;
	~Nbar_block();

	void UseItem();
};
*/

class Nbar_block : public Citem
{

	bool	*nbars;
	int		*tmp_bars;
//	CBar	*bar;
//	CPoint2 *pos;
public:
	Nbar_block(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,bool *nbar, int *tmp_bar);

	void UseItem();
};


#endif
