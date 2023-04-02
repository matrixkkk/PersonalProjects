// UserListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "UserListView.h"


// UserListView

IMPLEMENT_DYNCREATE(UserListView, CListView)

UserListView::UserListView()
{

}

UserListView::~UserListView()
{
}

BEGIN_MESSAGE_MAP(UserListView, CListView)
END_MESSAGE_MAP()

// UserListView 진단입니다.

#ifdef _DEBUG
void UserListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void UserListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// UserListView 메시지 처리기입니다.


// 리스트 컬럼 메뉴 셋팅
void UserListView::SetColumnMenu()
{
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	LV_ITEM lvItem;
	wchar_t *listColumn[3] = {L"아이디", L"아이피", L"포트번호", };
	int nWidth[3] = { 100,100, 100};

	for(int i=0; i< 3; i++)
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = listColumn[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = nWidth[i];
		GetListCtrl().InsertColumn(i, &lvColumn);
	}

}

void UserListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SetColumnMenu();
}
