// DlgMember.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyClientDoc.h"
#include "MainFrm.h"
#include "DlgMember.h"


// DlgMember ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DlgMember, CDialog)

DlgMember::DlgMember(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMember::IDD, pParent)
	, m_strID(_T(""))
	, m_strPW(_T(""))
	, m_strGameID(_T(""))
{

}

DlgMember::~DlgMember()
{
}

void DlgMember::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_JOIN_ID, m_strID);
	DDV_MaxChars(pDX, m_strID, 25);
	DDX_Text(pDX, IDC_EDIT_JOIN_PW, m_strPW);
	DDV_MaxChars(pDX, m_strPW, 25);
	DDX_Text(pDX, IDC_EDIT_JOIN_GAMEID, m_strGameID);
	DDV_MaxChars(pDX, m_strGameID, 25);
}


BEGIN_MESSAGE_MAP(DlgMember, CDialog)
	ON_BN_CLICKED(ID_JOIN_OK, &DlgMember::OnBnClickedJoinOk)
	ON_BN_CLICKED(ID_JOIN_CANCEL, &DlgMember::OnBnClickedJoinCancel)
END_MESSAGE_MAP()


// DlgMember �޽��� ó�����Դϴ�.

void DlgMember::OnBnClickedJoinOk()
{

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE );

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( pDoc->m_MyLobbyClient.RequestJoinMemeber( m_strID, m_strPW, m_strGameID ) )
	{
		m_strID		= _T("");
		m_strPW		= _T("");
		m_strGameID = _T("");

		UpdateData( FALSE );

		if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );
	}

}

void DlgMember::OnBnClickedJoinCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_strID		= _T("");
	m_strPW		= _T("");
	m_strGameID = _T("");

	UpdateData( FALSE );

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );
}
