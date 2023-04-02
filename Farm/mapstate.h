#ifndef MAPSTATE_H
#define MAPSTATE_H

#include "gamestate.h"
#include "common.h"

class CMapState :public CGameState
{

	 SDL_Surface		*bg;		//배경
	 SDL_Surface		*selete_bar;//선택 바.
	 SDL_Rect			r_select;
	 int				frame_x,frame_y;		//프레임		
	 int map;
public:
	
	CMapState() { }
	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);
	
	
};

#endif
