// RoomInterface.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "RoomInterface.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// RoomInterface

IMPLEMENT_DYNCREATE(RoomInterface, CFormView)

RoomInterface::RoomInterface()
	: CFormView(RoomInterface::IDD)
{
	m_bClick = FALSE; 
}

RoomInterface::~RoomInterface()
{
}

void RoomInterface::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_GAME_START, m_CtlReadyOrStart);
	DDX_Control(pDX, IDC_STATIC_ROOM_NAME, m_ctlRoomName);
	DDX_Control(pDX, IDC_STATIC_MYSELF_IN_ROOM, m_ctlMyID);
	DDX_Control(pDX, IDC_BUTTON_GAME_INVITATION, m_ctlInvitation);
}

BEGIN_MESSAGE_MAP(RoomInterface, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_GAME_LEAVE, &RoomInterface::OnBnClickedButtonGameLeave)
	ON_BN_CLICKED(IDC_BUTTON_GAME_START, &RoomInterface::OnBnClickedButtonGameStart)
	ON_BN_CLICKED(IDC_BUTTON_GAME_INVITATION, &RoomInterface::OnBnClickedButtonGameInvitation)
END_MESSAGE_MAP()


// RoomInterface 진단입니다.

#ifdef _DEBUG
void RoomInterface::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void RoomInterface::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// RoomInterface 메시지 처리기입니다.

void RoomInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"돋움체"); 

	m_CtlReadyOrStart.SetFont(&m_Font, TRUE);
	m_ctlRoomName.SetFont( &m_Font, TRUE );
	m_ctlMyID.SetFont( &m_Font, TRUE );
	m_ctlInvitation.SetFont( &m_Font, TRUE );

	m_bClick = FALSE;
}

// 방 이름을 셋팅
void  RoomInterface::UpdateMyGameRoomName( CString &strGameRoomName )
{
	m_ctlRoomName.SetWindowTextW( strGameRoomName );
}


void RoomInterface::UpdateMyGameID( CString &strMyID )
{
	CString strMyGameID = strMyID;

	strMyGameID += _T("님! 깨달읍시다.!!");
	
	m_ctlMyID.SetWindowTextW( strMyGameID );

}

// 방장인지 일반 유저에 따라 버튼 Text를 갱신
void RoomInterface::SetBtnName( BOOL nUserType, BOOL bPush )
{
	/*
	STATE_DEFAULT = 0,	// 기본 상태
	STATE_READY,		// 레디 상태
	STATE_GAMMING,		// 게임중 상태
	STATE_CAPTAIN		// 방장
	*/

	// 방장이면
	if( nUserType ) 
	{
		m_CtlReadyOrStart.SetWindowTextW( _T("게임 시작") );		

		if( bPush )
		{
			m_CtlReadyOrStart.EnableWindow( TRUE );
		}
		else
		{
			m_CtlReadyOrStart.EnableWindow( FALSE );		
		}

		m_ctlInvitation.ShowWindow( SW_SHOW );
	}

	// 일반 참여자라면
	else		
	{
		m_CtlReadyOrStart.EnableWindow( TRUE );

		if( bPush )
			m_CtlReadyOrStart.SetWindowTextW( _T("준비 완료") );
		else
			m_CtlReadyOrStart.SetWindowTextW( _T("게임 준비") );

		m_ctlInvitation.ShowWindow( SW_HIDE );
	}
}


void RoomInterface::OnBnClickedButtonGameLeave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
		pDoc->m_MyLobbyClient.RequestOutRoom();	
}

void RoomInterface::OnBnClickedButtonGameStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		// 방장이면
		if( pDoc->m_MyLobbyClient.IsCaptain() )
			pDoc->m_MyLobbyClient.RequestStartGame();

		// 일반 참여자라면
		else
		{
			if( pDoc->m_MyLobbyClient.IsReady() )
			{
				m_CtlReadyOrStart.SetWindowTextW( _T("게임 준비") );
				pDoc->m_MyLobbyClient.GetReadyState() = FALSE;
			}
			else
			{
				m_CtlReadyOrStart.SetWindowTextW( _T("준비 완료") );
				pDoc->m_MyLobbyClient.GetReadyState() = TRUE;
			}	

			pDoc->m_MyLobbyClient.RequestReadyGame();
		}
	}
}

void RoomInterface::OnBnClickedButtonGameInvitation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		pDoc->m_MyLobbyClient.RequestInvitation();
	}
}
