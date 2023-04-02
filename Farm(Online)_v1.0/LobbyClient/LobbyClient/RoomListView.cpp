// RoomListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyClientDoc.h"
#include "RoomListView.h"


// RoomListView

IMPLEMENT_DYNCREATE(RoomListView, CListView)

RoomListView::RoomListView()
{

}

RoomListView::~RoomListView()
{
}

BEGIN_MESSAGE_MAP(RoomListView, CListView)
END_MESSAGE_MAP()


// RoomListView 진단입니다.

#ifdef _DEBUG
void RoomListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void RoomListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

void RoomListView::SetColumnMenu()//드라이브 아이템 헤더
{
	
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	LV_ITEM lvItem;
	wchar_t *listColumn[4] = {L"방번호", L"방이름", L"게임상태", L"참여인원"};
	int nWidth[4] = { 100,400, 100, 100};

	for(int i=0; i< 4; i++)
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = listColumn[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = nWidth[i];
		GetListCtrl().InsertColumn(i, &lvColumn);
	}
}


// RoomListView 메시지 처리기입니다.

void RoomListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	SetColumnMenu();
}
