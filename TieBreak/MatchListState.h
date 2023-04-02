/*

#ifndef  _MATCHLISTSTATE_H_
#define _MATCHLISTSTATE_H_

#include "GameState.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class CMatchListState : public CGameState {

private:

    static CMatchListState* m_Instance;
    Mix_Music *m_Bgm;                                      // BackGround Music

public:

        static CMatchListState* CreateInstance() {

            if( !m_Instance )
                m_Instance = new CIntroState;

            return m_Instance;
        }

        ~CMatchListState() { }

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game ) ;

    virtual void Init                          ( void );
    virtual void Release                 ( void );


private:

    CMatchListState() { }
    CMatchListState( const CMatchListState& rhs) { }

};

static CMatchListState::m_Instance = 0;

#endif

*/