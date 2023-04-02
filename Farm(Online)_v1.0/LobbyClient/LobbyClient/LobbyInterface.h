#pragma once

#include "FixedSplitterWnd.h"
#include "afxwin.h"

// LobbyInterface �� ���Դϴ�.

class LobbyInterface : public CFormView
{
	DECLARE_DYNCREATE(LobbyInterface)

public:
	LobbyInterface();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LobbyInterface();

private:

	CStatic m_ctlMySelf;		// �� ���̵� ���
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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


