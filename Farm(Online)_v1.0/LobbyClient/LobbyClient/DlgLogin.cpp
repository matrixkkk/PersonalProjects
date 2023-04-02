// DlgLogin.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgLogin.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// DlgLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgLogin, CDialog)

DlgLogin::DlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(DlgLogin::IDD, pParent)
	, m_strID(_T(""))
	, m_strPW(_T(""))
{

}

DlgLogin::~DlgLogin()
{
}

void DlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strID);
	DDV_MaxChars(pDX, m_strID, 25);
	DDX_Text(pDX, IDC_EDIT2, m_strPW);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ctlIP);
}


BEGIN_MESSAGE_MAP(DlgLogin, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_LOGIN_OK, &DlgLogin::OnBnClickedLoginOk)
	ON_BN_CLICKED(ID_LOGIN_CANCEL, &DlgLogin::OnBnClickedLoginCancel)
END_MESSAGE_MAP()


// DlgLogin 메시지 처리기입니다.

BOOL DlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgLogin::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );

	CDialog::OnClose();
}

void DlgLogin::OnBnClickedLoginOk()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	BYTE ip[4] = { 0, };
	CString sIP;
	
	m_ctlIP.GetAddress(ip[0], ip[1], ip[2], ip[3] );
	sIP.Format( L"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

	if( pDoc->m_MyLobbyClient.RequestLogin( sIP, m_strID, m_strPW ) )
	{

		m_strID = _T("");
		m_strPW = _T("");

		UpdateData( FALSE );

		if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );
	}

}

void DlgLogin::OnBnClickedLoginCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_strID = _T("");
	m_strPW = _T("");

	UpdateData( FALSE );

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );
}

