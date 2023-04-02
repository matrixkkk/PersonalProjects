#ifndef TILE_H
#define TILE_H

#include <bitset>
#include "NifMesh.h"
#include "BoundBox.h"
#include "NiTriShapeEx.h"
#include "BillBoardAtChunk.h"
#include "StaticObjectAtChunk.h"
#include "DecalAtChunk.h"
#include "CollisionManager.h"
#include "CharacterManager.h"


#define MULTITEXTURESET		2


struct TileVertex : public NiMemObject
{
	NiPoint3 p;
	NiPoint3 n;
	NiPoint2 t1;
	NiPoint2 t2;
};


class Tile : public NiMemObject
{
	enum { SLOD_LEFT, SLOD_TOP, SLOD_RIGHT, SLOD_BOTTOM };
	enum { GROUND_UP, GROUND_DOWN, GROUND_RESET };	

	int						m_nLodLevel;		// SLOD 레벨
	int						m_nCrackType;		// Crack Type
	int						m_nCellCount;		// 타일 내 가로 세로 셀 갯수
	int						m_nTileCount;
	int						m_nWidthVtxCount;
	int						m_nVtxCount;		// 버텍스 갯수
	int						m_nTriangles;		// 삼각형 갯수
	int						m_nXIndex;			// X, Y 인덱스 위치
	int						m_nYIndex;
	unsigned int			m_uiIndexCount;		// 인덱스 버퍼가 보유하는 것으로 생성된 인덱스 수
	unsigned int			m_uiIBSize;			// 인덱스 버퍼의 크기를 바이트
	unsigned int			m_nPixelSize;		// 픽셀 가로 갯수
	int						m_nCurrentShaderMap;// 현재 그리기 알파텍스쳐 인덱스
	float					m_fPixelSize;

	float					m_fWidthSize;		// 타일 사이즈 X
	float					m_fHeightSize;		// 타일 사이즈 Y
	
	int						m_nIndex;			// 타일 번호
	std::bitset<4>			m_bitCrackType;		// 크랙 구분을 위한 BitSet		//□□□□   0000

	NiPoint3				m_vCenterPos;		// 타일의 중심위치
	NiPoint3				m_vMin, m_vMax;		// 타일의 Min, Max;
	
	NiTriShapeExPtr			m_spTile;			// Tile Geometry	
	NiNodePtr				m_spTileNode;
	NiTexturingPropertyPtr	m_spTexture;		// TileTexture
	
	NiSourceTexturePtr		m_spBaseTexture;	// 기본 텍스쳐
	NiSourceTexturePtr		m_spControlMap;		// 알파값 조절 컨트롤 맵

	bool					m_bDrawBB;			// 바운드 박스 그리기
	bool					m_bGetOldIB;		// Old 인덱스 버퍼
	
	BoundBox				*m_pBoundBox;		// 바운딩 박스

	D3DIndexBufferPtr		m_pIB;				// 인덱스 버퍼
	D3DIndexBufferPtr		m_pOldIB;			// 인덱스 올드 버퍼

	BillBoardAtChunk		*m_pBillBoardAtChunk;	// 빌보드 청크

	DecalAtChunk			*m_pDecalAtChunk;		// 데칼 청크

	StaticObjectAtChunk		*m_pStaticObjectAtChunk;// 스태틱 오브젝트 청크

	CollisionManager		*m_pCollsionManager;	// 충돌 관리 

public:

	Tile();
	~Tile();

	BOOL		CreateGeometry( int nCellCount, int nTileCount, float fWidthSize, float fHeightSize, float fXPos, float fYPos, int xIndex, int yIndex, float fUVSpacing, float tU, float tV, NiNode *pRootNode, NiRenderer* pkRenderer );

	BOOL		CreateTexture( NiFixedString &fileName );
	
	void		ReleaseObject( NiNode *pRootNode );
	void		UpdateObject();
	void		RenderObject(NiRenderer* pkRenderer);
	void		RenderObject( NiRenderer* pkRenderer, D3DDevicePtr pkD3DDevice );

public:

	float		GetDistance( const NiPoint3 &vPosition );

	void		SetIB( LPDIRECT3DINDEXBUFFER9 pIB, unsigned int uiIndexCount, unsigned int uiIBSize );
	void		SetIB( LPDIRECT3DINDEXBUFFER9 pIB );
	void		SetLodLevel( int nLodLevel ){ m_nLodLevel = nLodLevel; }	// SLOD 레벨 셋팅
	void		SetCrackType( int nCrackType){ m_nCrackType = nCrackType; } // CrackType 셋팅
	void		SetOldIB();
	void		SetShaderMaterial( NiMaterial *pMaterial );
	void		SetBillBoardChunk( BillBoardAtChunk *pBillBoard );
	void		SetDecalChunk( DecalAtChunk *pDecal );
	
	int			GetLodLevel() const { return m_nLodLevel; }					// SLOD 레벨 얻는다.
	NiPoint3	GetCenterPos()		{ return m_vCenterPos; }				// 타일의 중앙 Pos값을 얻어온다.	
	void		GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax );			// 타일의 Min Max를 얻는다.
	DecalAtChunk* GetDecalAtChunk(){	return m_pDecalAtChunk; }
	StaticObjectAtChunk* GetStaticObjectAtChunk(){ return m_pStaticObjectAtChunk; }
	
	void		SetIndex(int nIndex){ m_nIndex = nIndex; }
	int			GetIndex() const	{ return m_nIndex;   }

	//			SLOD 방향에 따라 비트값 셋팅
	void		SetBitCrackType( int slodType );

	//			컬링여부를 결정
	void		SetCulled( bool bCull );

	NiNode*		GetRootObject();

	//			현재 셋팅된 비트셋의 값을 얻어온다.
	int			GetBitSetValue(){ return (int)m_bitCrackType.to_ulong(); }
	void		SetBitReset()	{ m_bitCrackType.reset(); }

	//			광선과 바운딩박스 충돌 여부 검사
	BOOL		CollisionBB( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *dis );

	BOOL		CollisionTialeAntObjectBB(	const NiPoint3& pos, const NiPoint3& dir, 
											const NiPoint2& pos1, const NiPoint2& pos2,
											NiPick *pPick, float *fDistTile, float *fDisObject, int *nPickType  );

	BOOL		CheckIntersectionWithTile( const NiPoint2& pos1, const NiPoint2& pos2 );
	
	//			두 직선의 교차 판정
	BOOL		GetIntersectPoint(const NiPoint2& AP1, const NiPoint2& AP2, const NiPoint2& BP1, const NiPoint2& BP2, NiPoint2* IP);

	//			광선과 Tile 내 정점과 가장 가까운 충돌 위치 포착
	BOOL		GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, int *xIndex, int *zIndex);

	//			관리하고 있는 StaticObjectChunk에 있는 Object들에 대해서 Picking Ray 판정 실시
	BOOL		PickRayAtObjectGroup( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *pickNor, NiPick *pPick, float *fDist );

	BOOL		GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *vNormal, NiPoint3 *pVtx, 
				NiPick *pPick, float *fDist, int &xIndex, int &yIndex );

	BOOL		GetPickPositionAntUV( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, NiPoint2 *fUV );

	BOOL		IsTileArea( const NiPoint3& vPos );

	//			바운딩 박스를 그린다.
	void		SetDrawBB( bool bDraw );

	//			Terrain의 pHeightMap에 현재 타일의 정점 정보들을 셋팅
	void		GetTileVtxData( int xPos, int zPos, int nCntWidthVtx, NiPoint3 *pHeightMapPosition, NiPoint3 *pHeightMapNormal );

	//			오브젝트와 충돌한 지점의 위치와 노멀을 얻는다.
	BOOL		GetHeightPos( NiPoint3& vPos, NiPoint3& vHeight, NiPoint3& vNormal, NiPick *pPick );

	//			정점의 새로운 노멀값을 셋팅
	void		SetNormalOfVtx( NiPoint3* spHeightMap );

	void		SetTexture( NiTexture *pTexture );
	void		SetTextureProperty( NiTexturingProperty *pTexturingProperty );

	//			타일 중앙 위치
	void		GetCenterPos( NiPoint3 &vPos );	

	//			빌보드 청크 생성 여부
	BOOL		IsInitBillBoardAtChunk();

	//			빌보드 청크 생성
	BOOL		GenerateBillBoardAtChunk();

	//			스태틱 오브젝트 청크 생성 여부
	BOOL		IsInitStaticAtChunk();

	//			스태틱 오브젝트 청크 생성 
	BOOL		GenerateStaticObjectAtChunk();

	//			빌보드 청크에 타입에 따라 빌보드 오브젝트 추가
	BOOL		AddBillBoardAtTile( const NiPoint3& vPos );

	//			빌보드 청크에 추가된 빌보드 오브젝트 픽킹
	BOOL		PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *m_pPick );

	//			데칼 청크 생성 여부
	BOOL		IsInitDecalAtChunk();

	//			컬리전 Manager 생성 여부
	BOOL		IsInitCollisionManger();

	//			컬리전 Manager 생성
	BOOL		GenerateCollisionManger();

	//			데칼 청크 생성
	BOOL		GenerateDecalAtChunk();

	//			데칼 청크에 타입에 따라 빌보드 오브젝트 추가
	BOOL		AttachDecalAtTile( const NiPoint3* vPickPos, const NiPoint3* vNormal, const NiPoint3* vTangent, const int *nHeightMapIndex, int nDecalType, int nDecalPlace );

	//-----------------------------------------------------
	//
	//				텍스쳐 관련 함수 모음
	//
	//-----------------------------------------------------

	//			블렌딩 텍스쳐 1개 추가
	BOOL		AddShaderMap( NiFixedString &fileName, int index );

	//			알파값을 조절할 텍스쳐 생성 - SourceTexture
	BOOL		CreateControlMap	( NiSourceTexturePtr &pSourceTex );

	//			쉐이더 맵 스케일 조절
	BOOL		ShaderMapScaleTransForm( const NiPoint2& scaleRate );

	BOOL		ShaderMapScaleTransForm( const NiPoint2& scaleRate, int nTarget );

	//			현재 지정한 쉐이더 맵 스케일 조절
	BOOL		CurrentShaderMapScaleTransForm( const NiPoint2& scaleRate );

	//			지정한 인덱스에 해당하는 쉐이더 맵과 연관된 콘트롤 맵의 RGBA를 택하여 Reset;
	BOOL		ResetShaderMap( int resetType );

	//			현재 선택한 텍스쳐의 알파값 조절
	void		DrawControlMapRGBA( const NiPoint2& pickPixel, float fInsideLength, float fOutsideLength, int drawType );


	//-----------------------------------------------------
	//
	//				공간 분할 관련 함수 모음
	//
	//-----------------------------------------------------

	//			해당 오브젝트가 타일 영역에 포함 되는지 여부 
//	BOOL		IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, int &nTileNum, NifMesh *pMesh );
	BOOL		IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, NiGeometry *pMesh, int &nIncludeType );

	//			공간 분할된 오브젝트를 해당 타일에 포함
	void		SetRenderingGroup( NiNode *pObject, int &nIncludeType );

	//			공간 분할된 컬리전용 충돌박스를 해당 타일에 포함
	void		SetCollisionGroup( NiNode *pObject, const NiTransform& transform );

	void		SetCollisionGroupForMap( NiNode *pObject );	

	void		SetCollisionTransform( const NiTransform &transform );

	void		SetRenderingTransform( const NiTransform &transform );

	BOOL		SpaceDivisionRenderAndCollision( NifMesh *pMesh );

	//			케릭터를 각 타일의 CollsionManager에 등록하고 초기화 한다.
	BOOL		SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager );

	 

	CollisionManager*	GetCollisionManager();

	//-------------- Save & Load ----------------------

//	BOOL	Save( CArchive& ar );
	BOOL	Load( FILE* file );

	//-------------------------------------------------

	//			Search를 위한 Operator 정의
	bool operator == ( const Tile* pTile ){	 return m_nIndex == pTile->GetIndex(); }
	bool operator >  ( const Tile* pTile ){	 return m_nIndex >  pTile->GetIndex(); }
	bool operator <  ( const Tile* pTile ){	 return m_nIndex <  pTile->GetIndex(); }

private :

};

#endif