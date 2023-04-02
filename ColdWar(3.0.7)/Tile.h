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

	int						m_nLodLevel;		// SLOD ����
	int						m_nCrackType;		// Crack Type
	int						m_nCellCount;		// Ÿ�� �� ���� ���� �� ����
	int						m_nTileCount;
	int						m_nWidthVtxCount;
	int						m_nVtxCount;		// ���ؽ� ����
	int						m_nTriangles;		// �ﰢ�� ����
	int						m_nXIndex;			// X, Y �ε��� ��ġ
	int						m_nYIndex;
	unsigned int			m_uiIndexCount;		// �ε��� ���۰� �����ϴ� ������ ������ �ε��� ��
	unsigned int			m_uiIBSize;			// �ε��� ������ ũ�⸦ ����Ʈ
	unsigned int			m_nPixelSize;		// �ȼ� ���� ����
	int						m_nCurrentShaderMap;// ���� �׸��� �����ؽ��� �ε���
	float					m_fPixelSize;

	float					m_fWidthSize;		// Ÿ�� ������ X
	float					m_fHeightSize;		// Ÿ�� ������ Y
	
	int						m_nIndex;			// Ÿ�� ��ȣ
	std::bitset<4>			m_bitCrackType;		// ũ�� ������ ���� BitSet		//�����   0000

	NiPoint3				m_vCenterPos;		// Ÿ���� �߽���ġ
	NiPoint3				m_vMin, m_vMax;		// Ÿ���� Min, Max;
	
	NiTriShapeExPtr			m_spTile;			// Tile Geometry	
	NiNodePtr				m_spTileNode;
	NiTexturingPropertyPtr	m_spTexture;		// TileTexture
	
	NiSourceTexturePtr		m_spBaseTexture;	// �⺻ �ؽ���
	NiSourceTexturePtr		m_spControlMap;		// ���İ� ���� ��Ʈ�� ��

	bool					m_bDrawBB;			// �ٿ�� �ڽ� �׸���
	bool					m_bGetOldIB;		// Old �ε��� ����
	
	BoundBox				*m_pBoundBox;		// �ٿ�� �ڽ�

	D3DIndexBufferPtr		m_pIB;				// �ε��� ����
	D3DIndexBufferPtr		m_pOldIB;			// �ε��� �õ� ����

	BillBoardAtChunk		*m_pBillBoardAtChunk;	// ������ ûũ

	DecalAtChunk			*m_pDecalAtChunk;		// ��Į ûũ

	StaticObjectAtChunk		*m_pStaticObjectAtChunk;// ����ƽ ������Ʈ ûũ

	CollisionManager		*m_pCollsionManager;	// �浹 ���� 

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
	void		SetLodLevel( int nLodLevel ){ m_nLodLevel = nLodLevel; }	// SLOD ���� ����
	void		SetCrackType( int nCrackType){ m_nCrackType = nCrackType; } // CrackType ����
	void		SetOldIB();
	void		SetShaderMaterial( NiMaterial *pMaterial );
	void		SetBillBoardChunk( BillBoardAtChunk *pBillBoard );
	void		SetDecalChunk( DecalAtChunk *pDecal );
	
	int			GetLodLevel() const { return m_nLodLevel; }					// SLOD ���� ��´�.
	NiPoint3	GetCenterPos()		{ return m_vCenterPos; }				// Ÿ���� �߾� Pos���� ���´�.	
	void		GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax );			// Ÿ���� Min Max�� ��´�.
	DecalAtChunk* GetDecalAtChunk(){	return m_pDecalAtChunk; }
	StaticObjectAtChunk* GetStaticObjectAtChunk(){ return m_pStaticObjectAtChunk; }
	
	void		SetIndex(int nIndex){ m_nIndex = nIndex; }
	int			GetIndex() const	{ return m_nIndex;   }

	//			SLOD ���⿡ ���� ��Ʈ�� ����
	void		SetBitCrackType( int slodType );

	//			�ø����θ� ����
	void		SetCulled( bool bCull );

	NiNode*		GetRootObject();

	//			���� ���õ� ��Ʈ���� ���� ���´�.
	int			GetBitSetValue(){ return (int)m_bitCrackType.to_ulong(); }
	void		SetBitReset()	{ m_bitCrackType.reset(); }

	//			������ �ٿ���ڽ� �浹 ���� �˻�
	BOOL		CollisionBB( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *dis );

	BOOL		CollisionTialeAntObjectBB(	const NiPoint3& pos, const NiPoint3& dir, 
											const NiPoint2& pos1, const NiPoint2& pos2,
											NiPick *pPick, float *fDistTile, float *fDisObject, int *nPickType  );

	BOOL		CheckIntersectionWithTile( const NiPoint2& pos1, const NiPoint2& pos2 );
	
	//			�� ������ ���� ����
	BOOL		GetIntersectPoint(const NiPoint2& AP1, const NiPoint2& AP2, const NiPoint2& BP1, const NiPoint2& BP2, NiPoint2* IP);

	//			������ Tile �� ������ ���� ����� �浹 ��ġ ����
	BOOL		GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, int *xIndex, int *zIndex);

	//			�����ϰ� �ִ� StaticObjectChunk�� �ִ� Object�鿡 ���ؼ� Picking Ray ���� �ǽ�
	BOOL		PickRayAtObjectGroup( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *pickNor, NiPick *pPick, float *fDist );

	BOOL		GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *vNormal, NiPoint3 *pVtx, 
				NiPick *pPick, float *fDist, int &xIndex, int &yIndex );

	BOOL		GetPickPositionAntUV( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, NiPoint2 *fUV );

	BOOL		IsTileArea( const NiPoint3& vPos );

	//			�ٿ�� �ڽ��� �׸���.
	void		SetDrawBB( bool bDraw );

	//			Terrain�� pHeightMap�� ���� Ÿ���� ���� �������� ����
	void		GetTileVtxData( int xPos, int zPos, int nCntWidthVtx, NiPoint3 *pHeightMapPosition, NiPoint3 *pHeightMapNormal );

	//			������Ʈ�� �浹�� ������ ��ġ�� ����� ��´�.
	BOOL		GetHeightPos( NiPoint3& vPos, NiPoint3& vHeight, NiPoint3& vNormal, NiPick *pPick );

	//			������ ���ο� ��ְ��� ����
	void		SetNormalOfVtx( NiPoint3* spHeightMap );

	void		SetTexture( NiTexture *pTexture );
	void		SetTextureProperty( NiTexturingProperty *pTexturingProperty );

	//			Ÿ�� �߾� ��ġ
	void		GetCenterPos( NiPoint3 &vPos );	

	//			������ ûũ ���� ����
	BOOL		IsInitBillBoardAtChunk();

	//			������ ûũ ����
	BOOL		GenerateBillBoardAtChunk();

	//			����ƽ ������Ʈ ûũ ���� ����
	BOOL		IsInitStaticAtChunk();

	//			����ƽ ������Ʈ ûũ ���� 
	BOOL		GenerateStaticObjectAtChunk();

	//			������ ûũ�� Ÿ�Կ� ���� ������ ������Ʈ �߰�
	BOOL		AddBillBoardAtTile( const NiPoint3& vPos );

	//			������ ûũ�� �߰��� ������ ������Ʈ ��ŷ
	BOOL		PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *m_pPick );

	//			��Į ûũ ���� ����
	BOOL		IsInitDecalAtChunk();

	//			�ø��� Manager ���� ����
	BOOL		IsInitCollisionManger();

	//			�ø��� Manager ����
	BOOL		GenerateCollisionManger();

	//			��Į ûũ ����
	BOOL		GenerateDecalAtChunk();

	//			��Į ûũ�� Ÿ�Կ� ���� ������ ������Ʈ �߰�
	BOOL		AttachDecalAtTile( const NiPoint3* vPickPos, const NiPoint3* vNormal, const NiPoint3* vTangent, const int *nHeightMapIndex, int nDecalType, int nDecalPlace );

	//-----------------------------------------------------
	//
	//				�ؽ��� ���� �Լ� ����
	//
	//-----------------------------------------------------

	//			���� �ؽ��� 1�� �߰�
	BOOL		AddShaderMap( NiFixedString &fileName, int index );

	//			���İ��� ������ �ؽ��� ���� - SourceTexture
	BOOL		CreateControlMap	( NiSourceTexturePtr &pSourceTex );

	//			���̴� �� ������ ����
	BOOL		ShaderMapScaleTransForm( const NiPoint2& scaleRate );

	BOOL		ShaderMapScaleTransForm( const NiPoint2& scaleRate, int nTarget );

	//			���� ������ ���̴� �� ������ ����
	BOOL		CurrentShaderMapScaleTransForm( const NiPoint2& scaleRate );

	//			������ �ε����� �ش��ϴ� ���̴� �ʰ� ������ ��Ʈ�� ���� RGBA�� ���Ͽ� Reset;
	BOOL		ResetShaderMap( int resetType );

	//			���� ������ �ؽ����� ���İ� ����
	void		DrawControlMapRGBA( const NiPoint2& pickPixel, float fInsideLength, float fOutsideLength, int drawType );


	//-----------------------------------------------------
	//
	//				���� ���� ���� �Լ� ����
	//
	//-----------------------------------------------------

	//			�ش� ������Ʈ�� Ÿ�� ������ ���� �Ǵ��� ���� 
//	BOOL		IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, int &nTileNum, NifMesh *pMesh );
	BOOL		IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, NiGeometry *pMesh, int &nIncludeType );

	//			���� ���ҵ� ������Ʈ�� �ش� Ÿ�Ͽ� ����
	void		SetRenderingGroup( NiNode *pObject, int &nIncludeType );

	//			���� ���ҵ� �ø����� �浹�ڽ��� �ش� Ÿ�Ͽ� ����
	void		SetCollisionGroup( NiNode *pObject, const NiTransform& transform );

	void		SetCollisionGroupForMap( NiNode *pObject );	

	void		SetCollisionTransform( const NiTransform &transform );

	void		SetRenderingTransform( const NiTransform &transform );

	BOOL		SpaceDivisionRenderAndCollision( NifMesh *pMesh );

	//			�ɸ��͸� �� Ÿ���� CollsionManager�� ����ϰ� �ʱ�ȭ �Ѵ�.
	BOOL		SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager );

	 

	CollisionManager*	GetCollisionManager();

	//-------------- Save & Load ----------------------

//	BOOL	Save( CArchive& ar );
	BOOL	Load( FILE* file );

	//-------------------------------------------------

	//			Search�� ���� Operator ����
	bool operator == ( const Tile* pTile ){	 return m_nIndex == pTile->GetIndex(); }
	bool operator >  ( const Tile* pTile ){	 return m_nIndex >  pTile->GetIndex(); }
	bool operator <  ( const Tile* pTile ){	 return m_nIndex <  pTile->GetIndex(); }

private :

};

#endif