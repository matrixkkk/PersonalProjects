// DlgCreateRoom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgCreateRoom.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"



// DlgCreateRoom 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgCreateRoom, CDialog)

DlgCreateRoom::DlgCreateRoom(CWnd* pParent /*=NULL*/)
	: CDialog(DlgCreateRoom::IDD, pParent)
	, m_strRoomName(_T(""))
{
}

DlgCreateRoom::~DlgCreateRoom()
{
}

void DlgCreateRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text( pDX, IDC_EDIT_ROOMNAME, m_strRoomName );
	DDX_Control(pDX, IDC_COMBO_MAX_PLAYER, m_CtlMaxPlayer);
}


BEGIN_MESSAGE_MAP(DlgCreateRoom, CDialog)
	ON_BN_CLICKED(ID_ROOM_MAKE_OK, &DlgCreateRoom::OnBnClickedRoomMakeOk)
	ON_BN_CLICKED(ID_ROOM_MAKE_CANCEL, &DlgCreateRoom::OnBnClickedRoomMakeCancel)
END_MESSAGE_MAP()




// DlgCreateRoom 메시지 처리기입니다.
BOOL DlgCreateRoom::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_strRoomName = _T("역시 동물 농장이 최고야!");

	CString strMaxNum[] = {_T("2"), _T("3"), _T("4"), _T("5"), _T("6") };

	for( int i=0 ; i < 5 ; i++ )
		m_CtlMaxPlayer.AddString( strMaxNum[ i ] );
	
	m_CtlMaxPlayer.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;  
}


void DlgCreateRoom::OnBnClickedRoomMakeOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
			
	BYTE nMaxPlayer = static_cast< BYTE >( m_CtlMaxPlayer.GetCurSel() ) + 2;
		
	if( pDoc->m_MyLobbyClient.RequestCreateRoom( m_strRoomName, nMaxPlayer ) )
	{
		m_strRoomName = _T("역시 동물 농장이 최고야!");
		m_CtlMaxPlayer.SetCurSel( 0 );

		UpdateData( FALSE );

		if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );
	}
}

void DlgCreateRoom::OnBnClickedRoomMakeCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_strRoomName = _T("역시 동물 농장이 최고야!");
	m_CtlMaxPlayer.SetCurSel( 0 );

	if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );
}
