#pragma once
#include "afxcmn.h"
#include "resource.h"


// DlgLogin 대화 상자입니다.

class DlgLogin : public CDialog
{
	DECLARE_DYNAMIC(DlgLogin)

public:

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGIN_AND_REGISTERMEMBER };

	CString			m_strID;
	CString			m_strPW;
	CIPAddressCtrl	m_ctlIP;

public:
	DlgLogin(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgLogin();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedLoginOk();
	afx_msg void OnBnClickedLoginCancel();
};
