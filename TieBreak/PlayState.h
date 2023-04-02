#ifndef  _PLAYSTATE_H_
#define _PLAYSTATE_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <deque>
#include "GameState.h"
#include "Player.h"             //ĳ����
#include "ball.h"               //��
#include "gamecontrol.h"        //���� ����
#include "AI.h"


#define TARGET_FPS 30

enum EventType{ SERVEFAULT, WOMANWIN, MANWIN,  NOEVENT };

class CPlayState : public CGameState {

private:
    static CPlayState           *m_Instance;        // �ν��Ͻ� ������
protected:

    Mix_Music                   *m_Bgm;             // BackGround Music
    SDL_Surface                 *m_BgSurface;        // ��� ȭ��
    SDL_Surface                 *m_Event;
    SDL_Surface                 *m_Score;
    SDL_Surface                 *userscore;
    SDL_Surface                 *comscore;
    CGameApp                    *m_Game;            // GameEngine;
    CPlayer                     *m_ControlPlayer;   //1P
    CPlayer                     *m_ControlPlayer1;  //2P
    CBall                       *ball;              //��
    Cgameplaycontrol            *Control;           //���� ��Ʈ��
    TTF_Font                    *font;
    CPoint3                     ballPoint;
    int                         eventtype;


    std::deque<std::string>     m_CharacterOrder;   //ĳ���� ����

public:

    static CPlayState* CreateInstance() {

        if( 0 == m_Instance )
        {
            m_Instance = new CPlayState;
        }
        return m_Instance;
    }


        ~CPlayState(){ }
        void GetGameApp( CGameApp* game ){  m_Game = game; }

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game );

    virtual void Init                    ( void );
    virtual void Release                 ( void );

    void DrawEvent( CGameApp* game );
    void DrawScore( CGameApp* game );
    void SetEvent( int type ){  eventtype = type; }
    int GetEvent(){    return eventtype;   }


protected:

    CPlayState(){ }
    CPlayState( const CPlayState& rhs ){  }


};


#endif
