#pragma once
#include "afxwin.h"


// DlgJoinerInfo ��ȭ �����Դϴ�.

class DlgJoinerInfo : public CDialog
{
	DECLARE_DYNAMIC(DlgJoinerInfo)

public:
	
	CStatic m_ctlJoinerID;
	CStatic m_ctlJoinerRecord;
	CStatic m_ctlJoinerExp;
	CStatic m_ctlJoinerLevel;

	CFont   m_Font;				// ��Ʈ

public:

	BOOL UpdateJoinerInfo( char *pInfo );

public:
	DlgJoinerInfo(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgJoinerInfo();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_JOINER_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedJoinerInfoOk();
};
