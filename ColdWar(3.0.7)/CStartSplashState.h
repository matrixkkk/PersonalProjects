#ifndef _startupsplash_h
#define _startupsplash_h

#include "CGameUIState.h"


// ���� �����ϰ� �ΰ� ���� �̹����� �����ִ� ����

class CStartSplashState : public CGameUIState
{
public:
	CStartSplashState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

protected:

	virtual void OnUpdate(float fCurrentTime);
};


#endif // _startupsplash_h