#include "CEditBox.h"

void han2unicode(char *src, Uint16 *dest);

void CEditBox::CreateEditBox(SDL_Rect pos,DWORD state)
{
	bEnable = false;
	disPlayIdx = 0;
	ZeroMemory(&cursorCoordSrc,sizeof(cursorCoordSrc));
	ZeroMemory(&cursorCoordDes,sizeof(cursorCoordDes));
	drawRect = pos;
	editBoxform = state;
	cursorCount = 0;

	//��Ʈ �ε�
	font = TTF_OpenFont("../../Game/font/HYNAMB.ttf",10);
	

	//Ŀ�� ������ ������
	cursorCoordSrc.x = drawRect.x + 5;	//Ŀ���� ���� x ��ġ
	cursorCoordSrc.y = drawRect.y + 5;

	drawTextSrc.x = drawRect.x +5;
	drawTextSrc.y = drawRect.y +5;
	
	//����ƽ ����Ʈ �ڽ��� ù ��ġ
	
	staticDrawTextSrc.x = 505;
	staticDrawTextSrc.y = 572;
	
	//Ŀ�� ������ ����
	cursorCoordDes.x = cursorCoordSrc.x;
	cursorCoordDes.y = cursorCoordSrc.y+15;
}

void CEditBox::EnableSet(bool flags)
{
	if(textBuffer.empty())	//��� ���� ��츸 Ȱ��ȭ�� ������.
		this->bEnable = flags;
}

void CEditBox::ReadEdit(char ch)
{
	if(textBuffer.size() > 58)		//�ִ� ���� ������ ���� ��
		return;
	if(ch==13)		//����Ű���
	{
		for(UINT i=0;i<textBuffer.size();i++)
		{
			if(staticBuffer.size() > 1024)
			{
				staticBuffer.pop_front();
			}
			staticBuffer.push_back(textBuffer[i]);
		}
		textBuffer.clear();
		cursorCoordSrc = drawTextSrc;
		return;
	}
	textBuffer.push_back(ch);	
	cursorCoordSrc.x+=6;
}
void CEditBox::DrawEditBox(SDL_Surface* pScreen)
{	

	//����Ʈ �ڽ��� ���� ����
	char text[60];
	
	UINT ret;
	for(ret=0;ret<textBuffer.size();ret++)
	{
		text[ret] = textBuffer[ret];
	}
	text[ret] = '\0';
	Uint16	transUnicode[1024];

	han2unicode(text,transUnicode);

	SDL_Color color = { 0,0,0 };
	ttfBlended = TTF_RenderUNICODE_Blended(font,transUnicode,color);
	
	SDL_BlitSurface(ttfBlended,NULL,pScreen,&drawTextSrc);
	
	//����ƽ �ڽ��� �� ����
	char staticText[1024];
	for(ret=0;ret<staticBuffer.size();ret++)
	{
		staticText[ret] = staticBuffer[ret];
	}
	staticText[ret] = '\0';
	han2unicode(staticText,transUnicode);
	
	ttfBlended = TTF_RenderUNICODE_Blended(font,transUnicode,color);
	
	SDL_BlitSurface(ttfBlended,NULL,pScreen,&staticDrawTextSrc);
	

	//�а� ���� ����Ʈ �ڽ� (Ŀ���� ����) Ȱ��ȭ �ø� �ؽ�Ʈ �׸�
	//Ŀ�� �׸���
	if(bEnable)		//����Ʈ �ڽ��� Ȱ��ȭ�� ���
	{		
		cursorCount++;
		cursorCount=cursorCount%80;
		if(cursorCount > 40)		
			return;
		int dotsY = cursorCoordSrc.y;			
		while(dotsY <= cursorCoordDes.y)
		{
			DrawPixel(pScreen,cursorCoordSrc.x,dotsY);
			dotsY++;
		}		
	}
			
	
	SDL_FreeSurface(ttfBlended);	
	
}

void CEditBox::DrawPixel(SDL_Surface* pScreen,int x,int y)
{
   Uint32 color = SDL_MapRGB(pScreen->format, 0, 0, 0);

    if ( SDL_MUSTLOCK(pScreen) ) {
        if ( SDL_LockSurface(pScreen) < 0 ) {
            return;
        }
    }
    switch (pScreen->format->BytesPerPixel) {
        case 1: { /* 8-bpp ��� ���� */
            Uint8 *bufp;

            bufp = (Uint8 *)pScreen->pixels + y*pScreen->pitch + x;
            *bufp = color;
        }
        break;

        case 2: { /* �Ƹ� 15-bpp �ƴϸ� 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)pScreen->pixels + y*pScreen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3: { /* ���� 24-bpp mode, ���� ������ �ʴ´� */
            Uint8 *bufp;

            bufp = (Uint8 *)pScreen->pixels + y*pScreen->pitch + x * 3;
            if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
                bufp[0] = color;
                bufp[1] = color >> 8;
                bufp[2] = color >> 16;
            } else {
                bufp[2] = color;
                bufp[1] = color >> 8;
                bufp[0] = color >> 16;
            }
        }
        break;

        case 4: { /* �Ƹ� 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)pScreen->pixels + y*pScreen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
    if ( SDL_MUSTLOCK(pScreen) ) {
        SDL_UnlockSurface(pScreen);
    }
    SDL_UpdateRect(pScreen, x, y, 1, 1);
}

