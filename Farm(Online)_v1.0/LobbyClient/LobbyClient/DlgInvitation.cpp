// DlgInvitation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgInvitation.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// DlgInvitation 대화 상자입니다.

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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"돋움체");

	 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// DlgInvitation 메시지 처리기입니다.


void DlgInvitation::SetRoomInfo( wchar_t *pMsg, BYTE nRoomNo )
{
	CString strMsg = pMsg ;
	strMsg += _T(" 에서 같이 기도하기를 원합니다.");
	m_cltInviteMsg.SetWindowTextW(_T(""));
	m_cltInviteMsg.SetWindowTextW( strMsg );

	m_nRoomNo = nRoomNo;
}


void DlgInvitation::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	else
		this->ShowWindow( SW_SHOW );

	m_nRoomNo = 0;

}
