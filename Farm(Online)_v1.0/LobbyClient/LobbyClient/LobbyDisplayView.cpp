// LobbyDisplayView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyDisplayView.h"


// LobbyDisplayView

IMPLEMENT_DYNCREATE(LobbyDisplayView, CView)

LobbyDisplayView::LobbyDisplayView()
{

}

LobbyDisplayView::~LobbyDisplayView()
{
}

BEGIN_MESSAGE_MAP(LobbyDisplayView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LobbyDisplayView �׸����Դϴ�.

void LobbyDisplayView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
}


// LobbyDisplayView �����Դϴ�.

#ifdef _DEBUG
void LobbyDisplayView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LobbyDisplayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// LobbyDisplayView �޽��� ó�����Դϴ�.

int LobbyDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	if( !m_wndSplitUp.CreateStatic( this, 3, 1 ) )
	{
		TRACE0("Fail to create splitter window.\n");
		return FALSE;    // failed to create
	}

	CRect rect;
	GetClientRect(&rect);
	CCreateContext *pContext = (CCreateContext*) lpCreateStruct->lpCreateParams;

	m_wndSplitUp.CreateView(0, 0, RUNTIME_CLASS(LobbyInterface), CSize( rect.Width(),100 ), pContext);	
//	m_wndSplitUp.CreateView(1, 0, RUNTIME_CLASS(RoomList), CSize(rect.Width(),rect.Height() / 2), pContext);

	if(!m_wndSplitRight.CreateStatic(&m_wndSplitUp, 1, 2,WS_CHILD | WS_VISIBLE | WS_BORDER,	m_wndSplitUp.IdFromRowCol(2, 0)))
	{
		TRACE0("Fail to create splitter window.\n");
		return FALSE;
	}

	//4. �ι�°�� ���� ���ҵ� �����쿡 ù��° �並 ���δ�.
	m_wndSplitRight.CreateView(0, 0, RUNTIME_CLASS(CChatInLobby), CSize(rect.Width() - 200 ,rect.Height() / 2), pContext);

	//5. �ι�°�� ���� ���ҵ� �����쿡 �ι�° �並 ���δ�.
	m_wndSplitRight.CreateView(0, 1, RUNTIME_CLASS(PlayerList), CSize(  200,		rect.Height() / 2 ), pContext);

	if( !m_wndSplitRight1.CreateStatic( &m_wndSplitUp, 1, 2,WS_CHILD | WS_VISIBLE | WS_BORDER,	m_wndSplitUp.IdFromRowCol(1, 0)))
	{
		TRACE0("Fail to create splitter window.\n");
		return FALSE;
	}

	//5. �ι�°�� ���� ���ҵ� �����쿡 �ι�° �並 ���δ�.
	m_wndSplitRight1.CreateView(0, 0, RUNTIME_CLASS(MenuInterface)	, CSize(  250,		rect.Height() / 2 )			, pContext);

	//4. �ι�°�� ���� ���ҵ� �����쿡 ù��° �並 ���δ�.
	m_wndSplitRight1.CreateView(0, 1, RUNTIME_CLASS(RoomList)		, CSize(rect.Width() - 250 ,rect.Height() / 2)	, pContext);

	

	return 0;
}

void LobbyDisplayView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_wndSplitUp.MoveWindow(0,	0,	cx,	cy);
	m_wndSplitUp.SetRowInfo(0, 50, 0);
	m_wndSplitUp.SetRowInfo(1, cy/2 , 0);

	m_wndSplitRight.SetColumnInfo(0, cx/3 * 2, 0);
	m_wndSplitRight.SetColumnInfo(1, cx/3, 0);
	
	m_wndSplitUp.RecalcLayout();	
}

void LobbyDisplayView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	ReleaseCapture();
}

void LobbyDisplayView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CView::OnPaint()��(��) ȣ������ ���ʽÿ�.


}

void LobbyDisplayView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

}
