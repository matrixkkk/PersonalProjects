#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM );  

HINSTANCE g_hInst;
LPSTR lpszClass="지뢰찾기";
typedef struct each_status{
	int status;
	int covered;
}EACH;

RECT Level1[9][9],Level2[16][16],Level3[16][30]; //각 레벨별 렉트
EACH level1_status[9][9],level2_status[16][16],level3_status[16][30]; //각 칸의 상태
int times=0;

void DrawBitmap(HDC hDC,RECT rect,POINT pt,HBITMAP hBit,POINT size) //비트맵  함수
{
	HDC MemDC;
	HBITMAP OldBitmap;
		
	MemDC=CreateCompatibleDC(hDC);
	OldBitmap=(HBITMAP)SelectObject(MemDC,hBit);
	
	BitBlt(hDC,rect.left,rect.top,size.x,size.y,MemDC,pt.x,pt.y,SRCCOPY);
	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}
int blank_open(int i,int j,int level);
void counter(HDC hDC,int bomber,RECT rect,int key,HBITMAP hBit);

void LEVEL1(HDC hDC,int *game,int *mine3x3,HBITMAP hBitbutton)
{	
	POINT p,size;
	size.x=size.y=16;
	int i,j,k,m;		
	for(i=0;i<9;i++)
		 {
			for(j=0;j<9;j++)
			{	
				if(*game==0 && level1_status[i][j].status==10) level1_status[i][j].covered=0; //게임이 오버 됐을때 지뢰 다 까기
				*mine3x3=0;				
				if(level1_status[i][j].covered==1) //1 안까진 경우 
				{
					p.x=0;
					p.y=0;
					DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);				
				}
				else if(level1_status[i][j].covered==2) //2 깃발인 경우
				{
					p.x=160,p.y=0;
					DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);
				}
				else if(level1_status[i][j].covered==0)
				{
					switch(level1_status[i][j].status)
					{						
						case 0:
							p.x=16,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 1:
							p.x=32,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 2:
							p.x=48,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 3:
							p.x=64,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 4:
							p.x=80,p.y=0;;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 5:
							p.x=96,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 6:
							p.x=112,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 7:
							p.x=128,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 8:
							p.x=144,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 10:
							p.x=192,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 11:
							p.x=176,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);							
							break;
						case 12:
							p.x=160,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);
							break;
					}
				}				
				if(level1_status[i][j].status!=10)
				{
					k=i-1;
					if(k<0) k=0;
					while(k<i+2 && k<9)
					{
						m=j-1;
						if(m<0) m=0;
						while(m<j+2 && m<9)
						{
						if(level1_status[k][m].status==10) (*mine3x3)++;
						m++;
						}
						k++;
					}
					level1_status[i][j].status=*mine3x3;					
				}				
			}
		}	 
}

void LEVEL2(HDC hDC,int *game,int *mine3x3,HBITMAP hBitbutton)
{
	POINT p,size;
	int i,j,k,m;	
	size.x=size.y=16;
	for(i=0;i<16;i++)
		 {
			for(j=0;j<16;j++)
			{	
				if(*game==0 && level2_status[i][j].status==10) level2_status[i][j].covered=0; //게임이 오버 됐을때 지뢰 다 까기
				*mine3x3=0;				
				if(level2_status[i][j].covered==1) //1 안까진 경우 
				{
					p.x=0;
					p.y=0;
					DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);				
				}
				else if(level2_status[i][j].covered==2) //2 깃발인 경우
				{
					p.x=160,p.y=0;
					DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);					
				}
				else if(level2_status[i][j].covered==0)
				{
					switch(level2_status[i][j].status)
					{
						case 0:
							p.x=16,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 1:
							p.x=32,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 2:
							p.x=48,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 3:
							p.x=64,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 4:
							p.x=80,p.y=0;;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 5:
							p.x=96,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 6:
							p.x=112,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 7:
							p.x=128,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 8:
							p.x=144,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 10:
							p.x=192,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 11:
							p.x=176,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);							
							break;
						case 12:
							p.x=160,p.y=0;
							DrawBitmap(hDC,Level2[i][j],p,hBitbutton,size);
							break;

					}
				}				
				if(level2_status[i][j].status!=10)
				{
					k=i-1;
					if(k<0) k=0;
					while(k<i+2 && k<16)
					{
						m=j-1;
						if(m<0) m=0;
						while(m<j+2 && m<16)
						{
						if(level2_status[k][m].status==10) (*mine3x3)++;
						m++;
						}
						k++;
					}
					level2_status[i][j].status=*mine3x3;					
				}				
			}
		}	 
}

void LEVEL3(HDC hDC,int *game,int *mine3x3,HBITMAP hBitbutton)
{
	POINT p,size;
	int i,j,k,m;	
	size.x=size.y=16;
	for(i=0;i<16;i++)
		 {
			for(j=0;j<30;j++)
			{	
				if(*game==0 && level3_status[i][j].status==10) level3_status[i][j].covered=0; //게임이 오버 됐을때 지뢰 다 까기
				*mine3x3=0;				
				if(level3_status[i][j].covered==1) //1 안까진 경우 
				{
					p.x=0;
					p.y=0;
					DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);				
				}
				else if(level3_status[i][j].covered==2) //2 깃발인 경우
				{
					p.x=160,p.y=0;
					DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);
				}
				else if(level3_status[i][j].covered==0)
				{					
					switch(level3_status[i][j].status)
					{
						case 0:
							p.x=16,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 1:
							p.x=32,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 2:
							p.x=48,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 3:
							p.x=64,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 4:
							p.x=80,p.y=0;;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 5:
							p.x=96,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 6:
							p.x=112,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 7:
							p.x=128,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 8:
							p.x=144,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 10:
							p.x=192,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 11:
							p.x=176,p.y=0;
							DrawBitmap(hDC,Level3[i][j],p,hBitbutton,size);							
							break;
						case 12:
							p.x=160,p.y=0;
							DrawBitmap(hDC,Level1[i][j],p,hBitbutton,size);
							break;
					}
				}				
				if(level3_status[i][j].status!=10)
				{
					k=i-1;
					if(k<0) k=0;
					while(k<i+2 && k<16)
					{
						m=j-1;
						if(m<0) m=0;
						while(m<j+2 && m<30)
						{
						if(level3_status[k][m].status==10) (*mine3x3)++;
						m++;
						}
						k++;
					}
					level3_status[i][j].status=*mine3x3;					
				}				
			}
		}	 
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd )
{
 HWND hWnd;                  
 MSG Message;
 WNDCLASS WndClass;         
 g_hInst=hInstance;
 WndClass.cbClsExtra=0; 
 WndClass.cbWndExtra=0; 
 WndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH); 
 WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
 WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 WndClass.style=CS_HREDRAW|CS_VREDRAW; 
 WndClass.lpfnWndProc=(WNDPROC)WndProc; 
 WndClass.hInstance=hInstance;
 WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
 WndClass.lpszClassName=lpszClass; 
 RegisterClass(&WndClass);
	   
 hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW 
     ,CW_USEDEFAULT 
     ,CW_USEDEFAULT 
     ,175 
     ,255
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
	HPEN mypen,white,oldpen;
	HBRUSH mybrush,oldbrush;
	static HBITMAP hBitbutton,hSmile,hNumber;     
	static int currLevel=1,mine=10,mine3x3=0,game=1,/*times=0,*/flag; //currLevel=현재 레벨 mine=지뢰 셋팅 갯수 game=1 게임중 
	static RECT bomber_board[3],time_board[3];                //flag 는 깃발을 꼽은 갯수
	static int bomber[3],time_b[3]; 
	static HWND button;
	static BOOL timer=TRUE;
	int i,j,k,m,r,x,y,blocks=0; 
	RECT nomal,smile,ws;
	POINT mouse,p,size;
	srand(time(NULL));
	PAINTSTRUCT ps;
	HDC hDC;
 switch(iMessage){  
 case WM_CREATE:
	 x=y=0;	 
	 hBitbutton=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP2)); //비트맵 리소스 로드	 
	 hSmile=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP3)); //스마일
	 hNumber=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP4)); //숫자판
	 HBRUSH hGray;
	 hGray=CreateSolidBrush(RGB(192,192,192));
	 button=CreateWindow("button",NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,600,500,700,800,hWnd,(HMENU)0, g_hInst,NULL);
	 SetClassLong(hWnd,GCL_HBRBACKGROUND,(LONG)hGray);	 
	 for(r=0;r<3;r++)
	 {
		 k=10,m=50;
		for(i=0;i<9+x;i++)
		{
			if(r==1) x=7;
			else if(r==2) y=14;
			for(j=0;j<9+x+y;j++)
			{			 
				if(r==0)
				{
					level1_status[i][j].covered=1; //상태 FALSE은 아직 안까인 상태			 
					SetRect(&Level1[i][j],k,m,k+16,m+16);
				}
				else if(r==1)
				{				
					level2_status[i][j].covered=0; //상태 FALSE은 아직 안까인 상태			 
					SetRect(&Level2[i][j],k,m,k+16,m+16);
				}
				else if(r==2)
				{					
					level3_status[i][j].covered=1; //상태 FALSE은 아직 안까인 상태			 
					SetRect(&Level3[i][j],k,m,k+16,m+16);
				}
				k+=16;
			}			
			k=10;
			m+=16;
		}
	 }
	 for(i=mine;i>0;i--)
	 {
		 j=rand()%9;
		 k=rand()%9;
		 if(level1_status[j][k].status==10) i++; 
		 else
		 {
			level1_status[j][k].status=10;
		 }
	 }
	 flag=mine;
	 break;
 case WM_PAINT:
	 hDC=BeginPaint(hWnd,&ps);
	 GetClientRect (hWnd,&ws); //현재 윈도우 크기 얻기;;
	 switch(currLevel)
	 {
		case 1:
			mybrush=CreateSolidBrush(RGB(192,192,192));		
			oldbrush=(HBRUSH)SelectObject(hDC,mybrush);
			SetRect(&nomal,8,5,155,40);
			Rectangle(hDC,nomal.left,nomal.top,nomal.right,nomal.bottom);
			SelectObject(hDC,oldbrush);	
			for(i=0,k=18,m=5;i<3;i++)
			{
				SetRect(&bomber_board[i],nomal.left+m,nomal.top+5,nomal.left+k,nomal.top+28);
				SetRect(&time_board[i],nomal.right-k,nomal.top+5,nomal.right-m,nomal.top+28);
				m+=13,k+=13;
			}			
			smile.left=(ws.right-ws.left)/2-13;
			smile.right=smile.left+26;
			smile.top=bomber_board[0].top;
			smile.bottom=bomber_board[0].bottom;
			Rectangle(hDC,bomber_board[0].left,bomber_board[0].top,bomber_board[0].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[1].left,bomber_board[1].top,bomber_board[1].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[2].left,bomber_board[2].top,bomber_board[2].right,bomber_board[0].bottom);
			Rectangle(hDC,time_board[0].left,time_board[0].top,time_board[0].right,time_board[0].bottom);
			Rectangle(hDC,time_board[1].left,time_board[1].top,time_board[1].right,time_board[1].bottom);
			Rectangle(hDC,time_board[2].left,time_board[2].top,time_board[2].right,time_board[2].bottom);
						
			mypen=CreatePen(PS_SOLID,2,RGB(128,128,128));			
			white=(HPEN)GetStockObject(WHITE_PEN);			
			oldpen=(HPEN)SelectObject(hDC,mypen);
			MoveToEx(hDC,9,49,NULL);
			LineTo(hDC,154,49);
			SelectObject(hDC,white);
			LineTo(hDC,154,194);
			LineTo(hDC,9,194);
			SelectObject(hDC,mypen);
			LineTo(hDC,9,49);
			SelectObject(hDC,oldpen);			
			LEVEL1(hDC,&game,&mine3x3,hBitbutton);

			break;
		case 2:
			mybrush=CreateSolidBrush(RGB(192,192,192));		
			oldbrush=(HBRUSH)SelectObject(hDC,mybrush);
			SetRect(&nomal,8,5,267,40);
			Rectangle(hDC,nomal.left,nomal.top,nomal.right,nomal.bottom);
			SelectObject(hDC,oldbrush);
			for(i=0,k=18,m=5;i<3;i++)
			{
				SetRect(&bomber_board[i],nomal.left+m,nomal.top+5,nomal.left+k,nomal.top+28);
				SetRect(&time_board[i],nomal.right-k,nomal.top+5,nomal.right-m,nomal.top+28);
				m+=13,k+=13;
			}			
			smile.left=(ws.right-ws.left)/2-13;
			smile.right=smile.left+26;
			smile.top=bomber_board[0].top;
			smile.bottom=bomber_board[0].bottom;
			Rectangle(hDC,bomber_board[0].left,bomber_board[0].top,bomber_board[0].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[1].left,bomber_board[1].top,bomber_board[1].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[2].left,bomber_board[2].top,bomber_board[2].right,bomber_board[0].bottom);
			Rectangle(hDC,time_board[0].left,time_board[0].top,time_board[0].right,time_board[0].bottom);
			Rectangle(hDC,time_board[1].left,time_board[1].top,time_board[1].right,time_board[1].bottom);
			Rectangle(hDC,time_board[2].left,time_board[2].top,time_board[2].right,time_board[2].bottom);
			
			mypen=CreatePen(PS_SOLID,2,RGB(128,128,128));	
			white=(HPEN)GetStockObject(WHITE_PEN);
			oldpen=(HPEN)SelectObject(hDC,mypen);
			MoveToEx(hDC,9,49,NULL);
			LineTo(hDC,266,49);
			SelectObject(hDC,white);
			LineTo(hDC,266,307);
			LineTo(hDC,9,307);
			SelectObject(hDC,mypen);
			LineTo(hDC,9,49);
			SelectObject(hDC,oldpen);
			LEVEL2(hDC,&game,&mine3x3,hBitbutton);
			break;
		case 3:
			mybrush=CreateSolidBrush(RGB(192,192,192));		
			oldbrush=(HBRUSH)SelectObject(hDC,mybrush);
			SetRect(&nomal,8,5,493,40);
			Rectangle(hDC,nomal.left,nomal.top,nomal.right,nomal.bottom);
			SelectObject(hDC,oldbrush);
			for(i=0,k=18,m=5;i<3;i++)
			{
				SetRect(&bomber_board[i],nomal.left+m,nomal.top+5,nomal.left+k,nomal.top+28);
				SetRect(&time_board[i],nomal.right-k,nomal.top+5,nomal.right-m,nomal.top+28);
				m+=13,k+=13;
			}			
			smile.left=(ws.right-ws.left)/2-13;
			smile.right=smile.left+26;
			smile.top=bomber_board[0].top;
			smile.bottom=bomber_board[0].bottom;
			Rectangle(hDC,bomber_board[0].left,bomber_board[0].top,bomber_board[0].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[1].left,bomber_board[1].top,bomber_board[1].right,bomber_board[0].bottom);
			Rectangle(hDC,bomber_board[2].left,bomber_board[2].top,bomber_board[2].right,bomber_board[0].bottom);
			Rectangle(hDC,time_board[0].left,time_board[0].top,time_board[0].right,time_board[0].bottom);
			Rectangle(hDC,time_board[1].left,time_board[1].top,time_board[1].right,time_board[1].bottom);
			Rectangle(hDC,time_board[2].left,time_board[2].top,time_board[2].right,time_board[2].bottom);
			
			mypen=CreatePen(PS_SOLID,2,RGB(128,128,128));	
			white=(HPEN)GetStockObject(WHITE_PEN);
			oldpen=(HPEN)SelectObject(hDC,mypen);
			MoveToEx(hDC,9,49,NULL);
			LineTo(hDC,490,49);
			SelectObject(hDC,white);
			LineTo(hDC,490,307);
			LineTo(hDC,9,307);
			SelectObject(hDC,mypen);
			LineTo(hDC,9,49);
			SelectObject(hDC,oldpen);
			LEVEL3(hDC,&game,&mine3x3,hBitbutton);
			break;
	 }	 
	 bomber[0]=0;
	 bomber[1]=flag/10;
	 bomber[2]=flag%10;
	 time_b[2]=times/100;
	 time_b[1]=(times%100)/10;
	 time_b[0]=(times%100)%10;
	 for(i=0;i<3;i++)
	 {
		 if(flag<0 && i==0)
		 {
			counter(hDC,bomber[i],bomber_board[i],1,hNumber);
		 }
		 else counter(hDC,bomber[i],bomber_board[i],0,hNumber);		 
	 }
	 for(i=0;i<3;i++)
	 {
		 counter(hDC,time_b[i],time_board[i],0,hNumber);
	 }
	 switch(currLevel)
	 {
	 case 1: m=k=0;
		 for(i=0;i<9+m;i++)
		 {
			for(j=0;j<9+m+k;j++)
			{
				if(level1_status[i][j].covered!=0) blocks++;
			}	
		}
		 break;
	 case 2: m=7,k=0;
		 for(i=0;i<9+m;i++)
		 {
			for(j=0;j<9+m+k;j++)
			{
				if(level2_status[i][j].covered!=0) blocks++;
			}
		 }
		 break;
	 case 3: m=7,k=14;
		for(i=0;i<9+m;i++)
		{
			for(j=0;j<9+m+k;j++)
			{
				if(level3_status[i][j].covered!=0) blocks++;
			}
		}	
		break;
	 }	 

	 if(mine==0) //종료 조건 
	 {
		if(currLevel==1 && blocks==10)
		{
			if(MessageBox(hWnd,"다시 할래요?ㅎㅎ","게임 클리어!",MB_YESNO)!=IDYES)		
			{
				DeleteObject(hSmile);
				DeleteObject(hNumber);
				DeleteObject(hBitbutton);
				PostQuitMessage(0);
			}			
		}
		else if(currLevel==2 && blocks==40)
		{
			if(MessageBox(hWnd,"다시 할래요?ㅎㅎ","게임 클리어!",MB_YESNO)!=IDYES)		
			{
				DeleteObject(hSmile);
				DeleteObject(hNumber);
				DeleteObject(hBitbutton);
				PostQuitMessage(0);
			}			
		}
		else if(currLevel==3 && blocks==99)
		{
			if(MessageBox(hWnd,"다시 할래요?ㅎㅎ","게임 클리어!",MB_YESNO)!=IDYES)		
			{
				DeleteObject(hSmile);
				DeleteObject(hNumber);
				DeleteObject(hBitbutton);
				PostQuitMessage(0);
			}			
		}
		KillTimer(hWnd,1);
	 }
	 DeleteObject(oldbrush);
	 DeleteObject(oldpen);
	 DeleteObject(mypen);
	 DeleteObject(white);
	 hDC=BeginPaint(button,&ps);
	 size.x=size.y=26;
	 p.x=p.y=0;
	 SetRect(&ws,0,0,0,0);
	 SetWindowPos(button,HWND_TOP,smile.left,smile.top,26,26,SWP_NOZORDER);
	 DrawBitmap(hDC,ws,p,hSmile,size);	
	 EndPaint(hWnd,&ps);
	 break;
 case WM_TIMER:
	 switch(wParam)
	 {
	 case 1:
		 times++;
		 InvalidateRect(hWnd,&time_board[0],TRUE);
		 InvalidateRect(hWnd,&time_board[1],TRUE);
		 break;
	 }
	 break;
 case WM_DESTROY:
		DeleteObject(hSmile);
		DeleteObject(hNumber);
		DeleteObject(hBitbutton);
		PostQuitMessage(0);
		break;
 case WM_COMMAND:
	 blocks=0;
	 timer=TRUE;
	 times=0;
	 KillTimer(hWnd,1);
	 switch(wParam)
	 {
		case ID_MENU2:
			DeleteObject(hSmile);
			DeleteObject(hNumber);
			DeleteObject(hBitbutton);
			PostQuitMessage(0);
			return 0;
		case ID_LEVEL1: currLevel=1;
			SetWindowPos(hWnd,HWND_TOP,CW_USEDEFAULT,CW_USEDEFAULT,175,255,SWP_NOMOVE);						
			break;
		case ID_LEVEL2: currLevel=2;			
			SetWindowPos(hWnd,HWND_TOP,CW_USEDEFAULT,CW_USEDEFAULT,300,373,SWP_NOMOVE);
			break;
		case ID_LEVEL3:	currLevel=3;
			SetWindowPos(hWnd,HWND_TOP,CW_USEDEFAULT,CW_USEDEFAULT,505,365,SWP_NOMOVE);
			break;
	 }
	 switch(currLevel)
	 { 
		case 1:
			m=k=0;			
			mine=10;
			break;
		case 2:
			m=7,k=0;			
			mine=40;
			break;
		case 3:
			m=7,k=14;			
			mine=99;
			break;
	 }	 
	 game=1; //멈춘 게임의 실행
	 for(i=0;i<9+m;i++)//안깐 상태로 모두 복구 
	 {
		for(j=0;j<9+m+k;j++)
		{
			if(currLevel==1) 
			{
				level1_status[i][j].covered=1;
				level1_status[i][j].status=0;
			}
			else if(currLevel==2)
			{
				level2_status[i][j].covered=1;
				level2_status[i][j].status=0;
			}
			else
			{
				level3_status[i][j].covered=1;
				level3_status[i][j].status=0;
			}
		}
	 }
	 for(i=mine;i>0;i--)//지뢰 생성
	 {
		j=rand()%(9+m);
		r=rand()%(9+m+k);
		switch(currLevel)
		{
			case 1:
				if(level1_status[j][r].status==10) i++;
				else
				{
					level1_status[j][r].status=10;
				}
				break;
			case 2:
				if(level2_status[j][r].status==10) i++;
				else
				{
					level2_status[j][r].status=10;
				}
				break;
			case 3:
				if(level3_status[j][r].status==10) i++;
				else
				{
					level3_status[j][r].status=10;
				}
				break;
		}
	}
	 flag=mine;
	InvalidateRect(hWnd,NULL,TRUE);		
	break;
 case WM_RBUTTONDOWN:
	 mouse.x=(int)LOWORD(lParam);
	 mouse.y=(int)HIWORD(lParam);
	 switch(currLevel)
	 {
		case 1:
			m=k=0;
			break;
		case 2:
			m=7,k=0;
			break;
		case 3:
			m=7,k=14;
			break;
	 } 
	 if(game)
	 {
		 for(i=0;i<9+m;i++)
		 {
			 for(j=0;j<9+m+k;j++)
			 {
				 if(PtInRect(&Level1[i][j],mouse)==1 && currLevel==1)
				 {
					 if(level1_status[i][j].covered==1 && level1_status[i][j].status==10)
					 {
						 level1_status[i][j].covered=2;
						 mine--,flag--;
					 }
					 else if(level1_status[i][j].covered==1)
					 {
						level1_status[i][j].covered=2;
						flag--;
					 }
					 else if(level1_status[i][j].covered==2)
					 {
						level1_status[i][j].covered=1;
						if(level1_status[i][j].status==10) mine++,flag++;
						else flag++;
					 }
				 }
				 else if(PtInRect(&Level2[i][j],mouse)==1 && currLevel==2)
				 {
					 if(level2_status[i][j].covered==1 && level2_status[i][j].status==10)
					 {
						  level2_status[i][j].covered=2;
						  mine--,flag--;
					 }
					 else if(level2_status[i][j].covered==1)
					 {
						 level2_status[i][j].covered=2;
						 flag--;
					 }
					 else if(level2_status[i][j].covered==2)
					 {
						level2_status[i][j].covered=1;
						if(level2_status[i][j].status==10) mine++,flag++;
						else flag++;
					 }
				 }
				 else if(PtInRect(&Level3[i][j],mouse)==1 && currLevel==3)
				 {
					 if(level3_status[i][j].covered==1 && level3_status[i][j].status==10)
					 {
						  level3_status[i][j].covered=2;
						  mine--,flag--;
					 }
					 else if(level3_status[i][j].covered==1)
					 {
						 level3_status[i][j].covered=2;
						 flag--;
					 }
					 else if(level3_status[i][j].covered==2)
					 {
						level2_status[i][j].covered=1;
						if(level3_status[i][j].status==10) mine++,flag++;
						else flag++;
					 }
				 }
			 }
		 }
	 }
	 InvalidateRect(hWnd,NULL,TRUE);
	 break;

 case WM_LBUTTONDOWN:
	if(timer)
	{
		SetTimer(hWnd,1,1000,NULL);
		timer=FALSE;
	}
	mouse.x=(int)LOWORD(lParam);
	mouse.y=(int)HIWORD(lParam);
	switch(currLevel)
	{
		case 1:
			m=k=0;
			break;
		case 2:
			m=7,k=0;
			break;
		case 3:
			m=7,k=14;
			break;
	}
	if(game)
	{
		for(i=0;i<9+m;i++)
		{
			for(j=0;j<9+m+k;j++)
			{
				if(PtInRect(&Level1[i][j],mouse)==1 && currLevel==1)
				{
					if(level1_status[i][j].covered==2) flag++;
					level1_status[i][j].covered=0; //뒤집어 깐다.
					if(level1_status[i][j].status==10)//선택한 것이 폭탄이라면
					{
						game=0; //게임 오버
						KillTimer(hWnd,1);
						level1_status[i][j].status=11;						
					}
					else if(level1_status[i][j].status==0)//선택한 곳의 3x3에 폭탄이 없는 0이 오는경우
					{					
						blank_open(i,j,currLevel);
					}					
					InvalidateRect(hWnd,NULL,TRUE);
				}
				else if(PtInRect(&Level2[i][j],mouse)==1 && currLevel==2)
				{
					level2_status[i][j].covered=0; //뒤집어 깐다.
					if(level2_status[i][j].status==10)//선택한 것이 폭탄이라면
					{
						game=0; //게임 오버
						KillTimer(hWnd,1);
						level2_status[i][j].status=11;						
					}
					else if(level2_status[i][j].status==0)//선택한 곳의 3x3에 폭탄이 없는 0이 오는경우
					{					
						blank_open(i,j,currLevel);
					}
					else if(level2_status[i][j].covered==2) flag++;
					InvalidateRect(hWnd,NULL,TRUE);
				}
				else if(PtInRect(&Level3[i][j],mouse)==1 && currLevel==3)
				{
					level3_status[i][j].covered=0; //뒤집어 깐다.
					if(level3_status[i][j].status==10)//선택한 것이 폭탄이라면
					{
						game=0; //게임 오버
						KillTimer(hWnd,1);
						level3_status[i][j].status=11;						
					}
					else if(level3_status[i][j].status==0)//선택한 곳의 3x3에 폭탄이 없는 0이 오는경우
					{					
						blank_open(i,j,currLevel);
					}
					else if(level3_status[i][j].covered==2) flag++;
					InvalidateRect(hWnd,NULL,TRUE);
				}
			}
		}
	}
	break;
 
 }

return(DefWindowProc(hWnd,iMessage,wParam,lParam)); 
}

void counter(HDC hDC,int bomber,RECT rect,int key,HBITMAP hBit)
{
	POINT p,size;	
	size.x=13;
	size.y=23;
	if(bomber<0) bomber*=-1;
	switch(bomber)
	{
		case 0: 
			if(key==0) p.x=0,p.y=0; 
			else p.x=130,p.y=0;			
 			break;
		case 1: p.x=13,p.y=0;
			break;
		case 2: p.x=26,p.y=0;
			break;
		case 3: p.x=39,p.y=0;
			break;
		case 4: p.x=52,p.y=0;
			break;
		case 5: p.x=65,p.y=0;
			break;
		case 6: p.x=78,p.y=0;
			break;
		case 7: p.x=91,p.y=0;
			break;
		case 8: p.x=104,p.y=0;
			break;
		case 9: p.x=117,p.y=0;
			break;
	}
	DrawBitmap(hDC,rect,p,hBit,size);	
}

int blank_open(int i,int j,int level)
{
	switch(level)
	{
		int k,m;
		case 1:						
			level1_status[i][j].covered=0;
			k=i-1;
			if(k<0) k=0;
			while(k<i+2 && k<9)
			{
				m=j-1;
				if(m<0) m=0;
				while(m<j+2 && m<9)
				{					
					if(level1_status[k][m].status==0 && level1_status[k][m].covered==1) blank_open(k,m,level);//재귀 호출 사용
					else if(level1_status[k][m].covered==1)
					{
						level1_status[k][m].covered=0;
					}
					m++;
				}
				k++;
			}
			break;
		case 2:
			level2_status[i][j].covered=0;
			k=i-1;
			if(k<0) k=0;
			while(k<i+2 && k<16)
			{
				m=j-1;
				if(m<0) m=0;
				while(m<j+2 && m<16)
				{					
					if(level2_status[k][m].status==0 && level2_status[k][m].covered==1) blank_open(k,m,level);//재귀 호출 사용
					else if(level2_status[k][m].covered==1)
					{
						level2_status[k][m].covered=0;
					}
					m++;
				}
				k++;
			}
			break;
		case 3:
			level3_status[i][j].covered=0;
			k=i-1;
			if(k<0) k=0;
			while(k<i+2 && k<16)
			{
				m=j-1;
				if(m<0) m=0;
				while(m<j+2 && m<30)
				{					
					if(level3_status[k][m].status==0 && level3_status[k][m].covered==1) blank_open(k,m,level);//재귀 호출 사용
					else if(level3_status[k][m].covered==1)
					{
						level3_status[k][m].covered=0;
					}
					m++;
				}
				k++;
			}
			break;
	}
	return 0;
}


