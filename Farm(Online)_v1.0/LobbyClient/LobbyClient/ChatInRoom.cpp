// ChatInRoom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "ChatInRoom.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// CChatInRoom

IMPLEMENT_DYNCREATE(CChatInRoom, CFormView)

CChatInRoom::CChatInRoom()
	: CFormView(CChatInRoom::IDD)
	, m_strSendMsg(_T(""))
{

}

CChatInRoom::~CChatInRoom()
{
}

void CChatInRoom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHATDISPLAY_ROOM	, m_CtlChatMsg );
	DDX_Text(	pDX, IDC_EDIT_CHAT_ROOM			, m_strSendMsg );
}

BEGIN_MESSAGE_MAP(CChatInRoom, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CChatInRoom::OnBnClickedButton1)
END_MESSAGE_MAP()


// CChatInRoom 진단입니다.

#ifdef _DEBUG
void CChatInRoom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CChatInRoom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CChatInRoom 메시지 처리기입니다.

void CChatInRoom::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
}

void CChatInRoom::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	UpdateData(TRUE);

	if( NULL == pDoc || m_strSendMsg.GetLength() <= 0 )
		return;

	pDoc->m_MyLobbyClient.RequestChatInRoom( m_strSendMsg );

	m_strSendMsg = _T("");
	UpdateData(FALSE);

}

void CChatInRoom::UpdateChatEdit()
{
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL == pDoc )
		return;
	
	pDoc->m_MyLobbyClient.UpdateChat( m_CtlChatMsg );	
}

void CChatInRoom::ResetChat()
{
	m_CtlChatMsg.SetReadOnly( FALSE );
	m_CtlChatMsg.SetFocus();
	m_CtlChatMsg.SetSel( 0, -1, TRUE );
	m_CtlChatMsg.Clear();
	m_CtlChatMsg.SetReadOnly( TRUE );
}


BOOL CChatInRoom::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message) 
	{
	
	case WM_KEYDOWN:

		switch(int(pMsg->wParam)) 
		{		
		
		case VK_RETURN:
				
			OnBnClickedButton1();
			break;
			
		case VK_ESCAPE:			
		
			break;		
		}

		break;	
	}

	return CFormView::PreTranslateMessage(pMsg);

}