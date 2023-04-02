#pragma once
#include "afxwin.h"


// DlgCreateRoom 대화 상자입니다.

class DlgCreateRoom : public CDialog
{
	DECLARE_DYNAMIC(DlgCreateRoom)

public:
	DlgCreateRoom(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgCreateRoom();

public:
	CString m_strRoomName;
	CComboBox m_CtlMaxPlayer;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRoomMakeOk();
	afx_msg void OnBnClickedRoomMakeCancel();

	
};
