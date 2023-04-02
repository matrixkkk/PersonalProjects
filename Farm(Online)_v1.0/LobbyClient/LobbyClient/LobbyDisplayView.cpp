// LobbyDisplayView.cpp : 구현 파일입니다.
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


// LobbyDisplayView 그리기입니다.

void LobbyDisplayView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// LobbyDisplayView 진단입니다.

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


// LobbyDisplayView 메시지 처리기입니다.

int LobbyDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
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

	//4. 두번째로 정적 분할된 윈도우에 첫번째 뷰를 붙인다.
	m_wndSplitRight.CreateView(0, 0, RUNTIME_CLASS(CChatInLobby), CSize(rect.Width() - 200 ,rect.Height() / 2), pContext);

	//5. 두번째로 정적 분할된 윈도우에 두번째 뷰를 붙인다.
	m_wndSplitRight.CreateView(0, 1, RUNTIME_CLASS(PlayerList), CSize(  200,		rect.Height() / 2 ), pContext);

	if( !m_wndSplitRight1.CreateStatic( &m_wndSplitUp, 1, 2,WS_CHILD | WS_VISIBLE | WS_BORDER,	m_wndSplitUp.IdFromRowCol(1, 0)))
	{
		TRACE0("Fail to create splitter window.\n");
		return FALSE;
	}

	//5. 두번째로 정적 분할된 윈도우에 두번째 뷰를 붙인다.
	m_wndSplitRight1.CreateView(0, 0, RUNTIME_CLASS(MenuInterface)	, CSize(  250,		rect.Height() / 2 )			, pContext);

	//4. 두번째로 정적 분할된 윈도우에 첫번째 뷰를 붙인다.
	m_wndSplitRight1.CreateView(0, 1, RUNTIME_CLASS(RoomList)		, CSize(rect.Width() - 250 ,rect.Height() / 2)	, pContext);

	

	return 0;
}

void LobbyDisplayView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	ReleaseCapture();
}

void LobbyDisplayView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.


}

void LobbyDisplayView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

}
