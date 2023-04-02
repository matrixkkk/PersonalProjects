/*

#ifndef  _PLAYERSTATE_H_
#define _PLAYERSTATE_H_

#include "GameApp.h"

enum PlayerState{ STAND,  RIGHTMOVE,  LEFTMOVE, UPMOVE, DOWNMOVE, RIGHTSWING, LEFTSWING, LEFTDIVING,  RIGHTDIVING,  SMASHING, SURVE };
enum RuleState   { SURVEMODE, BATTLEMODE};
enum ActionType { PLAYMODE, DELAYMODE };
enum Direction    { LEFTDIRECTION, RIGHTDIRECTION, FRONTDIRECTION };

class CPlayerState {

public:

    static CPlayerState* CreateInstance() { return 0; }

    virtual void Swing            ( void ) = 0;
    virtual void Diving            ( void ) = 0;
    virtual void LeftWalk         ( void ) = 0;
    virtual void RightWalk       ( void ) = 0;
    virtual void UpWalk          ( void )  = 0;
    virtual void DownWalk      ( void ) = 0;


public:

    virtual ~CPlayerState(){ }

protected:

    void Update( CGameApp* game );



protected:

    CPlayerState() { }
    CPlayerState( const CPlayerState& rhs ) { }

};

#endif

*/