#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mmsystem.h>
#include "resource.h"
//POINT com_xy;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM );
HINSTANCE g_hInst;
LPSTR lpszClass="Smash";
POINT stack;
typedef struct item_list{
	int left;
	int right;
	int top;
	int bottom;
	int item;
}ITEM;

void DrawBitmap(HDC hDC,POINT p,HBITMAP hBit,RECT rect) //비트맵  함수
{
	HDC MemDC;
	HBITMAP OldBitmap;
	BITMAP bit;
	int x,y;
	
	MemDC=CreateCompatibleDC(hDC);
	OldBitmap=(HBITMAP)SelectObject(MemDC,hBit);
	GetObject(hBit,sizeof(BITMAP),&bit);
	x=bit.bmWidth;
	y=bit.bmHeight;
	
	TransparentBlt(hDC,rect.left,rect.top,x,y,MemDC,p.x,p.y,x,y,RGB(255,0,0));
	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}
void DrawBitmap2(HDC hDC,POINT p,HBITMAP hBit,ITEM item) //비트맵  함수
{
	HDC MemDC;
	HBITMAP OldBitmap;
	BITMAP bit;
	int x,y;
	
	MemDC=CreateCompatibleDC(hDC);
	OldBitmap=(HBITMAP)SelectObject(MemDC,hBit);
	GetObject(hBit,sizeof(BITMAP),&bit);
	x=bit.bmWidth;
	y=bit.bmHeight;
	
	TransparentBlt(hDC,item.left,item.top,x,y,MemDC,p.x,p.y,x,y,RGB(255,0,0));
	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}

POINT vecter(POINT N,POINT D_vector) //반사 벡터 구하는 함수 
{
	double DN_inner_product,NN_inner_product; //D와 N의 내적 N과 N의 내적
	double product;
	POINT temp;
	D_vector.x*=-1,D_vector.y*=-1;
	DN_inner_product=(double)(D_vector.x*N.x+D_vector.y*N.y);
	NN_inner_product=(double)(N.x*N.x+N.y*N.y);
	product=(double)(DN_inner_product/NN_inner_product)*2;
	N.x=product*N.x;
	N.y=product*N.y;
	temp.x=N.x-D_vector.x;
	temp.y=N.y-D_vector.y;
	return temp;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd )
{
 HWND hWnd;                  
 MSG Message;
 WNDCLASS WndClass;         
 g_hInst=hInstance;
 WndClass.cbClsExtra=0; 
 WndClass.cbWndExtra=0; 
 WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH); 
 WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
 WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 WndClass.style=CS_HREDRAW|CS_VREDRAW; 
 WndClass.lpfnWndProc=(WNDPROC)WndProc; 
 WndClass.hInstance=hInstance;
 WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
 WndClass.lpszClassName=lpszClass; 
 RegisterClass(&WndClass);
	   
 hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW 
     ,100
     ,30 
     ,810 
     ,680 
     ,NULL 
     ,(HMENU)NULL 
     ,hInstance 
     ,NULL); 
 
 ShowWindow(hWnd,   
       nShowCmd);
 while(GetMessage(&Message,0,0,0)){	 
	 TranslateMessage(&Message);
	 DispatchMessage(&Message);	 
 }
 
 return Message.wParam;
} 

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage
       , WPARAM wParam, LPARAM lParam)
{
	static RECT table,item1,item2; //table : 테이블 공간
	static RECT Goal1,Goal2; //골대 공간
	static RECT ball[3],create_item; //볼 공간 ,테이블에 생기는 아이템
	static POINT ball_xy[3]; //볼 위치좌표
	static POINT ball_movexy[3],com_movexy;//,power_vector; //볼의 벡터값
	POINT N;
	static POINT P_1,P_2,power_vector; //N: 법선 벡터 P_1:라켓1 벡터 P_2:라켓2의 벡터
	static RECT racket1,racket2;
	static int item_speed=1,contect1=0,contect2=0; //스피드 아이템 contect=볼과 라켓이 여러번 접촉 방지
	static int turn=1,playing=0; //turn=치는 순서,playing=게임중임을 나타내는 스위치 ;
	static int button_left1=0,button_right1=0,button_up1=0,button_down1=0;  //1p의 움직임을 부드럽게 하기위해
	static int button_left2=0,button_right2=0,button_up2=0,button_down2=0; //2p의 움직임을 부드럽게 하기위해
	static char use_item1[20],use_item2[20]; 
	static RECT power_dis_1p,power_dis_2p,score_board;//power_dis 힘을 보여주는 디스플레이 ,score_board 점수판 
	static int score1=0,score2=0,current_item=0; //score1 :1p의 점수 score2: 2p의 점수 current_item: 현재 테이블에 있는 아이템 종류
	static BOOL item=FALSE; //아이템이 생성 됐는지 안됐는지를 보여줌
	//static int item_list1[3]={0,},item_list2[3]={0,};
	static int attack_speed_up=0,attack_triple_ball=0,attack_invisible_ball=0;
	static int attack_control_shock1=0,attack_control_shock2=0,attack_change_orbit=0;
	static int defence_slow_ball=0,defence_shield1=0,defence_shield2=0,defence_change_racket1=0,defence_change_racket2=0,defence_create_obstacle=0;
	static HBITMAP hBit_ball,hBit_racket,hBit_table,hBit_item,hBit_back,hOld_bit;
	static HBITMAP hBit_racket60,hBit_invisi,hBit_shield,hBit_speed,hBit_triple;
	static HBITMAP hBit_control,hBit_obstacle,hBit_orbit,hBit_change,hBit_slow,hBit_create_ob;
	static HBITMAP hBit_create_shield,hBit_background;
	static char str1[20],str2[20],str_score[10];
	static ITEM attack_item1[3],attack_item2[3],defence_item1[3],defence_item2[3];//아이템 디스플레이
	static RECT shield1,shield2,obstacle; //실드 공간과 장애물 공간
	static int obstacle_desti=0; //장애물의 이동 방향 0 ->  1 <-
	static int user=0;// 1:컴퓨터 0: 사람
	int i,j;
	srand(time(NULL));
	POINT p;
	PAINTSTRUCT ps;
	HDC hDC;
	static HDC hBack_buffer;
	RECT temp;
 
	switch(iMessage){  
		case WM_CREATE:
			for(i=0;i<3;i++)
			{
				attack_item1[i].item=attack_item2[i].item=0;
				defence_item1[i].item=defence_item2[i].item=0;
			}
			hBit_ball=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP1));
			hBit_table=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP2));
			hBit_racket=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP3));
			hBit_item=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP4));
			hBit_racket60=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP5));
			hBit_invisi=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP6));
			hBit_shield=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP7));
			hBit_speed=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP8));
			hBit_triple=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP9));
			hBit_control=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP10));
			hBit_obstacle=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP11));
			hBit_orbit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP12));
			hBit_change=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP13));
			hBit_slow=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP14));
			hBit_create_ob=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP15));
			hBit_create_shield=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP16));
			hBit_background=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP17));
			ball_movexy[0].x=ball_movexy[0].y=0; 
			ball_xy[0].x=ball_xy[0].y=0;
			P_1.x=P_2.y=P_2.x=P_2.y=0;
			SetTimer(hWnd,5,30,NULL);
			SetRect(&table,200,100,600,550);			
			SetRect(&item1,640,480,790,640);
			SetRect(&item2,10,10,190,100);
			SetRect(&Goal1,300,550,500,560);
			SetRect(&Goal2,300,90,500,100);
			SetRect(&racket1,380,510,420,550);
			SetRect(&racket2,380,100,420,140);
			SetRect(&power_dis_2p,10,120,125,140);
			SetRect(&power_dis_1p,640,450,750,470);
			SetRect(&score_board,620,320,670,340);
			attack_item2[0].left=13,attack_item2[0].top=44;
			attack_item2[0].right=43,attack_item2[0].bottom=74;
			attack_item2[1].left=45,attack_item2[1].top=44;
			attack_item2[1].right=75,attack_item2[1].bottom=74;
			attack_item2[2].left=78,attack_item2[2].top=44;
			attack_item2[2].right=108,attack_item2[2].bottom=74;
			defence_item2[0].left=13,defence_item2[0].top=76;
			defence_item2[0].right=43,defence_item2[0].bottom=106;
			defence_item2[1].left=45,defence_item2[1].top=76;
			defence_item2[1].right=75,defence_item2[1].bottom=106;
			defence_item2[2].left=78,defence_item2[2].top=76;
			defence_item2[2].right=108,defence_item2[2].bottom=106;
			attack_item1[0].left=643,attack_item1[0].top=514;
			attack_item1[0].right=673,attack_item1[0].bottom=544;
			attack_item1[1].left=675,attack_item1[1].top=514;
			attack_item1[1].right=705,attack_item1[1].bottom=544;
			attack_item1[2].left=708,attack_item1[2].top=514;
			attack_item1[2].right=738,attack_item1[2].bottom=544;
			defence_item1[0].left=643,defence_item1[0].top=546;
			defence_item1[0].right=673,defence_item1[0].bottom=576;
			defence_item1[1].left=675,defence_item1[1].top=546;
			defence_item1[1].right=705,defence_item1[1].bottom=576;
			defence_item1[2].left=708,defence_item1[2].top=546;
			defence_item1[2].right=738,defence_item1[2].bottom=576;
			SetRect(&shield1,200,500,600,510);
			SetRect(&shield2,200,150,600,160);
			SetRect(&obstacle,380,305,420,345);
			SetTimer(hWnd,8,6000,NULL);//아이템 생성 타이머			
			break;
		case WM_PAINT:
				hDC=GetDC(hWnd);
				hBack_buffer=CreateCompatibleDC(hDC);//임시 버퍼 생성
				GetClientRect(hWnd,&temp);
				p.x=p.y=0;
				hDC=BeginPaint(hWnd,&ps);
				if(hBit_back==NULL)
				{
					hBit_back=CreateCompatibleBitmap(hDC,temp.right,temp.bottom); //생성 윈도우 크기에 맞는 비트맵 생성
				}
				hOld_bit=(HBITMAP)SelectObject(hBack_buffer,hBit_back);
				//FillRect(hBack_buffer,&temp,GetSysColorBrush(COLOR_WINDOW));
				DrawBitmap(hBack_buffer,p,hBit_background,temp);
				if(turn==1)//1p 선공 일때 볼의 위치
				{
					if(playing==0)
					{
						ball_xy[0].x=550;
						ball_xy[0].y=500;
						ball_movexy[0].x=-2;
						ball_movexy[0].y=-2;
						playing=1; //임시
					}                   								
				}
				else//2p 선공일 때 볼의 위치 
				{
					if(playing==0)
					{
						ball_xy[0].x=250;
						ball_xy[0].y=150;
						ball_movexy[0].x=2;
						ball_movexy[0].y=2;
						playing=1; //임시
					}					
				}
				if(attack_triple_ball) //트리플 볼 실행
				{					
					SetRect(&ball[1],ball_xy[1].x-15,ball_xy[1].y-15,ball_xy[1].x+15,ball_xy[1].y+15);
					SetRect(&ball[2],ball_xy[2].x-15,ball_xy[2].y-15,ball_xy[2].x+15,ball_xy[2].y+15);
				}
							
				if(racket1.top<325) racket1.top=325,racket1.bottom=365;//라켓의 이동 범위 제한
				if(racket1.left<200) racket1.left=200,racket1.right=240;
				if(racket1.right>600) racket1.right=600,racket1.left=560;
				if(racket1.bottom>550) racket1.bottom=550,racket1.top=510;
				if(racket2.top<100) racket2.top=100,racket2.bottom=140;
				if(racket2.left<200) racket2.left=200,racket2.right=240;
				if(racket2.right>600) racket2.right=600,racket2.left=560;
				if(racket2.bottom>325) racket2.bottom=325,racket2.top=285;
				
				if(IntersectRect(&temp,&ball[0],&racket1)==1 && contect1==0)//1p 라켓과 공이 겹칠 때
				{	
					sndPlaySound("ting.wav",SND_ASYNC);
				/*	if(button_left1==1) P_1.x-=7,P_1.y=0;//라켓의 힘벡터
					if(button_right1==1) P_1.x+=7,P_1.y=0;
					if(button_up1==1) P_1.y-=7,P_1.x=0;
					if(button_down1==1) P_1.y+=7,P_1.x=0;
					if(button_left1==1 && button_up1==1) P_1.x+=2,P_1.y+=2;
					else if(button_right1==1 && button_up1==1) P_1.x-=2,P_1.y+=2;
					else if(button_left1==1 && button_down1==1) P_1.x+=2,P_1.y-=2;
					else if(button_right1==1 && button_down1==1) P_1.x-=2,P_1.y-=2;
					*/
					N.x=(racket1.left+20-ball_xy[0].x)*-1;//법선 벡터
					N.y=(racket1.top+20-ball_xy[0].y)*-1;
					power_vector.x=N.x/10;
					power_vector.y=N.y/10;
					if(power_vector.x>=0) power_vector.x+=2;
					else power_vector.x-=2;
					if(power_vector.y>=0) power_vector.y+=2;
					else power_vector.y-=2;
								
					contect1=1;
					contect2=0;
					ball_movexy[0]=vecter(N,ball_movexy[0]);
					
					turn=1;//1p가 친것을 나타냄
				
					ball_movexy[0].x=ball_movexy[0].x+power_vector.x;
					ball_movexy[0].y=ball_movexy[0].y+power_vector.y;//볼의 방향과 힘의 방향의 합벡터				
					
					if(attack_speed_up) //스피드 볼 아이템 효과 끄기 
					{
						SetTimer(hWnd,5,30,NULL);
						attack_speed_up=0;
					}	
					if(attack_triple_ball) //트리플 볼 아이템 효과 끄기 
					{
						attack_triple_ball=0;
					}
					if(defence_change_racket1) 
					{
						defence_change_racket1=0;						
						SetRect(&racket1,racket1.left+10,racket1.top+10,racket1.right-10,racket1.bottom-10);
					}
				}
				else if(IntersectRect(&temp,&ball[0],&racket2)==1 && contect2==0)//2p 라켓과 공이 겹칠 때
				{					
					sndPlaySound("ting.wav",SND_ASYNC);
					/*if(button_left2==1) P_2.x-=7,P_2.y=0; //라켓의 힘 벡터 
					else if(button_right2==1) P_2.x+=7,P_2.y=0;
					else if(button_up2==1) P_2.y-=7,P_2.x=0;
					else if(button_down2==1) P_2.y+=7,P_2.x=0;
					if(button_left2==1 && button_up2==1) P_2.x+=2,P_2.y+=2;
					else if(button_right2==1 && button_up2==1) P_2.x-=2,P_2.y+=2;
					else if(button_left2==1 && button_down2==1) P_2.x+=2,P_2.y-=2;
					else if(button_right2==1 && button_down2==1) P_2.x-=2,P_2.y-=2;*/
					N.x=(racket2.left+20-ball_xy[0].x)*-1; //N 법선 벡터 
					N.y=(racket2.top+20-ball_xy[0].y)*-1;
					
					power_vector.x=N.x/10;
					power_vector.y=N.y/10;
					if(power_vector.x>=0) power_vector.x+=2;
					else power_vector.x-=2;
					if(power_vector.y>=0) power_vector.y+=2;
					else power_vector.y-=2;
					
					contect1=0;
					contect2=1;
					ball_movexy[0]=vecter(N,ball_movexy[0]);
					turn=2;//2p가 친것을 나타냄					
					ball_movexy[0].x=ball_movexy[0].x+power_vector.x;
					ball_movexy[0].y=ball_movexy[0].y+power_vector.y;					
				
					if(attack_speed_up) //스피드 볼 아이템 효과 끄기 
					{
						SetTimer(hWnd,5,30,NULL);
						attack_speed_up=0;
					}
					if(attack_triple_ball) //트리플 볼 아이템 효과 끄기 
					{
						attack_triple_ball=0;
					}	
					if(defence_change_racket2) 
					{
						defence_change_racket2=0;
						SetRect(&racket2,racket2.left+10,racket2.top+10,racket2.right-10,racket2.bottom-10);
					}
				}
				else
				{
					contect1=0;
					contect2=0;
				}
				if(IntersectRect(&temp,&ball[0],&racket1)==1 || IntersectRect(&temp,&ball[0],&racket2))//공과 라켓이 안겹치게 하기
				{
					ball_xy[0].x+=ball_movexy[0].x*7;
					ball_xy[0].y+=ball_movexy[0].y*7;			
				}			
				if(attack_speed_up==1) //스피드 볼 아이템 효과 
				{						
					SetTimer(hWnd,5,1,NULL);
				}
				if(ball_xy[0].x<=220)//공이 테이블 안에서만 놀게 ㅋ
				{
					contect1=0;
					contect2=0;
					ball_xy[0].x=220;
					ball_movexy[0].x=-ball_movexy[0].x;
					sndPlaySound("ting.wav",SND_ASYNC);
				}
				else if(ball_xy[0].x>=580)
				{	
					contect1=0;
					contect2=0;
					ball_xy[0].x=580;
					ball_movexy[0].x=-ball_movexy[0].x;
					sndPlaySound("ting.wav",SND_ASYNC);
				}
				if(ball_xy[0].y<=100)//테이블 위 
				{
					if(Goal2.left+15<=ball_xy[0].x && Goal2.right-15>=ball_xy[0].x)//2P 골대 안일 때
					{
						sndPlaySound("goal.wav",SND_ASYNC);
						playing=0,score1++;
						turn=2;
						ball_movexy[0].x=ball_movexy[0].y=0;
						defence_create_obstacle=0; //골이 들어가면 장애물 사라진다.
					}
					else
					{
						sndPlaySound("ting.wav",SND_ASYNC);
						ball_xy[0].y=100;
						ball_movexy[0].y=-ball_movexy[0].y;
						if(ball_movexy[0].x>0) ball_movexy[0].x-=1;
						else ball_movexy[0].x+=1;
						if(ball_movexy[0].y>0) ball_movexy[0].y-=1;
						else ball_movexy[0].y+=1;
					}
					if(attack_speed_up==1 && turn==1) //스피드 볼 아이템 효과 끄기 
					{
						SetTimer(hWnd,5,30,NULL);
						attack_speed_up=0;
					}
					contect1=0;
					contect2=0;						
				}
				else if(ball_xy[0].y>=550)//테이블 아래 
				{
					if(Goal1.left+15<=ball_xy[0].x && Goal1.right-15>=ball_xy[0].x)//1P 골대 안일 때
					{
						sndPlaySound("goal.wav",SND_ASYNC);
						playing=0,score2++;
						turn=1;
						ball_movexy[0].x=ball_movexy[0].y=0;
						defence_create_obstacle=0; //골이 들어가면 장애물 사라진다.
					}						
					else 
					{
						sndPlaySound("ting.wav",SND_ASYNC);
						ball_xy[0].y=540;
						ball_movexy[0].y=-ball_movexy[0].y;
						if(ball_movexy[0].x>0) ball_movexy[0].x-=1;
						else ball_movexy[0].x+=1;
						if(ball_movexy[0].y>0) ball_movexy[0].y-=1;
						else ball_movexy[0].y+=1;
					}
					contect1=0;
					contect2=0;
					if(attack_speed_up==1 && turn==2) //스피드 볼 아이템 효과 끄기 
					{
						SetTimer(hWnd,5,30,NULL);
						attack_speed_up=0;
					}
				}					
				if(IntersectRect(&temp,&ball[0],&create_item)==1 && item==TRUE) //공과 아이템이 접촉 할 때
				{
					sndPlaySound("item.wav",SND_ASYNC);
					if(current_item<6)//공격 아이템 일 경우
					{
						for(i=0;i<3;i++)
						{
							if(turn==1)
							{
								if(attack_item1[i].item==0) 
								{
									attack_item1[i].item=current_item;
									break;
								}
							}
							else
							{
								if(attack_item2[i].item==0)
								{
									attack_item2[i].item=current_item;
									break;
								}
							}
						}
					}
					else
					{
						for(i=0;i<3;i++)
						{
							if(turn==1)
							{
								if(defence_item1[i].item==0)
								{
									defence_item1[i].item=current_item;
									break;
								}
							}					
							else
							{
								if(defence_item2[i].item==0) 
								{
									defence_item2[i].item=current_item;
									break;
								}
							}
						}
					}
					item=FALSE;
				}
				SetRect(&ball[0],ball_xy[0].x-15,ball_xy[0].y-15,ball_xy[0].x+15,ball_xy[0].y+15);
				Rectangle(hBack_buffer,power_dis_1p.left,power_dis_1p.top,power_dis_1p.right,power_dis_1p.bottom);
				Rectangle(hBack_buffer,power_dis_2p.left,power_dis_2p.top,power_dis_2p.right,power_dis_2p.bottom);
				TextOut(hBack_buffer,power_dis_1p.left+5,power_dis_1p.top+3,use_item1,lstrlen(use_item1));
				TextOut(hBack_buffer,power_dis_2p.left+5,power_dis_2p.top+3,use_item2,lstrlen(use_item2));
				SetRect(&temp,table.left-15,table.top-15,table.right,table.bottom);			
				DrawBitmap(hBack_buffer,p,hBit_table,temp);
				if(attack_invisible_ball==0) DrawBitmap(hBack_buffer,p,hBit_ball,ball[0]);
				if(attack_triple_ball) //트리플 볼 그림 띄우기
				{
					DrawBitmap(hBack_buffer,p,hBit_ball,ball[1]);
					DrawBitmap(hBack_buffer,p,hBit_ball,ball[2]);
				}
				if(defence_create_obstacle) //장애물 생성 시 
				{
					DrawBitmap(hBack_buffer,p,hBit_create_ob,obstacle);					
					if(IntersectRect(&temp,&ball[0],&obstacle)) //공과 장애물이 접촉 시
					{
						sndPlaySound("ting.wav",SND_ASYNC);
						ball_movexy[0].x=-ball_movexy[0].x;
						ball_movexy[0].y=-ball_movexy[0].y;
					}
					if(IntersectRect(&temp,&ball[0],&obstacle))//공과 장애물이 안겹치게 하기
					{
						ball_xy[0].x+=ball_movexy[0].x*7;
						ball_xy[0].y+=ball_movexy[0].y*7;			
					}
				}
				DrawBitmap(hBack_buffer,p,hBit_item,item1);
				DrawBitmap(hBack_buffer,p,hBit_item,item2);
				for(i=0;i<3;i++)//아이템 디스플레이 
				{
					if(attack_item1[i].item!=0) 
					{	switch(attack_item1[i].item)
						{
							case 1: DrawBitmap2(hBack_buffer,p,hBit_speed,attack_item1[i]);
								break;
							case 2: DrawBitmap2(hBack_buffer,p,hBit_triple,attack_item1[i]);
								break;
							case 3: DrawBitmap2(hBack_buffer,p,hBit_invisi,attack_item1[i]);
								break;
							case 4: DrawBitmap2(hBack_buffer,p,hBit_control,attack_item1[i]);
								break;
							case 5: DrawBitmap2(hBack_buffer,p,hBit_orbit,attack_item1[i]);
								break;
							case 6: DrawBitmap2(hBack_buffer,p,hBit_slow,attack_item1[i]);
								break;
							case 7: DrawBitmap2(hBack_buffer,p,hBit_shield,attack_item1[i]);
								break;
							case 8: DrawBitmap2(hBack_buffer,p,hBit_change,attack_item1[i]);
								break;
							case 9: DrawBitmap2(hBack_buffer,p,hBit_obstacle,attack_item1[i]);
								break;
						}
					}						
					if(defence_item1[i].item!=0)
					{
						switch(defence_item1[i].item)
						{
							case 1: DrawBitmap2(hBack_buffer,p,hBit_speed,defence_item1[i]);
								break;
							case 2: DrawBitmap2(hBack_buffer,p,hBit_triple,defence_item1[i]);
								break;
							case 3: DrawBitmap2(hBack_buffer,p,hBit_invisi,defence_item1[i]);
								break;
							case 4: DrawBitmap2(hBack_buffer,p,hBit_control,defence_item1[i]);
								break;
							case 5: DrawBitmap2(hBack_buffer,p,hBit_orbit,defence_item1[i]);
								break;
							case 6: DrawBitmap2(hBack_buffer,p,hBit_slow,defence_item1[i]);
								break;
							case 7: DrawBitmap2(hBack_buffer,p,hBit_shield,defence_item1[i]);
								break;
							case 8: DrawBitmap2(hBack_buffer,p,hBit_change,defence_item1[i]);
								break;
							case 9: DrawBitmap2(hBack_buffer,p,hBit_obstacle,defence_item1[i]);
								break;
						}
					}						 
					if(attack_item2[i].item!=0) 
					{
						switch(attack_item2[i].item)
						{
							case 1: DrawBitmap2(hBack_buffer,p,hBit_speed,attack_item2[i]);
								break;
							case 2: DrawBitmap2(hBack_buffer,p,hBit_triple,attack_item2[i]);
								break;
							case 3: DrawBitmap2(hBack_buffer,p,hBit_invisi,attack_item2[i]);
								break;
							case 4: DrawBitmap2(hBack_buffer,p,hBit_control,attack_item2[i]);
								break;
							case 5: DrawBitmap2(hBack_buffer,p,hBit_orbit,attack_item2[i]);
								break;
							case 6: DrawBitmap2(hBack_buffer,p,hBit_slow,attack_item2[i]);
								break;
							case 7: DrawBitmap2(hBack_buffer,p,hBit_shield,attack_item1[i]);
								break;
							case 8: DrawBitmap2(hBack_buffer,p,hBit_change,attack_item2[i]);
								break;
							case 9: DrawBitmap2(hBack_buffer,p,hBit_obstacle,attack_item2[i]);
								break;
						}
					}					
					if(defence_item2[i].item!=0) 
					{
						switch(defence_item2[i].item)
						{
							case 1: DrawBitmap2(hBack_buffer,p,hBit_speed,defence_item2[i]);
								break;
							case 2: DrawBitmap2(hBack_buffer,p,hBit_triple,defence_item2[i]);
								break;
							case 3: DrawBitmap2(hBack_buffer,p,hBit_invisi,defence_item2[i]);
								break;
							case 4: DrawBitmap2(hBack_buffer,p,hBit_control,defence_item2[i]);
								break;
							case 5: DrawBitmap2(hBack_buffer,p,hBit_orbit,defence_item2[i]);
								break;
							case 6: DrawBitmap2(hBack_buffer,p,hBit_slow,defence_item2[i]);
								break;
							case 7: DrawBitmap2(hBack_buffer,p,hBit_shield,defence_item2[i]);
								break;
							case 8: DrawBitmap2(hBack_buffer,p,hBit_change,defence_item2[i]);
								break;
							case 9: DrawBitmap2(hBack_buffer,p,hBit_obstacle,defence_item2[i]);
								break;
						}
					}					 
				}
				
				
				if(defence_change_racket1) DrawBitmap(hBack_buffer,p,hBit_racket60,racket1);
				else DrawBitmap(hBack_buffer,p,hBit_racket,racket1);
				if(defence_change_racket2) DrawBitmap(hBack_buffer,p,hBit_racket60,racket2);
				else DrawBitmap(hBack_buffer,p,hBit_racket,racket2);
				if(defence_shield1==1) //1p 실드 아이템 사용시 
				{
					DrawBitmap(hBack_buffer,p,hBit_create_shield,shield1);
					if(ball_xy[0].y>495) //공이 실드에 닿았을 때
					{
						sndPlaySound("ting.wav",SND_ASYNC);
						ball_xy[0].y=495;
						ball_movexy[0].y=-ball_movexy[0].y;
						defence_shield1=0;
					}
				}
				else if(defence_shield2==1)
				{
					DrawBitmap(hBack_buffer,p,hBit_create_shield,shield2);
					if(ball_xy[0].y<165) //공이 실드에 닿았을 때
					{
						sndPlaySound("ting.wav",SND_ASYNC);
						ball_xy[0].y=165;
						ball_movexy[0].y=-ball_movexy[0].y;
						defence_shield2=0;
					}

				}				
				if(item)//아이템 생성
				{
					switch(current_item)
					{
						case 1: DrawBitmap(hBack_buffer,p,hBit_speed,create_item);
							break;
						case 2: DrawBitmap(hBack_buffer,p,hBit_triple,create_item);
							break;
						case 3: DrawBitmap(hBack_buffer,p,hBit_invisi,create_item);
							break;
						case 4: DrawBitmap(hBack_buffer,p,hBit_control,create_item);
							break;
						case 5: DrawBitmap(hBack_buffer,p,hBit_orbit,create_item);
							break;
						case 6: DrawBitmap(hBack_buffer,p,hBit_slow,create_item);
							break;
						case 7: DrawBitmap(hBack_buffer,p,hBit_shield,create_item);
							break;
						case 8: DrawBitmap(hBack_buffer,p,hBit_change,create_item);
							break;
						case 9: DrawBitmap(hBack_buffer,p,hBit_obstacle,create_item);
							break;
					}					
				}
				Rectangle(hBack_buffer,score_board.left,score_board.top,score_board.right,score_board.bottom);
				TextOut(hBack_buffer,score_board.left+5,score_board.top-20,"1p  2p",6);
				sprintf(str_score,"%d : %d",score1,score2);
				TextOut(hBack_buffer,score_board.left+5,score_board.top+3,str_score,lstrlen(str_score));			
				BitBlt(hDC,0,0,800,600,hBack_buffer,0,0,SRCCOPY);			
				
				if(score1>=10 || score2>=10) //10점을 내서 끝나는 경우 
				{
					KillTimer(hWnd,5);
					KillTimer(hWnd,10);
					if(score1>=10)
					{
						if(MessageBox(hWnd,"다시 하시겠습니까?","1p 승리",MB_YESNO)==IDYES)
						{
							SetTimer(hWnd,5,30,NULL);
							if(user==1) SetTimer(hWnd,10,1,NULL);
							SendMessage(hWnd,273,40003,0);
						}
						else
						{
							SendMessage(hWnd,2,0,0);
						}
					}
					else
					{
						if(MessageBox(hWnd,"다시 하시겠습니까?","2p 승리",MB_YESNO)==IDYES)
						{
							SetTimer(hWnd,5,30,NULL);
							if(user==1) SetTimer(hWnd,10,1,NULL);
							SendMessage(hWnd,273,40003,0);
						}
						else
						{
							SendMessage(hWnd,2,0,0);
						}
					}
				}
				DeleteObject(hOld_bit);
				DeleteDC(hBack_buffer);
				EndPaint(hWnd,&ps);
				break;
 
		case WM_DESTROY:
				KillTimer(hWnd,1);
				KillTimer(hWnd,2);
				KillTimer(hWnd,3);
				KillTimer(hWnd,4);
				KillTimer(hWnd,5);
				KillTimer(hWnd,6);
				KillTimer(hWnd,7);
				KillTimer(hWnd,8);
				KillTimer(hWnd,9);
				KillTimer(hWnd,10);
				DeleteObject(hBit_ball);
				DeleteObject(hBit_table);
				DeleteObject(hBit_racket);
				DeleteObject(hBit_back);
				DeleteObject(hBit_racket60);
				DeleteObject(hBit_invisi);
				DeleteObject(hBit_create_shield);				
				DeleteObject(hBit_speed);
				DeleteObject(hBit_triple);
				DeleteObject(hBit_control);
				DeleteObject(hBit_obstacle);
				DeleteObject(hBit_orbit);
				DeleteObject(hBit_change);
				DeleteObject(hBit_slow);
				DeleteObject(hBit_create_ob);
				DeleteObject(hBit_shield);
				DeleteObject(hBit_background);
				PostQuitMessage(0);
				return 0;
		case WM_TIMER:			
			switch(wParam)
			{
				case 1://1p의 부드러운 움직임					
					if(button_left1==1) //1p의 left키가 눌러진 상태 일 때
					{
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때 
						{
							racket1.left+=5;
							racket1.right+=5;
						}
						else 
						{
							racket1.left-=5;
							racket1.right-=5;
						}
					}
					else if(button_up1==1) //1p의 up키가 눌러진 상태 일 때
					{
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.top+=5;
							racket1.bottom+=5;
						}
						else
						{
							racket1.top-=5;
							racket1.bottom-=5;
						}
					}
					else if(button_right1==1)//1p의 right키가 눌러진 상태 일 때
					{
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.left-=5;
							racket1.right-=5;
						}
						else
						{
							racket1.left+=5;
							racket1.right+=5;
						}
					}
					else if(button_down1==1)//1p의 down키가 눌러진 상태 일 때
					{
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.top-=5;
							racket1.bottom-=5;
						}
						else
						{
							racket1.top+=5;
							racket1.bottom+=5;
						}
					}
					SetRect(&temp,racket1.left-10,racket1.top-10,racket1.right+10,racket1.bottom+10);
					InvalidateRect(hWnd,NULL,FALSE);				
					break;
				case 2:  //1p의 동시 키 입력일 때 부드러운 움직임
					if(button_left1==1 && button_up1==1) //좌측 상 대각선 움직임
					{
						KillTimer(hWnd,1); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.left+=5;
							racket1.right+=5;
							racket1.top+=5;
							racket1.bottom+=5;
						}
						else
						{
							racket1.left-=5;
							racket1.right-=5;
							racket1.top-=5;
							racket1.bottom-=5;						
						}
					}
					else if(button_left1==1 && button_down1==1) //좌측 하 대각선 움직임
					{
						KillTimer(hWnd,1); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.left+=5;
							racket1.right+=5;
							racket1.top-=5;
							racket1.bottom-=5;
						}
						else
						{
							racket1.left-=5;
							racket1.right-=5;
							racket1.top+=5;
							racket1.bottom+=5;						
						}												
					}	
					else if(button_right1==1 && button_up1==1)// 우측 상 대각선 움직임
					{
						KillTimer(hWnd,1); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.left+=5;
							racket1.right+=5;
							racket1.top-=5;
							racket1.bottom-=5;
						}
						else
						{
							racket1.left+=5;
							racket1.right+=5;
							racket1.top-=5;
							racket1.bottom-=5;						
						}										
					}
					else if(button_right1==1 && button_down1==1)// 우측 하 대각선 움직임
					{
						KillTimer(hWnd,1); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock1)//컨트롤 쇼크 실행이 일 때
						{
							racket1.left-=5;
							racket1.right-=5;
							racket1.top-=5;
							racket1.bottom-=5;
						}
						else
						{
							racket1.left+=5;
							racket1.right+=5;
							racket1.top+=5;
							racket1.bottom+=5;						
						}											
					}
					SetRect(&temp,racket1.left-10,racket1.top-10,racket1.right+10,racket1.bottom+10);
					InvalidateRect(hWnd,NULL,FALSE);
					break;
				case 3:    //2p의 부드러운 움직임
					if(button_left2==1) //2p의 left키가 눌러진 상태 일 때
					{
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left+=5;
							racket2.right+=5;
						}
						else
						{
							racket2.left-=5;
							racket2.right-=5;
						}
					}
					else if(button_up2==1) //1p의 up키가 눌러진 상태 일 때
					{
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.top+=5;
							racket2.bottom+=5;
						}
						else
						{
							racket2.top-=5;
							racket2.bottom-=5;
						}
					}
					else if(button_right2==1)//1p의 right키가 눌러진 상태 일 때
					{
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left-=5;
							racket2.right-=5;
						}
						else
						{
							racket2.left+=5;
							racket2.right+=5;
						}
					}
					else if(button_down2==1)//1p의 down키가 눌러진 상태 일 때
					{
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.top-=5;
							racket2.bottom-=5;
						}
						racket2.top+=5;
						racket2.bottom+=5;
					}
					SetRect(&temp,racket2.left-10,racket2.top-10,racket2.right+10,racket2.bottom+10);
					InvalidateRect(hWnd,NULL,FALSE);
					break;
				case 4:  //2p의 동시 키 입력일 때 부드러운 움직임
					if(button_left2==1 && button_up2==1) //좌측 상 대각선 움직임
					{
						KillTimer(hWnd,3); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left+=5;
							racket2.right+=5;
							racket2.top+=5;
							racket2.bottom+=5;
						}
						else
						{						
							racket2.left-=5;
							racket2.right-=5;
							racket2.top-=5;
							racket2.bottom-=5;
						}
					}
					else if(button_left2==1 && button_down2==1) //좌측 하 대각선 움직임
					{
						KillTimer(hWnd,3); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left+=5;
							racket2.right+=5;
							racket2.top-=5;
							racket2.bottom-=5;
						}
						else
						{
							racket2.left-=5;
							racket2.right-=5;
							racket2.top+=5;
							racket2.bottom+=5;
						}
					}
					else if(button_right2==1 && button_up2==1)// 우측 상 대각선 움직임
					{
						KillTimer(hWnd,3); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left-=5;
							racket2.right-=5;
							racket2.top+=5;
							racket2.bottom+=5;
						}
						else
						{
							racket2.left+=5;
							racket2.right+=5;
							racket2.top-=5;
							racket2.bottom-=5;
						}
					}
					else if(button_right2==1 && button_down2==1)// 우측 하 대각선 움직임
					{
						KillTimer(hWnd,3); //다른 움직임을 하기 때문에 1번 타이머를 꺼둔다
						if(attack_control_shock2)//컨트롤 쇼크 실행이 일 때
						{
							racket2.left+=5;
							racket2.right+=5;
							racket2.top+=5;
							racket2.bottom+=5;
						}
						else
						{
							racket2.left+=5;
							racket2.right+=5;
							racket2.top+=5;
							racket2.bottom+=5;
						}
					}
					SetRect(&temp,racket2.left-10,racket2.top-10,racket2.right+10,racket2.bottom+10);
					InvalidateRect(hWnd,NULL,FALSE);
					break;
				case 5: //볼의 움직임 타이머
					if(defence_create_obstacle)//장애물 이동
					{
						if(obstacle_desti==0) //오른쪽 이동
						{
							obstacle.left+=1;
							obstacle.right+=1;
						}
						else
						{
							obstacle.left-=1;
							obstacle.right-=1;
						}
						if(obstacle.right>600) obstacle_desti=1;
						else if(obstacle.right<200) obstacle_desti=0;
					}
					if(defence_slow_ball)//슬로우 볼 사용시 
					{
						ball_movexy[0].x=(ball_movexy[0].x)/2; 
						ball_movexy[0].y=(ball_movexy[0].y)/2;
						defence_slow_ball=0;
					}
					if(abs(ball_movexy[0].x)>13) 
					{
						if(ball_movexy[0].x>0) ball_movexy[0].x=16;
						else ball_movexy[0].x=-16;
					}
					if(abs(ball_movexy[0].y)>16)
					{
						if(ball_movexy[0].y>0) ball_movexy[0].y=16;
						else ball_movexy[0].y=-16;
					}
					ball_xy[0].x+=ball_movexy[0].x; //볼의 이동
					ball_xy[0].y+=ball_movexy[0].y;	
					stack.x=ball_xy[0].x;
					stack.y=ball_xy[0].y;
					if(attack_triple_ball) //트리플 볼 이동
					{
						ball_xy[1].x+=ball_movexy[1].x; //볼의 이동
						ball_xy[1].y+=ball_movexy[1].y;
						ball_xy[2].x+=ball_movexy[2].x; //볼의 이동
						ball_xy[2].y+=ball_movexy[2].y;
					}
					P_1.x=P_1.y=P_2.x=P_2.y=0;	
				
					InvalidateRect(hWnd,NULL,FALSE);					
					break;					
				case 8: //아이템 생성
					i=rand()%370+230;
					j=rand()%390+130;
					current_item=rand()%9+1;
					item=TRUE;
					SetRect(&create_item,i-15,j-15,i+15,j+15);
					break;
				case 9: //아이템 사용시 순간 딜레이
					SetTimer(hWnd,5,30,NULL);
					if(attack_change_orbit) ball_movexy[0].x=(ball_movexy[0].x)*-1;						
					KillTimer(hWnd,9);
					attack_invisible_ball=0;//인비지볼 꺼짐					
					attack_change_orbit=0;
					break;
				case 6: //1p컨트롤 쇼크 지속 시간 
					attack_control_shock1=0;
					break;
				case 7: //2p컨트롤 쇼크 지속 시간
					attack_control_shock2=0;
					break;				
				case 10: //com 컨트롤 이동
					for(i=0;i<3;i++)
					{
						if(attack_item2[i].item!=0)
						{
							if(ball_xy[i].y>300 && turn==2 && ball_movexy[0].x>0)
							{
								switch(i)
								{
									case 0: SendMessage(hWnd,256,81,1048577);
										break;
									case 1: SendMessage(hWnd,256,87,1114113);
										break;
									case 2: SendMessage(hWnd,256,69,1179649);
										break;
								}
							}
						}
						if(defence_item2[i].item!=0)
						{
							if(ball_movexy[0].y<0 && ball_xy[0].y<300)
							{
								switch(i)
								{
									case 0: SendMessage(hWnd,256,65,1966081);
										break;
									case 1: SendMessage(hWnd,256,83,2031617);
										break;
									case 2: SendMessage(hWnd,256,68,2097153);
										break;
								}
							}
						}
					}
					if(ball_movexy[0].y<0 && ball_xy[0].y<300)
					{
						if(ball_xy[0].x>racket2.left+20)
						{
							if((ball_xy[0].x-racket2.left+20)>0)
							{
								racket2.left+=7;
								racket2.right+=7;
							}
						}
						else
						{
							if((racket2.left+20-ball_xy[0].x)>0)
							{
								racket2.left-=7;
								racket2.right-=7;
							}
						}
					}
					if(ball_movexy[0].y>0)
					{
						p.x=400;
						p.y=150;
						if(racket2.left+20-p.x>0)
						{
							racket2.left-=5;
							racket2.right-=5;
						}
						if(racket2.top+20-p.y>0)
						{
							racket2.top-=5;
							racket2.bottom-=5;
						}
					}
					break;
			}				
			break;
		case WM_KEYUP:
			switch(wParam)
			{
				case VK_LEFT:					
					if(button_up1==0 && button_down1==0 && button_right1==0) KillTimer(hWnd,1);
					else SetTimer(hWnd,1,10,NULL);		//if를 쓰는 이유는 left버튼을 누른 상태에서 동시에  
					button_left1=0;						//else 의 셋타이머는 대각이동 중 둘중 하나만 키업인 상태 일때를 위해임
					KillTimer(hWnd,2);//up을 누른후 바로 left버튼을 땠을때 up이 되게 하기 위함					
					break;
				case VK_UP:
					if(button_left1==0 && button_down1==0 && button_right1==0) KillTimer(hWnd,1);
					else SetTimer(hWnd,1,10,NULL);
					KillTimer(hWnd,2);										
					button_up1=0;
					break;
				case VK_RIGHT:
					if(button_up1==0 && button_left1==0 && button_down1==0) KillTimer(hWnd,1);					
					else SetTimer(hWnd,1,10,NULL);
					KillTimer(hWnd,2);										
					button_right1=0;
					break;
				case VK_DOWN:
					if(button_up1==0 && button_left1==0 && button_right1==0) KillTimer(hWnd,1);
					else SetTimer(hWnd,1,10,NULL);
					KillTimer(hWnd,2);					
					button_down1=0;				
					break;
				case 'J':					
					if(button_up2==0 && button_down2==0 && button_right2==0) KillTimer(hWnd,3);
					else SetTimer(hWnd,3,10,NULL);		//if를 쓰는 이유는 left버튼을 누른 상태에서 동시에  
					button_left2=0;						//else 의 셋 타이머는 대각이동 중 둘중 하나만 키업인 상태 일때를 위해임
					KillTimer(hWnd,4);//up을 누른후 바로 left버튼을 땠을때 up이 되게 하기 위함				
					break;
				case 'I':
					if(button_left2==0 && button_down2==0 && button_right2==0) KillTimer(hWnd,3);
					else SetTimer(hWnd,3,10,NULL);
					KillTimer(hWnd,4);									
					button_up2=0;
					break;
				case 'L':
					if(button_up2==0 && button_left2==0 && button_down2==0) KillTimer(hWnd,3);					
					else SetTimer(hWnd,3,10,NULL);
					KillTimer(hWnd,4);									
					button_right2=0;
					break;
				case 'K':
					if(button_up2==0 && button_left2==0 && button_right2==0) KillTimer(hWnd,3);
					else SetTimer(hWnd,3,10,NULL);
					KillTimer(hWnd,4);										
					button_down2=0;
					break;
			}
			break;
		
		case WM_COMMAND:
			switch(wParam)
			{
				case ID_MENU1: //새 게임
					playing=0,turn=1;
					contect1=0,contect2=0;					
					score1=score2=0;
					attack_speed_up=0;
					attack_triple_ball=0;
					attack_change_orbit=0;
					attack_control_shock1=0;
					attack_control_shock2=0;
					attack_invisible_ball=0;
					defence_slow_ball=0;
					defence_shield1=0;
					defence_shield2=0;
					defence_create_obstacle=0;
					ball_movexy[0].x=ball_movexy[0].y=0;
					SetRect(&racket1,380,510,420,550);
					SetRect(&racket2,380,100,420,140);
					for(i=0;i<3;i++)
					{
						attack_item1[i].item=0;
						attack_item2[i].item=0;
						defence_item1[i].item=0;
						defence_item2[i].item=0;
					}
					break;
				case ID_MENU2:
					KillTimer(hWnd,1);
					KillTimer(hWnd,2);
					KillTimer(hWnd,3);
					KillTimer(hWnd,4);
					KillTimer(hWnd,5);
					KillTimer(hWnd,6);
					KillTimer(hWnd,7);
					KillTimer(hWnd,8);
					KillTimer(hWnd,9);
					DeleteObject(hBit_ball);
					DeleteObject(hBit_table);
					DeleteObject(hBit_racket);
					DeleteObject(hBit_back);				
					PostQuitMessage(0);
					break;
				case ID_MENU4: user=0;
					KillTimer(hWnd,10);
					SendMessage(hWnd,273,40003,0);
					break;
				case ID_MENU5: user=1;
					SetTimer(hWnd,10,1,NULL);//com 이동 타이머
					SendMessage(hWnd,273,40003,0);
					break;
			}
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 'J':if(button_left2==0){					
					button_left2=1;				
					SetTimer(hWnd,3,10,NULL);									
					if( GetAsyncKeyState('I') & 0x8000 ) //2p left의 전 키가 Up일 때
					{						
						SetTimer(hWnd,4,10,NULL); //좌측 상 대각선 타이머 가동 
					}
					else if( GetAsyncKeyState('K') & 0x8000 ) //2p left의 전 키가 down일 때
					{					
						SetTimer(hWnd,4,10,NULL); //좌측 하 대각선 타이머 가동 
					}
						}
					break;
				case 'I':if(button_up2==0){ 
					button_up2=1; //반복 키 입력 막기				
					SetTimer(hWnd,3,10,NULL);					
					if( GetAsyncKeyState('J') & 0x8000 )//2p up의 전 키가 left일 때 
					{
						SetTimer(hWnd,4,10,NULL); //좌측 상 대각선 타이머 가동
					}
					else if( GetAsyncKeyState('L') & 0x8000 )//2p up의 전 키가 right일때
					{
						SetTimer(hWnd,4,10,NULL); //좌측 상 대각선 타이머 가동
					}
						   }
					break;
				case 'L':if(button_right2==0){
					button_right2=1;//반복 키 입력 막기
					SetTimer(hWnd,3,10,NULL);					
					if( GetAsyncKeyState('I') & 0x8000 ) //2p right의 전 키가 Up일 때
					{						
						SetTimer(hWnd,4,10,NULL); //우측 상 대각선 타이머 가동 
					}
					else if( GetAsyncKeyState('K') & 0x8000 ) //2p right의 전 키가 down일 때
					{					
						SetTimer(hWnd,4,10,NULL); //우측 하 대각선 타이머 가동 
					}
							  }
					break;
				case 'K':if(button_down2==0){ 
					button_down2=1; //반복 키 입력 막기				
					SetTimer(hWnd,3,10,NULL);				
					if( GetAsyncKeyState('J') & 0x8000 )//2p up의 전 키가 left일 때 
					{
						SetTimer(hWnd,4,10,NULL); //좌측 상 대각선 타이머 가동
					}
					else if( GetAsyncKeyState('L') & 0x8000 )//2p up의 전 키가 right일때
					{
						SetTimer(hWnd,4,10,NULL); //좌측 상 대각선 타이머 가동
					}					
						   }
					break;				
				case VK_LEFT:if(button_left1==0){
					button_left1=1;//반복 키 입력 막기									
					SetTimer(hWnd,1,10,NULL);										
					if( GetAsyncKeyState(VK_UP) & 0x8000 ) //1p left의 전 키가 Up일 때
					{						
						SetTimer(hWnd,2,10,NULL); //좌측 상 대각선 타이머 가동 
					}
					else if( GetAsyncKeyState(VK_DOWN) & 0x8000 ) //1p left의 전 키가 down일 때
					{					
						SetTimer(hWnd,2,10,NULL); //좌측 하 대각선 타이머 가동 
					}					
						}
					break;
				case VK_UP:if(button_up1==0){ 
					button_up1=1; //반복 키 입력 막기				
					SetTimer(hWnd,1,10,NULL);					
					if( GetAsyncKeyState(VK_LEFT) & 0x8000 )//1p up의 전 키가 left일 때 
					{
						SetTimer(hWnd,2,10,NULL); //좌측 상 대각선 타이머 가동
					}
					else if( GetAsyncKeyState(VK_RIGHT) & 0x8000 )//1p up의 전 키가 right일때
					{
						SetTimer(hWnd,2,10,NULL); //좌측 상 대각선 타이머 가동
					}					
						   }
					break;
				case VK_RIGHT:if(button_right1==0){
					button_right1=1;//반복 키 입력 막기
					SetTimer(hWnd,1,10,NULL);					
					if( GetAsyncKeyState(VK_UP) & 0x8000 ) //1p right의 전 키가 Up일 때
					{						
						SetTimer(hWnd,2,10,NULL); //우측 상 대각선 타이머 가동 
					}
					else if( GetAsyncKeyState(VK_DOWN) & 0x8000 ) //1p right의 전 키가 down일 때
					{					
						SetTimer(hWnd,2,10,NULL); //우측 하 대각선 타이머 가동 
					}					
							  }
					break;
				case VK_DOWN:if(button_down1==0){
					button_down1=1; //반복 키 입력 막기				
					SetTimer(hWnd,1,10,NULL);					
					if( GetAsyncKeyState(VK_LEFT) & 0x8000 ) //1p down의 전 키가 left일 때
					{					
						SetTimer(hWnd,2,10,NULL); //좌측 하 대각선 타이머 가동 
					}
					else if( GetAsyncKeyState(VK_RIGHT) & 0x8000 ) //1p down의 전 키가 right일 때
					{					
						SetTimer(hWnd,2,10,NULL); //우측 하 대각선 타이머 가동 
					}					
							 }
					break;
				case VK_INSERT://1p 공격 아이템 슬롯 1 				
					if(turn==1 && attack_item1[0].item!=0)
					{
						switch(attack_item1[0].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Triple ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock2=1;
								SetTimer(hWnd,7,7000,NULL);
								strcpy(use_item1,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item1[0].item=0;						
					}
					break;
				case VK_HOME:
					if(turn==1 && attack_item1[1].item!=0)
					{
						switch(attack_item1[1].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Triple ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock2=1;
								SetTimer(hWnd,7,7000,NULL);
								strcpy(use_item1,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item1[1].item=0;
					}
					break;
				case VK_PRIOR:
					if(turn==1 && attack_item1[2].item!=0)
					{
						switch(attack_item1[2].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Trible ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock2=1;
								SetTimer(hWnd,7,7000,NULL);
								strcpy(use_item1,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item1,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item1[2].item=0;
					}
					break;
				case VK_DELETE:
					if(defence_item1[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item1[0].item)
					{						
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item1,"Slow ball");
							break;
						case 7: //실드 
							defence_shield1=1;
							strcpy(use_item1,"Shield");
							break;
						case 8: //라켓
							defence_change_racket1=1;
							strcpy(use_item1,"Great racket");
							SetRect(&racket1,racket1.left-10,racket1.top-10,racket1.right+10,racket1.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item1,"Create obstacle");
							break;
					}
					defence_item1[0].item=0;
					break;
				case VK_END:
					if(defence_item1[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item1[1].item)
					{						
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item1,"Slow ball");
							break;
						case 7: //실드 
							defence_shield1=1;
							strcpy(use_item1,"Shield");
							break;
						case 8: //라켓
							defence_change_racket1=1;
							strcpy(use_item1,"Great racket");
							SetRect(&racket1,racket1.left-10,racket1.top-10,racket1.right+10,racket1.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item1,"Create obstacle");
							break;
					}
					defence_item1[1].item=0;
					break;
				case VK_NEXT:
					if(defence_item1[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item1[2].item)
					{
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item1,"Slow ball");
							break;
						case 7: //실드 
							defence_shield1=1;
							strcpy(use_item1,"Shield");
							break;
						case 8: //라켓
							defence_change_racket1=1;
							strcpy(use_item1,"Great racket");
							SetRect(&racket1,racket1.left-10,racket1.top-10,racket1.right+10,racket1.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item1,"Create obstacle");
							break;
					}
					defence_item1[2].item=0;
					break;
				case 'Q':
					if(turn==2 && attack_item2[0].item!=0)
					{
						switch(attack_item2[0].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Trible ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock1=1;
								SetTimer(hWnd,6,7000,NULL);
								strcpy(use_item2,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item2[0].item=0;
					}
					break;
				case 'W':
					if(turn==2 && attack_item2[1].item!=0)
					{
						switch(attack_item2[1].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Trible ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock1=1;
								SetTimer(hWnd,6,7000,NULL);
								strcpy(use_item2,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item2[1].item=0;
					}
					break;
				case 'E':
					if(turn==2 && attack_item2[2].item!=0)
					{
						switch(attack_item2[2].item)
						{
							case 1://스피드 볼
								attack_speed_up=1;
								KillTimer(hWnd,5);//순간 멈춤 기능
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Speed up");
								sndPlaySound("speed.wav",SND_ASYNC);
								break;
							case 2://트리플 볼
								ball_xy[2]=ball_xy[1]=ball_xy[0];
								ball_movexy[1].x=-(ball_movexy[0].x/2+3);
								ball_movexy[2].x=ball_movexy[0].x/2+3;
								ball_movexy[1].y=ball_movexy[2].y=ball_movexy[0].y;
								attack_triple_ball=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Trible ball");
								sndPlaySound("triple.wav",SND_ASYNC);
								break;
							case 3://인비지 볼
								attack_invisible_ball=1;
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Invisible ball");
								break;
							case 4://컨트롤 쇼크
								attack_control_shock1=1;
								SetTimer(hWnd,6,7000,NULL);
								strcpy(use_item2,"Control shock");
								break;
							case 5://궤도 변화
								attack_change_orbit=1;
								KillTimer(hWnd,5);
								SetTimer(hWnd,9,500,NULL);
								strcpy(use_item2,"Change orbit");
								sndPlaySound("orbit.wav",SND_ASYNC);
								break;
						}
						attack_item2[2].item=0;
					}
					break;
				case 'A':
					if(defence_item2[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item2[0].item)
					{
						sndPlaySound("defence.wav",SND_ASYNC);
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item2,"Slow ball");
							break;
						case 7: //실드 
							defence_shield2=1;
							strcpy(use_item2,"Shield");
							break;
						case 8: //라켓
							defence_change_racket2=1;
							strcpy(use_item2,"Great racket");
							SetRect(&racket2,racket2.left-10,racket2.top-10,racket2.right+10,racket2.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item2,"Create obstacle");
							break;
					}
					defence_item2[0].item=0;
					break;					
				case 'S':
					if(defence_item2[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item2[1].item)
					{
						sndPlaySound("defence.wav",SND_ASYNC);
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item2,"Slow ball");
							break;
						case 7: //실드 
							defence_shield2=1;
							strcpy(use_item2,"Shield");
							break;
						case 8: //라켓
							defence_change_racket2=1;
							strcpy(use_item2,"Great racket");
							SetRect(&racket2,racket2.left-10,racket2.top-10,racket2.right+10,racket2.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item2,"Create obstacle");
							break;
					}
					defence_item2[1].item=0;
					break;
				case 'D':
					if(defence_item2[0].item!=0) sndPlaySound("defence.wav",SND_ASYNC);
					switch(defence_item2[2].item)
					{
						sndPlaySound("defence.wav",SND_ASYNC);
						case 6: //슬로우 볼
							defence_slow_ball=1;
							strcpy(use_item2,"Slow ball");
							break;
						case 7: //실드 
							defence_shield2=1;
							strcpy(use_item2,"Shield");
							break;
						case 8: //라켓
							defence_change_racket2=1;
							strcpy(use_item2,"Great racket");
							SetRect(&racket2,racket2.left-10,racket2.top-10,racket2.right+10,racket2.bottom+10);
							break;
						case 9: //장애물
							defence_create_obstacle=1;
							strcpy(use_item2,"Create obstacle");
							break;
					}
					defence_item2[2].item=0;
			}
			break; 
 }

return(DefWindowProc(hWnd,iMessage,wParam,lParam)); 
}


