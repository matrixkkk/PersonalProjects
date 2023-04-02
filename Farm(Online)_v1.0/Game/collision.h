#ifndef COLLISION_H_
#define COLLISION_H_

#include "ball.h"
#include "item.h"
#include "block.h"
#include "SDL/SDL.h"

//���� Bar�� �浹 üũ�� ���� �Լ� ��ü
class Check_Bar
{
private:
	SDL_Rect	bar;
	Mix_Chunk	*soundEffect;

public:
	Check_Bar(SDL_Rect &b,Mix_Chunk *effect):bar(b),soundEffect(effect){
	}
	//���� bar�� �浹 üũ
	void operator ()(CBall &ball);  //�� �� �Լ��� ball�� ���ҷ� �ϴ� �����̳ʿ� ���̴� �Լ� ��ü��
	//bar�� ������ �浹 üũ
	bool operator() (Citem *item);	//
	bool operator()	(CBlock &block);	// �׳� �Լ� ��ü ��Ȱ��
};

//��ֹ� �� �浿 üũ �ϴ� �Լ� ��ü
class obstacle
{
private:
	SDL_Rect ob;
public:
	obstacle(SDL_Rect &o): ob(o){
	}
	bool operator () (CBall &ball)   //�̰� ���
	{
		if( collision(ob,ball.GetRect()))
		{
			return true;
		}
		else 
			return false; //�̰� ��� ��Ű? ������
	}
};


#endif
