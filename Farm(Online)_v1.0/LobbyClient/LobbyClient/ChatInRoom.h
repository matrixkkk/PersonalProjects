#pragma once
#include "afxwin.h"



// CChatInRoom 폼 뷰입니다.

class CChatInRoom : public CFormView
{
	DECLARE_DYNCREATE(CChatInRoom)

public:
	CChatInRoom();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CChatInRoom();

public:

	CString m_strSendMsg;	// Send 채팅 메시지
	CEdit	m_CtlChatMsg;	// 화면 출력 채팅 메시지
	CFont	m_Font;			// 폰트 설정

public:

	void UpdateChatEdit();
	void ResetChat();

public:
	enum { IDD = IDD_CHATINROOM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedButton1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
};


