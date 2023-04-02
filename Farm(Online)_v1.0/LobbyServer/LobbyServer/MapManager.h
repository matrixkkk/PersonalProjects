#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <algorithm>
#include <functional>
#include <vector>
#include <string>


const	CString		MEDIA_PATH( _T("Data/Map/") );

struct MapInfo{

	BYTE		m_nMapNum;			// �� �ε���
	USHORT		m_nCntBlocks;		// Block ����
	CString		m_strMapName;		// �� �̸�
	CString		m_strBmpName;		// Bitmap �̸�
	CString		m_strMapInfo;		// �� ����
	

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

	//		�� �ʱ�ȭ
	void	Init		( void );

	//		�� �߰�
	void	AddMap		( MapInfo&	mapInfo );

	//		�� ����
	BOOL	DeleteMap	( BYTE nMapIndex	);

	//		�� ã��
	BOOL	FindMap		( MapInfo&	mapInfo, BYTE nMapIndex			);
	BOOL	FindMap		( MapInfo&	mapInfo, CString&	strMapName	);	

	//		�� ����
	int		GetMapCount	( void ){ return static_cast< int >( m_vecMapList.size() ); }

};

#endif