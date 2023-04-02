#ifndef TERRAIN_H
#define TERRAIN_H


#include "QuardTree.h"
#include "Frustum.h"
#include "NifMesh.h"
#include "Tile.h"
#include "GBObject.h"
#include "BillBoardManager.h"
#include "NiCommonMaterialLibrary.h"
#include "TextureBlendMaterial.h"
#include "DecalManager.h"
#include "BulletDecalFactory.h"
#include "CharacterManager.h"
#include "CollisionManager.h"
//#include "CUerCharacter.h"

#include <cstdio>
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <Functional>
#include <algorithm>
#include <NiDX9IBManager.h>


//Ŭ���� ���漱��
class NiDX9RendererEx;
class QuadTree;
class ZFLog;

	
#define	LOD_LEVEL		4
#define TEXTURE_COUNT	4

#define PICK_UI_SIZE	32

const float PICKING_LIMIT_DISTANCE = 80.0f;	


struct BlendTextureInfo 
{
	short			m_nTexureIndex;
	NiFixedString	m_strTexName;
	static int		m_nCntBlendTexture;
	NiPoint2		m_fTextureScale;

	BlendTextureInfo()
	{
		m_nTexureIndex = -1;
		m_fTextureScale.x = 1.0f;
		m_fTextureScale.y = 1.0f;
	}

};

struct IBInfo
{
	unsigned int m_uiIndexCount;
	unsigned int m_usIBSize;

	IBInfo( unsigned int uiIndexCount, unsigned int usIBSize )
	{
		m_uiIndexCount = uiIndexCount;
		m_usIBSize = usIBSize;
	}
};

struct BoundColliInfo
{
	unsigned short	m_usTileNum;
	float			m_fDistance;
};


class Terrain : public GBObject {

public:
	// SLOD �ε���
	enum { SLOD_DEFAULT, SLOD_LEFT, SLOD_TOP, SLOD_RIGHT, SLOD_BOTTOM, 
		SLOD_LEFT_BOTTOM, SLOD_RIGHT_TOP, SLOD_LEFT_TOP, SLOD_RIGHT_BOTTOM };

	//	���� TransForm Ÿ��
	enum { GROUND_UP, GROUND_DOWN, GROUND_RESET };

	//	�ؽ��� Draw Ÿ��
	enum { TEXTURE_DRAW, TEXTURE_ERASE };

	//	�ؽ��� ���� Ÿ��
	enum { BLEND_TEX_R, BLEND_TEX_G, BLEND_TEX_B, BLEND_TEX_A, BLEND_TEX_COUNT };

	//	�ؽ��� ��Ʈ�Ѹ� ����Ÿ��
	enum { ALPHA_00, ALPHA_FF };

	enum { TILE_PICK = 1, OBJECT_PICK = 2, OBJECT_TILE_PICK = 3 };


private:

	int				m_nTriangles;			// �� ���� �ʿ��� �ﰢ�� ����
	int				m_nTotalVtxs;			// ��ü ���ؽ� ����
	int				m_nTileCount;			// ������ �����ϴ� ���� ���� Ÿ�� ����
	int				m_nCellCount;			// Ÿ���� �����ϴ� ���� ���� �� ����
	int				m_nWidthVtxCount;		// ���� ���ؽ� ����
	int				m_nWidthRect;			// ���� �� ����
	float			m_fWidthSize;			// ������ �����ϴ� �� Ÿ���� ���� ����
	float			m_fHeightSize;			// ������ �����ϴ� �� Ÿ���� ���� ����
	float			m_fWidthSizeOfTile;  	// Ÿ�� �ϳ��� ���� ����
	float			m_fHeightSizeOfTile;	// Ÿ�� �ϳ��� ���� ����
	int				m_maxSLODLevel;			// SLOD ����
	float			m_SLODRange;			// SDLD ����				
	unsigned int	m_nPixelSize;			// ��Ʈ�� ���� �ȼ� ������
	float			m_fVtxWidthSpacing;		// ���� ���ؽ����� ����
	float			m_fVtxHeightSpacing;	// ���� ���ؽ����� ����
	
	float			m_fPixelSize;			// 1�ȼ� ������
	float			m_fUVSpacing;

	
	float			m_fInsideLength;		// �����ؽ��� �׸��� ���� ���� �ִ� ������ ũ��
	float			m_fOutsideLength;		// �����ؽ��� �׸��� �ٱ��� ���� �ִ� ������ ũ��
	int				m_nCntCheckPixel;		// �����ؽ��� ��ŷ�� �ȼ��κ��� �˻��� �ȼ��� �ִ� ����
	int				m_nCurrentDrawMode;		// �����ؽ��� Draw ���
	
	int				m_nCurrentShaderMap;	// ���� �׸��� �����ؽ��� �ε���
	int				m_nShaderMapCount;		// TextureBlendMaterial ShaderMap ����

	NiFixedString	m_strBaseTexName;		// �⺻ �ؽ��� �̸�

	// Width/Height parameters
    static unsigned int ms_uiAlphaPreviewW;
    static unsigned int ms_uiAlphaPreviewH;	

	NiDX9RendererEx			*m_pRenderer;
	D3DDevicePtr			m_pkD3DDevice;			//typedef LPDIRECT3DDEVICE9 D3DDevicePtr;

	NiNodePtr				m_spTerrainRoot;	
	QuadTree				*m_pQuadTree;			// ����Ʈ��
	FrustumEx				*m_pFrustum;			// ����ü �ø�
	NiPoint3				*m_pHeightMapPosition;
	NiPoint3				*m_pHeightMapNormal;
	unsigned int			*m_pUsTriIndex;
	NiCamera				*m_pCamera;				// ī�޶�

	NiPick					*m_pPick;				// ���� ��ŷ

	D3DFORMAT				m_eFormat;
	NiRenderedTexturePtr	m_spRenderTexture;
	NiRenderTargetGroupPtr	m_spRenderTargetGroup;
	
	D3DTexturePtr			m_pkD3DTexture;	
	D3DTexturePtr			m_pkD3DReadTexture;

	NiTexturingPropertyPtr	m_spTex;	
	NiDynamicTexturePtr		m_pControlBlendTexture;
	NiSourceTexturePtr		m_pControlBlendTextures;
	NiMaterialProperty		*m_Color;		// ����

	NiMatrix3				m_matCameraRot;

//	NiTriShapeData			*m_pHeightData;
		
	// Texture
	NiTexturePtr			m_spBaseTexture;

	// SLOD�� ���� �ε��� ����
	LPDIRECT3DINDEXBUFFER9	(*m_pIB)[ 9 ];						// ������¿� �ε�������

	float					m_fAlpahMapWidth;					// ���ĸ� Width , Height
	float					m_fAlpahMapHeight;					

	// Object Container
	std::vector< Tile* >		m_vecTiles;
	std::vector< IBInfo >		m_vecIBInfos;
	std::vector< int >			m_vecValidTilesNum;

	BlendTextureInfo			m_vecBlendTextureInfo[ 4 ];

	BoundColliInfo				*m_pTileBoundColliInfo;
	BoundColliInfo				*m_pOjbectBoundColliInfo;
	

	static Terrain				*ms_pTerrain;

	NiNode						*m_pPlayerNode;

public:

	Terrain( int nTileCount, int nCellCount, float fWidthSize, float fHeightSize );
	~Terrain();
	
	//--------------------- ������ �Լ� ---------------------

	NiNode* GetObject(){ return m_spTerrainRoot; }
	BOOL	UpdateObject( float fElapsedTime  );
	BOOL	ReleaseObject();

	static	Terrain*	CreateManager( int nTileCount, int nCellCount, float fWidthSize, float fHeightSize );
	static	Terrain*	GetManager();
	static  BOOL		IsManager();
	static	void		ReleaseManager();

	//-------------------------------------------------------	

protected:

	virtual void SetHeightMap();

	// ���ؽ� ���� ����
	virtual BOOL InitVBTile	();

	// �ε��� ���� ����
	virtual BOOL InitIB		();

	// SLOD �ε��� ���� ����
	virtual BOOL SetSLODIB	( int nSLODLevel );

	// SLOD ���� ����
	virtual void SetSLODLevel();

public:

	//	���� Node ����
	NiNode* GetTerrainRoot();

	//-------------------------------------------------------
	
	// ��ŷ NiPick ����
	virtual void CreatePick();

	//	�ε��� ���� ������ ����
	void SetIndexBufferManager( NiDX9IndexBufferManager *pIBManager );

	//------------------------------------------------------------------
	//
	//						Geometry ���� �Լ�
	//
	//------------------------------------------------------------------


	//			���� �ʱ�ȭ
	BOOL		InitTerrain( NiRenderer *pRenderer );

	//			ī�޶� ����Ʈ ��
	void		SetCamera( NiCamera *pCamera )	{ m_pCamera = pCamera; }

	//			SLOD ���� ����
	void		SetSDLORANGE( float slodRange )	{ m_SLODRange = slodRange; }

	//			���̴� �� ���� ����
	void		SetShderMapCount( int nCount )	{ m_nShaderMapCount = nCount; }

	//			���� ���� Ÿ�� ���� �� ����
	int			GetCntTiles()					{ return m_nTileCount; }
	int			GetCntCells()					{ return m_nCellCount; }
	//			���� ������ ���� ���� ũ��
	float		GetTerrainWidth()				{ return m_fWidthSize;	}
	float		GetTerrainHeight()				{ return m_fHeightSize; }

	float		GetWidthCellSpacing(){ return m_fVtxWidthSpacing; }
	float		GetHeightCellSpacing(){ return m_fVtxHeightSpacing; }

	NiPoint3&	GetVtxPosition( unsigned int nVtxIndex );
	NiPoint3&	GetVtxNormal( unsigned int nVtxIndex );

	NiPoint3*	GetTerrainVertices();
	NiPoint3*	GetTerrainNormals();

	Tile*		GetTile( const  int& xIndex, const int& yIndex );

	void	GetDecalTriIndex(const int *nHeightMapIndex,  const float *fLimitWidth, const float *fLimitHeight, std::vector< DecalTriIndex >& vecDecalTri );

	void	GetDecalTriIndex(const int *nHeightMapIndex,  const int &nCntWidthRect, const int &	nCntheightRect, std::vector< int >& vecVtxIndex );
	
	//		�ε����� �̿��Ͽ� ������ ��ġ ���͸� ��´�.
	void	GetVec3Terrain( int index, NiPoint3& vec ){ if( NULL != m_pHeightMapPosition )	vec = m_pHeightMapPosition[ index ]; }

	//		���� ��ŷ ������ ���.
	BOOL	PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction, NiPoint3 *PickPos, NiPoint3 *vPickNor, NiPoint3 *pPickedVtxs, 
		
		int &nPickedTileNum,  int &xTriIndex, int &yTriIndex, int &nPickPlace  );

	//		���� ��ŷ ������ ���.
	BOOL	PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction,NiPoint3& pkPickPoint );	//3���� ���ڴ� ��ŷ�� ������ ����

	//		��ŷ�� ������ ��ġ�� �����带 �߰��Ѵ�.
	BOOL	PickTerrainAndAddBillBoard( const NiPoint3& pos, const NiPoint3& direction );

	//		���� ����ü Epsilion ����
	void	SetFrustumEpsilion	( float fFrustumRange	)	{ if( NULL != m_pFrustum ) m_pFrustum->SetPlaneEpsilon( fFrustumRange ); }

	//		���� SLOD ����
	void	SetSlodRange		( float fSlodRange		)	{ m_SLODRange = fSlodRange; }

	//		�ٿ�� �ڽ� �׸��� ����
	void	SetDrawBB			( bool	bDraw			);

	//		������ ����� �籸��
	void	CalculatorNormals();

			
	void	SetBillBoardChunk( int nXIndex, int nYIndex, BillBoardAtChunk* pChunk );

	//		��Į ����
	BOOL	AttachDecalAtTile( const NiPoint3* vPickedPos, NiPoint3* vPickedNor, NiPoint3 *pPickedVtxs, const int &nPickedTileNum, const int &nXTriIndex, const int &nYTriIndex );

	//		��ŷ�� ������Ʈ�� ��ġ������ ����� �̿��Ͽ� ���õ� Tile�� DecalAtChunk�� �ش� ��Į��
	BOOL	AttachDecalOfStaticObject( const NiPoint3* pos, const  NiPoint3* dir, const int &nPickedObjectTileNum );

	//		�ش� ��ġ�� �����ϴ� Ÿ���� �ε����� ��´�.
	BOOL	GetTileIndex( const NiPoint3& pos, int &xIndex, int &yIndex );

	//		��Ÿ�ϵ��� ������� ��ü ���������� ��� m_pHeightMap�� ����
	void	SetHeightMapFromTiles();


	//------------------------------------------------------------------------------------
	//
	//						ĳ���� ������ �浹 ó�� �Լ� ����
	//
	//------------------------------------------------------------------------------------

	void	SetPlayer( NiNode *pPlayer );

	//		������ Object�� �浹 ��ġ�� ����� ��´�.
	BOOL	GetHeightPos( NiPoint3& vPos, NiPoint3& vHeight, NiPoint3& vNor );

	//		ĳ���� ��ġ�� �ش��ϴ� CollisionManager�� �����Ѵ�.
	CollisionManager*	GetCollisionManager( NiPoint3& vPos );






	//------------------------------------------------------------------
	//
	//						Texture ���� �Լ�
	//
	//------------------------------------------------------------------

	//		TextureBledMaterial Craete
	BOOL	CreateTextureBlendMaterial();

	//		�ؽ��� ����
	BOOL	CreateTexture		( NiFixedString& strFileName,  BOOL bFirst = TRUE );	

	//		���ĸ� ���� ���̳��� �ؽ��Ĥ�
	BOOL	CreateAlphaMap		( NiDynamicTexturePtr &pApphaTex );

	//		���ĸ� ���� �ҽ� �ؽ���
	BOOL	CreateControlMap	( NiSourceTexturePtr &pSourceTex );

	//		���� �����Ͽ� ������� ���̴� �� ���� 
	int		GetShaderMapCount	(){	return m_nShaderMapCount; }

	//		���� �ؽ��� 1�� �߰�
	BOOL	AddShaderMap( NiFixedString &fileName );

	//		��ü ���̴� �� ������ ����
	BOOL	ShaderMapScaleTransForm( const NiPoint2& scaleRate );

	//		�̸����� ǥ�ø� ���� ����Ÿ�� ����
	void	CreateRenderTarget( NiRenderer *pRenderer );

	//		���� ���ĸʿ� �귯�� ũ�⸸ŭ �׸���.
	void	DrawControlMapRGBA	( const NiPoint3& vPos, const NiPoint3& vDir );
	
	//		������Ʈ�� �ٿ��ڽ��� Ÿ���� �ٿ�� �ڽ��� ���ؼ� �ش� Ÿ���� �ø��� �׷쿡 �߰�
//	void	IsCollisionOfTile( NifMesh *pMesh );
	void	IsCollisionOfTile( NiGeometry *pObject );

	//		�ε� �ؼ�  �ش� ������Ʈ�� �ش� Ÿ���� ������ �׷쿡 Attach
	void	SetRenderingGroup( NiNode *pObject, int &nTileNum, int &nIncludeType );

	//		�ε� �ؼ� �ش� ������Ʈ�� �ش� Ÿ���� �ø��� Manager �׷쿡 �߰�
	void	SetCollisionGroup( NiNode *pObject, const NiTransform& transform, int &nTileNum, int &nIncludeType );

	//		�ε� �ؼ� �ش� ������Ʈ�� �ش� Ÿ���� �ø��� Manager �׷쿡 �߰�
	void	SetCollisionGroupAboutMap( NiNode *pObject, int &nTileNum, int &nIncludeType );

	//		�ɸ��͸� �� Ÿ���� CollsionManager�� ����Ѵ�.
	void	SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager );

	void	SetCollisionTransform( const NiTransform &transform );

	void	SetRenderingTransform( const NiTransform &transform );
	
	
	//		���� Draw Mode ���� or ���
	void	SetDrawMode			( int nMode		)	{ m_nCurrentDrawMode = nMode; }
	int		GetDrawMode			( void			)	{ return m_nCurrentDrawMode;  }

	void	SetInnerDrawRange	( float fRange	)	{ m_fInsideLength = fRange; }
	void	SetOutterDrawRange	( float fRange	)	{ m_fOutsideLength= fRange; }

	BOOL	IsCurrentShaderMap()	{ return m_nCurrentShaderMap != -1; }	

	

	//-------------------------------------------------------

	//---------------------- Save & Load --------------------
/*
	void	SaveHeader( CArchive& ar );
	void	SaveData( CArchive& ar );
	void	SaveNameData( std::ofstream& fwrite );
*/
	void	LoadData(  FILE* file );
	void	LoadNameData( int &nCnt, std::list<NiFixedString>& listFileName );

	//-------------------------------------------------------

};


#endif