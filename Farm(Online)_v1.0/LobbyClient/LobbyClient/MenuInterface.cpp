// MenuInterface.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "MenuInterface.h"


// MenuInterface

IMPLEMENT_DYNCREATE(MenuInterface, CFormView)

MenuInterface::MenuInterface()
	: CFormView(MenuInterface::IDD)
{

}

MenuInterface::~MenuInterface()
{
}

void MenuInterface::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MenuInterface, CFormView)
END_MESSAGE_MAP()


// MenuInterface 진단입니다.

#ifdef _DEBUG
void MenuInterface::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void MenuInterface::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// MenuInterface 메시지 처리기입니다.

void MenuInterface::OnInitialUpdate()
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

//	m_ctlMySelf.SetFont(&m_Font, TRUE);	
	
}