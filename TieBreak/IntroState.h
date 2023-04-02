#ifndef  _INTROSTATE_H_
#define _INTROSTATE_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "GameState.h"

class CIntroState : public CGameState {

private:

    static CIntroState      *m_Instance;                                // �ν��Ͻ� ������
    Mix_Music                 *m_Bgm;                                      // BackGround Music
    SDL_Surface             *m_BgSurface;                             // ��� ȭ��

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
