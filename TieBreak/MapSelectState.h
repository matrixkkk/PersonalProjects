#ifndef  _MENUSTATE_H_
#define _MENUSTATE_H_

#include "GameState.h"
#include "ImageButton.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <map>

class CMapSelectState : public CGameState {

private:


    static CMapSelectState	 	 *m_Instance;
    SDL_Surface                         *m_BgSurface;                           //BackGroun Screen;
    SDL_Surface                         *m_MapList;
    Mix_Music                             *m_Bgm;                                    //BackGround Music

    CImageButton                        *buttonStart;
    CImageButton                        *buttonCancel;

    std::map< std::string, CImageButton* > m_BoxList;

    bool                                       selected;
    int  _x;
    int  _y;

public:

        static CMapSelectState* CreateInstance() {

            if( 0 == m_Instance )
            {
                m_Instance = new CMapSelectState;
            }

            return m_Instance;
        }

        ~CMapSelectState() { }

        void MapSelect(CGameApp* game, int x, int y );

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game ) ;

    virtual void Init                          ( void );
    virtual void Release                 ( void );


private:

    CMapSelectState() { }
    CMapSelectState( const CMapSelectState& rhs) { }

};

#endif


