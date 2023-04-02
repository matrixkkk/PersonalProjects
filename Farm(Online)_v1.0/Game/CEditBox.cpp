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

	//폰트 로드
	font = TTF_OpenFont("../../Game/font/HYNAMB.ttf",10);
	

	//커서 라인의 시작점
	cursorCoordSrc.x = drawRect.x + 5;	//커서의 최초 x 위치
	cursorCoordSrc.y = drawRect.y + 5;

	drawTextSrc.x = drawRect.x +5;
	drawTextSrc.y = drawRect.y +5;
	
	//스테틱 에디트 박스의 첫 위치
	
	staticDrawTextSrc.x = 505;
	staticDrawTextSrc.y = 572;
	
	//커서 라인의 끝점
	cursorCoordDes.x = cursorCoordSrc.x;
	cursorCoordDes.y = cursorCoordSrc.y+15;
}

void CEditBox::EnableSet(bool flags)
{
	if(textBuffer.empty())	//비어 있을 경우만 활성화가 꺼진다.
		this->bEnable = flags;
}

void CEditBox::ReadEdit(char ch)
{
	if(textBuffer.size() > 58)		//최대 쓰기 가능한 문자 수
		return;
	if(ch==13)		//엔터키라면
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

	//에디트 박스에 글자 쓰기
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
	
	//스태틱 박스에 글 쓰기
	char staticText[1024];
	for(ret=0;ret<staticBuffer.size();ret++)
	{
		staticText[ret] = staticBuffer[ret];
	}
	staticText[ret] = '\0';
	han2unicode(staticText,transUnicode);
	
	ttfBlended = TTF_RenderUNICODE_Blended(font,transUnicode,color);
	
	SDL_BlitSurface(ttfBlended,NULL,pScreen,&staticDrawTextSrc);
	

	//읽고 쓰기 에디트 박스 (커서가 보임) 활성화 시만 텍스트 그림
	//커서 그리기
	if(bEnable)		//에디트 박스가 활성화인 경우
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
        case 1: { /* 8-bpp 라고 가정 */
            Uint8 *bufp;

            bufp = (Uint8 *)pScreen->pixels + y*pScreen->pitch + x;
            *bufp = color;
        }
        break;

        case 2: { /* 아마 15-bpp 아니면 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)pScreen->pixels + y*pScreen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3: { /* 느린 24-bpp mode, 보통 사용되지 않는다 */
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

        case 4: { /* 아마 32-bpp */
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

