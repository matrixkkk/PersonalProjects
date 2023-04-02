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

	//		NiNode Object ����
	NiNode* GetRootObject();

	//		Update
	void	UpdateObject( NiPoint3* vCameraPos );

	//		������ 1�� �߰�
	BOOL	AddBillBoard( const  NiPoint3& vPos, NiBillboardNode *pAddObject, int type );

	//		������ 1�� ����
	BOOL	DeleteBillBoard( NiBillboardNode *pDeleteObject );

	//		������ 1�� Ÿ�Կ� �ش��ϴ� ��ü ������ ������Ʈ ����
	BOOL	DeleteBillBoardType( int nIndex );

	BOOL	DeleteSelectedBillBoard();

	//		������ ��ŷ
	BOOL	PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *pPick, float *fDis );

	//		�ø�
	void	SetAppCulled( bool bCull );

	//		���� ���� ������ ����
	int		GetCountBillBoard();

	//		ũ�� ����
	void	SetCurrentScaling( int nType, float fScale );
	
	//		���İ��� �����Ͽ� ���̰� �� ���� �Ÿ� ����
	void	SetDistanceApplyAlpha( float fDistance );

	BOOL	IsSelectedBillBoard();
	NiBillboardNode* GetSelectedBillBoard();

	void	SetNullSelectedBillBoard();

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

	//		���� ����, �ε�
//	void	SaveData( CArchive& ar );

	void	LoadData( FILE *file, std::map< int, BillBoardObject* >& ObjectInfo );


};

class RuntimeStringCmp {

public:
	// �� ������ ���� ���
	enum cmp_mode{ normal, nocase };

private:
	// ���� �񱳸��

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