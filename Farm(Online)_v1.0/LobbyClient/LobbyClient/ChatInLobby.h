#pragma once



// CChatInLobby 폼 뷰입니다.

class CChatInLobby : public CFormView
{
	DECLARE_DYNCREATE(CChatInLobby)

public:
	CChatInLobby();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CChatInLobby();

public:
	CString m_strSendMsg;
	CEdit	m_CtlChatMsg;
	CFont	m_Font;

public:

	void UpdateChatEdit();
	void ResetChat();
	
public:
	enum { IDD = IDD_CHATINLOBBY };
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);	

public:
	afx_msg void OnBnClickedChatLobbySend();
	
};


