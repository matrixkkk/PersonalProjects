#pragma once
#include "afxwin.h"


// DlgJoinerInfo 대화 상자입니다.

class DlgJoinerInfo : public CDialog
{
	DECLARE_DYNAMIC(DlgJoinerInfo)

public:
	
	CStatic m_ctlJoinerID;
	CStatic m_ctlJoinerRecord;
	CStatic m_ctlJoinerExp;
	CStatic m_ctlJoinerLevel;

	CFont   m_Font;				// 폰트

public:

	BOOL UpdateJoinerInfo( char *pInfo );

public:
	DlgJoinerInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgJoinerInfo();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_JOINER_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedJoinerInfoOk();
};
