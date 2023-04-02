// DlgCreateRoom.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgCreateRoom.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"



// DlgCreateRoom ��ȭ �����Դϴ�.

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




// DlgCreateRoom �޽��� ó�����Դϴ�.
BOOL DlgCreateRoom::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_strRoomName = _T("���� ���� ������ �ְ��!");

	CString strMaxNum[] = {_T("2"), _T("3"), _T("4"), _T("5"), _T("6") };

	for( int i=0 ; i < 5 ; i++ )
		m_CtlMaxPlayer.AddString( strMaxNum[ i ] );
	
	m_CtlMaxPlayer.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;  
}


void DlgCreateRoom::OnBnClickedRoomMakeOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData( TRUE );

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
			
	BYTE nMaxPlayer = static_cast< BYTE >( m_CtlMaxPlayer.GetCurSel() ) + 2;
		
	if( pDoc->m_MyLobbyClient.RequestCreateRoom( m_strRoomName, nMaxPlayer ) )
	{
		m_strRoomName = _T("���� ���� ������ �ְ��!");
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_strRoomName = _T("���� ���� ������ �ְ��!");
	m_CtlMaxPlayer.SetCurSel( 0 );

	if( this->IsWindowVisible() )
			this->ShowWindow( SW_HIDE );
		else
			this->ShowWindow( SW_SHOW );
}
