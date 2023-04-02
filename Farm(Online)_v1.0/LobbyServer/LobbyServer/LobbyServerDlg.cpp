// LobbyServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "LobbyServer.h"
#include "LobbyServerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CLobbyServerDlg ��ȭ ����




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


// CLobbyServerDlg �޽��� ó����

BOOL CLobbyServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_bCurrnetServerState	=	FALSE;						// ���� ���� ���� ����		
	m_strServerState = "���� ����";
//	m_EditSendMsg.SetWindowTextW( _T("") );					// ������ ������ ��� Client�鿡�� ���� �޼��� ����

	m_LobbyServer.SetDlgHandle( this->m_hWnd );
	UpdateData(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CLobbyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CLobbyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLobbyServerDlg::OnBnClickedServerStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( !m_bCurrnetServerState )
	{
		if( m_LobbyServer.Begin() )
		{
			m_bCurrnetServerState	= TRUE;
			m_strServerState		= "���� ����";	// ���� ������ ���� ���¸� ǥ��			
			UpdateData(FALSE);		
		}
	}
}

void CLobbyServerDlg::OnBnClickedServerEnd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( m_bCurrnetServerState )
	{
		if( m_LobbyServer.End() )
		{
			m_bCurrnetServerState	= FALSE;
			m_strServerState		= "���� ����";	// ���� ������ ���� ���¸� ǥ��			
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData(TRUE);

	if( m_strSendMsg.GetLength() <= 0 )
		return;

	CString strMsg = _T("[���ֽ�] ");
	strMsg += m_strSendMsg;

	m_LobbyServer.SendMsgOfManager( strMsg );

	m_strSendMsg = _T("");
	UpdateData(FALSE);

}

void CLobbyServerDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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
