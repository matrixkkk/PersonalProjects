#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <windows.h>

#include "ImageButton.h"


CImageButton::CImageButton( int x, int y, int w, int h, const  std::string &imagename )
{
    SDL_Surface *temp =SDL_LoadBMP( imagename.c_str()  );

     if( NULL != temp )
    {
        m_Image = SDL_DisplayFormat( temp );
        SDL_FreeSurface( temp );
    }
    else
         MessageBox( NULL, "SDL_DisplayFormat ERROR", "ERROR", NULL);


    if( NULL != m_Image )
    {
        SDL_SetColorKey( m_Image, SDL_SRCCOLORKEY, SDL_MapRGB( m_Image->format, 255, 255, 255 ) );
    }

    m_Box.x     = x;
    m_Box.y     = y;
    m_Box.w    = w;
    m_Box.h     = h;

    p.x = x;
    p.y = y;

    bClicked = false;


}

void CImageButton::Draw( CGameApp* game )
{
    SDL_BlitSurface( m_Image, NULL, game->getScreen(), &m_Box );
}

void CImageButton::Release()
{
    if( NULL != m_Image ) SDL_FreeSurface( m_Image );
}

bool CImageButton::CheckCollision( int x, int y)
{
    if( ( x > m_Box.x ) && ( x < m_Box.x + m_Box.w ) && ( y > m_Box.y ) && ( y < m_Box.y + m_Box.h ) )
    {

        return bClicked = true;;
    }
    return false;
}
