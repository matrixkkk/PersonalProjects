#ifndef DECALSYSTEM_H
#define DECALSYSTEM_H


#include "GBObject.h"
#include "DecalObject.h"
#include "DecalAtChunk.h"
#include "Terrain.h"
#include "BulletDecalFactory.h"
#include "ExplosionDecalFactory.h"
#include "StaticObjectDecalFactory.h"

#include <list>
#include <vector>
#include <string>



// ��Į ���� �ð�
const float g_fDecalLifeTime = 10.0f;

// ź�� ũ��
const float g_fBulletMarkSize = 0.15f;


class DecalManager : public GBObject
{	
public:

	enum{ BULLET_MARK, BOOLD_MARK, BOMB_MARK, DECAL_COUNT };

	enum{ OUTDOOR, STATIC_OBJECT, INDOOR };


private:

	NiNodePtr					m_spDecalRoot;

	Terrain						*m_pTerrain;

	static DecalManager			*m_pDecaldManager;	

	float						m_fLastUpdateTime;

	int							m_nCntExplosionLimitRect;	// ��ŷ ���� Rect�� ���� �� �� �� �� �˻� ��Ʈ ���� : ��ġ�� Ŭ���� ���� ���� ũ�� Ŀ��

	//	��Į ������Ʈ
	std::vector< DecalObject >	m_listDecalObjects;

	// Ÿ�Ϻ� ��Į ����
	std::list< DecalAtChunk* >		m_listDecalChunks;

	std::vector< DecalTriIndex >	m_vecDecalTriIndex;

	std::vector< int >				m_vecVtxIndex;

	NiTexturingPropertyPtr		m_spBulletTexture;

	BulletDecalFactory			*m_pBulletDecalFactory;		// ���� ź��, ����

	ExplosionDecalFactory		*m_pExplosionDecalFactory;	// ���� ���� ����

	StaticObjectDecalFactory	*m_pStaticObjectDecalFactory;// ����ƽ ������Ʈ�� ��Į ����


public:

	NiNode* GetObject();
	BOOL	UpdateObject( float fElapsedTime  );
	BOOL	ReleaseObject();
	BOOL	RenderObject( NiRenderer *pkRenderer );

	static	DecalManager*	CreateManager();
	static	DecalManager*	GetManager();
	static  BOOL			IsManager();
	static	void			ReleaseManager();
	
	void	SetWireFrame( bool bWire );

	//		��Į �����Ʈ �߰�
	BOOL	AddObject( const NiFixedString& fileName, int nType );

	//		��Į �����Ʈ ����
	BOOL	DeleteObject( const NiFixedString& fileName );

	//		��Į �����Ʈ ����
	BOOL	Intialize();

	//		ûũ�� ��Į�� ����ġ
	//NiTriShape*	AddDecalAtChunk(  DecalAtChunk* pDecal, int nType );

	//		��Į ûũ ����
	DecalAtChunk*	GenerateDecalAtChunk();

	//		��Į ������ ���� ������ ����
	void	SetTerrain( Terrain *pTerrain );

	//		��Į�� Scene�� �����Ѵ�.
	BOOL	GenerateDecal( const NiPoint3* vCenter, const NiPoint3* vNormal, const  NiPoint3* vTangent,
		const int *nHeightMapIndex, int nDecalType, int nDecalPlace, DecalAtChunk *pDecalChunk );	

	BOOL	GenerateBooldAtTerrain( const NiPoint3* vCenter, const NiPoint3* vNormal, const  NiPoint3* vTangent, const int *nHeightMapIndex, DecalAtChunk *pDecalChunk );

	BOOL	GenerateExplosionAtTerrain( const int *nHeightMapIndex, DecalAtChunk *pDecalChunk );

	BOOL	GenerateDecalForStaticobject( const NiPoint3 *vPos, const NiPoint3 *vNormal, DecalAtChunk *pDecalChunk, int &nDecalType );


	

protected:

	DecalManager();
	~DecalManager();

};



#endif
