#ifndef CMENUSTATE_H
#define CMENUSTATE_H

#include "CGameUIState.h"


class CMenuState : public CGameUIState
{

	CEGUI::FrameWindow *m_pNewGameBox;
	CEGUI::FrameWindow *m_pExitGameBox;
	CEGUI::FrameWindow *m_pConnectGameBox;
	CEGUI::FrameWindow *m_pCreateGameBox;

	float m_fStartTime;
	float m_fLastTime;
	CEGUI::Window*	m_pBackImg;

	//메뉴 배경 사운드 채널
	FMOD::Channel*	   m_pBGChannel;

public:
	
	CMenuState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

protected:
	
	virtual bool Initialize();

	// Overrides and concrete implementations of IBaseState methods.
	virtual void OnUpdate(float fCurrentTime);
	virtual void OnLeave(const char* szNext);

	bool ConnectGame( bool bType, const CEGUI::String& strID);
	bool ConnectGame( bool bType, const CEGUI::String& strID, const CEGUI::String& strIP);

	// UI event handlers
	bool Event_NewGameButtonClick(const CEGUI::EventArgs &rE);
	bool Event_OptionButtonClick(const CEGUI::EventArgs &rE);
	bool Event_CreditButtonClick(const CEGUI::EventArgs &rE);
	bool Event_ExitGameButtonClick(const CEGUI::EventArgs &rE);

	bool Event_CreateGameButtonClick(const CEGUI::EventArgs &rE);
	bool Event_JoinGameButtonClick(const CEGUI::EventArgs &rE);

	bool Event_CreateGameBoxOkButtonClick(const CEGUI::EventArgs &rE);
	bool Event_CreateGameBoxCancelButtonClick(const CEGUI::EventArgs &rE);
	
	bool Event_CaptainIDTextAdded(const CEGUI::EventArgs &rE);
	
		
	bool Event_ConnectOkButtonClick(const CEGUI::EventArgs &rE);
	bool Event_ConnectCancelButtonClick(const CEGUI::EventArgs &rE);

	bool Event_ExitOkButtonClick(const CEGUI::EventArgs &rE);
	bool Event_ExitCancelButtonClick(const CEGUI::EventArgs &rE);

	bool Event_IPTextAdded(const CEGUI::EventArgs &rE);
	bool Event_IDTextAdded(const CEGUI::EventArgs &rE);

	bool Event_ResetCaret(const CEGUI::EventArgs &rE);
	bool Event_IPEditBoxActive(const CEGUI::EventArgs &rE);
	bool Event_IPEditBoxDeactive(const CEGUI::EventArgs &rE);
	bool Event_IDEditBoxActive(const CEGUI::EventArgs &rE);
	bool Event_IDEditBoxDeactive(const CEGUI::EventArgs &rE);
	bool Event_CreateGameEditBoxActive(const CEGUI::EventArgs &rE);
	bool Event_CreateGameEditBoxDeactive(const CEGUI::EventArgs &rE);
	
	void AddChatText(const CEGUI::String& pText);

};

#endif 