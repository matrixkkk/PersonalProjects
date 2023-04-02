// ChatInRoom.cpp : ���� �����Դϴ�.
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


// CChatInRoom �����Դϴ�.

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


// CChatInRoom �޽��� ó�����Դϴ�.

void CChatInRoom::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
}

void CChatInRoom::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
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