// DlgInvitation.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgInvitation.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// DlgInvitation ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DlgInvitation, CDialog)

DlgInvitation::DlgInvitation(CWnd* pParent /*=NULL*/)
	: CDialog(DlgInvitation::IDD, pParent)
{

	m_nRoomNo = 0;

}

DlgInvitation::~DlgInvitation()
{
}

void DlgInvitation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INVITATION, m_cltInviteMsg);
}


BEGIN_MESSAGE_MAP(DlgInvitation, CDialog)
	ON_BN_CLICKED(INVITATION_OK, &DlgInvitation::OnBnClickedOk)
	ON_BN_CLICKED(INVITATION_CANCEL, &DlgInvitation::OnBnClickedCancel)
END_MESSAGE_MAP()



BOOL DlgInvitation::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"����ü");

	 

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

// DlgInvitation �޽��� ó�����Դϴ�.


void DlgInvitation::SetRoomInfo( wchar_t *pMsg, BYTE nRoomNo )
{
	CString strMsg = pMsg ;
	strMsg += _T(" ���� ���� �⵵�ϱ⸦ ���մϴ�.");
	m_cltInviteMsg.SetWindowTextW(_T(""));
	m_cltInviteMsg.SetWindowTextW( strMsg );

	m_nRoomNo = nRoomNo;
}


void DlgInvitation::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if( NULL != pDoc && m_nRoomNo >= 0 )
	{
		if( pDoc->m_MyLobbyClient.RequestJoinRoom( m_nRoomNo ) )
		{
			m_cltInviteMsg.SetWindowTextW( _T("") );

			if( this->IsWindowVisible() )
				this->ShowWindow( SW_HIDE );
			else
				this->ShowWindow( SW_SHOW );
		}		
	}

	m_nRoomNo = 0;
}

void DlgInvitation::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );

	m_nRoomNo = 0;

}
