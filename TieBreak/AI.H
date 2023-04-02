#ifndef _AI_H_
#define _AI_H_

#include "common.h"
#include "ball.h"
#include "Player.h"

class CAi
{
private:
	int x;	//이동해야할 x좌표
	int y;	//이동해야할 y좌표
	int action;		//0 :stand 1:left 이동 2:right 이동
public:
	CAi();
	void AIaction(CBall *ball,CPlayer *user,CPlayer *com);
};

#endif
