#ifndef TURTLE_H_
#define TURTLE_H_

//#include "common.h"
#include "CTimer.h"

//��ֹ� �ź��� : 
class CTurtle
{
	SDL_Surface		*animationSheet;		//�ִϸ��̼� ��Ʈ
	SDL_Surface		*screen;				//�׸��� ���� �� ǥ��
	
	SDL_Rect		posRect;				//�ź����� ��ġ
	
	bool			alive;					//true ��� ���� false �׾� ����
	int				frame;					
	int				animationFrame;			//�ִϸ��̼� ������
	int				movement;				//�̵�����
	double			duration;				//���ӽð�
	
	CTimer timer;							//Ÿ�̸� Ŭ����
	
public:
	void Init(SDL_Surface *pScreen);					//�ʱ�ȭ
	void Move();										//�ź��� �̵�
	void Draw();
	void CleanUp();	
	
	SDL_Rect	GetRect()			{ return posRect; }
	bool		GetLive()			{ return alive;	  }
	
	void		SetLive(bool live);  

	vector<int>		ball_id;				//�浹�� �� ID �����̳�
};

#endif
