// LobbyInterface.cpp : 구현 파일입니다.
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


// LobbyInterface 진단입니다.

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


// LobbyInterface 메시지 처리기입니다.

void LobbyInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	// 폰트 설정
//	m_ctlMySelf.Create( _T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect( 5,5, 150, 80), this );
	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"돋움체"); 

	m_ctlMySelf.SetFont(&m_Font, TRUE);	
	
}

// 로비에서 로그인 화면으로 전환시 나의 게임 아이디를 출력하기 위해 갱신
void LobbyInterface::UpdateMyGameID( CString &strMyGameID )
{
	CString strMySelf( strMyGameID );

	strMySelf += _T(" 님 신을 믿으십니까? " );
	
	m_ctlMySelf.SetWindowTextW( strMySelf );
}


// 로비에서 로그인 화면으로 전환시 소켓 정보 제거후 Disconnect한후 화면 갱신
void LobbyInterface::OnBnClickedLeaveLobby()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		pDoc->OnCreateRoom();
	}
}

void LobbyInterface::OnBnClickedEnterRoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	static RoomList	*pRoomList	= reinterpret_cast< RoomList* >( pFrame->m_pLobbyDisplay->m_wndSplitUp.GetPane( 1, 0 ) );

	if( NULL != pRoomList )
		if( 0 > pRoomList->m_nSelectedItem )
			return;
	
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MyLobbyClient.RequestJoinRoom( static_cast< BYTE >( pRoomList->m_nSelectedItem ) );

}
