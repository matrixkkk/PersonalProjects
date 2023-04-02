#ifndef  _IMAGEBUTTON_H_
#define _IMAGEBUTTON_H_

#include <SDL/SDL.h>
#include <string>
#include <windows.h>

#include "GameApp.h"


// Image Button Ŭ���� �޴� ���࿡�� ���� �̹����� ��ǥ�ϴ� Ŭ����
// Ȯ�强�� ��� Super Class ����
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
