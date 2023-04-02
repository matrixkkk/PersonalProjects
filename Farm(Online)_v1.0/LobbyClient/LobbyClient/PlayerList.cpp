// PlayerList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "PlayerList.h"


// PlayerList

IMPLEMENT_DYNCREATE(PlayerList, CListView)

PlayerList::PlayerList()
{

}

PlayerList::~PlayerList()
{
}

BEGIN_MESSAGE_MAP(PlayerList, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &PlayerList::OnLvnItemchanged)
END_MESSAGE_MAP()


// PlayerList 진단입니다.

#ifdef _DEBUG
void PlayerList::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void PlayerList::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// PlayerList 메시지 처리기입니다.

void PlayerList::SetColumnMenu()
{
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	
	wchar_t *listColumn[3] = {L"아이디", L"아이피", L"포트번호", };
	int nWidth[3] = { 102,105, 105};

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


void PlayerList::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SetColumnMenu();

	DWORD style = LVS_EX_GRIDLINES | LVS_EX_FLATSB;
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(style));
	GetListCtrl().ModifyStyle(0, LVS_EDITLABELS);
/*
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

	*/
}

void PlayerList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
