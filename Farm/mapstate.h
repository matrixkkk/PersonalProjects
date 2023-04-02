#ifndef MAPSTATE_H
#define MAPSTATE_H

#include "gamestate.h"
#include "common.h"

class CMapState :public CGameState
{

	 SDL_Surface		*bg;		//���
	 SDL_Surface		*selete_bar;//���� ��.
	 SDL_Rect			r_select;
	 int				frame_x,frame_y;		//������		
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
