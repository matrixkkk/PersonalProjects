#pragma once

#include "FixedSplitterWnd.h"
#include "RoomList.h"
#include "PlayerList.h"
#include "ChatInLobby.h"
#include "MenuInterface.h"
#include "LobbyInterface.h"



// LobbyDisplayView ���Դϴ�.

class LobbyDisplayView : public CView
{
	DECLARE_DYNCREATE(LobbyDisplayView)

	CFixedSplitterWnd m_wndSplitUp;	
	CFixedSplitterWnd m_wndSplitRight;
	CFixedSplitterWnd m_wndSplitRight1;	//

	

public:

	LobbyDisplayView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LobbyDisplayView();

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
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	afx_msg void OnPaint();
public:
	virtual void OnInitialUpdate();
};


