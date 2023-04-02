// RoomList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "RoomList.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"



// RoomList

IMPLEMENT_DYNCREATE(RoomList, CListView)

RoomList::RoomList()
{
	m_nSelectedItem = -1;
	m_rgbLightGray	= RGB( 247, 247, 247 );
	m_rgbWhite		= RGB( 255, 255, 255 );
}

RoomList::~RoomList()
{
}

BEGIN_MESSAGE_MAP(RoomList, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &RoomList::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &RoomList::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &RoomList::OnNMCustomdraw)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// RoomList 진단입니다.

#ifdef _DEBUG
void RoomList::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void RoomList::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// RoomList 메시지 처리기입니다.

// 리스트 컬럼 메뉴 셋팅
void RoomList::SetColumnMenu()
{

	long style = GetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE);
	style &= ~(LVS_TYPEMASK);
	style |= LVS_REPORT;
	SetWindowLong(GetListCtrl().m_hWnd, GWL_STYLE, style);

	LVCOLUMN lvColumn;
	wchar_t *listColumn[4] = {L"방번호", L"방이름", L"게임상태", L"참여인원"};
	int nWidth[4] = { 100,400, 95, 95};

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

void RoomList::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SetColumnMenu();

	DWORD style = LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_FULLROWSELECT ;
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(style));
	GetListCtrl().ModifyStyle(0, LVS_EDITLABELS);	

	m_nSelectedItem = -1;
}


void RoomList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_nSelectedItem = pNMLV->iItem;

	if( 0 > m_nSelectedItem )
		return;

	*pResult = 0;
}


void RoomList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_nSelectedItem = pNMLV->iItem;

	if( 0 > m_nSelectedItem )
		return;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( pDoc->m_MyLobbyClient.RequestJoinRoom( static_cast<BYTE>( m_nSelectedItem ) ) )
		m_nSelectedItem = -1;

	*pResult = 0;
}


void RoomList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	DWORD_PTR row = lplvcd->nmcd.dwItemSpec;

	switch( lplvcd->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
		// Modify item text and or background
	case CDDS_ITEMPREPAINT:
		{
			lplvcd->clrText = RGB(0,0,0);
			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}

		// Modify sub item text and/or background
	case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			if( row % 10 < 5 ){
				lplvcd->clrTextBk = m_rgbWhite;
			}
			else{
				lplvcd->clrTextBk = m_rgbLightGray;
			}

			*pResult = CDRF_DODEFAULT;
			return;
		}
	}

}

BOOL RoomList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	POINT point;

	CListCtrl& ICtrl = GetListCtrl();
	int chunk_count;

/*	SCROLLINFO si;
	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_HORZ, &si);
	rect.left -= si.nPos;
*/
	CBrush light_gray( m_rgbLightGray );
	CBrush white( m_rgbWhite );

	pDC->FillRect(&rect, &white);
	int i;
	/*
	chunk_count = ICtrl.GetHeaderCtrl()->GetItemCount();
	for( i = 0 ; i <= chunk_count; i++ )
	{
		rect.right = rect.left + ICtrl.GetColumnWidth(i);
		pDC->FillRect(&rect, i);
		rect.left += ICtrl.GetColumnWidth(i)
	}*/
	chunk_count = ICtrl.GetItemCount();
	for( i = 0 ; i <= chunk_count; i++ )
	{
		ICtrl.GetItemPosition(i, &point);
		rect.top = point.y;
		ICtrl.GetItemPosition(i + 1, &point);
		rect.bottom = point.y;

		if( i % 10 < 5 )
			pDC->FillRect(&rect, &white);
		else
			pDC->FillRect(&rect, &light_gray);
	}

	light_gray.DeleteObject();
	white.DeleteObject();

	//return CListView::OnEraseBkgnd(pDC);
	return FALSE;

}
