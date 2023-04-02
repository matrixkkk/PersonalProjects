#ifndef CGAMEROOMSTATE_H
#define CGAMEROOMSTATE_H

#include "CGameUIState.h"
#include <string>

const int MAXUNIT				=	6;
const int MAX_PLAYER			=	14;
const int MAX_CHATLISTBOX_ITEM	=	15;
const int MAX_ID				=	20;    // ID�� �ִ� ����
const float DEFAULT_ALPHA		= 0.7f;


class CGameRoomState : public CGameUIState
{

	CEGUI::FrameWindow	*m_pChatBox;
	CEGUI::FrameWindow	*m_pPlayerBox;
	CEGUI::Titlebar		*m_pTitleBar;
	CEGUI::ButtonBase	*m_pReadyAndStartBtn;
	CEGUI::Combobox		*m_pJobComboBox;
	CEGUI::Editbox		*m_pChatEditBox;
	CEGUI::Listbox		*m_pChatListBox;
	CEGUI::Listbox		*m_pPlayerListBox;

	CEGUI::Window		*m_pPlayers[ MAX_PLAYER ];

	BYTE				m_nCntTotalPlayers;	//
	BYTE				m_nCntUSAPlayers;	//
	BYTE				m_nCntEUPlayers;	//	

	BYTE				m_nSelectedJopCode;	// ���� �ڵ�

	static bool			m_bCaptain;			// ���� ����
	CEGUI::String		m_strSelfID;		// �ڽ��� ���� ���̵�
	CEGUI::String		m_strIP;
	BYTE				m_nMyPosition;		// �濡�� ���� ��ġ��

	CRITICAL_SECTION	m_csPlayerStatic;	

	FMOD::Channel*		m_pBGChannel;		//�κ� ������� ä��

public:

	//	����
	enum{ MARIN, SNIPER, MEDIC, AMMO, ENGINEER, JOB_MAX };
	
	CGameRoomState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

	void		SetCaptain( bool bCaptain ){ m_bCaptain = bCaptain; }
	void		SetID( const CEGUI::String& strID );
	const char*	GetID();
	void		SetIP( const CEGUI::String& ip );
	void		SetStaticText( BYTE nPos, BYTE nJob, CEGUI::String& pszName );
	void		SetEmptyStaticText( BYTE nPos );
	void		SetMyPosition( BYTE nPos );	
	void		ChangePos( BYTE nOld, BYTE nNew );
	void		InsertPlayerList( CEGUI::String& pszName, BYTE nID );
	void		RemovePlayerList( const CEGUI::String& pszName, BYTE nID );
	void		AddChatText(const CEGUI::String& pText);
	void		SetStaticTextAlpha( BYTE nPos, BYTE nType  );
	bool		ChangeTeam( BYTE nTeam );

protected:
	
	virtual bool Initialize();

	// Overrides and concrete implementations of IBaseState methods.
	virtual void OnUpdate(float fCurrentTime);
	virtual void OnLeave(const char* szNext);

	// UI event handlers
	bool Event_HelpButtonClick(const CEGUI::EventArgs &rE);
	bool Event_OptionButtonClick(const CEGUI::EventArgs &rE);
	bool Event_ExitRoomButtonClick(const CEGUI::EventArgs &rE);

	bool Event_USAButtonClick(const CEGUI::EventArgs &rE);
	bool Event_EUButtonClick(const CEGUI::EventArgs &rE);
	bool Event_Guerrilla0ButtonClick(const CEGUI::EventArgs &rE);	
	bool Event_Guerrilla1ButtonClick(const CEGUI::EventArgs &rE);

	bool Event_ReadyAndStartButtonClick(const CEGUI::EventArgs &rE);

	bool Event_ChatTextAdded(const CEGUI::EventArgs &rE);
	bool Event_EditBoxActivate(const CEGUI::EventArgs &rE);
	bool Event_EditBoxDeactivate(const CEGUI::EventArgs &rE);

	bool Event_HandleSelectChange(const CEGUI::EventArgs &rE);

	bool Event_MapChange(const CEGUI::EventArgs &rE);
	
	
};

#endif 