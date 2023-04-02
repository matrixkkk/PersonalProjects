#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <algorithm>
#include <functional>
#include <vector>
#include <string>


const	CString		MEDIA_PATH( _T("Data/Map/") );

struct MapInfo{

	BYTE		m_nMapNum;			// ∏  ¿Œµ¶Ω∫
	USHORT		m_nCntBlocks;		// Block ∞πºˆ
	CString		m_strMapName;		// ∏  ¿Ã∏ß
	CString		m_strBmpName;		// Bitmap ¿Ã∏ß
	CString		m_strMapInfo;		// ∏  º≥∏Ì
	

	MapInfo()
	{
		m_nMapNum		= 0;
		m_nCntBlocks	= 0;
	}

	BOOL operator == ( const MapInfo& _mapInfo ) const { return this->m_nMapNum == _mapInfo.m_nMapNum; }
	BOOL operator >  ( const MapInfo& _mapInfo ) const { return this->m_nMapNum >  _mapInfo.m_nMapNum; }
	BOOL operator <  ( const MapInfo& _mapInfo ) const { return this->m_nMapNum <  _mapInfo.m_nMapNum; }
	BOOL operator <= ( const MapInfo& _mapInfo ) const { return this->m_nMapNum <= _mapInfo.m_nMapNum; }
	BOOL operator >= ( const MapInfo& _mapInfo ) const { return this->m_nMapNum >= _mapInfo.m_nMapNum; }
};


class MapManager{


	std::vector< MapInfo >		m_vecMapList;

public:


	MapManager();
	~MapManager();

	//		∏  √ ±‚»≠
	void	Init		( void );

	//		∏  √ﬂ∞°
	void	AddMap		( MapInfo&	mapInfo );

	//		∏  ªË¡¶
	BOOL	DeleteMap	( BYTE nMapIndex	);

	//		∏  √£±‚
	BOOL	FindMap		( MapInfo&	mapInfo, BYTE nMapIndex			);
	BOOL	FindMap		( MapInfo&	mapInfo, CString&	strMapName	);	

	//		∏  ∞πºˆ
	int		GetMapCount	( void ){ return static_cast< int >( m_vecMapList.size() ); }

};

#endif