#pragma once

#include "LobbyClientView.h"
// LoginDisplayView ���Դϴ�.

class LoginDisplayView : public CLobbyClientView //public CView
{

public:

	CBitmap		m_bitMapMain;

public:
	DECLARE_DYNCREATE(LoginDisplayView)
	LoginDisplayView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

// Implementation
protected:
	virtual ~LoginDisplayView();
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.

	
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


