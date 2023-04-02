#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "GameApp.h"
#include "IntroState.h"
#include "MenuState.h"


CIntroState *CIntroState::m_Instance = 0;

void CIntroState::Init()
{
    SDL_Surface *temp = SDL_LoadBMP("BMP/intro.bmp");

    m_BgSurface = SDL_DisplayFormat( temp );

    SDL_FreeSurface( temp );

    SDL_ShowCursor( false );

}

void CIntroState::Draw( CGameApp* game )
{
    SDL_BlitSurface( m_BgSurface, NULL, game->getScreen(), NULL );
    SDL_Flip( game->getScreen() );
}

void CIntroState::Release()
{
	SDL_FreeSurface(m_BgSurface);
}

void CIntroState::Update( CGameApp* game )
{

}

void CIntroState::HandleEvents( CGameApp* game )
{
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {

					case SDLK_ESCAPE:

                        game->Quit();
						break;

                    case SDLK_SPACE:

                        ChangeState( game, CMenuState::CreateInstance() );

                        break;

				}
				break;

		}
	}
}


