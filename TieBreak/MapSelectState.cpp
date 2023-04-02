#include "MapSelectState.h"
#include "PlayState.h"
#include "AIPlayState.h"
#include "CharacterOrderState.h"
#include <windows.h>




CMapSelectState *CMapSelectState::m_Instance = 0;

void CMapSelectState::Init()
{
    SDL_Surface *temp = SDL_LoadBMP("BMP/mapselect.bmp");

    m_BgSurface = SDL_DisplayFormat( temp );

    temp        = SDL_LoadBMP( "BMP/court_list.bmp" );

    m_MapList   = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    m_BoxList[ "treecourt"          ] = new CImageButton( 600,    50,  152, 115, "BMP/treecourt_small.bmp"            );
    m_BoxList[ "wildcourt"          ] = new CImageButton( 600,  180,  152, 115, "BMP/wildcourt_small.bmp"            );
    m_BoxList[ "lawncourt"          ] = new CImageButton( 600,  310,  152, 115, "BMP/lawncourt_small.bmp"           );
    m_BoxList[ "chrismascourt"      ] = new CImageButton(  600,  440,  152, 115, "BMP/chrismascourt_small.bmp"    );

    buttonStart         = new CImageButton( 100,  490, 170, 45, "BMP/team_start.bmp"        );
    buttonCancel        = new CImageButton( 300,  490, 170, 45, "BMP/team_cancel.bmp"    );


    if ( ( m_Bgm = Mix_LoadMUS("SOUND/comeagain.mp3") ) == NULL )
    {
        MessageBox( NULL, "Sound Open Fail", "Sound Open Fail", MB_OK );
        return;
    }
    if( Mix_PlayMusic( m_Bgm, -1 ) == -1 )
    {
        MessageBox( NULL, "Mix_PlayMusic Fail", "Mix_PlayMusic Fail", MB_OK );
    }

    SDL_ShowCursor( true );

    selected = false;

}

void CMapSelectState::Draw( CGameApp* game )
{
    SDL_Rect src, dest;

    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );

     for( std::map< std::string , CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
    {
        ( it->second )->Draw( game );
    }



     if( selected == true )
      {
          src.x     =  400*_x;
          src.y     =  300*_y;
          src.w     =  400;
          src.h     =  300;
        //--------------
          dest.x   = 70;
          dest.y   = 160;

          SDL_BlitSurface( m_MapList, &src, game->getScreen(), &dest );
      }

    buttonStart->Draw( game);
    buttonCancel->Draw( game);

    SDL_Flip( game->getScreen() );
}

void CMapSelectState::Release()
{
	Mix_HaltMusic();
	Mix_FreeMusic    (    m_Bgm               );

	SDL_FreeSurface(    m_BgSurface     );
	SDL_FreeSurface(    m_MapList          );

    if( !m_BoxList.empty() )
    {
        for( std::map< std::string, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
            delete (it->second);

        m_BoxList.clear();
    }

    delete buttonStart;
    delete buttonCancel;

}

void CMapSelectState::MapSelect(CGameApp* game, int x, int y )
{
    static std::string mapName;

    for( std::map< std::string, CImageButton* >::const_iterator it  = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
    {
        if(  (it->second)->CheckCollision( x, y) )
        {

            if( ( it->first )      == "treecourt" )
            {
                _x = _y = 0;
            }
            else if( ( it->first ) == "wildcourt" )
            {
                _x = 1;
                _y = 0;
            }
            else if( ( it->first ) == "lawncourt" )
            {
                _x = 0;
                _y = 1;
            }
            else if( ( it->first ) == "chrismascourt" )
            {
                _x = _y = 1;
            }

            selected = true;
            mapName = it->first;
            break;
        }
    }

    if( selected && buttonStart->CheckCollision( x, y )  )
    {
        game->GetStageParameter()->SetMap( mapName );
        if( 0 == game->GetStageParameter()->GetPlaytype() )
        {
            CPlayState::CreateInstance()->GetGameApp( game );
            ChangeState( game, CPlayState::CreateInstance() );                            //  플레이어 선택 모드로 전환
        }
        else
        {
            AIPlayState::CreateInstance()->GetGameApp( game );
            ChangeState( game, AIPlayState::CreateInstance() );
        }
    }

    if( buttonCancel->CheckCollision( x, y ) )
    {
        ChangeState( game, CCharacterOrderState::CreateInstance() );
    }

}


void CMapSelectState::Update( CGameApp* game )
{

}

void CMapSelectState::HandleEvents( CGameApp* game )
{
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
                Release();
				game->Quit();
				break;

            case SDL_MOUSEBUTTONDOWN:
                        MapSelect( game, event.button.x, event.button.y );
                        break;


			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE:
                        ChangeState( game, CCharacterOrderState::CreateInstance() );
						break;



				}
				break;

		}
	}
}

