#include "gameover.h"

#include <SDL/SDL.h>
#include "gameengine.h"
#include "gamestate.h"
#include "introstate.h"
#include "mapstate.h"


CGameOver gameOver;;
extern CMapState mapState;

void CGameOver::Init(CGameEngine* game)
{
	frame=0;
	SDL_Surface* temp;
	temp = SDL_LoadBMP("image/ending_1_1.bmp");
	end1 = SDL_DisplayFormat(temp);
	temp = SDL_LoadBMP("image/ending_1_2.bmp");
	end2 = SDL_DisplayFormat(temp);
	temp = SDL_LoadBMP("image/ending_2_1.bmp");
	end3 = SDL_DisplayFormat(temp);
	temp = SDL_LoadBMP("image/ending_2_2.bmp");
	end4 = SDL_DisplayFormat(temp);
		    
    SDL_FreeSurface(temp);
}

void CGameOver::Cleanup()
{
	SDL_FreeSurface(end1);
	SDL_FreeSurface(end2);
	SDL_FreeSurface(end3);
	SDL_FreeSurface(end4);
}

void CGameOver::Pause()
{
}

void CGameOver::Resume()
{
}

void CGameOver::HandleEvents(CGameEngine* game)
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
					case SDLK_UP:
						frame--;
						if(frame < 0) frame=0;
						frame=frame%2;
						break;
					case SDLK_DOWN:
						frame++;
						frame=frame%2;
						break;
					case SDLK_RETURN:
						if(frame==0)
							game->ChangeState( &mapState);
						else
							game->Quit();
						break;					
				}
				break;
		}
	}
}

void CGameOver::Update(CGameEngine* game)
{

}

void CGameOver::Draw(CGameEngine* game)
{
	if(game->map==0)	//1p ½Â¸®
	{
		if(frame==0)
		{
			SDL_BlitSurface(end1, NULL, game->screen, NULL);
		}
		else
		{
			SDL_BlitSurface(end2, NULL, game->screen, NULL);
		}
	}
	else
	{
		if(frame==0)
		{
			SDL_BlitSurface(end3, NULL, game->screen, NULL);
		}
		else
		{
			SDL_BlitSurface(end4, NULL, game->screen, NULL);
		}
	}	
}
