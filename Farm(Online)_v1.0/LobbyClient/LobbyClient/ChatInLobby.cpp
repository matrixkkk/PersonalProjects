// ChatInLobby.cpp : 구현 파일입니다.
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


// CChatInLobby 진단입니다.

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


// CChatInLobby 메시지 처리기입니다.

void CChatInLobby::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );
	
	// 폰트 바꾸기
	LOGFONT lf; 
	::ZeroMemory( &lf , sizeof(LOGFONT)); 

    lf.lfCharSet = 0x81;					// 한글 Character Set 
    lf.lfHeight = 15;						// 20 point 크기 
    wcscpy_s( lf.lfFaceName, L"돋움체");	// 궁서체로 font setting 
    m_Font.CreateFontIndirect(&lf);        

    m_CtlChatMsg.SetFont((CFont*)&m_Font, TRUE); 

//	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
//	pDoc->m_MyLobbyClient.SetDlgOfLobbyChat( this->m_hWnd );
	
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void CChatInLobby::OnBnClickedChatLobbySend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

