#ifndef CEDITBOX_H_
#define CEDITBOX_H_

#include <deque>
#include <windows.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

using namespace std;

#define READ_ONLY	0
#define READ_WRITE	1

class CEditBox
{	
	SDL_Rect		drawRect;			//에디트 박스가 그려질 위치와 크기
	
	DWORD			editBoxform;			//에디트 박스 형태 0: 읽기만 가능 1:읽기 쓰기 가능
	bool			bEnable;				//Edit 활성화를 나타내는 변수 true일 경우는 마우스로 클릭했을 경우
	UINT			disPlayIdx;				//textBuffer에서 보여줄 위치의 idx값
	int				cursorCount;			//커서 플리커링을 위한 카운터

	TTF_Font		*font;					//에디트 박스 폰트
	SDL_Surface		*ttfBlended;		//폰트를 그릴 표면
	SDL_Rect		drawTextSrc;		//텍스트를 그릴 시작점.
	SDL_Rect		staticDrawTextSrc;	//스태틱 에디터를 그릴 시작점.

	SDL_Rect		cursorCoordSrc,cursorCoordDes;			//커서의 위치
	
	
	void	DrawPixel(SDL_Surface* pScreen,int x,int y);
public:
	void	CreateEditBox(SDL_Rect pos,DWORD state);		//에디트 박스 생성.
	void	DrawEditBox(SDL_Surface* pScreen);				//커서를 그림
	void	EnableSet(bool flags);							//사용 가능 셋팅
	void	ReadEdit(char ch);								//입력되는 키 읽기
	
	bool	GetEnable()			{	return bEnable; }

	//vector<char>	textBuffer;		//텍스트를 저장하기 위한 버퍼 최대 저장수는 512
	string		textBuffer;
	deque<char> staticBuffer;
};



#endif
