#pragma once

#include "FixedSplitterWnd.h"
#include "RoomInterface.h"
#include "ChatInRoom.h"
#include "JoinerList.h"
#include "MapInterface.h"

// RoomDisplayView 뷰입니다.

class RoomDisplayView : public CView
{
	DECLARE_DYNCREATE(RoomDisplayView)

	CFixedSplitterWnd m_wndSplitUp;
	CFixedSplitterWnd m_wndSplitRight;

public:
	RoomDisplayView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RoomDisplayView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
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


