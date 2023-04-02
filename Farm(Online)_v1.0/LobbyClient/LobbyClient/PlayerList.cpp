// PlayerList.cpp : ���� �����Դϴ�.
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


// PlayerList �����Դϴ�.

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


// PlayerList �޽��� ó�����Դϴ�.

void PlayerList::SetColumnMenu()
{
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	
	wchar_t *listColumn[3] = {L"���̵�", L"������", L"��Ʈ��ȣ", };
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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}
