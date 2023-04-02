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
	SDL_Rect		drawRect;			//����Ʈ �ڽ��� �׷��� ��ġ�� ũ��
	
	DWORD			editBoxform;			//����Ʈ �ڽ� ���� 0: �б⸸ ���� 1:�б� ���� ����
	bool			bEnable;				//Edit Ȱ��ȭ�� ��Ÿ���� ���� true�� ���� ���콺�� Ŭ������ ���
	UINT			disPlayIdx;				//textBuffer���� ������ ��ġ�� idx��
	int				cursorCount;			//Ŀ�� �ø�Ŀ���� ���� ī����

	TTF_Font		*font;					//����Ʈ �ڽ� ��Ʈ
	SDL_Surface		*ttfBlended;		//��Ʈ�� �׸� ǥ��
	SDL_Rect		drawTextSrc;		//�ؽ�Ʈ�� �׸� ������.
	SDL_Rect		staticDrawTextSrc;	//����ƽ �����͸� �׸� ������.

	SDL_Rect		cursorCoordSrc,cursorCoordDes;			//Ŀ���� ��ġ
	
	
	void	DrawPixel(SDL_Surface* pScreen,int x,int y);
public:
	void	CreateEditBox(SDL_Rect pos,DWORD state);		//����Ʈ �ڽ� ����.
	void	DrawEditBox(SDL_Surface* pScreen);				//Ŀ���� �׸�
	void	EnableSet(bool flags);							//��� ���� ����
	void	ReadEdit(char ch);								//�ԷµǴ� Ű �б�
	
	bool	GetEnable()			{	return bEnable; }

	//vector<char>	textBuffer;		//�ؽ�Ʈ�� �����ϱ� ���� ���� �ִ� ������� 512
	string		textBuffer;
	deque<char> staticBuffer;
};



#endif
