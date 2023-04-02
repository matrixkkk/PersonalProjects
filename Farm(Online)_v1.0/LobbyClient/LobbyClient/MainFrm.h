// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

#include "LoginDisplayView.h"
#include "LobbyDisplayView.h"
#include "RoomDisplayView.h"

//-----------------------------------
//
//		MFC ����� ���� Message
//
#define		UM_UPDATE_DISPLAY	(WM_USER + 11)
//-----------------------------------

enum{

	LOGIN_DISPLAY,
	LOBBY_DISPLAY,
	ROOM_DISPLAY
};


enum{
		
	//	�κ� �������̽�
	LOBBY_MYSELF,				// �κ񿡼� ID ���
	LOBBY_CHAT_UPDATE,			// �κ� ä��

	LOBBY_PLAYER_ADD_ONE,		// �÷��̾� 1�� �߰�
	LOBBY_PLAYER_ADD_MORE,		// �÷��̾� N�� �߰�
	LOBBY_PLAYER_DELETE_ONE,	// �÷��̾� 1�� ����
	LOBBY_PLAYER_DELETE_ALL,	// �÷��̾� N�� ����
	
	LOBBY_ROOM_ADD_ONE,			// �� 1�� �߰�
	LOBBY_ROOM_ADD_MORE,		// �� N�� �߰�
	LOBBY_ROOM_DELETE_ONE,		// �� 1�� ����
	LOBBY_ROOM_DELETE_ALL,		// �� N�� ����

	LOBBY_ROOM_UPDATE_ONE,		// �� ���� ���� ����
	LOBBY_ROOM_INVITATION,		// �κ񿡼� �ʴ� �޽��� 


	// �� ���� ���̽�
	ROOM_CHAT_UPDATE,			// �濡�� ä��

	ROOM_NAME,					// �� �̸� ���
	ROOM_MYSELF,				// �濡�� ID ���
	ROOM_UPDATE_BUTTON,			// ���� ����, �غ� ��ư ����

	ROOM_PLAYER_ADD_ONE,		// �� �����ο� 1�� �߰�
	ROOM_PLAYER_ADD_MORE,		// �� �����ο� N�� �߰�
	ROOM_PLAYER_DELETE_ONE,		// �� �����ο� 1�� ����
	ROOM_PLAYER_DELETE_ALL,		// �� �����ο� N�� ����

	ROOM_PLAYER_UPDATE_ONE,		// �� �����ο� 1���� ���� ����

	ROOM_MAPINFO_CHANGE,		// �濡�� �� ���� ���� ����
	ROOM_IS_CAPTAIN,			// �濡�� �������� ��������
	ROOM_UPDATE_ALL_STATE,		// ��ȿ� �ִ� ���� ���� ���� ����

	ROOM_JOINER_INFO,			// ��� 1�� ���� ���� ���

	ROOM_GAME_START



};


class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:

	UINT				m_nViewMode;
	
	CView				*m_pOldActiveView;
	CView				*m_pNewActiveView;

	LoginDisplayView	*m_pLoginDisplay;
	LobbyDisplayView	*m_pLobbyDisplay;
	RoomDisplayView		*m_pRoomDisplay;

// �۾��Դϴ�.
public:

	void ChangeDisplay( UINT nID );

	void ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay );
	

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar	m_wndLoginInterface;
//	CToolBar    m_wndEnvironment;

// ������ �޽��� �� �Լ�
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
	// ������ �������� ������ Pane View �����͸� ��´�.
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


