// hatInLobby.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "hatInLobby.h"


// ChatInLobby

IMPLEMENT_DYNCREATE(ChatInLobby, CFormView)

ChatInLobby::ChatInLobby()
	: CFormView(ChatInLobby::IDD)
{

}

ChatInLobby::~ChatInLobby()
{
}

void ChatInLobby::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ChatInLobby, CFormView)
END_MESSAGE_MAP()


// ChatInLobby 진단입니다.

#ifdef _DEBUG
void ChatInLobby::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ChatInLobby::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// ChatInLobby 메시지 처리기입니다.
