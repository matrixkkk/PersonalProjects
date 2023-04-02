#pragma once

#include "resource.h"
// DlgMember 대화 상자입니다.

class DlgMember : public CDialog
{
	DECLARE_DYNAMIC(DlgMember)

public:

	CString m_strID;
	CString m_strPW;
	CString m_strGameID;

public:
	DlgMember(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgMember();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MEMBERSHIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	
public:
	afx_msg void OnBnClickedJoinOk();
	afx_msg void OnBnClickedJoinCancel();
public:
	
};
