#ifndef  _MENUSTATE_H_
#define _MENUSTATE_H_

#include "GameState.h"
#include "ImageButton.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <map>

class CMenuState : public CGameState {

private:

    enum GameType { ARCADE, STORY, QUIT };

    static CMenuState		*m_Instance;
    SDL_Surface             *m_BgSurface;                           //BackGroun Screen;
    Mix_Music               *m_Bgm;                                    //BackGround Music

    std::map< GameType, CImageButton* > m_BoxList;

public:

        static CMenuState* CreateInstance() {

            if( 0 == m_Instance )
            {
                m_Instance = new CMenuState;
            }

            return m_Instance;
        }

        ~CMenuState() { }

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game ) ;

    virtual void Init                          ( void );
    virtual void Release                 ( void );


private:

    CMenuState() { }
    CMenuState( const CMenuState& rhs) { }

};


#endif


