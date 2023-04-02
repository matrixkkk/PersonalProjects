// LobbyInterface.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyInterface.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// LobbyInterface

IMPLEMENT_DYNCREATE(LobbyInterface, CFormView)

LobbyInterface::LobbyInterface()
	: CFormView(LobbyInterface::IDD)
{

}

LobbyInterface::~LobbyInterface()
{
}

void LobbyInterface::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_NAME, m_ctlMySelf);
}

BEGIN_MESSAGE_MAP(LobbyInterface, CFormView)
	ON_BN_CLICKED(IDC_LEAVE_LOBBY, &LobbyInterface::OnBnClickedLeaveLobby)
	ON_BN_CLICKED(IDC_CREATE_ROOM, &LobbyInterface::OnBnClickedCreateRoom)
	ON_BN_CLICKED(IDC_ENTER_ROOM, &LobbyInterface::OnBnClickedEnterRoom)
END_MESSAGE_MAP()


// LobbyInterface �����Դϴ�.

#ifdef _DEBUG
void LobbyInterface::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LobbyInterface::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// LobbyInterface �޽��� ó�����Դϴ�.

void LobbyInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	// ��Ʈ ����
//	m_ctlMySelf.Create( _T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect( 5,5, 150, 80), this );
	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"����ü"); 

	m_ctlMySelf.SetFont(&m_Font, TRUE);	
	
}

// �κ񿡼� �α��� ȭ������ ��ȯ�� ���� ���� ���̵� ����ϱ� ���� ����
void LobbyInterface::UpdateMyGameID( CString &strMyGameID )
{
	CString strMySelf( strMyGameID );

	strMySelf += _T(" �� ���� �����ʴϱ�? " );
	
	m_ctlMySelf.SetWindowTextW( strMySelf );
}


// �κ񿡼� �α��� ȭ������ ��ȯ�� ���� ���� ������ Disconnect���� ȭ�� ����
void LobbyInterface::OnBnClickedLeaveLobby()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		::PostMessage( pDoc->m_MyLobbyClient.GetMyHwnd(), UM_UPDATE_DISPLAY, LOBBY_PLAYER_DELETE_ALL, 0 );
		::PostMessage( pDoc->m_MyLobbyClient.GetMyHwnd(), UM_UPDATE_DISPLAY, LOBBY_ROOM_DELETE_ALL, 0 );
		pDoc->m_MyLobbyClient.Release();
		pDoc->m_MyLobbyClient.ChangeDisplay( LOGIN_DISPLAY );
	}
}

void LobbyInterface::OnBnClickedCreateRoom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		pDoc->OnCreateRoom();
	}
}

void LobbyInterface::OnBnClickedEnterRoom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	static RoomList	*pRoomList	= reinterpret_cast< RoomList* >( pFrame->m_pLobbyDisplay->m_wndSplitUp.GetPane( 1, 0 ) );

	if( NULL != pRoomList )
		if( 0 > pRoomList->m_nSelectedItem )
			return;
	
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MyLobbyClient.RequestJoinRoom( static_cast< BYTE >( pRoomList->m_nSelectedItem ) );

}
