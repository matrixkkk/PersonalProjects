// DlgJoinerInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "DlgJoinerInfo.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"



// DlgJoinerInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgJoinerInfo, CDialog)

DlgJoinerInfo::DlgJoinerInfo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgJoinerInfo::IDD, pParent)
{

}

DlgJoinerInfo::~DlgJoinerInfo()
{
}

void DlgJoinerInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_JOINER_ID, m_ctlJoinerID);
	DDX_Control(pDX, IDC_STATIC_JOINER_RECORD, m_ctlJoinerRecord);
	DDX_Control(pDX, IDC_STATIC_JOINER_EXPERIENCE, m_ctlJoinerExp);
	DDX_Control(pDX, IDC_STATIC_JOINER_LEVEL, m_ctlJoinerLevel);
}


BEGIN_MESSAGE_MAP(DlgJoinerInfo, CDialog)
	ON_BN_CLICKED(ID_JOINER_INFO_OK, &DlgJoinerInfo::OnBnClickedJoinerInfoOk)
END_MESSAGE_MAP()


// DlgJoinerInfo 메시지 처리기입니다.

BOOL DlgJoinerInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Font.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"돋움체");

	m_ctlJoinerID.SetFont( &m_Font, TRUE );
	m_ctlJoinerID.SetWindowTextW( _T("") );

	m_ctlJoinerRecord.SetFont( &m_Font, TRUE );
	m_ctlJoinerRecord.SetWindowTextW( _T("") );

	m_ctlJoinerExp.SetFont( &m_Font, TRUE );
	m_ctlJoinerExp.SetWindowTextW( _T("") );

	m_ctlJoinerLevel.SetFont( &m_Font, TRUE );
	m_ctlJoinerLevel.SetWindowTextW( _T("") );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgJoinerInfo::OnBnClickedJoinerInfoOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ctlJoinerID.SetWindowTextW( _T("") );
	m_ctlJoinerRecord.SetWindowTextW( _T("") );
	m_ctlJoinerExp.SetWindowTextW( _T("") );
	m_ctlJoinerLevel.SetWindowTextW( _T("") );			

	if( this->IsWindowVisible() )
		this->ShowWindow( SW_HIDE );
	
}

BOOL DlgJoinerInfo::UpdateJoinerInfo( char *pInfo )
{
	
	BYTE nPosInRoom, nWin, nFail, nLevel;
	USHORT nExp;

	CString strRecord, strExp, strLevel, strID;
	
	::CopyMemory( &nPosInRoom	, pInfo						, sizeof(BYTE)		);
	::CopyMemory( &nWin			, pInfo	+ sizeof(BYTE)		, sizeof(BYTE)		);
	::CopyMemory( &nFail		, pInfo	+ sizeof(BYTE)*2	, sizeof(BYTE)		);
	::CopyMemory( &nLevel		, pInfo	+ sizeof(BYTE)*3	, sizeof(BYTE)		);
	::CopyMemory( &nExp			, pInfo	+ sizeof(BYTE)*4	, sizeof(USHORT)	);

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MyLobbyClient.GetJoinerName( strID, nPosInRoom );

	strRecord.Format( L"%d전 %d승 %d패", nWin + nFail, nWin, nFail );
	strLevel.Format( L"%d", nLevel );
	strExp.Format( L"%d", nExp );

	m_ctlJoinerID.SetWindowTextW( strID );
	m_ctlJoinerRecord.SetWindowTextW( strRecord );
	m_ctlJoinerLevel.SetWindowTextW( strLevel );
	m_ctlJoinerExp.SetWindowTextW( strExp );

	return TRUE;

}
