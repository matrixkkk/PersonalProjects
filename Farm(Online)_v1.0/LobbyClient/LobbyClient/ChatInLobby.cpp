// ChatInLobby.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "ChatInLobby.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// CChatInLobby

IMPLEMENT_DYNCREATE(CChatInLobby, CFormView)

CChatInLobby::CChatInLobby()
	: CFormView(CChatInLobby::IDD),
	m_strSendMsg( _T("") )
	
{

}

CChatInLobby::~CChatInLobby()
{
}

void CChatInLobby::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text	(	pDX, IDC_EDIT_CHAT_LOBBY		, m_strSendMsg );
	DDX_Control	(	pDX, IDC_EDIT_CHATDISPLAY_LOBBY	, m_CtlChatMsg );	
}

BEGIN_MESSAGE_MAP(CChatInLobby, CFormView)
	
	ON_BN_CLICKED(IDC_CHAT_LOBBY_SEND, &CChatInLobby::OnBnClickedChatLobbySend)
END_MESSAGE_MAP()


// CChatInLobby �����Դϴ�.

#ifdef _DEBUG
void CChatInLobby::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CChatInLobby::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CChatInLobby �޽��� ó�����Դϴ�.

void CChatInLobby::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );
	
	// ��Ʈ �ٲٱ�
	LOGFONT lf; 
	::ZeroMemory( &lf , sizeof(LOGFONT)); 

    lf.lfCharSet = 0x81;					// �ѱ� Character Set 
    lf.lfHeight = 15;						// 20 point ũ�� 
    wcscpy_s( lf.lfFaceName, L"����ü");	// �ü�ü�� font setting 
    m_Font.CreateFontIndirect(&lf);        

    m_CtlChatMsg.SetFont((CFont*)&m_Font, TRUE); 

//	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
//	pDoc->m_MyLobbyClient.SetDlgOfLobbyChat( this->m_hWnd );
	
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


void CChatInLobby::OnBnClickedChatLobbySend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	UpdateData(TRUE);

	if( NULL == pDoc || m_strSendMsg.GetLength() <= 0 )
		return;

	pDoc->m_MyLobbyClient.RequestChatInLobby( m_strSendMsg );

	m_strSendMsg = _T("");
	UpdateData(FALSE);
	
}


void CChatInLobby::UpdateChatEdit()
{
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL == pDoc )
		return;
	
	pDoc->m_MyLobbyClient.UpdateChat( m_CtlChatMsg );	
}

void CChatInLobby::ResetChat()
{
	m_CtlChatMsg.SetReadOnly( FALSE );
	m_CtlChatMsg.SetFocus();
	m_CtlChatMsg.SetSel( 0, -1, TRUE );
	m_CtlChatMsg.Clear();
	m_CtlChatMsg.SetReadOnly( TRUE );
}

BOOL CChatInLobby::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message) 
	{
	
	case WM_KEYDOWN:

		switch(int(pMsg->wParam)) 
		{		
		
		case VK_RETURN:
				
			OnBnClickedChatLobbySend();
			break;
			
		case VK_ESCAPE:			
		
			break;		
		}

		break;	
	}

	return CFormView::PreTranslateMessage(pMsg);
}

