#ifndef  _CHARACTERORDESTATE_H_
#define _CHARACTERORDESTATE_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include <deque>


#include "GameState.h"
#include "ImageButton.h"
#include "Team.h"



class CCharacterOrderState : public CGameState {

private:

    enum TeamArray{ MBC = 0, HANBIT, KTF, T1, GZ, SOUL, KHAN, TOP };

    static CCharacterOrderState* m_Instance;
    Mix_Music                    *m_Bgm;                                      // BackGround Music
    SDL_Surface                *m_BgSurface;
    SDL_Surface                *m_Arrow;
    SDL_Surface                *m_Numver;

    SDL_Rect                       m_NumberBox[ 3 ];

    CImageButton               *buttonStart;
    CImageButton               *buttonCancel;


    int                                  m_SelectedTeam;
    bool                               selectOrder;

    std::map< std::string, CImageButton*>  m_CharacterList;
    std::deque<std::string>                         m_CharacterOrder;
    std::vector< POINT >                             m_PointList;

//    std::map< int, CImageButton* > m_BoxList;

public:

        static CCharacterOrderState* CreateInstance() {

            if( !m_Instance )
                m_Instance = new CCharacterOrderState;

            return m_Instance;
        }

        ~CCharacterOrderState() { }

        void GetPlayerList( CPlayerList *PlayerList );
        void SetTeam( int selectTeam ){ m_SelectedTeam = selectTeam;   }
        void SelectOrder(CGameApp* game, int x, int y );

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game ) ;

    virtual void Init                          ( void );
    virtual void Release                 ( void );


private:

    CCharacterOrderState() { }
    CCharacterOrderState( const CCharacterOrderState& rhs) { }

};



#endif
