// LoginView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LoginView.h"


// LoginView

IMPLEMENT_DYNCREATE(LoginView, CView)

LoginView::LoginView()
{

}

LoginView::~LoginView()
{
}

BEGIN_MESSAGE_MAP(LoginView, CView)
END_MESSAGE_MAP()


// LoginView �׸����Դϴ�.

void LoginView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
}


// LoginView �����Դϴ�.

#ifdef _DEBUG
void LoginView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LoginView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// LoginView �޽��� ó�����Դϴ�.
