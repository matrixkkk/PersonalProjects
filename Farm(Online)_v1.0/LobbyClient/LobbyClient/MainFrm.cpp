// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "LobbyClient.h"
//#include "LobbyClientView.h"
//#include "RoomListView.h"
//#include "UserListView.h"
//#include "ChatView.h"

#include "MainFrm.h"
#include "NetLobbyClient.h"
//#include "LobbyClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND_RANGE(ID_LOGIN_DISPLAY, ID_ROOM_DISPLAY , OnViewMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_LOGIN_DISPLAY, ID_ROOM_DISPLAY, OnUpdateViewMode)
	ON_COMMAND( ID_BUTTON_LOGIN_DLG_VIEW, OnLoginDlg )
	ON_COMMAND( ID_BUTTON_MEMBER_DLG_VIEW, OnMemberDlg )
	ON_MESSAGE( UM_UPDATE_DISPLAY, OnChangeDisplay)
	
//	ON_WM_GETDLGCODE()
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.

	m_nViewMode			= LOGIN_DISPLAY;
	
	m_pOldActiveView	= NULL;
	m_pNewActiveView	= NULL;

	m_pLoginDisplay		= NULL;
	m_pLobbyDisplay		= NULL;
	m_pRoomDisplay		= NULL;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	//ȯ������ 
	if (!m_wndLoginInterface.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || 
		!m_wndLoginInterface.LoadToolBar(IDR_LOGIN_INTERFACE_TOOLBAR) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
/*
	//ȯ������ 
	if (!m_wndEnvironment.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndEnvironment.LoadToolBar(IDR_ENVIRONMENT_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
*/	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	m_wndEnvironment.EnableDocking(CBRS_ALIGN_ANY);
	m_wndLoginInterface.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
	
	DockControlBar(&m_wndToolBar);
//	DockControlBar(&m_wndEnvironment);	
	DockControlBar(&m_wndLoginInterface);	

	m_wndToolBar.ShowWindow(FALSE);	

//	m_wndEnvironment.ShowWindow(TRUE);	

//	LoadBarState(_T("Bar State"));	//���� ��ġ ����

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.style &= ~FWS_ADDTOTITLE;	//���� ǥ�õǰ�
	

	return TRUE;
}


// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame �޽��� ó����
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{

	m_nViewMode			= LOGIN_DISPLAY;
	
	m_pOldActiveView	= NULL;
	m_pNewActiveView	= NULL;

	m_pLoginDisplay		= NULL;
	m_pLobbyDisplay		= NULL;
	m_pRoomDisplay		= NULL;

	m_pLobbyDisplay = new LobbyDisplayView;
	m_pLobbyDisplay->Create(NULL, NULL, WS_BORDER | WS_CHILD, CFrameWnd::rectDefault, this, LOBBY_DISPLAY, pContext);
	m_pLobbyDisplay->ShowWindow(SW_HIDE);
	m_pLobbyDisplay->SetDlgCtrlID(LOBBY_DISPLAY);

	m_pRoomDisplay	 = new RoomDisplayView;
	m_pRoomDisplay->Create(NULL, NULL, WS_BORDER | WS_CHILD, CFrameWnd::rectDefault, this, ROOM_DISPLAY, pContext);
	m_pRoomDisplay->ShowWindow(SW_HIDE);
	m_pRoomDisplay->SetDlgCtrlID(ROOM_DISPLAY);

	RecalcLayout();

	ChangeDisplay( m_nViewMode );

	return CFrameWnd::OnCreateClient(lpcs, pContext);

}

void CMainFrame::ChangeDisplay( UINT nID )
{
	if (m_nViewMode == nID)		return;

	if (!m_pOldActiveView)
		m_pOldActiveView = GetActiveView();
	else
		m_pOldActiveView = m_pNewActiveView;

	m_nViewMode = nID;

	switch (nID)
	{
	case	LOGIN_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pLoginDisplay );			
			m_wndLoginInterface.ShowWindow( TRUE );
			break;

	case	LOBBY_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pLobbyDisplay );			
			m_wndLoginInterface.ShowWindow( FALSE );
			break;

	case	ROOM_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pRoomDisplay );			
			m_wndLoginInterface.ShowWindow( FALSE );
			break;
	}

	//active view swap
	if ( NULL != m_pNewActiveView)
	{
		if (m_pOldActiveView == m_pNewActiveView) return;

		m_pNewActiveView->ShowWindow(SW_SHOW);
		m_pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);		

		if( NULL != m_pOldActiveView )
		{
			m_pOldActiveView->ShowWindow(SW_HIDE);
			m_pOldActiveView->SetDlgCtrlID(m_nViewMode);
		}

		RecalcLayout();
	}
	
}




void CMainFrame::OnUpdateViewMode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
//	pCmdUI->SetRadio(m_nViewMode == pCmdUI->m_nID);
	pCmdUI->SetCheck(m_nViewMode == pCmdUI->m_nID);
}

void CMainFrame::ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay )
{

}

void CMainFrame::OnViewMode(UINT nID) 
{
	//���� Ȱ��ȭ �Ǿ��ִ� View�� ���� View�̹Ƿ� m_pUMMIView�� �����Ѵ�.
// 	if (!m_pUMMIView)
// 		m_pUMMIView =(CUMMIView *) GetActiveView();
	
	if (m_nViewMode == nID)		return;

	if (!m_pOldActiveView)
		m_pOldActiveView = GetActiveView();
	else
		m_pOldActiveView = m_pNewActiveView;

	m_nViewMode = nID;

	ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsVisible(), FALSE);

//	ShowControlBar(&m_wndEnvironment, !m_wndEnvironment.IsVisible(), FALSE);


	switch (nID)
	{
	case	ID_LOGIN_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pLoginDisplay );
			m_wndLoginInterface.ShowWindow(TRUE);
			break;

	case	ID_LOBBY_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pLobbyDisplay );
			m_wndLoginInterface.ShowWindow(FALSE);
			break;

	case	ID_ROOM_DISPLAY:
			m_pNewActiveView = reinterpret_cast< CView* >( m_pRoomDisplay );
			m_wndLoginInterface.ShowWindow(FALSE);
			break;
	}

	//active view swap
	if ( NULL != m_pNewActiveView)
	{
		if (m_pOldActiveView == m_pNewActiveView) return;

		CRect rect;
		
		SetActiveView(m_pNewActiveView);
		m_pNewActiveView->ShowWindow(SW_SHOW);
		m_pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

		if( NULL != m_pOldActiveView )
		{
			m_pOldActiveView->ShowWindow(SW_HIDE);
			m_pOldActiveView->SetDlgCtrlID(m_nViewMode);
		}

		RecalcLayout();
	}
	
	GetActiveDocument()->UpdateAllViews(NULL);	


}

LRESULT CMainFrame::OnChangeDisplay( WPARAM wParam, LPARAM lParam )
{

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)GetActiveDocument();

	if( NULL == pDoc )
		return 0;
	
	static PlayerList		*pPlayerList		= reinterpret_cast< PlayerList* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 1 ) );
	static RoomList			*pRoomList			= reinterpret_cast< RoomList* >			( m_pLobbyDisplay->m_wndSplitRight1.GetPane( 0, 1 ) );
	static MenuInterface	*pMenuInterface		= reinterpret_cast< MenuInterface* >	( m_pLobbyDisplay->m_wndSplitRight1.GetPane( 0, 0 ) );
	static CChatInLobby		*pChatInLobby		= reinterpret_cast< CChatInLobby* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 0 ) );
	static LobbyInterface	*pLobbyInterface	= reinterpret_cast< LobbyInterface* >	( m_pLobbyDisplay->m_wndSplitUp.GetPane( 0, 0 ) );
		
	static JoinerList		*pJoinerList		= reinterpret_cast< JoinerList* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 1 ) );
	static CChatInRoom		*pChatInRoom		= reinterpret_cast< CChatInRoom* >		( m_pRoomDisplay->m_wndSplitUp.GetPane( 2, 0 ) );
	static RoomInterface	*pRoomInterface		= reinterpret_cast< RoomInterface* >	( m_pRoomDisplay->m_wndSplitUp.GetPane( 0, 0 ) );
	static MapInterface		*pMapInterface		= reinterpret_cast< MapInterface* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 0 ) );
	RoomInfo *pRoomInfo = NULL;


	switch( static_cast< int >( wParam ) )
	{

	//----------------------------------------------------------------------------------------------------------------------------------
	//
	//								�κ� �������̽�
	//
	//----------------------------------------------------------------------------------------------------------------------------------

	
	case LOBBY_PLAYER_ADD_MORE:
		// �÷��̾� ����Ʈ N�� �߰� ����		
		
		if( NULL != pPlayerList )
			pDoc->m_MyLobbyClient.UpdatePlayerListInLobby( pPlayerList );

		break;

	case LOBBY_PLAYER_ADD_ONE:
		// �÷��̾� ����Ʈ N�� �߰� ����			

		if( NULL != pPlayerList )
			pDoc->m_MyLobbyClient.UpdateOnePlayerListInLobby( static_cast< BYTE >( lParam ), pPlayerList );

		break;		

	case LOBBY_PLAYER_DELETE_ONE:
		// �÷��̾� ����Ʈ 1�� ���� ����

		if( NULL != pPlayerList )
			pPlayerList->GetListCtrl().DeleteItem( static_cast< BYTE >( lParam ) );	
//			pDoc->m_MyLobbyClient.DeletePlayerInLobby( static_cast< BYTE >( lParam ) , pPlayerList );

		break;

	case LOBBY_PLAYER_DELETE_ALL:
		// �÷��̾� ����Ʈ�� ��� �׸� ����

		if( NULL != pPlayerList )
			pPlayerList->GetListCtrl().DeleteAllItems();

		if( NULL != pChatInLobby )
			pChatInLobby->ResetChat();

		break;

	case LOBBY_ROOM_ADD_MORE :
		// ���� �� ����Ʈ �߰� ����

		if( NULL != pRoomList )
			pDoc->m_MyLobbyClient.UpdateRoomListInLobby( pRoomList );

		break;

	case LOBBY_ROOM_ADD_ONE:
		// �κ� �� 1�� �߰� ����

		if( NULL != pRoomList )
			pDoc->m_MyLobbyClient.UpdateOneRoomListInLobby( static_cast< BYTE >( lParam ), pRoomList );

		break;

	case LOBBY_ROOM_DELETE_ONE:
		// �κ� �� 1�� ���� ����

		if( NULL != pRoomList )
			pRoomList->GetListCtrl().DeleteItem( static_cast< BYTE >( lParam ) );

		break;

	case LOBBY_ROOM_DELETE_ALL:
		// �κ� �� ����Ʈ ���� ����

		if( NULL != pRoomList )
			pRoomList->GetListCtrl().DeleteAllItems();

		break;

	case LOBBY_ROOM_UPDATE_ONE:
		// �κ� �� ����Ʈ�� 1�� �� ���� ����

		if( NULL != pRoomList )
			pDoc->m_MyLobbyClient.UpdateRoomStateInLobby( reinterpret_cast< RoomInfo* >( lParam ), pRoomList );			

			break;

	case LOBBY_CHAT_UPDATE:
		// �κ� ä�� ����

		if( NULL != pChatInLobby )
			pChatInLobby->UpdateChatEdit();

		break;	
	
	case LOBBY_MYSELF:
		// �ڽ��� ���̵� ���

		if( NULL != pRoomInterface )
			pLobbyInterface->UpdateMyGameID( pDoc->m_MyLobbyClient.GetMyGameID() );

		break;

	case LOBBY_ROOM_INVITATION:
		// �ʴ� �޽��� ���̾�α� ���

		if( NULL != pDoc->m_pDlgInvitation )
		{
			if( pDoc->m_pDlgInvitation->IsWindowVisible() )
				pDoc->m_pDlgInvitation->ShowWindow( SW_HIDE );
			else
			{
				pRoomInfo = reinterpret_cast< RoomInfo* >( lParam );
				pDoc->m_pDlgInvitation->SetRoomInfo( pRoomInfo->m_wstrRoomName, pRoomInfo->m_nListNo );	
				pDoc->m_pDlgInvitation->ShowWindow( SW_SHOW );
			}
		}

		break;


	//----------------------------------------------------------------------------------------------------------------------------------
	//
	//								�� �������̽�
	//
	//----------------------------------------------------------------------------------------------------------------------------------

	case ROOM_CHAT_UPDATE :
		// �濡�� ä��

		if( NULL != pChatInRoom )
			pChatInRoom->UpdateChatEdit();

		break;

	case ROOM_PLAYER_ADD_ONE :

		// �� �����ο� 1�� �߰� �Ǵ� 1���� ���� ���� ����

		if( NULL != pJoinerList )
			pDoc->m_MyLobbyClient.UpdateOneJoinerListInRoom( static_cast< BYTE >( lParam ), pJoinerList );

		break;

	case ROOM_PLAYER_ADD_MORE :
		// �� �����ο� N�� �߰�

		if( NULL != pJoinerList )
			pDoc->m_MyLobbyClient.UpdateJoinerListInRoom( pJoinerList );

		break;

	case ROOM_PLAYER_DELETE_ONE :
		// �濡 ���� 1���� ����Ʈ���� ���� ����		

		if( NULL != pJoinerList )
			pDoc->m_MyLobbyClient.UpdateDeleteJoinerInRoom( static_cast< BYTE >( lParam ), pJoinerList );

		break;

	case ROOM_PLAYER_DELETE_ALL :
		// �� �����ο� N�� ����

		if( NULL != pJoinerList )
			pJoinerList->GetListCtrl().DeleteAllItems();

		if( NULL != pChatInRoom )
			pChatInRoom->ResetChat();

		break;

	case ROOM_NAME:
		// �� �̸� ���

		if( NULL != pRoomInterface )
			pRoomInterface->UpdateMyGameRoomName( pDoc->m_MyLobbyClient.GetConnRoomName() );

		break;

	case ROOM_MYSELF:
		// �濡�� ID ���

		if( NULL != pRoomInterface )
			pRoomInterface->UpdateMyGameID( pDoc->m_MyLobbyClient.GetMyGameID() );

		break;

	case ROOM_UPDATE_BUTTON:
		// ���� ����, �غ� ��ư ����

		if( NULL != pRoomInterface )
			pRoomInterface->SetBtnName( pDoc->m_MyLobbyClient.IsCaptain(), static_cast< BOOL >( lParam ) );

		break;

	case ROOM_PLAYER_UPDATE_ONE:
		// �� �����ο� 1���� ���� ����

		if( NULL != pJoinerList )
			pDoc->m_MyLobbyClient.UpdateJoinerListState( static_cast< BYTE >( lParam ), pJoinerList );

		break;

	case ROOM_MAPINFO_CHANGE:
		// �濡�� �� ���� ���� ����

		if( NULL != pMapInterface )
			pMapInterface->UpdateMapInfo( static_cast< BYTE >( lParam ) );

		break;

	case ROOM_IS_CAPTAIN:
		// �濡�� �������� ��������
		if( NULL != pMapInterface )
			pMapInterface->SetRoomCaptain( static_cast< BOOL >( lParam ) );

		break;

	case ROOM_UPDATE_ALL_STATE:
		// �� �ȿ� ��� ������ ���� ���� ����
		if( NULL != pJoinerList )
			pDoc->m_MyLobbyClient.UpdateAllJoinerList( pJoinerList );

		break;

	case ROOM_JOINER_INFO:
		// ��� 1�� ���� ���� ���

		if( NULL != pDoc->m_pDlgJoinerInfo )
		{
			if( pDoc->m_pDlgJoinerInfo->IsWindowVisible() )
				pDoc->m_pDlgJoinerInfo->ShowWindow( SW_HIDE );
			else
			{
				char *pInfo = reinterpret_cast< char* >( lParam );
				if( pDoc->m_pDlgJoinerInfo->UpdateJoinerInfo( pInfo ) )
				{
					pDoc->m_pDlgJoinerInfo->ShowWindow( SW_SHOW );
					delete[] pInfo;
				}
			}
		}

		break;

	case ROOM_GAME_START:

		if( NULL != pDoc->m_pThread )
		{
			pDoc->m_pThread->SuspendThread();

			_spawnl( _P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(), NULL  );

			pDoc->m_pThread->ResumeThread();
		}

		break;
	}

	return 0;
}


void CMainFrame::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

//	SetActiveView(m_pMainView);

	SaveBarState(_T("Bar State"));	//���� ��ġ����

	OnViewMode(ID_LOGIN_DISPLAY);	
	
	PlayerList		*pPlayerList		= reinterpret_cast< PlayerList* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 1 )	);
	MenuInterface	*pMenuInterface		= reinterpret_cast< MenuInterface* >	( m_pLobbyDisplay->m_wndSplitRight1.GetPane( 0, 0 ) );
	RoomList		*pRoomList			= reinterpret_cast< RoomList* >			( m_pLobbyDisplay->m_wndSplitRight1.GetPane( 0, 1 )	);	
	CChatInLobby	*pChatInLobby		= reinterpret_cast< CChatInLobby* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 0 )	);
	LobbyInterface	*pLobbyInterface	= reinterpret_cast< LobbyInterface* >	( m_pLobbyDisplay->m_wndSplitUp.GetPane( 0, 0 )		);
		
	JoinerList		*pJoinerList		= reinterpret_cast< JoinerList* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 1 )	);
	CChatInRoom		*pChatInRoom		= reinterpret_cast< CChatInRoom* >		( m_pRoomDisplay->m_wndSplitUp.GetPane( 2, 0 )		);
	RoomInterface	*pRoomInterface		= reinterpret_cast< RoomInterface* >	( m_pRoomDisplay->m_wndSplitUp.GetPane( 0, 0 )		);
	MapInterface	*pMapInterface		= reinterpret_cast< MapInterface* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 0 )	);

	if( NULL != pPlayerList )
	{
		delete pPlayerList;
		pPlayerList = NULL;
	}

	if( NULL != pRoomList )
	{
		delete pRoomList;
		pRoomList = NULL;
	}

	if( NULL != pChatInLobby )
	{
		delete pChatInLobby;
		pChatInLobby = NULL;
	}

	if( NULL != pLobbyInterface )
	{
		delete pLobbyInterface;
		pLobbyInterface = NULL;
	}

	if( NULL != pJoinerList )
	{
		delete pJoinerList;
		pJoinerList = NULL;
	}

	if( NULL != pChatInRoom )
	{
		delete pChatInRoom;
		pChatInRoom = NULL;
	}

	if( NULL != pRoomInterface )
	{
		delete pRoomInterface;
		pRoomInterface = NULL;
	}

	if( NULL != pMapInterface )
	{
		delete pMapInterface;
		pMapInterface = NULL;
	}


	if( NULL != m_pLobbyDisplay )
	{
		m_pLobbyDisplay->DestroyWindow();
//		delete m_pLobbyDisplay;
//		m_pLobbyDisplay = NULL;
	}

	if( NULL != m_pRoomDisplay )
	{
		m_pRoomDisplay->DestroyWindow();
//		delete m_pRoomDisplay;
//		m_pRoomDisplay = NULL;
	}

	CFrameWnd::OnClose();
}

//UINT CMainFrame::OnGetDlgCode()
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//
//	return CFrameWnd::OnGetDlgCode();
//}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	LPMINMAXINFO lpminmax = (LPMINMAXINFO)lpMMI;
	lpminmax->ptMinTrackSize.x = 950;
	lpminmax->ptMinTrackSize.y = 900;

	lpminmax->ptMaxTrackSize.x = 950;
	lpminmax->ptMaxTrackSize.y = 900;

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


void CMainFrame::OnLoginDlg()
{


}

void CMainFrame::OnMemberDlg()
{


}
BOOL CMainFrame::DestroyWindow()
{

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
/*
	OnViewMode(ID_LOGIN_DISPLAY);	

	if( NULL != m_pLobbyDisplay )
	{	
		PlayerList		*pPlayerList		= reinterpret_cast< PlayerList* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 1 ) );
		RoomList		*pRoomList			= reinterpret_cast< RoomList* >			( m_pLobbyDisplay->m_wndSplitUp.GetPane( 1, 0 ) );
		CChatInLobby	*pChatInLobby		= reinterpret_cast< CChatInLobby* >		( m_pLobbyDisplay->m_wndSplitRight.GetPane( 0, 0 ) );
		LobbyInterface	*pLobbyInterface	= reinterpret_cast< LobbyInterface* >	( m_pLobbyDisplay->m_wndSplitUp.GetPane( 0, 0 ) );

		if( NULL != pPlayerList )
		{
			delete pPlayerList;
			pPlayerList = NULL;
		}

		if( NULL != pRoomList )
		{
			delete pRoomList;
			pRoomList = NULL;
		}

		if( NULL != pChatInLobby )
		{
			delete pChatInLobby;
			pChatInLobby = NULL;
		}

		if( NULL != pLobbyInterface )
		{
			delete pLobbyInterface;
			pLobbyInterface = NULL;
		}
	}

	if( NULL != m_pRoomDisplay )
	{		
		JoinerList		*pJoinerList		= reinterpret_cast< JoinerList* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 1 ) );
		CChatInRoom		*pChatInRoom		= reinterpret_cast< CChatInRoom* >		( m_pRoomDisplay->m_wndSplitUp.GetPane( 2, 0 ) );
		RoomInterface	*pRoomInterface		= reinterpret_cast< RoomInterface* >	( m_pRoomDisplay->m_wndSplitUp.GetPane( 0, 0 ) );
		MapInterface	*pMapInterface		= reinterpret_cast< MapInterface* >		( m_pRoomDisplay->m_wndSplitRight.GetPane( 0, 0 ) );

		if( NULL != pJoinerList )
		{
			delete pJoinerList;
			pJoinerList = NULL;
		}

		if( NULL != pChatInRoom )
		{
			delete pChatInRoom;
			pChatInRoom = NULL;
		}

		if( NULL != pRoomInterface )
		{
			delete pRoomInterface;
			pRoomInterface = NULL;
		}

		if( NULL != pMapInterface )
		{
			delete pMapInterface;
			pMapInterface = NULL;
		}
	}
		

	if( NULL != m_pLobbyDisplay )
	{
		m_pLobbyDisplay->DestroyWindow();
		delete m_pLobbyDisplay;
		m_pLobbyDisplay = NULL;
	}

	if( NULL != m_pRoomDisplay )
	{
		m_pRoomDisplay->DestroyWindow();
		delete m_pRoomDisplay;
		m_pRoomDisplay = NULL;
	}

	*/

	return CFrameWnd::DestroyWindow();
}
