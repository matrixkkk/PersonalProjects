#ifndef CCREDITSTATE_H
#define CCREDITSTATE_H

#include "CGameUIState.h"


// ũ������ �����ִ� ȭ�� ����

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