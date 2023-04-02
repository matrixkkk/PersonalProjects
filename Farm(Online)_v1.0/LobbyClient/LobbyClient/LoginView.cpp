// LoginView.cpp : 구현 파일입니다.
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


// LoginView 그리기입니다.

void LoginView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// LoginView 진단입니다.

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


// LoginView 메시지 처리기입니다.
