#pragma once
#include "afxwin.h"


// DlgInvitation 대화 상자입니다.

class DlgInvitation : public CDialog
{
	DECLARE_DYNAMIC(DlgInvitation)

public:

	CStatic m_cltInviteMsg;		// 초대 메시지

	CFont   m_Font;				// 폰트

	BYTE	m_nRoomNo;			// 방 번호

public:

	void SetRoomInfo( wchar_t *pMsg, BYTE nRoomNo );

public:
	DlgInvitation(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgInvitation();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INVITATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
