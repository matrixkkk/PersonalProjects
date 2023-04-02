// LobbyServerDlg.h : 헤더 파일
//

#pragma once

#include "NetLobbyServer.h"
#include "afxwin.h"

// CLobbyServerDlg 대화 상자
class CLobbyServerDlg : public CDialog
{

	NetLobbyServer	m_LobbyServer;				
	BOOL			m_bCurrnetServerState;		// 헌재 서버 동작 상태			
//	CEdit			m_EditSendMsg;				// 서버의 접속한 모든 Client들에게 공지 메세지 전송
	CString			m_strSendMsg;
	
	CString m_strServerState;					// 서버 상태
	int m_nNumOfPlayers;
	int m_nNumOfRooms;

	// 생성입니다.
public:
	CLobbyServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOBBYSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

		

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
