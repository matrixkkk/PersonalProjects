// LobbyClientView.h : CLobbyClientView 클래스의 인터페이스
//

#pragma once
#include "LobbyClientDoc.h"



class CLobbyClientView : public CView
{
public: // serialization에서만 만들어집니다.
	CLobbyClientView();
	DECLARE_DYNCREATE(CLobbyClientView)

// 특성입니다.
public:
	CLobbyClientDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CLobbyClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // LobbyClientView.cpp의 디버그 버전
inline CLobbyClientDoc* CLobbyClientView::GetDocument() const
   { return reinterpret_cast<CLobbyClientDoc*>(m_pDocument); }
#endif

