#ifndef  _GAMEAPP_H_
#define _GAMEAPP_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include "Team.h"
#include "StageParameter.h"

#define TARGET_FPS  30


//--------------------------------------------
const int SCREEN_WIDTH        = 800;  //윈도우 가로
const int SCREEN_HEIGHT       = 600;  //윈도우 세로
const int SCREEN_BPP          = 24;
//--------------------------------------------

class CGameState;

class CGameApp {

private:

//    typedef std::vector<CGameState*> GameVector;
//    GameVector              m_GameStack;

    CGameState              *m_GameState;                                            // 게임상태 변수
    SDL_Surface             *m_Screen;
    TTF_Font                *font;
    bool                    m_Running;
    CPlayerList             m_PlayerList;
    CStageParameter         m_StageParameter;
    Uint32                  m_CurTick;


public:

    void ChangeState( CGameState *pGameState );    // 게임상태 변경

public:

//-----------------------------------------
    void HandleEvents();
    void Update();
    void Draw();
    void UpdateScreen();
    void Init();
    void Release();

    bool Run(){ return m_Running; }
    void Quit(){ m_Running = false; }
//-----------------------------------------
    void RegulateFPS();

    CPlayerList*          GetPlayerList()       { return &m_PlayerList;              }
    CStageParameter*      GetStageParameter()   { return &m_StageParameter;     }


//-----------------------------------------

    SDL_Surface* getScreen(){ return m_Screen; }

};


#endif

