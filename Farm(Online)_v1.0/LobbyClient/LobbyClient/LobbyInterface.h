#pragma once

#include "FixedSplitterWnd.h"
#include "afxwin.h"

// LobbyInterface 폼 뷰입니다.

class LobbyInterface : public CFormView
{
	DECLARE_DYNCREATE(LobbyInterface)

public:
	LobbyInterface();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LobbyInterface();

private:

	CStatic m_ctlMySelf;		// 내 아이디 출력
	CFont   m_Font;

public:

	void UpdateMyGameID( CString &strMyGameID );


public:
	enum { IDD = IDD_LOBBYINTERFACE };
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
public:
	
public:
	afx_msg void OnBnClickedLeaveLobby();
public:
	afx_msg void OnBnClickedCreateRoom();
public:
	afx_msg void OnBnClickedEnterRoom();
};


