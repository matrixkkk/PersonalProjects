#ifndef BAR_H_
#define BAR_H_

#include "common.h"
#include "gameengine.h"
#include "CTimer.h"

class CBar
{
private:
	SDL_Surface		*s_bar;
	SDL_Rect		r_bar;
	
	
	bool    bEnable;  // 콘트롤 쇼크를 위한 변수
	CTimer	timer;
	double	duration;
public:
	CBar() { }
	CBar(CPoint2 &pos);

	void Init(SDL_Surface *p_bar);
	void Draw(CGameEngine* game);

	void SetRect(CPoint2 &pos);

	void MoveLeft();
	void MoveRight();
	void UseControlShock();
	SDL_Rect GetRect()		{ return r_bar; }

};


#endif
