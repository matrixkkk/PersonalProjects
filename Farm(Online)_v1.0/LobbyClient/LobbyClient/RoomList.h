#pragma once

#include "MyListCtrl.h"

// RoomList 뷰입니다.

class RoomList : public CListView
{
	DECLARE_DYNCREATE(RoomList)

public:
	RoomList();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RoomList();

public:

	int			m_nSelectedItem;
	COLORREF	m_rgbLightGray;
	COLORREF	m_rgbWhite;

//	CMyListCtrl m_CtlRoomList;

public:
	// 리스트 컬럼 메뉴 셋팅
	void SetColumnMenu();	

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


};


