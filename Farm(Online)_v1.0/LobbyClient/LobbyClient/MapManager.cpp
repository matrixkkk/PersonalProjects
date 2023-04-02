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

	tmpMapInfo.m_nCntBlocks	=	150;
	tmpMapInfo.m_strMapName	=	_T("01.적절하게 살기");
	tmpMapInfo.m_strBmpName	=	_T("rect.bmp");
	tmpMapInfo.m_strMapInfo =	_T("나는 적절하다. 고로 존재한다.");	
	//tmpMapInfo.m_bitMap.LoadBitmapW( IDB_BITMAP1 );		

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	152;
	tmpMapInfo.m_strMapName	=	_T("02.왼손은 거들뿐");
	tmpMapInfo.m_strBmpName	=	_T("check.bmp");
	tmpMapInfo.m_strMapInfo =	_T("왼손모가지 몽땅 걸기의 쾌감");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP2);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	180;
	tmpMapInfo.m_strMapName	=	_T("03.디제이쿠의 디제잉");
	tmpMapInfo.m_strBmpName	=	_T("hart.bmp");
	tmpMapInfo.m_strMapInfo =	_T("아임 매큐 아임 메큐 왓 더 뻑큐.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP3);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	170;
	tmpMapInfo.m_strMapName	=	_T("04.장기하와 얼굴들");
	tmpMapInfo.m_strBmpName	=	_T("smile.bmp");
	tmpMapInfo.m_strMapInfo =	_T("달이 차오른다 죽자.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	111;
	tmpMapInfo.m_strMapName	=	_T("05.심영의 분노");
	tmpMapInfo.m_strBmpName	=	_T("bigwindow.bmp");
	tmpMapInfo.m_strMapInfo =	_T("내가 죽다니 이 무슨.");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	162;
	tmpMapInfo.m_strMapName	=	_T("06.잔진의 예능 정착기");
	tmpMapInfo.m_strBmpName	=	_T("house.bmp");
	tmpMapInfo.m_strMapInfo =	_T("이제 나도 안정하게 정착할떄지");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	220;
	tmpMapInfo.m_strMapName	=	_T("07.사상최강의 공익 김종국");
	tmpMapInfo.m_strBmpName	=	_T("king.bmp");
	tmpMapInfo.m_strMapInfo =	_T("북한이 절대 못 쳐들어오는 이유");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	211;
	tmpMapInfo.m_strMapName	=	_T("08.도라에몽의 자기방어술");
	tmpMapInfo.m_strBmpName	=	_T("sawlike.bmp");
	tmpMapInfo.m_strMapInfo =	_T("자진모리 패턴만 알면 끝");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	197;
	tmpMapInfo.m_strMapName	=	_T("09.우주신 인간들아");
	tmpMapInfo.m_strBmpName	=	_T("spiral.bmp");
	tmpMapInfo.m_strMapInfo =	_T("뽕뽕뽕~ 또로로로 똥! 똥!");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	160;
	tmpMapInfo.m_strMapName	=	_T("10.현실은 시궁창");
	tmpMapInfo.m_strBmpName	=	_T("stairs.bmp");
	tmpMapInfo.m_strMapInfo =	_T("현실은 시레기국");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

	AddMap( tmpMapInfo );

	tmpMapInfo.m_nCntBlocks	=	187;
	tmpMapInfo.m_strMapName	=	_T("11.단무지의 털은 캐비어");
	tmpMapInfo.m_strBmpName	=	_T("window.bmp");
	tmpMapInfo.m_strMapInfo =	_T("캐비어 뜯어라 오늘 죽자");
//	tmpMapInfo.m_bitMap.LoadBitmap(IDB_BITMAP4);

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

//		맵 찾기
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

//		맵 삭제
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


