
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "gameengine.h"


class CGameState
{
public:
	bool play;
	CGameState() { }
	virtual void Init(CGameEngine* game) = 0;
	virtual void Cleanup() = 0;

	virtual void Pause() = 0;
	virtual void Resume() = 0;

	virtual void HandleEvents(CGameEngine* game) = 0;
	virtual void Update(CGameEngine* game) = 0;
	virtual void Draw(CGameEngine* game) = 0;
	virtual MYDATA* Collect() = 0;
	
};

#endif
