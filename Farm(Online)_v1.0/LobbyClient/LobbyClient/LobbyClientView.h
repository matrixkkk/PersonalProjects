// LobbyClientView.h : CLobbyClientView Ŭ������ �������̽�
//

#pragma once
#include "LobbyClientDoc.h"



class CLobbyClientView : public CView
{
public: // serialization������ ��������ϴ�.
	CLobbyClientView();
	DECLARE_DYNCREATE(CLobbyClientView)

// Ư���Դϴ�.
public:
	CLobbyClientDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CLobbyClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // LobbyClientView.cpp�� ����� ����
inline CLobbyClientDoc* CLobbyClientView::GetDocument() const
   { return reinterpret_cast<CLobbyClientDoc*>(m_pDocument); }
#endif

