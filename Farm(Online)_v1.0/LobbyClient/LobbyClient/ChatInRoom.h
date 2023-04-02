#pragma once
#include "afxwin.h"



// CChatInRoom �� ���Դϴ�.

class CChatInRoom : public CFormView
{
	DECLARE_DYNCREATE(CChatInRoom)

public:
	CChatInRoom();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CChatInRoom();

public:

	CString m_strSendMsg;	// Send ä�� �޽���
	CEdit	m_CtlChatMsg;	// ȭ�� ��� ä�� �޽���
	CFont	m_Font;			// ��Ʈ ����

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedButton1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
};


