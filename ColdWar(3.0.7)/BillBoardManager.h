#ifndef BILLBOARDMANAGER_H
#define BILLBOARDMANAGER_H


#include <NiMain.h>

#include "GBObject.h"
#include "BillBoardAtChunk.h"
#include "BillBoardObject.h"
#include "Terrain.h"


#include <list>
#include <fstream>

class Terrain;



struct BillBoardInfo
{
	int					m_nIndex;
	float				m_fScaling;
	BillBoardObject*	m_pBillBoardObject;

	static int			m_nCountBillBoardOBject;

	BillBoardInfo()
	{
		m_nIndex = -1;
		m_fScaling = 1.0f;
		m_pBillBoardObject = NULL;
	}
};
	

class BillBoardManager : public GBObject
{

	NiNodePtr						m_spBillBoardRoot;

	std::list< BillBoardInfo >		m_listBillBoardObject;
	std::list< BillBoardAtChunk* >	m_listBillBoardChunk;

	BillBoardInfo					*m_pCurrnetBillBoardInfo;
	BillBoardAtChunk				*m_pSelectedBillBoardChunk;
	NiBillboardNode					*m_pSelectedBillBoardNode;

	static BillBoardManager			*m_pBillBoardManager;

	float							m_fAlphaDistance;
		

	NiCamera						*m_pCamera;
	NiPick							*m_pPick;

public:

	static	BillBoardManager* CreateManager();
	static	BillBoardManager* GetManager();
	static  BOOL	IsManager();
	static	void ReleaseManager();

	NiNode* GetObject();
	BOOL	UpdateObject( float fElapsedTime  );
	BOOL	ReleaseObject();

	void	CreatePick();

	void	SetCamera( NiCamera *pCamera );

	//		������ ������Ʈ �߰�
	BOOL	AddBillBoard( NiFixedString& fileName );

	//		������ ������Ʈ ����
	BOOL	DeleteBillBoard( const NiFixedString& fileName );

	BOOL	DeleteSelectedBillBoard();

	BOOL	PickBillBoard( const NiPoint3& pos, const NiPoint3& dir );

	//		������ ûũ ����
	BillBoardAtChunk* GenerateBillBoardAtChunk();

	//		������ ������Ʈ ���� ����
	BOOL	SetCurrentBillBoardObject( const NiFixedString& fileName );

	//		���� ���õǾ��� ������ ũ�� ����
	void	SetCurrentScaling( float fScale );

	float	GetCurrentScaling();

	//		���İ� ���� �Ÿ� ���� ����
	void	SetDistanceApplyAlpha( float fAlpha );

	void	SetSlectBillBoardChunk( BillBoardAtChunk* pBill );
	BillBoardAtChunk*	GetSelectedBillBoardChunk();

	BOOL	IsSelectedBillBoardNode();
	void	SetNullBillBoardNode();
	void	SetSlectBillBoardNode( NiBillboardNode *pNode );
	NiBillboardNode* GetSelectedBillBoardNode();


	BOOL	IsSelectedBillBoardChunk();
	void	SetNullBillBoardChunk();

	//		���� ���õǾ��� ������ ������Ʈ
	BillBoardObject*	GetCurrentBillBoardObject();
	int					GetCurrentBillBoardType();

	BillBoardObject*	GetBillBoardObject( int nType );
/*		
	void	SaveHeader( CArchive& ar );
	void	SaveData( CArchive& ar );
	void	SaveNameData( std::ofstream& fwrite );
*/
	void	LoadData( FILE *file, std::map<int, BillBoardObject* >& ObjectInfo, Terrain *pTerrain );
	void	LoadNameData( FILE *file,  std::map<int, BillBoardObject*>& ObjectInfo, std::list<NiFixedString>& listFileName );

protected:

	BillBoardManager();
	~BillBoardManager();

};


#endif