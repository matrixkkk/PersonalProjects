#ifndef  _GAMESTATE_H_
#define _GAMESTATE_H_

#include "GameApp.h"


//class CGameApp;

class CGameState {

public:

    static CGameState* CreateInstance() { return 0; }

    virtual void Update                   ( CGameApp* game ) = 0;
    virtual void Draw                      ( CGameApp* game ) = 0;
    virtual void HandleEvents         ( CGameApp* game ) = 0;

    virtual void Init                          ( void ) = 0;
    virtual void Release                 ( void ) = 0;

public:

    virtual ~CGameState(){ }

protected:

    void ChangeState( CGameApp *gameApp, CGameState *pGameState )
    {
        gameApp->ChangeState( pGameState );
    }

protected:

    CGameState() { }
    CGameState( const CGameState& rhs ) { }

};

#endif

