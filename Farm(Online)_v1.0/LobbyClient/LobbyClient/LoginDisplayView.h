#pragma once

#include "LobbyClientView.h"
// LoginDisplayView 뷰입니다.

class LoginDisplayView : public CLobbyClientView //public CView
{

public:

	CBitmap		m_bitMapMain;

public:
	DECLARE_DYNCREATE(LoginDisplayView)
	LoginDisplayView();           // 동적 만들기에 사용되는 protected 생성자입니다.

// Implementation
protected:
	virtual ~LoginDisplayView();
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.

	
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnPaint();
};


