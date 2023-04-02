// JoinerList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "JoinerList.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"



// JoinerList

IMPLEMENT_DYNCREATE(JoinerList, CListView)

JoinerList::JoinerList()
{
	m_nSelectedItem = -1;
	m_rgbLightGray	= RGB( 247, 247, 247 );
	m_rgbWhite		= RGB( 255, 255, 255 );
}

JoinerList::~JoinerList()
{
}

BEGIN_MESSAGE_MAP(JoinerList, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &JoinerList::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &JoinerList::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &JoinerList::OnNMRclick)
	ON_COMMAND_RANGE( ID_OUT_PLAYER, ID_SECRET_CHAT_FOR_CAPTAIN, &JoinerList::OnRoomMenuControlForCaptain )
	ON_COMMAND_RANGE( ID_SHOW_INFO_FOR_USER, ID_SECRET_CHAT_FOR_USER, &JoinerList::OnRoomMenuControlForUser )
END_MESSAGE_MAP()


// JoinerList 진단입니다.

#ifdef _DEBUG
void JoinerList::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void JoinerList::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// JoinerList 메시지 처리기입니다.


void JoinerList::SetColumnMenu()
{
	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	wchar_t *listColumn[3] = { L"번호", L"아이디", L"상태" };
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
void JoinerList::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SetColumnMenu();

	DWORD style = LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_FULLROWSELECT;
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(style));
	GetListCtrl().ModifyStyle(0, LVS_EDITLABELS);
}

void JoinerList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_nSelectedItem = pNMLV->iItem;

	if( 0 > m_nSelectedItem )
		return;

	*pResult = 0;
}

void JoinerList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_nSelectedItem = pNMLV->iItem;

	if( 0 > m_nSelectedItem )
		return;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

//	if( pDoc->m_MyLobbyClient.RequestJoinRoom( static_cast<BYTE>( m_nSelectedItem ) ) )
//		m_nSelectedItem = -1;

	*pResult = 0;
}

void JoinerList::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_nSelectedItem = pNMLV->iItem;

	if( 0 > m_nSelectedItem )
		return;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( pDoc->m_MyLobbyClient.GetPosInRoom() == static_cast<BYTE>( m_nSelectedItem ) )
		return;

	CMenu popMenu;
	CMenu *pPopMenu = NULL;
	POINT point;
	
	GetCursorPos(&point);

	if( pDoc->m_MyLobbyClient.IsCaptain() )
	{		
		popMenu.LoadMenuW( IDR_ROOM_MENU_FOR_CAPTAIN );
		pPopMenu = popMenu.GetSubMenu( 0 );
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this );

	}
	else
	{
		popMenu.LoadMenuW( IDR_ROOM_MENU_FOR_USER );
		pPopMenu = popMenu.GetSubMenu( 0 );
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this );
	}

	*pResult = 0;
}

void JoinerList::OnRoomMenuControlForCaptain( UINT nID )
{

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	switch ( nID )
	{
	case ID_OUT_PLAYER:
		pDoc->m_MyLobbyClient.RequesCompulsiontOutRoom( static_cast<BYTE>( m_nSelectedItem ) );		
		break;

	case ID_GIVE_CAPTAIN:
		pDoc->m_MyLobbyClient.RequestGiveCaptain( static_cast<BYTE>( m_nSelectedItem ) );
		break;

	case ID_SHOW_INFO_FOR_CAPTAIN:
		pDoc->m_MyLobbyClient.RequestJoinerInfo( static_cast<BYTE>( m_nSelectedItem ) );		
		break;

	case ID_SECRET_CHAT_FOR_CAPTAIN:
		
		break;
	}	
}


void JoinerList::OnRoomMenuControlForUser( UINT nID )
{
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	switch ( nID )
	{
	case ID_SHOW_INFO_FOR_USER:
		pDoc->m_MyLobbyClient.RequestJoinerInfo( static_cast<BYTE>( m_nSelectedItem ) );		
		break;

	case ID_SECRET_CHAT_FOR_USER:

		break;	
	}

}