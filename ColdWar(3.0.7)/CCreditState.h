#ifndef CCREDITSTATE_H
#define CCREDITSTATE_H

#include "CGameUIState.h"


// 크레딧을 보여주는 화면 상태

class CCreditState : public CGameUIState
{
	float m_fStartTime;
	float m_fLastTime;
	CEGUI::Window*	m_pBackImg;

public:
	CCreditState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);
	
	virtual bool Initialize();

protected:

	virtual void OnUpdate(float fCurrentTime);
};


#endif