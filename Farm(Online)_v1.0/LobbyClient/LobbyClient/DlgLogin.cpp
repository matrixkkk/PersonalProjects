// DlgLogin.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgLogin.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// DlgLogin ��ȭ �����Դϴ�.

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


// DlgLogin �޽��� ó�����Դϴ�.

BOOL DlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DlgLogin::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );

	CDialog::OnClose();
}

void DlgLogin::OnBnClickedLoginOk()
{
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_strID = _T("");
	m_strPW = _T("");

	UpdateData( FALSE );

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );
}

