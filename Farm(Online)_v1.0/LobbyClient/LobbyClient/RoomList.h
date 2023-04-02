#pragma once

#include "MyListCtrl.h"

// RoomList ���Դϴ�.

class RoomList : public CListView
{
	DECLARE_DYNCREATE(RoomList)

public:
	RoomList();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~RoomList();

public:

	int			m_nSelectedItem;
	COLORREF	m_rgbLightGray;
	COLORREF	m_rgbWhite;

//	CMyListCtrl m_CtlRoomList;

public:
	// ����Ʈ �÷� �޴� ����
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


