#ifndef  _TEAMSELECTSTATE_H_
#define _TEAMSELECTSTATE_H_

#include "GameState.h"
#include "ImageButton.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <map>

class CTeamSelectState : public CGameState {

private:

    enum TeamArray{ MBC = 0, HANBIT, KTF, T1, GZ, SOUL, KHAN, TOP };

    static CTeamSelectState     *m_Instance;        // 인스턴스

    Mix_Music                            *m_Bgm;               // BackGround Music
    SDL_Surface                        *m_BgSurface;   //
    CImageButton                       *buttonStart;
    CImageButton                       *buttonCancel;
    int                                         selectTeam;

    bool                                     selected;

    std::map< int , CImageButton* > m_BoxList;


public:

        static CTeamSelectState* CreateInstance() {

            if( !m_Instance )
                m_Instance = new CTeamSelectState;

            return m_Instance;
        }

        ~CTeamSelectState() { }

        void SlectTeam( CGameApp* game, int x, int y );

public:

    virtual void Update                   ( CGameApp* game );
    virtual void Draw                      ( CGameApp* game );
    virtual void HandleEvents         ( CGameApp* game ) ;

    virtual void Init                          ( void );
    virtual void Release                 ( void );

private:

    CTeamSelectState() { }
    CTeamSelectState( const CTeamSelectState& rhs) { }

};



#endif

