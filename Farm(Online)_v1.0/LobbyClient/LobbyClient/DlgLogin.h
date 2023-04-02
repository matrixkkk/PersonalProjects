#pragma once
#include "afxcmn.h"
#include "resource.h"


// DlgLogin ��ȭ �����Դϴ�.

class DlgLogin : public CDialog
{
	DECLARE_DYNAMIC(DlgLogin)

public:

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOGIN_AND_REGISTERMEMBER };

	CString			m_strID;
	CString			m_strPW;
	CIPAddressCtrl	m_ctlIP;

public:
	DlgLogin(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgLogin();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedLoginOk();
	afx_msg void OnBnClickedLoginCancel();
};
