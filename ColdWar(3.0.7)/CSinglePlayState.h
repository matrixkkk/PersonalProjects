/*
싱글 테스트용 크래스
네크워크 없이 오로지 

*/
#ifndef CSINGLEPLAYSTATE_H
#define CSINGLEPLAYSTATE_H

#include "GameApp.h"
#include "CGameUIState.h"
#include "WorldManager.h"

//#include "ObjectPick.h"


class CSinglePlayState : public CGameUIState
{
	enum{ ID, KILL, DEATH };
	enum{ USA,EU, GUERRILLA };

	CEGUI::FrameWindow		*m_pMenuBox;

	FMOD::Channel*			m_pBGChannel;		//배경 사운드 채널

public:
	
	CSinglePlayState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);
	~CSinglePlayState();

	//================================================================
	//					virtual
	//================================================================
	void OnUpdate(float fCurrentTime);
	void OnLeave(const char* szNext);
	void OnOverride(const char* szNext);
	void OnResume(const char* szPrevious);
	void OnProcessInput();
	void OnRender();


protected:

	// 게임 초기화
	virtual bool Initialize();
	//	UI 생성
	bool CreateUI();
	bool LoadAllEffectSound();	//모든 이펙트 사운드 로드

	void ProcessKeyBoard();		//키보드 처리 함수
	void ProcessMouse();		//마우스 처리 함수
	void ProcessKeyBoardForUI();

	//================================================================
	//					Event 처리
	//================================================================


	bool Event_MenuBoxExitBtnClick	(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxVoteBtnClick	(const CEGUI::EventArgs &rE);
	//================================================================
	//================================================================
	
	unsigned int idx;

};

#endif
