/*
Ŭ���� �� : CPlayState
�ۼ���	  : ������
����	  : ���� ������ �÷��� �ϴ� �κ�

*/
#ifndef CGAMEPLAYSTATE_H
#define CGAMEPLAYSTATE_H

#include "GameApp.h"
#include "CGameUIState.h"
#include "NetWorkManager.h"
#include "WorldManager.h"

//#include "ObjectPick.h"

const int	MAX_CHATLISTBOX_ITEM_IN_GAME= 7;
const float TERRITORY_DISPLAY_TIME		= 4.0f;
const float WIN_DISPLAY_TIME			= 4.0f;


class CGamePlayState : public CGameUIState
{
	enum{ ID, KILL, DEATH };
	enum{ USA,EU, GUERRILLA };

	CEGUI::FrameWindow		*m_pMenuBox;				// �޴� �ڽ�
	CEGUI::Editbox			*m_pChatEditBox;			// ä�� ����Ʈ �ڽ�
	CEGUI::Window			*m_pStaticInput;			// ä�� �Է� ǥ��
	CEGUI::Listbox			*m_pChatListBox;			// ä�� ����Ʈ �ڽ�
	CEGUI::Window			*m_pStaticRespone;			// ������ ���϶�
	CEGUI::Window			*m_pStaticTerritory;		// ������ ���� ������
	CEGUI::Window			*m_pStaticWin;				// �¸� ������ ǥ��
	CEGUI::ProgressBar		*m_pProgressbarRespone;		// ������ ���α׷���
	CEGUI::ProgressBar		*m_pProgressbarRebirth;		// ��Ȱ ���α׷���

	CEGUI::MultiColumnList	*m_pListMcl[3];				// ���� ǥ��
	CEGUI::Window			*m_pLogoImg[3];				// �� ������ �ΰ� �̹��� ǥ��

	CRITICAL_SECTION		m_csMcl;	
	CRITICAL_SECTION		m_csChatListBox;

	bool					m_bActivaChaEdit;

	float					m_fTimeTerritory;
	float					m_fTimeWinGame;
	
	FMOD::Channel*			m_pBGChannel;		//��� ���� ä��
public:
	
	CGamePlayState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);
//	CGamePlayState( const char *pcGUILayoutFilename = 0);
//	CGamePlayState();
	~CGamePlayState();

	//================================================================
	//					virtual
	//================================================================
	void OnUpdate(float fCurrentTime);
	void OnLeave(const char* szNext);
	void OnOverride(const char* szNext);
	void OnResume(const char* szPrevious);
	void OnProcessInput();
	void OnRender();

	//================================================================
	//					Update
	//================================================================
	void AddChatText(const CEGUI::String& pText);
	void UpdateKill( BYTE nID, BYTE nTeam, BYTE nKill );
	void UpdateDeath( BYTE nID, BYTE nTeam, BYTE nKill );
	void UpdateResponeProgress( float fTime );				//������
	void UpdateRebirthProgress( float fTime );				//��Ȱ
	void SetResponning( bool bRespone );					//������ ���α׷����� Ȱ�� ����
	void SetRebirthing( bool bRebirth );					//��Ȱ ���α׷����� Ȱ�� ����
	void ActiveTerritory( CEGUI::String& pszName, BYTE nArea );
	void WinGame( BYTE nTeam, bool = true );

	bool IsChatMode(){ return m_bActivaChaEdit; }

protected:

	// ���� �ʱ�ȭ
	virtual bool Initialize();
	//	UI ����
	bool CreateUI();
	bool LoadAllEffectSound();	//��� ����Ʈ ���� �ε�

	void ProcessKeyBoard();		//Ű���� ó�� �Լ�
	void ProcessMouse();		//���콺 ó�� �Լ�
	void ProcessKeyBoardForUI();

	//================================================================
	//					Event ó��
	//================================================================

	bool Event_ChatTextAdded		(const CEGUI::EventArgs &rE);

	bool Event_MenuBoxExitBtnClick	(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE);
	bool Event_MenuBoxVoteBtnClick	(const CEGUI::EventArgs &rE);
	bool Event_EditBoxActivate		(const CEGUI::EventArgs &rE);
	bool Event_EditBoxDeactivate	(const CEGUI::EventArgs &rE);
	//================================================================
	//================================================================
	
	unsigned int idx;

};

#endif
