// LobbyServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LobbyServer.h"
#include "LobbyServerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLobbyServerDlg 대화 상자




CLobbyServerDlg::CLobbyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLobbyServerDlg::IDD, pParent)
	, m_strServerState(_T(""))
	, m_nNumOfPlayers(0)
	, m_nNumOfRooms(0)
	, m_strSendMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLobbyServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
//	DDX_Control(pDX, IDC_EDIT2, m_EditSendMsg);
	DDX_Text(pDX, IDC_STATIC_SERVER_STATE		, m_strServerState);
	DDX_Text(pDX, IDC_STATIC_SERVER_PLAYERCOUNT	, m_nNumOfPlayers);
	DDX_Text(pDX, IDC_STATIC_SERVER_ROOMCOUNT	, m_nNumOfRooms);
	DDX_Text(pDX, IDC_EDIT_SEND_MSG				, m_strSendMsg );
}

BEGIN_MESSAGE_MAP(CLobbyServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SERVER_START, &CLobbyServerDlg::OnBnClickedServerStart)
	ON_BN_CLICKED(IDC_SERVER_END, &CLobbyServerDlg::OnBnClickedServerEnd)
	ON_BN_CLICKED(IDC_CHAT_SEND, &CLobbyServerDlg::OnBnClickedChatSend)
	ON_WM_KEYUP()
	ON_MESSAGE(UM_UPDATEDATA, OnUpdateData)
END_MESSAGE_MAP()


// CLobbyServerDlg 메시지 처리기

BOOL CLobbyServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_bCurrnetServerState	=	FALSE;						// 헌재 서버 동작 상태		
	m_strServerState = "서버 정지";
//	m_EditSendMsg.SetWindowTextW( _T("") );					// 서버의 접속한 모든 Client들에게 공지 메세지 전송

	m_LobbyServer.SetDlgHandle( this->m_hWnd );
	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLobbyServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLobbyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLobbyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLobbyServerDlg::OnBnClickedServerStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( !m_bCurrnetServerState )
	{
		if( m_LobbyServer.Begin() )
		{
			m_bCurrnetServerState	= TRUE;
			m_strServerState		= "서버 시작";	// 현재 서버의 동작 상태를 표시			
			UpdateData(FALSE);		
		}
	}
}

void CLobbyServerDlg::OnBnClickedServerEnd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_bCurrnetServerState )
	{
		if( m_LobbyServer.End() )
		{
			m_bCurrnetServerState	= FALSE;
			m_strServerState		= "서버 중지";	// 현재 서버의 동작 상태를 표시			
			m_nNumOfPlayers			= m_LobbyServer.GetNumOfPlayers();
			m_nNumOfRooms			= m_LobbyServer.GetNumOfRooms();
			UpdateData(FALSE);		
		}
	}
}

LRESULT CLobbyServerDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	
	if( wParam )
		UpdateData(TRUE);
	else
	{
		m_nNumOfPlayers		= m_LobbyServer.GetNumOfPlayers();
		m_nNumOfRooms		= m_LobbyServer.GetNumOfRooms();
		UpdateData(FALSE);
	}
	
	return 0;
}

void CLobbyServerDlg::OnBnClickedChatSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	if( m_strSendMsg.GetLength() <= 0 )
		return;

	CString strMsg = _T("[우주신] ");
	strMsg += m_strSendMsg;

	m_LobbyServer.SendMsgOfManager( strMsg );

	m_strSendMsg = _T("");
	UpdateData(FALSE);

}

void CLobbyServerDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CLobbyServerDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message) 
	{
	
	case WM_KEYDOWN:

		switch(int(pMsg->wParam)) 
		{		
		
		case VK_RETURN:
				
			OnBnClickedChatSend();
			break;
			
		case VK_ESCAPE:			
		
			break;		
		}

		break;	
	}

	return CDialog::PreTranslateMessage(pMsg);
}
