#pragma once

#include "FixedSplitterWnd.h"
#include "RoomInterface.h"
#include "ChatInRoom.h"
#include "JoinerList.h"
#include "MapInterface.h"

// RoomDisplayView ���Դϴ�.

class RoomDisplayView : public CView
{
	DECLARE_DYNCREATE(RoomDisplayView)

	CFixedSplitterWnd m_wndSplitUp;
	CFixedSplitterWnd m_wndSplitRight;

public:
	RoomDisplayView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~RoomDisplayView();

public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};


