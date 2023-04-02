#ifndef _COMMON_H_

#define _COMMON_H_

#include <math.h>
#include <SDL/SDL.h>
#include <windows.h>

#define RATE  0.999864 //세로축소비율  y
#define RATE_w 0.999515 //가로 축소 비율 x

#define R(x) (3.14/180*x) //degree-> radian   변환 매크로
#define FPM 25	//Fix per meter
#define Km(x) x*1000/3600 //km/s 를 m/s로 변환

//#define TEST //  TEST용 상수 메크로

#define xvL 0  //뷰포트의 x 최소값
#define xvR 800 //뷰포트의 x 최대값
#define yvB 600 //뷰포트의 y 최소값
#define yvT 0   //뷰포트의 y 최대값

#define xwL -400 //윈도우 포트의 x 최소값
#define xwR 400  //윈도우 포트의 x 최대값
#define ywB 0    //윈도우 포트의 y 최소값
#define ywT 1800 //윈도우 포트의 y 최대값

//////////공을 치는 구분자//////
#define USERHIT 0	//user가 쳤을 경우
#define COMHIT  1	//com이 쳤을 경우
#define USERHITSERVE 2 //user가 serve 하는 경우
#define COMHITSERVE 3 //Com가 serve 하는 경우
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




//////////플레이어 행동 ////////
//#define STAND 0
//#define STROKE 1

//////////////////////////
//////게임 상태 메크로////
//////////////////////////
#define USERSERVE 1
#define COMSERVE 2
#define PLAYING 0

//////////////////////////
////Timer signal 종류///////
////////////////////////
#define IT_USER_WIN 1   //user가 1득점함.
#define IT_COM_WIN 2	//com이 1득점함.
#define IT_USER_FAULT 3  //user fault
#define IT_USER_DOUBLE_FAULT 4
#define IT_COM_FAULT 5
#define IT_COM_DOUBLE_FAULT 6


struct CPoint3 //3 좌표 체계
{
	float x; //x좌표
	float y; //y좌표
	float z; //높이
};



struct ball_physics // 공의 물리 요소 구조체
{
	float v;
	int height_degree;
	int width_degree;
};

CPoint3 transCoordinate(int a,int b); //논리적 좌표 -> 물리적 좌표
CPoint3 win_view(int x,int y);        //윈도우 - 뷰포트 변환
void SetSDLRect(SDL_Rect *rect,int x,int y,int w,int h); //SDL_Rect 초기화
bool check_collision( SDL_Rect A, SDL_Rect B ); //바운딩 박스 충돌 체크
//void CorrectAngle(coordinate3 center, coordinate3 lhs, coordinate3 &rhs, int leftLine, int rightLine);

#endif
