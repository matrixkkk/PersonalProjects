#pragma once
#include "afxwin.h"



// RoomInterface 폼 뷰입니다.

class RoomInterface : public CFormView
{
	DECLARE_DYNCREATE(RoomInterface)

public:
	RoomInterface();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RoomInterface();

public:

	CButton m_CtlReadyOrStart;	// 버튼컨트롤
	CStatic m_ctlRoomName;		// 방 이름
	CButton m_ctlInvitation;	// 초대 하기

	CStatic m_ctlMyID;			// 내 아이디
	CFont   m_Font;				// 폰트
	BOOL	m_bClick;			// 버튼 클릭여부


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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	afx_msg void OnBnClickedButtonGameLeave();
	afx_msg void OnBnClickedButtonGameStart();

	
public:
	afx_msg void OnBnClickedButtonGameInvitation();
};


