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



// 데칼 수명 시간
const float g_fDecalLifeTime = 10.0f;

// 탄흔 크기
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

	int							m_nCntExplosionLimitRect;	// 픽킹 지점 Rect로 부터 상 하 좌 우 검사 렉트 갯수 : 수치가 클수록 폭파 흔적 크기 커짐

	//	데칼 오브젝트
	std::vector< DecalObject >	m_listDecalObjects;

	// 타일별 데칼 관리
	std::list< DecalAtChunk* >		m_listDecalChunks;

	std::vector< DecalTriIndex >	m_vecDecalTriIndex;

	std::vector< int >				m_vecVtxIndex;

	NiTexturingPropertyPtr		m_spBulletTexture;

	BulletDecalFactory			*m_pBulletDecalFactory;		// 지형 탄흔, 혈흔

	ExplosionDecalFactory		*m_pExplosionDecalFactory;	// 지형 폭파 흔적

	StaticObjectDecalFactory	*m_pStaticObjectDecalFactory;// 스태틱 오브젝트용 데칼 생성


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

	//		데칼 어브젝트 추가
	BOOL	AddObject( const NiFixedString& fileName, int nType );

	//		데칼 어브젝트 제거
	BOOL	DeleteObject( const NiFixedString& fileName );

	//		데칼 어브젝트 셋팅
	BOOL	Intialize();

	//		청크에 데칼을 어태치
	//NiTriShape*	AddDecalAtChunk(  DecalAtChunk* pDecal, int nType );

	//		데칼 청크 생성
	DecalAtChunk*	GenerateDecalAtChunk();

	//		데칼 적용할 지형 포인터 셋팅
	void	SetTerrain( Terrain *pTerrain );

	//		데칼을 Scene에 적용한다.
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
