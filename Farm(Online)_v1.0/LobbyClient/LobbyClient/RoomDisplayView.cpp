// RoomDisplayView.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "LobbyClient.h"
#include "RoomDisplayView.h"


// RoomDisplayView

IMPLEMENT_DYNCREATE(RoomDisplayView, CView)

RoomDisplayView::RoomDisplayView()
{

}

RoomDisplayView::~RoomDisplayView()
{
}

BEGIN_MESSAGE_MAP(RoomDisplayView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// RoomDisplayView 그리기입니다.

void RoomDisplayView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// RoomDisplayView 진단입니다.

#ifdef _DEBUG
void RoomDisplayView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void RoomDisplayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// RoomDisplayView 메시지 처리기입니다.

int RoomDisplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	m_wndSplitUp.CreateView(0, 0, RUNTIME_CLASS(RoomInterface)	, CSize( rect.Width(),100 )				, pContext);	
	m_wndSplitUp.CreateView(2, 0, RUNTIME_CLASS(CChatInRoom)	, CSize(rect.Width(),rect.Height() / 4 ), pContext);

	if (!m_wndSplitRight.CreateStatic(&m_wndSplitUp, 1, 2,WS_CHILD | WS_VISIBLE | WS_BORDER,	m_wndSplitUp.IdFromRowCol(1, 0)))
	{
		TRACE0("Fail to create splitter window.\n");
		return FALSE;
	}

	//4. 두번째로 정적 분할된 윈도우에 첫번째 뷰를 붙인다.
	m_wndSplitRight.CreateView(0, 0, RUNTIME_CLASS(MapInterface), CSize(rect.Width() - 200 ,rect.Height() / 2), pContext);

	//5. 두번째로 정적 분할된 윈도우에 두번째 뷰를 붙인다.
	m_wndSplitRight.CreateView(0, 1, RUNTIME_CLASS(JoinerList), CSize(  200,		rect.Height() / 2 ), pContext);

	return 0;
}

void RoomDisplayView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_wndSplitUp.MoveWindow(0,	0,	cx,	cy);
	m_wndSplitUp.SetRowInfo(0, 50, 0);
	m_wndSplitUp.SetRowInfo(1, cy/2 , 0);
	m_wndSplitUp.SetRowInfo(2, 200 , 0);

	m_wndSplitRight.SetColumnInfo(0, cx/4 , 0);
	m_wndSplitRight.SetColumnInfo(1, cx/4 * 3, 0);
	
	m_wndSplitUp.RecalcLayout();	
}

void RoomDisplayView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
