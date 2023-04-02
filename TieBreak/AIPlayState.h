#ifndef _AIPLAYSTATE_H_
#define _AIPLAYSTATE_H_

#include <SDL/SDL.h>
#include <string>
#include "PlayState.h"
//#include "AI.h"

class AIPlayState : public CPlayState
{

private:
    static AIPlayState           *m_Instance;        // 인스턴스 포인터
    CAi *AI;

public:
    virtual void Update         ( CGameApp* game );
    virtual void Release        ( void );
    virtual void HandleEvents   ( CGameApp* game );
    virtual void Init           ( void );
    virtual void Draw           ( CGameApp* game );

public:
    static AIPlayState* CreateInstance() {

        if( 0 == m_Instance )
        {
            m_Instance = new AIPlayState;
        }

            return m_Instance;
    }

//    ~AIPlayState(){ }

};

#endif