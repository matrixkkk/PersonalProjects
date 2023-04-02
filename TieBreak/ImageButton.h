#ifndef  _IMAGEBUTTON_H_
#define _IMAGEBUTTON_H_

#include <SDL/SDL.h>
#include <string>
#include <windows.h>

#include "GameApp.h"


// Image Button 클래스 메뉴 진행에서 각종 이미지를 대표하는 클래스
// 확장성이 없어서 Super Class 없음
class CImageButton{

    SDL_Rect            m_Box;
    SDL_Surface      *m_Image;
    bool                   bClicked;
    POINT                 p;

public:

    CImageButton( int x, int y, int w, int h, const std::string &imagename );
    ~CImageButton(){ Release(); }

    void     Draw( CGameApp *game);
    void     Release();
    bool     CheckCollision( int x, int y );
    bool     GetClicked(){  return bClicked;    }
    POINT   GetPoint(){ return p;   }


};

#endif
