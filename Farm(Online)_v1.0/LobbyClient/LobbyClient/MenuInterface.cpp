// MenuInterface.cpp : ���� �����Դϴ�.
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


// MenuInterface �����Դϴ�.

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


// MenuInterface �޽��� ó�����Դϴ�.

void MenuInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	// ��Ʈ ����
//	m_ctlMySelf.Create( _T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect( 5,5, 150, 80), this );
	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"����ü"); 

//	m_ctlMySelf.SetFont(&m_Font, TRUE);	
	
}