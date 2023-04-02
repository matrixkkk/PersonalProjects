#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <windows.h>

#include "GameApp.h"
#include "TeamSelectState.h"
#include "MenuState.h"
#include "CharacterOrderState.h"


CTeamSelectState *CTeamSelectState::m_Instance = 0;

void CTeamSelectState::Init()
{

    SDL_Surface *temp = SDL_LoadBMP("BMP/teamselect.bmp");       //배경화면

    m_BgSurface = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    m_BoxList[ MBC          ] = new CImageButton( 130, 250, 83, 108, "BMP/logo_pos_1.bmp"       );
    m_BoxList[ HANBIT      ] = new CImageButton( 280, 250, 83, 108, "BMP/logo_hanbit_1.bmp"   );
    m_BoxList[ KTF            ] = new CImageButton( 430, 250, 83, 108, "BMP/logo_ktf_1.bmp"         );
    m_BoxList[ T1              ] = new CImageButton( 580, 250, 83, 108, "BMP/logo_t1_1.bmp"          );
    m_BoxList[ GZ             ] = new CImageButton( 130, 370, 83, 108, "BMP/logo_plus_1.bmp"      );
    m_BoxList[ SOUL         ] = new CImageButton( 280, 370, 83, 108, "BMP/logo_soul_1.bmp"      );
    m_BoxList[ KHAN         ] = new CImageButton( 430, 370, 83, 108, "BMP/logo_khan_1.bmp"      );
    m_BoxList[ TOP           ] = new CImageButton( 580, 370, 83, 108, "BMP/logo_stop_1.bmp"       );

    buttonStart         = new CImageButton( 195,  515, 170, 45, "BMP/team_start.bmp"          );
    buttonCancel     = new CImageButton( 455, 515, 170, 45, "BMP/team_cancel.bmp"       );

    if ( ( m_Bgm = Mix_LoadMUS("SOUND/HappyWednesday.mp3") ) == NULL )
    {
        MessageBox( NULL, "Sound Open Fail", "Sound Open Fail", MB_OK );
        return;
    }
    if( Mix_PlayMusic( m_Bgm, -1 ) == -1 )
    {
        MessageBox( NULL, "Mix_PlayMusic Fail", "Mix_PlayMusic Fail", MB_OK );
    }

    SDL_ShowCursor( true );
    SDL_EnableKeyRepeat( 10, SDL_DEFAULT_REPEAT_INTERVAL );

    selected      = false;
    selectTeam = 0;

}

void CTeamSelectState::Draw( CGameApp* game )
{
    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );

     for( std::map< int , CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
    {
        ( it->second )->Draw( game );
    }

    buttonStart->Draw( game);
    buttonCancel->Draw( game);

    SDL_Flip( game->getScreen() );
}

void CTeamSelectState::Release()
{

	Mix_HaltMusic();
	Mix_HaltMusic();
	Mix_FreeMusic(        m_Bgm              );
	SDL_FreeSurface(    m_BgSurface     );

	if( !m_BoxList.empty() )
    {
       for( std::map< int, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
            delete ( it->second );

        m_BoxList.clear();
    }

    delete buttonStart;
    delete buttonCancel;
    selected     = false;
    selectTeam = 0;

}

void CTeamSelectState::Update( CGameApp* game )
{

}

void  CTeamSelectState::SlectTeam( CGameApp* game, int x, int y )
{
    for( std::map< int, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
    {
        if( ( it->second )->CheckCollision( x, y) )
        {
                selectTeam = it->first;
                selected      = true;
                break;
        }
    }

    if( selected == true && buttonStart->CheckCollision( x, y ) )
    {
        CCharacterOrderState::CreateInstance()->SetTeam( selectTeam );                                 //  팀을 셋팅
        CCharacterOrderState::CreateInstance()->GetPlayerList( game->GetPlayerList()  );        //  플레이어 DB 목록을 가져와서 셋팅
        game->GetStageParameter()->SetTeam( selectTeam );
        ChangeState( game, CCharacterOrderState::CreateInstance() );                                     //  플레이어 선택 모드로 전환

    }

    if( buttonCancel->CheckCollision( x, y ) )
    {
        ChangeState( game, CMenuState::CreateInstance() );
    }
}



void CTeamSelectState::HandleEvents( CGameApp* game )
{
	SDL_Event event;

	if ( SDL_PollEvent(&event) ) {

		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

            case SDL_MOUSEBUTTONDOWN:
                SlectTeam( game, event.button.x, event.button.y );
                break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
                        ChangeState( game, CMenuState::CreateInstance() );
						break;
				}
				break;
		}
	}
}


