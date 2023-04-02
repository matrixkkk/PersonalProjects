#ifndef BALL_H_
#define BALL_H_

#include "common.h"

class CBall
{
private:
	SDL_Surface		*s_ball;	//���� �̹����� ������ ǥ��
	SDL_Rect		 r_ball;	//���� Rect
	SDL_Rect		 BarPos;	//bar �� ��ġ

	CPoint2			 pos;		//���� ��ġ. �߾�.
	CPoint2			 vec;	    //���� ����
	
	int			speed;			//���� ���ǵ�
	int			id;				//�浹 Ȯ���� ���� ID ��
	int 		state;			//���� ���� 0: ���� ������ 1: ����������.   // int �� ��ȯ

	int         view;           //���� ��¿��ΰ�
	
	int			*minCollisionLine;	//���� �浹 �ּ� y����
public:
	CBall() { }
	CBall(CPoint2 &pos,SDL_Surface *p_sur,int *minLine=NULL);

	void Draw();
	void Move();
	void SetState(int stat);				//���� ���¸� �ٲ�.
	void SetSpeed(CPoint2 &des);			//���� �΋H�� ����.
	void SetBarPos(SDL_Rect	 &barpos); 
	void SetVector(CPoint2 &des);
	void SetID(int ids);					//id�� ���Ѵ�.
	void SetScore(int scores);				//������ ���Ѵ�.
	void SetCollisionLine(int *line);

	void InverseVec(CPoint2 &p);
	void ID_Reset();							//id�� �������� �����Ѵ�.

	void SetItemVector(int i_ball);         //���� ������ �����ϴ� �Լ�
	
	int     &GetView()      { return view;  }  //���� ��� ���θ� ����
	int     &GetState()		{ return state; }  //���� ���¸� ����   // bool �Լ��� int �� ����
	int		 GetID()        { return id;	}  //���� ID���� ���� �Ѵ�.
	int		&GetSpeed()		{ return speed; }	//���� �ӵ��� ����
	CPoint2  GetPos()		{ return pos;   }  //���� �߽� ����
	CPoint2  GetVec()		{ return vec;	}  //���� ���� ����
	SDL_Rect GetRect()	    { return r_ball;} 

	SDL_Surface* GetSurface() { return s_ball; }
	
};

#endif
