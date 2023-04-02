#include <SDL/SDL.h>
#include "gameengine.h"
#include "gamestate.h"
#include "playState.h"
#include "mapstate.h"


CMapState mapState;
extern CPlayState playState;

void CMapState::Init(CGameEngine* game)
{
	srand(time(NULL));
	frame_x=0,frame_y=0;

	SDL_Surface* temp;
	temp = SDL_LoadBMP("image/stage.bmp");
	bg = SDL_DisplayFormat(temp);
		
	temp = SDL_LoadBMP("image/select.bmp");
	selete_bar=SDL_DisplayFormat(temp);

	SDL_SetColorKey( selete_bar, SDL_SRCCOLORKEY, SDL_MapRGB( selete_bar->format, 255, 0, 0) );
	

	SetSDLRect(r_select,228,179,151,113);
/*	m_Bgm = Mix_LoadMUS("music/intro_music.mid");
	if(m_Bgm==NULL)
	{
		cout<<"À½¾Ç ·Îµù ½ÇÆÐ"<<endl;
	}

	Mix_PlayMusic( m_Bgm, -1 ); 
    */
    SDL_FreeSurface(temp);
}

void CMapState::Cleanup()
{
	SDL_FreeSurface(bg);
	SDL_FreeSurface(selete_bar);
		
	 //À½¾Ç Á¤Áö
  //Mix_HaltMusic();  
 // Mix_FreeMusic(m_Bgm);

}

void CMapState::Pause()
{
}

void CMapState::Resume()
{
}

void CMapState::HandleEvents(CGameEngine* game)
{
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->Quit();
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {					
					case SDLK_RETURN:					
						if(frame_x==0 && frame_y==0)
						{
							map=0;
						}
						else if(frame_x==1 && frame_y==0)
						{
							map=3;
						}
						else if(frame_x==0 && frame_y==2)
						{
							map=1;
						}
						else if(frame_x==1 && frame_y==2)
						{
							map=2;
						}
						else
						{
							map=rand()%4;
						}
						game->map=map;
						game->ChangeState( &playState);
						break;
					case SDLK_LEFT:
						frame_x--;
						if(frame_x < 0) frame_x=0;
						break;
					case SDLK_RIGHT:
						frame_x++;
						frame_x=frame_x%2;
						break;
					case SDLK_UP:
						frame_y--;
						if(frame_y<0) frame_y=0;
						break;
					case SDLK_DOWN:
						frame_y++;
						frame_y=frame_y%3;
						break;
				}
				break;
		}
	}
}

void CMapState::Update(CGameEngine* game)
{
	r_select.x=228+frame_x*179;
	r_select.y=179+frame_y*135;
	if(frame_y==1)
	{
		r_select.x=320;
	}
}

void CMapState::Draw(CGameEngine* game)
{
	SDL_BlitSurface(bg, NULL, game->screen, NULL);	
	SDL_BlitSurface(selete_bar, NULL, game->screen,&r_select );
}

