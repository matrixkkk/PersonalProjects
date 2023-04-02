#include "stdafx.h"
#include "MapManager.h"


MapManager::MapManager()
{
	m_vecMapList.clear();
}


MapManager::~MapManager()
{
	m_vecMapList.clear();
}

//		�� �ʱ�ȭ		
//		DB�� ���ؼ� ����� �� �ִ�.
void	MapManager::Init()
{
	MapInfo tmpMapInfo;

	tmpMapInfo.m_nCntBlocks	=	150;
	tmpMapInfo.m_strMapName	=	_T("01.�����ϰ� ���");
	tmpMapInfo.m_strBmpName	=	_T("rect.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� �����ϴ�. ��� �����Ѵ�.");	
	//tmpMapInfo.m_bitMap.LoadBitmapW( IDB_BITMAP1 );		

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	152;
	tmpMapInfo.m_strMapName	=	_T("02.�޼��� �ŵ��");
	tmpMapInfo.m_strBmpName	=	_T("check.bmp");
	tmpMapInfo.m_strMapInfo =	_T("�޼ո��� ���� �ɱ��� �谨");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP2);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	180;
	tmpMapInfo.m_strMapName	=	_T("03.���������� ������");
	tmpMapInfo.m_strBmpName	=	_T("hart.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� ��ť ���� ��ť �� �� ��ť.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP3);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	170;
	tmpMapInfo.m_strMapName	=	_T("04.����Ͽ� �󱼵�");
	tmpMapInfo.m_strBmpName	=	_T("smile.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� �������� ����.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	111;
	tmpMapInfo.m_strMapName	=	_T("05.�ɿ��� �г�");
	tmpMapInfo.m_strBmpName	=	_T("bigwindow.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� �״ٴ� �� ����.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	162;
	tmpMapInfo.m_strMapName	=	_T("06.������ ���� ������");
	tmpMapInfo.m_strBmpName	=	_T("house.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� ���� �����ϰ� �����ҋ���");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	220;
	tmpMapInfo.m_strMapName	=	_T("07.����ְ��� ���� ������");
	tmpMapInfo.m_strBmpName	=	_T("king.bmp");
	tmpMapInfo.m_strMapInfo =	_T("������ ���� �� �ĵ����� ����");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	211;
	tmpMapInfo.m_strMapName	=	_T("08.���󿡸��� �ڱ����");
	tmpMapInfo.m_strBmpName	=	_T("sawlike.bmp");
	tmpMapInfo.m_strMapInfo =	_T("������ ���ϸ� �˸� ��");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	197;
	tmpMapInfo.m_strMapName	=	_T("09.���ֽ� �ΰ����");
	tmpMapInfo.m_strBmpName	=	_T("spiral.bmp");
	tmpMapInfo.m_strMapInfo =	_T("�ͻͻ�~ �Ƿηη� ��! ��!");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	160;
	tmpMapInfo.m_strMapName	=	_T("10.������ �ñ�â");
	tmpMapInfo.m_strBmpName	=	_T("stairs.bmp");
	tmpMapInfo.m_strMapInfo =	_T("������ �÷��ⱹ");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	187;
	tmpMapInfo.m_strMapName	=	_T("11.�ܹ����� ���� ĳ���");
	tmpMapInfo.m_strBmpName	=	_T("window.bmp");
	tmpMapInfo.m_strMapInfo =	_T("ĳ��� ���� ���� ����");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

}

//		�� �߰�
void	MapManager::AddMap( MapInfo&	mapInfo )
{

	mapInfo.m_nMapNum = static_cast< BYTE >( m_vecMapList.size() );

	m_vecMapList.push_back( mapInfo );
}

//		�� ã��
BOOL	MapManager::FindMap( MapInfo& mapInfo, BYTE nMapIndex )
{

	mapInfo.m_nMapNum = nMapIndex;
	std::list< MapInfo >::iterator iterMap;
	
	if( std::binary_search( m_vecMapList.begin(), m_vecMapList.end(), mapInfo ) )
	{
		iterMap = std::lower_bound( m_vecMapList.begin(), m_vecMapList.end(), mapInfo );

	//	::CopyMemory( &mapInfo, &(*iterMap), sizeof(MapInfo) );

		mapInfo = *iterMap;

		return TRUE;
	}

	return FALSE;
}

//		�� ã��
BOOL	MapManager::FindMap( MapInfo& mapInfo, CString& strMapName )
{

	std::list< MapInfo >::iterator iterMap;

	for( iterMap = m_vecMapList.begin(); iterMap != m_vecMapList.end(); ++iterMap )
	{
		if( strMapName == iterMap->m_strMapName )
		{
			//::CopyMemory( &mapInfo, &(*iterMap), sizeof(MapInfo) );
			mapInfo = *iterMap;

			return TRUE;
		}
	}	

	return FALSE;
}

//		�� ����
BOOL	MapManager::DeleteMap	( BYTE nMapIndex )
{
	MapInfo mapInfo;

	mapInfo.m_nMapNum = nMapIndex;

	std::list< MapInfo >::iterator iterMap;
	
	if( std::binary_search( m_vecMapList.begin(), m_vecMapList.end(), mapInfo ) )
	{
		iterMap = std::lower_bound( m_vecMapList.begin(), m_vecMapList.end(), mapInfo );

		m_vecMapList.erase( iterMap );

		return TRUE;
	}

	return FALSE;
}


