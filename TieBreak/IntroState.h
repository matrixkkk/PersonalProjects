#ifndef  _INTROSTATE_H_
#define _INTROSTATE_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "GameState.h"

class CIntroState : public CGameState {

private:

    static CIntroState      *m_Instance;                                // 인스턴스 포인터
    Mix_Music                 *m_Bgm;                                      // BackGround Music
    SDL_Surface             *m_BgSurface;                             // 배경 화면

public:

        static CIntroState* CreateInstance() {

        if( 0 == m_Instance )
        {
            m_Instance = new CIntroState;
        }

            return m_Instance;
        }


        ~CIntroState(){ }

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game );

    virtual void Init                          ( void );
    virtual void Release                 ( void );


private:

    CIntroState(){ }
    CIntroState( const CIntroState& rhs ){  }


};


#endif
