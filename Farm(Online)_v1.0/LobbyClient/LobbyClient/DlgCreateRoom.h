#pragma once
#include "afxwin.h"


// DlgCreateRoom ��ȭ �����Դϴ�.

class DlgCreateRoom : public CDialog
{
	DECLARE_DYNAMIC(DlgCreateRoom)

public:
	DlgCreateRoom(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgCreateRoom();

public:
	CString m_strRoomName;
	CComboBox m_CtlMaxPlayer;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRoomMakeOk();
	afx_msg void OnBnClickedRoomMakeCancel();

	
};
