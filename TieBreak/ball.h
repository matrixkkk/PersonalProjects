#ifndef BALL_H
#define BALL_H

#include <SDL/SDL.h>
#include "common.h"
//#include "sprite.h"
#include "Player.h"

class CBall
{
	SDL_Surface* image;  //�� �̹����� �׸� ǥ��
	SDL_Surface* shadow; //�׸��� �̹����� �׸� ǥ��

	CPoint3 ball_position; //���� ��ġ ��ǥ
	CPoint3 shadow_position; //�׸����� ��ġ ��ǥ
	CPoint3 shadow_vector; //�׸����� ����
	CPoint3 original_spot; //���� ����
	SDL_Rect ball; //��
	SDL_Rect ball_Bound;
    CPoint3 SkillPos;
	//SDL_Rect ball_BoundShadow;
	int r;		   // ���� ������
	ball_physics quant;
	float Rapid; //���� ȸ��
	float time; //�ð��� ����
	int BallDir;	//���� ����  0:���� ���� 1: 1p->2p  2: 2p->1p
	bool serveok;   //���񽺸� ġ�°� ������ ��.
	bool Service;   //true : ���� �� false : ���� ��.
	int bounce;
	bool Skill;
public:
	CBall::CBall();
	CBall::~CBall();
	void init(int x,int y,int z); //���� ��ġ �ʱ�ȭ
	void draw();
	void get_v(ball_physics fm);//float v,int height_degree,int width_degree); //���� ���� ���� ��
	void ball_movement(); //���� �̵�
	void Ball_collision( CPlayer *user, CPlayer *com, int state );	//���� �÷��̾���� �浹 üũ
	int GetBallDir() { return BallDir;}		 //���� ���� ����
	CPoint3 GetBallPos(){ return ball_position;} //���� ��ġ ����
	bool GetServiceState(){ return Service;}	 //���� ������ ������ üũ
	void SetService(bool n){ Service=n;}
	void SetServeok(bool n){ serveok=n;}
	void Clear(){ ball.x=3000; bounce=0;}					//���� �ܻ� ����
	int GetBounce(){ return bounce; }	//���� �ٿ�� �� ����
	SDL_Rect GetBallShadowBounceBox() { return ball_Bound;} //������ �ٿ�� �ڽ� ����
	SDL_Rect GetBallBounceBox() { return ball;}
	void SetTime(float t){ time=t; } //�ð� ����(�߰�)
	void SetBounce() { bounce++;}
	ball_physics GetBallPhysics() { return quant; }		//���� ������ ����.(�߰�)
	CPoint3 Getoriginal_spot() { return original_spot;} //���� �ʱ� ��ġ ����(�߰�)
	void BounceReset(){ bounce=0; } //�ٿ �ʱ�ȭ (�߰�)


};

#endif
