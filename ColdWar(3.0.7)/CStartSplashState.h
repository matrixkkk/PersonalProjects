#ifndef _startupsplash_h
#define _startupsplash_h

#include "CGameUIState.h"


// 게임 시작하고 로고 등의 이미지를 보여주는 상태

class CStartSplashState : public CGameUIState
{
public:
	CStartSplashState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

protected:

	virtual void OnUpdate(float fCurrentTime);
};


#endif // _startupsplash_h