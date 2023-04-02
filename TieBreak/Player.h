#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <SDL/SDL.h>
#include <string>
#include "GameApp.h"
#include "common.h"

//#include "ball.h"

#define SPRITEFRMAE_SPEED   3       //스파라이트 스피드

enum Sex            { MAN = 100, WOMAN = 150 };
enum PlayerState    { STAND,RIGHTMOVE, LEFTMOVE, UPMOVE, DOWNMOVE, RIGHTSWING, LEFTSWING,LEFTDIVING,RIGHTDIVING,SMASHING,SURVE };
enum RuleState      { BATTLEMODE = 0, SURVEMODE};
enum ActionType     { PLAYMODE, DELAYMODE };
enum Direction      { LEFTDIRECTION, RIGHTDIRECTION, FRONTDIRECTION, NODIRECTION };

class CPlayer
{
    int         m_Strength;
    int         m_Speed;
    int         m_DefenceRange;
//---------------------------
    int         m_X, m_Y;
    int         frame;
    int         frame_size;
    int         state;
    int         direction;
    int         action;
    int         previous_action;
    int         CurrentRule;
    int         _state;

    bool        m_bLeft, m_bRight,m_bUp,m_bDown;
    bool        bSwing;

    SDL_Rect    m_Rect;     //그리기용 박스
    SDL_Rect    BoundBox;	//바운딩   박스
    //SDL_Rect    rect;       //캐릭터 충돌박스
//    SDL_Rect    m_RacketBox;
    SDL_Surface *m_Image;

public:
    // CPlayer(int type, std::string filename );
     CPlayer( int strength, int speed, int defencerange ) : m_Strength( strength ), m_Speed( speed ), m_DefenceRange( defencerange ){ }
    ~CPlayer(){ Release(); }
//---------키 조작-------------------------------
    void Stand          ( void );
    void LeftWalk       ( void );
    void RightWalk      ( void );
    void UpWalk         ( void );
    void DownWalk       ( void );
    void Swing          ( int x );//PlayerState SwingType );
    void Diving         ( int x );
    //void SwingBox       ( void );
//----------------------------------------------
    void Init           ( int type, std::string filename );
    void Release        ( void );
    void Reset          ( int type );
    void Draw           ( CGameApp* game );
    void Update         ( CGameApp* game );
//----------------------------------------------
    bool GetbSwing          (void)       { return bSwing;           }
    int GetStrength         (void) const { return m_Strength;       }
    int GetSpeed            (void) const { return m_Speed;          }
    int GetDefneceRange     (void) const { return m_DefenceRange;   }
    int GetState            (void) const { return state;            }
    int GetRuleState        (void) const { return CurrentRule;      }
	int Getaction           (void) const { return action;/*_state;*/      }//플레이어 액션 GET
	SDL_Rect& getRect       (void)       { return m_Rect;           }
//----------------------------------------------
    void SetPosition        ( int x, int y)  ;//{ m_Rect.x  = x; m_Rect.y = y;  }
    void SetLeft            ( bool type )    { m_bLeft   = type; }
    void SetRight           ( bool type )    { m_bRight  = type; }
    void SetUp              ( bool type )    { m_bUp     = type; }
    void SetDown            ( bool type )    { m_bDown   = type; }
    void SetState           ( int statetype ){ state = statetype;}
    void SetDirection       ( int direction ){ this->direction = direction; }
    void SetRuleMode        ( int type )     { CurrentRule = type;          }
    void Setaction          ( int act)     	 { _state = act;           }		//플레이어 액션 SET
    void SetBoundBox        ( int x, int y );
    void CheckDirection();
//----------------------------------------------
    SDL_Rect& getBoundingBox(void)           { return BoundBox;  }     //바운딩 박스 리턴..
    ball_physics Hit_Ball(int value,CPoint3 BallPos);   //공을 침
};

#endif
