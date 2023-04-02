#ifndef COPTIONSTATE_H
#define COPTIONSTATE_H

#include "CGameUIState.h"


//const int MAX_CHATLISTBOX_ITEM_IN_GAME = 7;
// 옵션 화면 상태

class COptionState : public CGameUIState
{
/*
	enum{ ID, KILL, DEATH };
	enum{ USA, EU, GUERRILLA };

	CEGUI::FrameWindow		*m_pMenuBox;
	CEGUI::Editbox			*m_pChatEditBox;
	CEGUI::Window			*m_pStaticInput;
	CEGUI::Listbox			*m_pChatListBox;

	CEGUI::MultiColumnList	*m_pListMcl[3];
	CEGUI::Window			*m_pLogoImg[3];

	CRITICAL_SECTION		m_csMcl;	

	bool					m_bActivaChaEdit;
*/
public:
	COptionState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

protected:

	virtual bool Initialize();

	virtual void OnUpdate(float fCurrentTime);
	virtual void OnLeave(const char* szNext);
/*
	bool Event_ChatTextAdded		(const CEGUI::EventArgs &rE);

	bool Event_MenuBoxExitBtnClick	(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxVoteBtnClick	(const CEGUI::EventArgs &rE);

	void UpdateKill( BYTE nID, BYTE nTeam, BYTE nKill );
	void UpdateDeath( BYTE nID, BYTE nTeam, BYTE nKill );

	void AddChatText(const CEGUI::String& pText);
*/
};


#endif