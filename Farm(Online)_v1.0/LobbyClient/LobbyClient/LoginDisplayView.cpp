// LoginDisplayView.cpp : 구현 파일입니다.
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


// LoginDisplayView 그리기입니다.

void LoginDisplayView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.

	CLobbyClientView::OnDraw(pDC);
}


// LoginDisplayView 진단입니다.

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


// LoginDisplayView 메시지 처리기입니다.

int LoginDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	CMainFrame *pFrame = (CMainFrame *)GetParent();

    if (!pFrame->m_pLoginDisplay)
	    pFrame->m_pLoginDisplay = reinterpret_cast< LoginDisplayView * >( this );


	return 0;
}

void LoginDisplayView::OnInitialUpdate()
{
	CLobbyClientView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_bitMapMain.LoadBitmapW( IDB_BITMAP12 );
}

void LoginDisplayView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CLobbyClientView::OnPaint()을(를) 호출하지 마십시오.
	
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap *pOldBit = (CBitmap*)MemDC.SelectObject(&m_bitMapMain);

	BITMAP Info;
	m_bitMapMain.GetBitmap(&Info);

	dc.BitBlt(0, 0, Info.bmWidth,Info.bmHeight, &MemDC,0,0,SRCCOPY);

	MemDC.SelectObject(&pOldBit);
}
