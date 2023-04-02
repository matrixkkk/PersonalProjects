// LobbyClientDoc.cpp : CLobbyClientDoc 클래스의 구현
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CLobbyClientDoc

IMPLEMENT_DYNCREATE(CLobbyClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CLobbyClientDoc, CDocument)

	ON_COMMAND( ID_BUTTON_LOGIN_DLG_VIEW, &CLobbyClientDoc::OnLogin)
	ON_COMMAND( ID_BUTTON_MEMBER_DLG_VIEW, &CLobbyClientDoc::OnMemberShip)	

END_MESSAGE_MAP()


// CLobbyClientDoc 생성/소멸

CLobbyClientDoc::CLobbyClientDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_pDlgLogin			= NULL;
	m_pDlgMember		= NULL;
	m_pDlgCreateRoom	= NULL;
	m_pDlgInvitation	= NULL;
	m_pDlgJoinerInfo	= NULL;

	m_pThread			= NULL;

	m_MapManager.Init();
}

CLobbyClientDoc::~CLobbyClientDoc()
{
}

BOOL CLobbyClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CLobbyClientDoc serialization

void CLobbyClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CLobbyClientDoc 진단

#ifdef _DEBUG
void CLobbyClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLobbyClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLobbyClientDoc 명령

void CLobbyClientDoc::OnLogin()
{
	
	if( NULL != m_pDlgLogin )
	{
		if( m_pDlgLogin->IsWindowVisible() )
			m_pDlgLogin->ShowWindow( SW_HIDE );
		else
		{
			m_pDlgLogin->ShowWindow( SW_SHOW );			

		}
			
	}
}


void CLobbyClientDoc::OnMemberShip()
{
	if( NULL != m_pDlgMember )
	{
		if( m_pDlgMember->IsWindowVisible() )
			m_pDlgMember->ShowWindow( SW_HIDE );
		else
		{
			m_pDlgMember->ShowWindow( SW_SHOW );

		}
	}
}	

void CLobbyClientDoc::OnCreateRoom()
{
	if( NULL != m_pDlgCreateRoom )
	{
		if( m_pDlgCreateRoom->IsWindowVisible() )
			m_pDlgCreateRoom->ShowWindow( SW_HIDE );
		else
		{
			m_pDlgCreateRoom->ShowWindow( SW_SHOW );
		}
	}
}

void CLobbyClientDoc::OnCloseDocument()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( NULL != m_pDlgLogin )
	{
		delete m_pDlgLogin;
		m_pDlgLogin = NULL;
	}

	if( NULL != m_pDlgMember )
	{
		delete m_pDlgMember;
		m_pDlgMember = NULL;
	}

	if( NULL != m_pDlgCreateRoom )
	{
		delete m_pDlgCreateRoom;
		m_pDlgCreateRoom = NULL;
	}

	if( NULL != m_pDlgInvitation )
	{
		delete m_pDlgInvitation;
		m_pDlgInvitation = NULL;
	}

	if( NULL != m_pDlgJoinerInfo )
	{
		delete m_pDlgJoinerInfo;
		m_pDlgJoinerInfo = NULL;
	}

	/* 다음과 같이 사용자 인터페이스 쓰레드는 메시지를 주고 받을 수 있습니다.   */ 
    if( NULL != m_pThread )
	{
		m_pThread->PostThreadMessage(WM_QUIT, 0, 0);    /* UI 쓰레드 종료 메시지 전송 */

		DWORD nExitCode = NULL ;
		GetExitCodeThread( m_pThread->m_hThread, &nExitCode ) ;
		TerminateThread( m_pThread->m_hThread, nExitCode ) ;
	}

	m_MyLobbyClient.End();

	CDocument::OnCloseDocument();
}
