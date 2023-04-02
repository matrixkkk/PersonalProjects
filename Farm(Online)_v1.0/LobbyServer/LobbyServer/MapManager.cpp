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

	tmpMapInfo.m_nCntBlocks	=	100;
	tmpMapInfo.m_strMapName	=	_T("Lost Temple");
	tmpMapInfo.m_strBmpName	=	_T("lost_temple.bmp");
	tmpMapInfo.m_strMapInfo =	_T("���� ���� �¾ ������ �ź��� ü���� �� �� �ִ�.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	150;
	tmpMapInfo.m_strMapName	=	_T("The Hunter");
	tmpMapInfo.m_strBmpName	=	_T("hunter.bmp");
	tmpMapInfo.m_strMapInfo =	_T("�Ƹ��ٿ� ������ �̰����� ���� Ű����.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	200;
	tmpMapInfo.m_strMapName	=	_T("Diablo");
	tmpMapInfo.m_strBmpName	=	_T("Diablo.bmp");
	tmpMapInfo.m_strMapInfo =	_T("��ǳ�� ���� ���⼭ ����.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	170;
	tmpMapInfo.m_strMapName	=	_T("����Ͽ� �󱼵�");
	tmpMapInfo.m_strBmpName	=	_T("face.bmp");
	tmpMapInfo.m_strMapInfo =	_T("�뷡�� ������ ��ȥ�� �츮�� ���δ�.");

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

	std::vector< MapInfo >::iterator iterMap;
	
	if( std::binary_search( m_vecMapList.begin(), m_vecMapList.end(), mapInfo ) )
	{
		iterMap = std::lower_bound( m_vecMapList.begin(), m_vecMapList.end(), mapInfo );

		::CopyMemory( &mapInfo, &(*iterMap), sizeof(MapInfo) );

		return TRUE;
	}

	return FALSE;
}

//		�� ã��
BOOL	MapManager::FindMap( MapInfo& mapInfo, CString& strMapName )
{

	std::vector< MapInfo >::iterator iterMap;

	for( iterMap = m_vecMapList.begin(); iterMap != m_vecMapList.end(); ++iterMap )
	{
		if( strMapName == iterMap->m_strMapName )
		{
			::CopyMemory( &mapInfo, &(*iterMap), sizeof(MapInfo) );

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

	std::vector< MapInfo >::iterator iterMap;
	
	if( std::binary_search( m_vecMapList.begin(), m_vecMapList.end(), mapInfo ) )
	{
		iterMap = std::lower_bound( m_vecMapList.begin(), m_vecMapList.end(), mapInfo );

		m_vecMapList.erase( iterMap );

		return TRUE;
	}

	return FALSE;
}


