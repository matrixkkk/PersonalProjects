#ifndef _ITEM_H_
#define _ITEM_H_

#include "ball.h"
#include "bar.h"
#include "common.h"

class Citem
{
protected:
	SDL_Rect		r_item;		//�������� ��ġ
	SDL_Rect		sheet_rect;	//��Ʈ ���� ��ġ
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

	int kind;			//�������� ����
	int own;			//���� ���� 1:1p 0:2p   2:�߸�
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


///---�� ����------------------------------------------- 
class AddBall : public Citem
{
	int idx;			//�����ϴ� ���� ����
	deque<CBall> *balls1,*balls2;
public:
	
	AddBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;
	
	void UseItem();
	
};
///----------------------------------------------------
class Bone : public Citem
{
	int	 *scores1,*scores2;		//������ ����
	int  player;	//��� �÷���
	deque<CBall> *balls1,*balls2;
public:
	Bone(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int *pscore1,int *pscore2);
	
	void UseItem();
};
//-------------------------------------------------------
class PlusPoint : public Citem
{
	int *scores1,*scores2;		//������ ����	
	int point;		// �� �� ����
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




///---�� ����-------------------------------------------
class RawBall : public Citem      // ������ ���� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	RawBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

class HighBall : public Citem     // ������ ���� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	HighBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};


///---�� �ӵ�-------------------------------------------
class SPupBall : public Citem      // ���ӵ� ���� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPupBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};
class SPdownBall : public Citem    // ���ӵ� ���� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPdownBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

///---�� ����(����, �Ⱥ��̰�)----------------------------
class SPstopBall : public Citem    // ���� ���� ��Ű�� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPstopBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};
class SPhideBall : public Citem    // ���� �Ⱥ��̰� �ϴ� �Լ�
{
	int idx;
	deque<CBall> *balls1,*balls2;
public:

	SPhideBall(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,deque<CBall> *ball1,deque<CBall> *ball2,int idx) ;

	void UseItem();
};

///---��Ʈ�� ��ũ ----------------------------------------

class ControlBar : public Citem    // ��Ʈ���� �ݴ�� ���ִ� �Լ�
{
	CBar *players1,*players2;
public:

	ControlBar(SDL_Rect item_pos,SDL_Surface *sheet,int owns,int _kind,CBar *player1,CBar *player2) ;

	void UseItem();  
};

///---��ֹ� �Լ� -----------------------------------------
/*
class Nbar_block : public Citem  // ��ֹ��� �������ִ� �Լ�
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
