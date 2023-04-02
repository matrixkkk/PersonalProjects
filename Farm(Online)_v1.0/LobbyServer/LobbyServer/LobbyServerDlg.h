// LobbyServerDlg.h : ��� ����
//

#pragma once

#include "NetLobbyServer.h"
#include "afxwin.h"

// CLobbyServerDlg ��ȭ ����
class CLobbyServerDlg : public CDialog
{

	NetLobbyServer	m_LobbyServer;				
	BOOL			m_bCurrnetServerState;		// ���� ���� ���� ����			
//	CEdit			m_EditSendMsg;				// ������ ������ ��� Client�鿡�� ���� �޼��� ����
	CString			m_strSendMsg;
	
	CString m_strServerState;					// ���� ����
	int m_nNumOfPlayers;
	int m_nNumOfRooms;

	// �����Դϴ�.
public:
	CLobbyServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOBBYSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

		

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnBnClickedServerStart();
	afx_msg void OnBnClickedServerEnd();
	afx_msg void OnBnClickedChatSend();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);


	
};
