
#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <SDL/SDL.h>
#include "gamestate.h"
#include <SDL/SDL_mixer.h>

class CIntroState : public CGameState
{
    SDL_Surface		*bg;
	SDL_Surface     *push;			//��ư �̹����� �ε��� ǥ��.
	SDL_Rect		anykey;	

	int				frame;
	Mix_Music       *m_Bgm;			//���� �ε�.

public:
	CIntroState() { }

	void Init(CGameEngine* game);
	void Cleanup();

	void Pause();
	void Resume();
	void Collect(MYDATA *data);			//������ ���� �Լ�
	
	void HandleEvents(CGameEngine* game);
	void Update(CGameEngine* game);
	void Draw(CGameEngine* game);	
};

#endif
