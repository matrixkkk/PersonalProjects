// DlgMember.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "LobbyClientDoc.h"
#include "MainFrm.h"
#include "DlgMember.h"


// DlgMember 대화 상자입니다.

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


// DlgMember 메시지 처리기입니다.

void DlgMember::OnBnClickedJoinOk()
{

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_strID		= _T("");
	m_strPW		= _T("");
	m_strGameID = _T("");

	UpdateData( FALSE );

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );
}
