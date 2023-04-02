#pragma once
#include "afxwin.h"



// RoomInterface �� ���Դϴ�.

class RoomInterface : public CFormView
{
	DECLARE_DYNCREATE(RoomInterface)

public:
	RoomInterface();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~RoomInterface();

public:

	CButton m_CtlReadyOrStart;	// ��ư��Ʈ��
	CStatic m_ctlRoomName;		// �� �̸�
	CButton m_ctlInvitation;	// �ʴ� �ϱ�

	CStatic m_ctlMyID;			// �� ���̵�
	CFont   m_Font;				// ��Ʈ
	BOOL	m_bClick;			// ��ư Ŭ������


public:

	void	UpdateMyGameRoomName( CString &strGameRoomName );
	void	UpdateMyGameID( CString &strMyID );
	void	SetBtnName( BOOL nUserType, BOOL bPush = FALSE );

public:
	enum { IDD = IDD_ROOMINTERFACE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	afx_msg void OnBnClickedButtonGameLeave();
	afx_msg void OnBnClickedButtonGameStart();

	
public:
	afx_msg void OnBnClickedButtonGameInvitation();
};


