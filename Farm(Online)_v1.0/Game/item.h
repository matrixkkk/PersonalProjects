#ifndef _ITEM_H_
#define _ITEM_H_

#include "ball.h"
#include "bar.h"
#include "common.h"
#include "turtle.h"

class Citem
{
protected:
	SDL_Rect		r_item;		//�������� ��ġ
	SDL_Rect		sheet_rect;	//��Ʈ ���� ��ġ
	SDL_Surface		*item_sheet;
	SDL_Surface		*screen;

	Mix_Chunk		*soundEffect;
	
	bool store;     //���� ������ ���������� �ƴ���
	int itemNum;	//������ �ѹ�
	

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


///---�� ����------------------------------------------- 
class AddBall : public Citem
{
	int idx;			//�����ϴ� ���� ����
	deque<CBall> *balls;
	int *topLine;
public:
	
	//*ball �߰��� �����̳� ������,idx ������ ���� ����
	AddBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball,int idx,int *line);
	
	void UseItem();
	
};
///----------------------------------------------------
class Bone : public Citem
{
	int	 *scores;		//������ ����
	
	deque<CBall> *balls;
public:
	//*ball �߰��� ���� ��� �����ϱ� ���� �� �����̳�, pscore ������ ����
	Bone(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball,int *pscore);
	
	void UseItem();
};
//-------------------------------------------------------
class PlusPoint : public Citem
{
	int *scores;		//������ ����	
	int point;		// �� �� ����
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




///---�� ����-------------------------------------------
class RawBall : public Citem      // �� ���� ���� �Լ�
{
	deque<CBall> *balls;
public:

	RawBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

class HighBall : public Citem     // ������ ���� �Լ�
{
	deque<CBall> *balls;
public:

	HighBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};


///---�� �ӵ�-------------------------------------------
class SPupBall : public Citem      // ���ӵ� ���� �Լ�
{
	deque<CBall> *balls;

public:

	SPupBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};
class SPdownBall : public Citem    // ���ӵ� ���� �Լ�
{	
	deque<CBall> *balls;
public:

	SPdownBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

///---�� ����(����, �Ⱥ��̰�)----------------------------
class SPstopBall : public Citem    // ���� ���� ��Ű�� �Լ�
{
	deque<CBall> *balls;
public:

	SPstopBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};
class SPhideBall : public Citem    // ���� �Ⱥ��̰� �ϴ� �Լ�
{
	deque<CBall> *balls;
public:

	SPhideBall(SDL_Rect item_pos,SDL_Surface *sheet,deque<CBall> *ball) ;

	void UseItem();
};

///---��Ʈ�� ��ũ ----------------------------------------

class ControlBar : public Citem    // ��Ʈ���� �ݴ�� ���ִ� �Լ�
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
