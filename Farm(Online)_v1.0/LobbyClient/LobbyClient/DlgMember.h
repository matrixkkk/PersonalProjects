#pragma once

#include "resource.h"
// DlgMember ��ȭ �����Դϴ�.

class DlgMember : public CDialog
{
	DECLARE_DYNAMIC(DlgMember)

public:

	CString m_strID;
	CString m_strPW;
	CString m_strGameID;

public:
	DlgMember(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgMember();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MEMBERSHIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	
public:
	afx_msg void OnBnClickedJoinOk();
	afx_msg void OnBnClickedJoinCancel();
public:
	
};
