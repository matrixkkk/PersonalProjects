#pragma once



// CChatInLobby �� ���Դϴ�.

class CChatInLobby : public CFormView
{
	DECLARE_DYNCREATE(CChatInLobby)

public:
	CChatInLobby();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);	

public:
	afx_msg void OnBnClickedChatLobbySend();
	
};


