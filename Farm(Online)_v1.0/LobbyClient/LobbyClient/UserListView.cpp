// UserListView.cpp : ���� �����Դϴ�.
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

// UserListView �����Դϴ�.

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


// UserListView �޽��� ó�����Դϴ�.


// ����Ʈ �÷� �޴� ����
void UserListView::SetColumnMenu()
{
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	LV_ITEM lvItem;
	wchar_t *listColumn[3] = {L"���̵�", L"������", L"��Ʈ��ȣ", };
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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	SetColumnMenu();
}
