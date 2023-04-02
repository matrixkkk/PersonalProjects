#pragma once
#include "afxwin.h"


// DlgInvitation ��ȭ �����Դϴ�.

class DlgInvitation : public CDialog
{
	DECLARE_DYNAMIC(DlgInvitation)

public:

	CStatic m_cltInviteMsg;		// �ʴ� �޽���

	CFont   m_Font;				// ��Ʈ

	BYTE	m_nRoomNo;			// �� ��ȣ

public:

	void SetRoomInfo( wchar_t *pMsg, BYTE nRoomNo );

public:
	DlgInvitation(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgInvitation();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_INVITATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
