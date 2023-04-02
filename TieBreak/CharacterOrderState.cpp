#include <windows.h>
#include <string>

#include "CharacterOrderState.h"
#include "TeamSelectState.h"
#include "MapSelectState.h"
#include "Team.h"


CCharacterOrderState *CCharacterOrderState::m_Instance = 0;

void CCharacterOrderState::Init()
{
    SDL_Surface *temp = SDL_LoadBMP( "BMP/characterselect.bmp" );
    m_BgSurface = SDL_DisplayFormat( temp );

    temp = SDL_LoadBMP("BMP/arrow.bmp");
    m_Arrow = SDL_DisplayFormat( temp );

    temp = SDL_LoadBMP("BMP/number.bmp");
    m_Numver = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    if ( ( m_Bgm = Mix_LoadMUS("SOUND/toyourbear.mp3") ) == NULL )
    {
        MessageBox( NULL, "Sound Open Fail", "Sound Open Fail", MB_OK );
        return;
    }

    if( Mix_PlayMusic( m_Bgm, -1 ) == -1 )
    {
        MessageBox( NULL, "Mix_PlayMusic Fail", "Mix_PlayMusic Fail", MB_OK );
        return;
    }

    SDL_ShowCursor( true );

    buttonStart      = new CImageButton( 195,  480, 170, 45, "BMP/team_start.bmp"        );
    buttonCancel     = new CImageButton( 455,  480, 170, 45, "BMP/team_cancel.bmp"       );

    SDL_SetColorKey( m_Numver, SDL_SRCCOLORKEY, SDL_MapRGB( m_Numver->format, 255, 255, 255 ) );

    for( int i = 0; i < 3; i++ )
    {
        m_NumberBox[i].x =   i   * 70;
        m_NumberBox[i].y =           0;
        m_NumberBox[i].w =         70;
        m_NumberBox[i].h =          66;
    }

    selectOrder = false;

}

void CCharacterOrderState::GetPlayerList( CPlayerList *PlayerList )
{
    std::vector<CDBPlayer*> PlayerNameList;

    PlayerList->GetPlayerList( m_SelectedTeam, PlayerNameList );

    std::vector< CDBPlayer* >::const_iterator it = PlayerNameList.begin();

    m_CharacterList[ (*it)->GetName() ] =    new CImageButton( 130, 200, 170, 240, (*it++)->GetBitmapName()  )   ;
    m_CharacterList[ (*it)->GetName() ] =    new CImageButton( 330, 200, 170, 240, (*it++)->GetBitmapName()  )   ;
    m_CharacterList[ (*it)->GetName() ] =    new CImageButton( 530, 200, 170, 240, (*it++)->GetBitmapName()  )   ;

}

void CCharacterOrderState::Draw( CGameApp* game )
{
    static int x1 = 0, x2 = 0;

    SDL_Rect o = {   0,   0, 800, 60 };
    SDL_Rect o1={   0, 60, 800, 60 };
    SDL_Rect r1, r2, r3, r4;

    r1.x = x1;
    r1.y = 0;
    r1.w = 800;
    r1.h = 60;

    r2.x = 800 + x1;
    r2.y = 0;
    r2.w = 800;
    r2.h = 60;

    r3.x = x2;
    r3.y = 540;
    r3.w = 800;
    r3.h = 60;

    r4.x = -800 + x2;
    r4.y = 540;
    r4.w = 800;
    r4.h = 60;

    x1 -=1;
    x2 +=1;
    if( x1 <= -800 ) x1 = 0;
    if( x2 >=  800 ) x2 = 0;

    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );

    SDL_BlitSurface( m_Arrow, &o, game->getScreen(),   &r1 );
    SDL_BlitSurface( m_Arrow, &o, game->getScreen(),   &r2 );
    SDL_BlitSurface( m_Arrow, &o1, game->getScreen(), &r3 );
    SDL_BlitSurface( m_Arrow, &o1, game->getScreen(), &r4 );

    for( std::map< std::string,  CImageButton* >::const_iterator it = m_CharacterList.begin() ; it != m_CharacterList.end(); ++it )
    {
        (it->second)->Draw( game );
    }

    buttonStart->Draw( game );
    buttonCancel->Draw( game);

    SDL_Rect temp;
    if( !m_PointList.empty() )
    {
        for( int i = 0; i < m_PointList.size(); i++ )
        {
            temp.x = m_PointList[ i ].x;
            temp.y = m_PointList[ i ].y;
            temp.w = 70;
            temp.h = 66;

            SDL_BlitSurface( m_Numver, &m_NumberBox[i], game->getScreen(), &temp );
        }

    }
    SDL_Flip( game->getScreen() );

}

void CCharacterOrderState::Release()
{
	Mix_HaltMusic();
	Mix_FreeMusic    (    m_Bgm               );

	SDL_FreeSurface(    m_BgSurface     );
	SDL_FreeSurface(    m_Arrow            );
	SDL_FreeSurface(    m_Numver         );

	for( std::map< std::string, CImageButton* >::const_iterator it = m_CharacterList.begin() ; it != m_CharacterList.end(); ++it )
    {
        delete (it->second);
    }
    m_CharacterList.clear();

    m_PointList.clear();
    m_CharacterOrder.clear();

    delete buttonStart;
    delete buttonCancel;

}

void CCharacterOrderState::Update( CGameApp* game )
{

}

void  CCharacterOrderState::SelectOrder( CGameApp* game, int x, int y )
{
    static int nCnt = 0;
    POINT p ;

    for( std::map< std::string, CImageButton* >::const_iterator it  = m_CharacterList.begin() ; it != m_CharacterList.end(); ++it )
    {
        if( !(it->second)->GetClicked() && (it->second)->CheckCollision( x, y) )
        {
            m_CharacterOrder.push_back( it->first );
            ++nCnt;
             p.x = ( it->second )->GetPoint().x + 40;
             p.y = ( it->second )->GetPoint().y - 50;

            m_PointList.push_back( p );
            break;
        }
    }

    if( nCnt >= 3 && buttonStart->CheckCollision( x, y ) )
    {
        game->GetStageParameter()->SetCharacter( m_CharacterOrder );
        ChangeState( game, CMapSelectState::CreateInstance() );                            //  플레이어 선택 모드로 전환
    }

    if( buttonCancel->CheckCollision( x, y ) )
    {
        ChangeState( game, CTeamSelectState::CreateInstance() );
    }

}

void CCharacterOrderState::HandleEvents( CGameApp* game )
{
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

            case SDL_MOUSEBUTTONDOWN:
                        SelectOrder( game, event.button.x, event.button.y );
                        break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE:
                        ChangeState( game, CTeamSelectState::CreateInstance() );
						break;

				}
				break;

		}
	}
}
