#ifndef BAR_H_
#define BAR_H_

#include "common.h"
#include "gameengine.h"

class CBar
{
private:
	SDL_Surface		*s_bar;
	SDL_Rect		r_bar;
	
	bool	left;
	bool	right;

	int     ControlS;  // 콘트롤 쇼크를 위한 변수

public:
	CBar() { }
	CBar(CPoint2 &pos);

	void Init(SDL_Surface *p_bar);
	void Draw(CGameEngine* game);

	void SetLeft(bool lh);
	void SetRight(bool rh);
	void Move(int bar_state);
	void SetRect(CPoint2 &pos);

	SDL_Rect GetRect()		{ return r_bar; }

	int	&ControlSK()		{ return ControlS; }	//콘트롤 변수값을 리턴
};


#endif
