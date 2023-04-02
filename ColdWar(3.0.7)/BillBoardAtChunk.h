#ifndef BillBoardAtChunk_H
#define BillBoardAtChunk_H

#include <map>
#include <string>
#include <algorithm>
#include <NiMain.h>

#include "BillBoardObject.h"



const  float START_DISTANCE_APPLY_ALPHA = 20.0f;
const  float END_DISTANCE_APPLY_ALPHA = 10.0f;


class BillBoardAtChunk : public NiMemObject
{	
	NiNodePtr			m_spRootBillBoard;	
	float				m_fDistanceApplyAlpha;

	NiBillboardNode		*m_pCurrentBillBoard;

	std::multimap< int, NiBillboardNode*, std::greater<int> >	m_BillBoardInfo;

	int					m_nTileXIndex, m_nTileYIndex;
	
public:

	BillBoardAtChunk();


	//		Release
	BOOL	ReleaseObject();

	//		NiNode Object 리턴
	NiNode* GetRootObject();

	//		Update
	void	UpdateObject( NiPoint3* vCameraPos );

	//		빌보드 1개 추가
	BOOL	AddBillBoard( const  NiPoint3& vPos, NiBillboardNode *pAddObject, int type );

	//		빌보드 1개 제거
	BOOL	DeleteBillBoard( NiBillboardNode *pDeleteObject );

	//		빌보드 1개 타입에 해당하는 전체 빌보드 오브젝트 제거
	BOOL	DeleteBillBoardType( int nIndex );

	BOOL	DeleteSelectedBillBoard();

	//		빌보드 픽킹
	BOOL	PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *pPick, float *fDis );

	//		컬링
	void	SetAppCulled( bool bCull );

	//		현재 존의 빌보드 갯수
	int		GetCountBillBoard();

	//		크기 조정
	void	SetCurrentScaling( int nType, float fScale );
	
	//		알파값을 조정하여 보이게 될 기준 거리 셋팅
	void	SetDistanceApplyAlpha( float fDistance );

	BOOL	IsSelectedBillBoard();
	NiBillboardNode* GetSelectedBillBoard();

	void	SetNullSelectedBillBoard();

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

	//		파일 저장, 로드
//	void	SaveData( CArchive& ar );

	void	LoadData( FILE *file, std::map< int, BillBoardObject* >& ObjectInfo );


};

class RuntimeStringCmp {

public:
	// 비교 기준을 위한 상수
	enum cmp_mode{ normal, nocase };

private:
	// 실제 비교모드

	const cmp_mode mode;

	static bool nocase_compare( char c1, char c2 )
	{
		return toupper(c1) <toupper(c2);
	}

public:

	RuntimeStringCmp( cmp_mode m = normal ) : mode(m) {}

	bool operator() ( const std::string& s1, const std::string& s2 ) const {

		if( mode == normal ) {
			return s1 < s2;
		}
		else
		{
			return std::lexicographical_compare( s1.begin(), s1.end(), s2.begin(), s2.end(), nocase_compare );
		}
	}
};

class RuntimeIntCmp {
	
public:

	RuntimeIntCmp();

	bool operator() ( const int& nType1, const int& nType2 ) const {

		return nType1 < nType2;
	}
};




#endif