#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <windows.h>

#include "GameApp.h"
#include "MenuState.h"
#include "TeamSelectState.h"

CMenuState *CMenuState::m_Instance = 0;


void CMenuState::Init()
{
//------------- BMP Setting-------------------
    SDL_Surface *temp = SDL_LoadBMP("BMP/menu.bmp");

    m_BgSurface = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    m_BoxList[ ARCADE ] = new CImageButton( 320, 420, 180, 45, "BMP/menu_arcade.bmp" );
    m_BoxList[ STORY   ] = new CImageButton( 320, 465, 180, 45, "BMP/menu_story.bmp" );
    m_BoxList[ QUIT      ] = new CImageButton( 320, 510, 180, 45, "BMP/menu_quit.bmp" );

    if ( ( m_Bgm = Mix_LoadMUS("SOUND/missyou.mp3") ) == NULL )
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

}

void CMenuState::Draw( CGameApp* game )
{
    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );

    for( std::map< GameType, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
    {
        ( it->second )->Draw( game );
    }
//   for_each( m_BoxList.begin(), m_BoxList.begin(), bind2nd( std::mem_fun( &CImageButton::Draw ), game  ) );

    SDL_Flip( game->getScreen() );
}

void CMenuState::Release()
{
    Mix_HaltMusic();
	Mix_FreeMusic(    m_Bgm              );

    SDL_FreeSurface(    m_BgSurface     );

    if( !m_BoxList.empty() )
    {
        for( std::map< GameType, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
        {
            ( it->second )->Release();
        }

        m_BoxList.clear();
    }


}

void CMenuState::Update( CGameApp* game )
{

}

void CMenuState::HandleEvents( CGameApp* game )
{
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

            case SDL_MOUSEBUTTONDOWN:

                for( std::map< GameType, CImageButton* >::const_iterator it = m_BoxList.begin() ; it != m_BoxList.end(); ++it )
                {
                    if( ( it->second )->CheckCollision( event.button.x, event.button.y) )
                    {
                        switch( it->first )
                        {
                            case ARCADE:
                                game->GetStageParameter()->SetPlayType(0);
                                ChangeState( game, CTeamSelectState::CreateInstance() );

                                break;

                            case STORY:
                                game->GetStageParameter()->SetPlayType(1);
                                ChangeState( game, CTeamSelectState::CreateInstance() );
                                break;
                            case QUIT:
                                game->Quit();
                                break;
                        }
                        break;
                    }
                }
                break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE:

                        game->Quit();
						break;


				}
				break;

		}
	}
}
