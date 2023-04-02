
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SDL/SDL.h>
#include "common.h"
#include <SDL/SDL_mixer.h>


class CGameState;

class CGameEngine
{
public:

	void Init(const char* title, int width=480, int height=272,
		      int bpp=0, bool fullscreen=false);
	void Cleanup();

	void ChangeState(CGameState* state);
	void PushState(CGameState* state);
	void PopState();

	void HandleEvents();
	void Update();
	void Draw();
	void UpdateScreen(void);

	void RegulateFPS(void);
	
	bool Running() { return m_running; }
	void Quit() { m_running = false; }

	int map;
	
	SDL_Surface* screen;		
	
private:
	// the stack of states
	vector<CGameState*> states;
	
	bool m_running;
	bool m_fullscreen;

  Uint32 curTicks;
};

#endif
