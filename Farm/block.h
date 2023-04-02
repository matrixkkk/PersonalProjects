#ifndef BLOCK_H_
#define BLOCK_H_

#include "common.h"
#include "gameengine.h"
#include "ball.h"

//���� ������ ��Ÿ���� Ŭ����
class CBlock
{
private:	
	SDL_Rect		rect;		//���� ��ġ
	SDL_Rect		sheet_rect;	//��Ʈ�� ��ġ
	SDL_Rect		animation_sheet;		//�ִϸ��̼� ��Ʈ

	int	kind;					//���� ����. 0 : �� 1:�� 2:���� 3.�� 4.�� 5.���ڿ� 6.�Ⱥμ����� ����.
	bool state;					//���� ��ȿ ���¸� ��Ÿ���� ���� ����/����
	bool block_break;		    //���� �ı��� �������� ��Ÿ��
	
	int frame;					//�ִϸ��̼� ������
	int frame_delay;			//�÷��� ������	

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


//�Լ���ü
class BlockDraw
{
private:
	SDL_Surface		*block;
	SDL_Surface		*screen;
	SDL_Surface     *ani;		//�ִϸ��̼� ǥ��
	
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
			screen = SDL_GetVideoSurface();						//�� ȭ���� �޸� �����͸� �����´�.
			if(b.Getbreak())
			{
				SDL_BlitSurface(ani,&b.GetAniRect(),screen,&b.GetRect());		//�� �ı� �ִϸ��̼� �׸���
			}
			else
				SDL_BlitSurface(block, &b.GetSheetRect(), screen, &b.GetRect());		//�� �׸���
		}
	}
};
//���� �浹�� ���� ã�� �Լ� ��ü
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
