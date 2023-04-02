#ifndef _COMMON_H_

#define _COMMON_H_

#include <math.h>
#include <SDL/SDL.h>
#include <windows.h>

#define RATE  0.999864 //������Һ���  y
#define RATE_w 0.999515 //���� ��� ���� x

#define R(x) (3.14/180*x) //degree-> radian   ��ȯ ��ũ��
#define FPM 25	//Fix per meter
#define Km(x) x*1000/3600 //km/s �� m/s�� ��ȯ

//#define TEST //  TEST�� ��� ��ũ��

#define xvL 0  //����Ʈ�� x �ּҰ�
#define xvR 800 //����Ʈ�� x �ִ밪
#define yvB 600 //����Ʈ�� y �ּҰ�
#define yvT 0   //����Ʈ�� y �ִ밪

#define xwL -400 //������ ��Ʈ�� x �ּҰ�
#define xwR 400  //������ ��Ʈ�� x �ִ밪
#define ywB 0    //������ ��Ʈ�� y �ּҰ�
#define ywT 1800 //������ ��Ʈ�� y �ִ밪

//////////���� ġ�� ������//////
#define USERHIT 0	//user�� ���� ���
#define COMHIT  1	//com�� ���� ���
#define USERHITSERVE 2 //user�� serve �ϴ� ���
#define COMHITSERVE 3 //Com�� serve �ϴ� ���
#define USERRIGHTHIT 4
#define COMRIGHTHIT 5
#define USERLEFTHIT 6
#define COMLEFTHIT 7
#define USERLEFTDIVEHIT 8
#define COMLEFTDIVEHIT 9
#define USERRIGHTDIVEHIT 10
#define COMRIGHTDIVEHIT 11
#define USERSMASH 12
#define COMSMASH 13




//////////�÷��̾� �ൿ ////////
//#define STAND 0
//#define STROKE 1

//////////////////////////
//////���� ���� ��ũ��////
//////////////////////////
#define USERSERVE 1
#define COMSERVE 2
#define PLAYING 0

//////////////////////////
////Timer signal ����///////
////////////////////////
#define IT_USER_WIN 1   //user�� 1������.
#define IT_COM_WIN 2	//com�� 1������.
#define IT_USER_FAULT 3  //user fault
#define IT_USER_DOUBLE_FAULT 4
#define IT_COM_FAULT 5
#define IT_COM_DOUBLE_FAULT 6


struct CPoint3 //3 ��ǥ ü��
{
	float x; //x��ǥ
	float y; //y��ǥ
	float z; //����
};



struct ball_physics // ���� ���� ��� ����ü
{
	float v;
	int height_degree;
	int width_degree;
};

CPoint3 transCoordinate(int a,int b); //���� ��ǥ -> ������ ��ǥ
CPoint3 win_view(int x,int y);        //������ - ����Ʈ ��ȯ
void SetSDLRect(SDL_Rect *rect,int x,int y,int w,int h); //SDL_Rect �ʱ�ȭ
bool check_collision( SDL_Rect A, SDL_Rect B ); //�ٿ�� �ڽ� �浹 üũ
//void CorrectAngle(coordinate3 center, coordinate3 lhs, coordinate3 &rhs, int leftLine, int rightLine);

#endif
