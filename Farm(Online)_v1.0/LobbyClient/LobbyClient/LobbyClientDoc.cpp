// LobbyClientDoc.cpp : CLobbyClientDoc Ŭ������ ����
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


// CLobbyClientDoc ����/�Ҹ�

CLobbyClientDoc::CLobbyClientDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
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

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CLobbyClientDoc serialization

void CLobbyClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CLobbyClientDoc ����

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


// CLobbyClientDoc ���

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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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

	/* ������ ���� ����� �������̽� ������� �޽����� �ְ� ���� �� �ֽ��ϴ�.   */ 
    if( NULL != m_pThread )
	{
		m_pThread->PostThreadMessage(WM_QUIT, 0, 0);    /* UI ������ ���� �޽��� ���� */

		DWORD nExitCode = NULL ;
		GetExitCodeThread( m_pThread->m_hThread, &nExitCode ) ;
		TerminateThread( m_pThread->m_hThread, nExitCode ) ;
	}

	m_MyLobbyClient.End();

	CDocument::OnCloseDocument();
}
