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

//		맵 초기화		
//		DB를 통해서 변경될 수 있다.
void	MapManager::Init()
{
	MapInfo tmpMapInfo;

	tmpMapInfo.m_nCntBlocks	=	100;
	tmpMapInfo.m_strMapName	=	_T("Lost Temple");
	tmpMapInfo.m_strBmpName	=	_T("lost_temple.bmp");
	tmpMapInfo.m_strMapInfo =	_T("태초 신이 태어난 곳에서 신비한 체험을 할 수 있다.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	150;
	tmpMapInfo.m_strMapName	=	_T("The Hunter");
	tmpMapInfo.m_strBmpName	=	_T("hunter.bmp");
	tmpMapInfo.m_strMapInfo =	_T("아름다운 여신은 이곳에서 꿈을 키웠다.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	200;
	tmpMapInfo.m_strMapName	=	_T("Diablo");
	tmpMapInfo.m_strBmpName	=	_T("Diablo.bmp");
	tmpMapInfo.m_strMapInfo =	_T("폭풍의 신이 여기서 잠들다.");

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	170;
	tmpMapInfo.m_strMapName	=	_T("장기하와 얼굴들");
	tmpMapInfo.m_strBmpName	=	_T("face.bmp");
	tmpMapInfo.m_strMapInfo =	_T("노래의 여신은 영혼을 살리고 죽인다.");

	AddMap( tmpMapInfo );

}

//		맵 추가
void	MapManager::AddMap( MapInfo&	mapInfo )
{

	mapInfo.m_nMapNum = static_cast< BYTE >( m_vecMapList.size() );

	m_vecMapList.push_back( mapInfo );
}

//		맵 찾기
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

//		맵 찾기
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

//		맵 삭제
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


