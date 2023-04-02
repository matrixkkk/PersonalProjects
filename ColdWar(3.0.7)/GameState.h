#ifndef CGAMESTATE_H_
#define CGAMESTATE_H_

#include "main.h"

class CGameState 
{
public:
	virtual bool Init() = 0;
	virtual void ProcessInput() = 0;
	virtual void UpdateFrame() = 0;
	virtual void RenderFrame() = 0;
	virtual void Cleanup() = 0;
};


#endif
