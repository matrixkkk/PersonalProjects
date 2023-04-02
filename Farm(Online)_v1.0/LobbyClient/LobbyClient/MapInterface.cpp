// MapInterface.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LobbyClient.h"
#include "MapInterface.h"
#include "MainFrm.h"
#include "LobbyClientDoc.h"


// MapInterface

IMPLEMENT_DYNCREATE(MapInterface, CFormView)

MapInterface::MapInterface()
	: CFormView(MapInterface::IDD)
{

}

MapInterface::~MapInterface()
{
}

void MapInterface::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_COMBO1, m_ctlMapNameList);
	DDX_Control(pDX, IDC_STATIC_MAP_NAME, m_ctlMapName);
	DDX_Control(pDX, IDC_STATIC_MAP_COUNT_BLOCKS, m_ctlCntBlocks);
	DDX_Control(pDX, IDC_STATIC_MAP_INFO, m_ctlMapInfo);
//	DDX_Control(pDX, IDC_STATIC_MAP_BITMAP, m_ctlBitMapPreview);
}

BEGIN_MESSAGE_MAP(MapInterface, CFormView)
	ON_CBN_SELCHANGE(IDC_COMBO1, &MapInterface::OnCbnSelchangeCombo1)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// MapInterface �����Դϴ�.

#ifdef _DEBUG
void MapInterface::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void MapInterface::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// MapInterface �޽��� ó�����Դϴ�.

void MapInterface::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CSize scrollSize;
	scrollSize.cx = 0;
	scrollSize.cy = 0;
	SetScrollSizes( MM_TEXT, scrollSize );

//	m_MapManager.Init();
	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	CString	strCntBlock, strMapName, strMapInfo;
	MapInfo	tmpMap;
	::ZeroMemory( m_bitMapList, sizeof( m_bitMapList ) );
	m_pCurrentBitmap = NULL;
	
	m_bitMapList[ 0 ].LoadBitmapW( IDB_BITMAP1 );
	m_bitMapList[ 1 ].LoadBitmapW( IDB_BITMAP2 );
	m_bitMapList[ 2 ].LoadBitmapW( IDB_BITMAP3 );
	m_bitMapList[ 3 ].LoadBitmapW( IDB_BITMAP4 );
	m_bitMapList[ 4 ].LoadBitmapW( IDB_BITMAP5 );
	m_bitMapList[ 5 ].LoadBitmapW( IDB_BITMAP6 );
	m_bitMapList[ 6 ].LoadBitmapW( IDB_BITMAP7 );
	m_bitMapList[ 7 ].LoadBitmapW( IDB_BITMAP8 );
	m_bitMapList[ 8 ].LoadBitmapW( IDB_BITMAP9 );
	m_bitMapList[ 9 ].LoadBitmapW( IDB_BITMAP10);
	m_bitMapList[ 10].LoadBitmapW( IDB_BITMAP11);

	m_pCurrentBitmap = &m_bitMapList[ 0 ];

	for( BYTE iMap = 0; iMap < pDoc->m_MapManager.GetMapCount() ; iMap++ )
	{		
		pDoc->m_MapManager.FindMap( tmpMap, iMap );
		
		m_ctlMapNameList.AddString( tmpMap.m_strMapName );		
//		m_vecBitMaps.push_back( tmpBitMap[ iMap ] );
	}

	m_ctlMapNameList.SetCurSel( 0 );

	pDoc->m_MapManager.FindMap( tmpMap, 0 );

	// �� �̸� ����
	strMapName = _T("�̸� : ");
	strMapName += tmpMap.m_strMapName;
	m_ctlMapName.SetWindowTextW( strMapName );

	// ��� ���� ����
	strCntBlock.Format( L"��� : %d ��", tmpMap.m_nCntBlocks );
	m_ctlCntBlocks.SetWindowTextW( strCntBlock );

	// �� ���� ����
	strMapInfo = _T("���� : ");
	strMapInfo += tmpMap.m_strMapInfo;
	m_ctlMapInfo.SetWindowTextW( strMapInfo );

	if( !pDoc->m_MyLobbyClient.IsCaptain() )
	{
		m_ctlMapNameList.ShowWindow( SW_HIDE );
//		m_ctlMapName.ShowWindow( SW_HIDE );
//		m_ctlCntBlocks.ShowWindow( SW_HIDE );
//		m_ctlMapInfo.ShowWindow( SW_HIDE );
	}

//	UpdateData(FALSE);
}

void MapInterface::SetRoomCaptain( BOOL bCaptain )
{
	if( bCaptain )
	{
		m_ctlMapNameList.ShowWindow( SW_SHOW );		
	}
	else
	{
		m_ctlMapNameList.ShowWindow( SW_HIDE );		
	}
}


void MapInterface::UpdateMapInfo( BYTE nMapIndex )
{
	MapInfo tmpMap;
	CString	strCntBlock, strMapName, strMapInfo;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MapManager.FindMap( tmpMap, nMapIndex );

	// �� �̸� ����
	strMapName = _T("�̸� : ");
	strMapName += tmpMap.m_strMapName;
	m_ctlMapName.SetWindowTextW( strMapName );

	// ��� ���� ����
	strCntBlock.Format( L"��� : %d ��", tmpMap.m_nCntBlocks );
	m_ctlCntBlocks.SetWindowTextW( strCntBlock );

	// �� ���� ����
	strMapInfo = _T("���� : ");
	strMapInfo += tmpMap.m_strMapInfo;
	m_ctlMapInfo.SetWindowTextW( strMapInfo );

	// �޺� �ڽ� ����
	m_ctlMapNameList.SetCurSel( nMapIndex );

	m_pCurrentBitmap = &m_bitMapList[ nMapIndex ];	
	Invalidate();

}


void MapInterface::OnCbnSelchangeCombo1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	MapInfo tmpMap;
	CString	strCntBlock, strMapName, strMapInfo;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MapManager.FindMap( tmpMap, m_ctlMapNameList.GetCurSel() );

	// �� �̸� ����
	strMapName = _T("�̸� : ");
	strMapName += tmpMap.m_strMapName;
	m_ctlMapName.SetWindowTextW( strMapName );

	// ��� ���� ����
	strCntBlock.Format( L"��� : %d ��", tmpMap.m_nCntBlocks );
	m_ctlCntBlocks.SetWindowTextW( strCntBlock );

	// �� ���� ����
	strMapInfo = _T("���� : ");
	strMapInfo += tmpMap.m_strMapInfo;
	m_ctlMapInfo.SetWindowTextW( strMapInfo );

	m_pCurrentBitmap = &m_bitMapList[ m_ctlMapNameList.GetCurSel() ];	
	Invalidate();

	if( pDoc->m_MyLobbyClient.IsCaptain() )
		pDoc->m_MyLobbyClient.RequestMapChange( static_cast< BYTE >( m_ctlMapNameList.GetCurSel() ) );
	
}

void MapInterface::GetMapInfo( MapInfo &mapInfo )
{
	
	MapInfo tmpMap;

	CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_MapManager.FindMap( tmpMap, m_ctlMapNameList.GetCurSel() );

	::CopyMemory( &mapInfo, &tmpMap, sizeof( MapInfo ) );	

}

void MapInterface::OnPaint()
{
	
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CFormView::OnPaint()��(��) ȣ������ ���ʽÿ�.
	

	// �ؽ��� ����Ʈ���� ���õ� BitMap �ٽ� �׸���
	if( m_ctlMapNameList.GetCurSel() >= 0 )
	{
		CPaintDC dc(this); // device context for painting

		CLobbyClientDoc *pDoc = (CLobbyClientDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
		
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap *pOldBit = (CBitmap*)MemDC.SelectObject(m_pCurrentBitmap);

		BITMAP Info;
		m_pCurrentBitmap->GetBitmap(&Info);

		dc.BitBlt(45, 45, PREVIEW_SIZE,PREVIEW_SIZE, &MemDC,0,0,SRCCOPY);

		MemDC.SelectObject(&pOldBit);
		
	}	
}
