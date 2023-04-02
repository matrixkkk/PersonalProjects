#include <SDL/SDL.h>
#include "gameengine.h"
#include "gamestate.h"
#include "introstate.h"
#include "mapstate.h"


CIntroState introState;
extern CMapState mapState;

void CIntroState::Init(CGameEngine* game)
{
	SDL_Surface* temp;
	temp = SDL_LoadBMP("image/intro.bmp");
	bg = SDL_DisplayFormat(temp);
	
	temp = SDL_LoadBMP("image/push.bmp");
	
	push=SDL_DisplayFormat(temp);
	SDL_SetColorKey( push, SDL_SRCCOLORKEY, SDL_MapRGB( push->format, 255, 0, 0) );
	SetSDLRect(anykey,260,540,300,60);
	frame=0;
	
	m_Bgm = Mix_LoadMUS("music/intro_music.mid");
	if(m_Bgm==NULL)
	{
		cout<<"À½¾Ç ·Îµù ½ÇÆÐ"<<endl;
	}

	Mix_PlayMusic( m_Bgm, -1 ); 
    
    SDL_FreeSurface(temp);
}

void CIntroState::Cleanup()
{
	SDL_FreeSurface(bg);
	SDL_FreeSurface(push);	
	 //À½¾Ç Á¤Áö
  Mix_HaltMusic();  
  Mix_FreeMusic(m_Bgm);

}

void CIntroState::Pause()
{
}

void CIntroState::Resume()
{
}

void CIntroState::HandleEvents(CGameEngine* game)
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
					default:
						game->ChangeState( &mapState);
						break;
				}
				break;
		}
	}
}

void CIntroState::Update(CGameEngine* game)
{
	frame++;	
	frame=frame%60;	
}

void CIntroState::Draw(CGameEngine* game)
{
	SDL_BlitSurface(bg, NULL, game->screen, NULL);
	if(frame < 30)
	{
		SDL_BlitSurface(push, NULL, game->screen, &anykey);
	}
}



