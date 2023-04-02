// RoomInterface.cpp : ���� �����Դϴ�.
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


// RoomInterface �����Դϴ�.

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


// RoomInterface �޽��� ó�����Դϴ�.

void RoomInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"����ü"); 

	m_CtlReadyOrStart.SetFont(&m_Font, TRUE);
	m_ctlRoomName.SetFont( &m_Font, TRUE );
	m_ctlMyID.SetFont( &m_Font, TRUE );
	m_ctlInvitation.SetFont( &m_Font, TRUE );

	m_bClick = FALSE;
}

// �� �̸��� ����
void  RoomInterface::UpdateMyGameRoomName( CString &strGameRoomName )
{
	m_ctlRoomName.SetWindowTextW( strGameRoomName );
}


void RoomInterface::UpdateMyGameID( CString &strMyID )
{
	CString strMyGameID = strMyID;

	strMyGameID += _T("��! �������ô�.!!");
	
	m_ctlMyID.SetWindowTextW( strMyGameID );

}

// �������� �Ϲ� ������ ���� ��ư Text�� ����
void RoomInterface::SetBtnName( BOOL nUserType, BOOL bPush )
{
	/*
	STATE_DEFAULT = 0,	// �⺻ ����
	STATE_READY,		// ���� ����
	STATE_GAMMING,		// ������ ����
	STATE_CAPTAIN		// ����
	*/

	// �����̸�
	if( nUserType ) 
	{
		m_CtlReadyOrStart.SetWindowTextW( _T("���� ����") );		

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

	// �Ϲ� �����ڶ��
	else		
	{
		m_CtlReadyOrStart.EnableWindow( TRUE );

		if( bPush )
			m_CtlReadyOrStart.SetWindowTextW( _T("�غ� �Ϸ�") );
		else
			m_CtlReadyOrStart.SetWindowTextW( _T("���� �غ�") );

		m_ctlInvitation.ShowWindow( SW_HIDE );
	}
}


void RoomInterface::OnBnClickedButtonGameLeave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
		pDoc->m_MyLobbyClient.RequestOutRoom();	
}

void RoomInterface::OnBnClickedButtonGameStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		// �����̸�
		if( pDoc->m_MyLobbyClient.IsCaptain() )
			pDoc->m_MyLobbyClient.RequestStartGame();

		// �Ϲ� �����ڶ��
		else
		{
			if( pDoc->m_MyLobbyClient.IsReady() )
			{
				m_CtlReadyOrStart.SetWindowTextW( _T("���� �غ�") );
				pDoc->m_MyLobbyClient.GetReadyState() = FALSE;
			}
			else
			{
				m_CtlReadyOrStart.SetWindowTextW( _T("�غ� �Ϸ�") );
				pDoc->m_MyLobbyClient.GetReadyState() = TRUE;
			}	

			pDoc->m_MyLobbyClient.RequestReadyGame();
		}
	}
}

void RoomInterface::OnBnClickedButtonGameInvitation()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc )
	{
		pDoc->m_MyLobbyClient.RequestInvitation();
	}
}
