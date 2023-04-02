// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "LoginDisplayView.h"
#include "LobbyDisplayView.h"
#include "RoomDisplayView.h"

//-----------------------------------
//
//		MFC 사용자 정의 Message
//
#define		UM_UPDATE_DISPLAY	(WM_USER + 11)
//-----------------------------------

enum{

	LOGIN_DISPLAY,
	LOBBY_DISPLAY,
	ROOM_DISPLAY
};


enum{
		
	//	로비 인터페이스
	LOBBY_MYSELF,				// 로비에서 ID 출력
	LOBBY_CHAT_UPDATE,			// 로비 채팅

	LOBBY_PLAYER_ADD_ONE,		// 플레이어 1명 추가
	LOBBY_PLAYER_ADD_MORE,		// 플레이어 N명 추가
	LOBBY_PLAYER_DELETE_ONE,	// 플레이어 1명 삭제
	LOBBY_PLAYER_DELETE_ALL,	// 플레이어 N명 삭제
	
	LOBBY_ROOM_ADD_ONE,			// 룸 1개 추가
	LOBBY_ROOM_ADD_MORE,		// 룸 N개 추가
	LOBBY_ROOM_DELETE_ONE,		// 룸 1개 삭제
	LOBBY_ROOM_DELETE_ALL,		// 룸 N개 삭제

	LOBBY_ROOM_UPDATE_ONE,		// 룸 상태 정보 갱신
	LOBBY_ROOM_INVITATION,		// 로비에서 초대 메시지 


	// 룸 인터 페이스
	ROOM_CHAT_UPDATE,			// 방에서 채팅

	ROOM_NAME,					// 방 이름 출력
	ROOM_MYSELF,				// 방에서 ID 출력
	ROOM_UPDATE_BUTTON,			// 게임 시작, 준비 버튼 갱신

	ROOM_PLAYER_ADD_ONE,		// 방 참여인원 1명 추가
	ROOM_PLAYER_ADD_MORE,		// 방 참여인원 N명 추가
	ROOM_PLAYER_DELETE_ONE,		// 방 참여인원 1명 삭제
	ROOM_PLAYER_DELETE_ALL,		// 방 참여인원 N명 삭제

	ROOM_PLAYER_UPDATE_ONE,		// 방 참여인원 1명의 상태 갱신

	ROOM_MAPINFO_CHANGE,		// 방에서 맵 정보 변경 갱신
	ROOM_IS_CAPTAIN,			// 방에서 유저인지 방장인지
	ROOM_UPDATE_ALL_STATE,		// 방안에 있는 유저 상태 정보 갱신

	ROOM_JOINER_INFO,			// 방안 1명 유저 정보 출력

	ROOM_GAME_START



};


class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

	UINT				m_nViewMode;
	
	CView				*m_pOldActiveView;
	CView				*m_pNewActiveView;

	LoginDisplayView	*m_pLoginDisplay;
	LobbyDisplayView	*m_pLobbyDisplay;
	RoomDisplayView		*m_pRoomDisplay;

// 작업입니다.
public:

	void ChangeDisplay( UINT nID );

	void ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay );
	

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar	m_wndLoginInterface;
//	CToolBar    m_wndEnvironment;

// 생성된 메시지 맵 함수
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewMode(CCmdUI* pCmdUI);
	afx_msg void OnViewMode(UINT nID);
	afx_msg void OnLoginDlg();
	afx_msg void OnMemberDlg();
	afx_msg LRESULT OnChangeDisplay( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
	


public:
	
	/*
	// 분할한 윈도우의 가각의 Pane View 포인터를 얻는다.
	CLobbyClientView*	GetRoomListPane(void);	
	CLobbyClientView*	GetUserListPane(void);
	CLobbyClientView*	GetChatPane(void);
	*/
public:
	afx_msg void OnClose();
public:
//	afx_msg UINT OnGetDlgCode();
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
public:
	virtual BOOL DestroyWindow();
};


