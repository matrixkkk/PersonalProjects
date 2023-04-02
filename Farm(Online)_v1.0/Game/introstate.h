
#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <SDL/SDL.h>
#include "gamestate.h"
#include <SDL/SDL_mixer.h>

class CIntroState : public CGameState
{
    SDL_Surface		*bg;
	SDL_Surface     *push;			//버튼 이미지를 로딩할 표면.
	SDL_Rect		anykey;	

	int				frame;
	Mix_Music       *m_Bgm;			//음악 로딩.

public:
	CIntroState() { }

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
