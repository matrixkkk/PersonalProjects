// LoginDisplayView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LoginDisplayView.h"

#include "MainFrm.h"


// LoginDisplayView

IMPLEMENT_DYNCREATE(LoginDisplayView, CView)

LoginDisplayView::LoginDisplayView()
{

}

LoginDisplayView::~LoginDisplayView()
{
}

BEGIN_MESSAGE_MAP(LoginDisplayView, CView)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LoginDisplayView �׸����Դϴ�.

void LoginDisplayView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.

	CLobbyClientView::OnDraw(pDC);
}


// LoginDisplayView �����Դϴ�.

#ifdef _DEBUG
void LoginDisplayView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LoginDisplayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// LoginDisplayView �޽��� ó�����Դϴ�.

int LoginDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	
	CMainFrame *pFrame = (CMainFrame *)GetParent();

    if (!pFrame->m_pLoginDisplay)
	    pFrame->m_pLoginDisplay = reinterpret_cast< LoginDisplayView * >( this );


	return 0;
}

void LoginDisplayView::OnInitialUpdate()
{
	CLobbyClientView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	m_bitMapMain.LoadBitmapW( IDB_BITMAP12 );
}

void LoginDisplayView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CLobbyClientView::OnPaint()��(��) ȣ������ ���ʽÿ�.
	
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap *pOldBit = (CBitmap*)MemDC.SelectObject(&m_bitMapMain);

	BITMAP Info;
	m_bitMapMain.GetBitmap(&Info);

	dc.BitBlt(0, 0, Info.bmWidth,Info.bmHeight, &MemDC,0,0,SRCCOPY);

	MemDC.SelectObject(&pOldBit);
}
