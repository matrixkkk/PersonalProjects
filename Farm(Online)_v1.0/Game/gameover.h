#ifndef GAME_OVER_H
#define GAME_OVER_H

#include "gamestate.h"
#include "gameengine.h"

class CGameOver : public CGameState
{
	SDL_Surface		*end1,*end2,*end3,*end4;
	int frame;

public:
	CGameOver() { }

	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();
	void Collect(MYDATA *data);			//데이터 수집 함수
	
	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);	
};


#endif
