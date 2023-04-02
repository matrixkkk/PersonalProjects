/*
클래스 명 : CPlayState
작성자	  : 김현구
설명	  : 실제 게임을 플레이 하는 부분

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

	CEGUI::FrameWindow		*m_pMenuBox;				// 메뉴 박스
	CEGUI::Editbox			*m_pChatEditBox;			// 채팅 에디트 박스
	CEGUI::Window			*m_pStaticInput;			// 채팅 입력 표시
	CEGUI::Listbox			*m_pChatListBox;			// 채팅 리스트 박스
	CEGUI::Window			*m_pStaticRespone;			// 리스폰 중일때
	CEGUI::Window			*m_pStaticTerritory;		// 점령지 차지 했을때
	CEGUI::Window			*m_pStaticWin;				// 승리 했을때 표시
	CEGUI::ProgressBar		*m_pProgressbarRespone;		// 리스폰 프로그래스
	CEGUI::ProgressBar		*m_pProgressbarRebirth;		// 부활 프로그래스

	CEGUI::MultiColumnList	*m_pListMcl[3];				// 전적 표시
	CEGUI::Window			*m_pLogoImg[3];				// 각 종족별 로고 이미지 표시

	CRITICAL_SECTION		m_csMcl;	
	CRITICAL_SECTION		m_csChatListBox;

	bool					m_bActivaChaEdit;

	float					m_fTimeTerritory;
	float					m_fTimeWinGame;
	
	FMOD::Channel*			m_pBGChannel;		//배경 사운드 채널
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
	void UpdateResponeProgress( float fTime );				//리스폰
	void UpdateRebirthProgress( float fTime );				//부활
	void SetResponning( bool bRespone );					//리스폰 프로그래스바 활성 제어
	void SetRebirthing( bool bRebirth );					//부활 프로그래스바 활성 제어
	void ActiveTerritory( CEGUI::String& pszName, BYTE nArea );
	void WinGame( BYTE nTeam, bool = true );

	bool IsChatMode(){ return m_bActivaChaEdit; }

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
