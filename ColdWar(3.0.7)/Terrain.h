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


//클래스 전방선언
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
	// SLOD 인덱스
	enum { SLOD_DEFAULT, SLOD_LEFT, SLOD_TOP, SLOD_RIGHT, SLOD_BOTTOM, 
		SLOD_LEFT_BOTTOM, SLOD_RIGHT_TOP, SLOD_LEFT_TOP, SLOD_RIGHT_BOTTOM };

	//	지형 TransForm 타입
	enum { GROUND_UP, GROUND_DOWN, GROUND_RESET };

	//	텍스쳐 Draw 타입
	enum { TEXTURE_DRAW, TEXTURE_ERASE };

	//	텍스쳐 블렌딩 타입
	enum { BLEND_TEX_R, BLEND_TEX_G, BLEND_TEX_B, BLEND_TEX_A, BLEND_TEX_COUNT };

	//	텍스쳐 컨트롤맵 리셋타입
	enum { ALPHA_00, ALPHA_FF };

	enum { TILE_PICK = 1, OBJECT_PICK = 2, OBJECT_TILE_PICK = 3 };


private:

	int				m_nTriangles;			// 한 셀에 필요한 삼각형 갯수
	int				m_nTotalVtxs;			// 전체 버텍스 갯수
	int				m_nTileCount;			// 지형을 구성하는 가로 세로 타일 갯수
	int				m_nCellCount;			// 타일을 구성하는 가로 세로 셀 갯수
	int				m_nWidthVtxCount;		// 가로 버텍스 갯수
	int				m_nWidthRect;			// 가로 셀 개수
	float			m_fWidthSize;			// 지형을 구성하는 한 타일의 가로 길이
	float			m_fHeightSize;			// 지형을 구성하는 한 타일의 세로 길이
	float			m_fWidthSizeOfTile;  	// 타일 하나의 가로 길이
	float			m_fHeightSizeOfTile;	// 타일 하나의 세로 길이
	int				m_maxSLODLevel;			// SLOD 레벨
	float			m_SLODRange;			// SDLD 범위				
	unsigned int	m_nPixelSize;			// 컨트롤 맵의 픽셀 사이즈
	float			m_fVtxWidthSpacing;		// 가로 버텍스간의 간격
	float			m_fVtxHeightSpacing;	// 세로 버텍스간의 간격
	
	float			m_fPixelSize;			// 1픽셀 사이즈
	float			m_fUVSpacing;

	
	float			m_fInsideLength;		// 알파텍스쳐 그릴때 안쪽 원의 최대 반지름 크기
	float			m_fOutsideLength;		// 알파텍스쳐 그릴때 바깥쪽 원의 최대 반지름 크기
	int				m_nCntCheckPixel;		// 알파텍스쳐 픽킹된 픽셀로부터 검사할 픽셀의 최대 갯수
	int				m_nCurrentDrawMode;		// 알파텍스쳐 Draw 모드
	
	int				m_nCurrentShaderMap;	// 현재 그리기 알파텍스쳐 인덱스
	int				m_nShaderMapCount;		// TextureBlendMaterial ShaderMap 갯수

	NiFixedString	m_strBaseTexName;		// 기본 텍스쳐 이름

	// Width/Height parameters
    static unsigned int ms_uiAlphaPreviewW;
    static unsigned int ms_uiAlphaPreviewH;	

	NiDX9RendererEx			*m_pRenderer;
	D3DDevicePtr			m_pkD3DDevice;			//typedef LPDIRECT3DDEVICE9 D3DDevicePtr;

	NiNodePtr				m_spTerrainRoot;	
	QuadTree				*m_pQuadTree;			// 쿼드트리
	FrustumEx				*m_pFrustum;			// 절두체 컬링
	NiPoint3				*m_pHeightMapPosition;
	NiPoint3				*m_pHeightMapNormal;
	unsigned int			*m_pUsTriIndex;
	NiCamera				*m_pCamera;				// 카메라

	NiPick					*m_pPick;				// 지형 픽킹

	D3DFORMAT				m_eFormat;
	NiRenderedTexturePtr	m_spRenderTexture;
	NiRenderTargetGroupPtr	m_spRenderTargetGroup;
	
	D3DTexturePtr			m_pkD3DTexture;	
	D3DTexturePtr			m_pkD3DReadTexture;

	NiTexturingPropertyPtr	m_spTex;	
	NiDynamicTexturePtr		m_pControlBlendTexture;
	NiSourceTexturePtr		m_pControlBlendTextures;
	NiMaterialProperty		*m_Color;		// 재질

	NiMatrix3				m_matCameraRot;

//	NiTriShapeData			*m_pHeightData;
		
	// Texture
	NiTexturePtr			m_spBaseTexture;

	// SLOD를 위한 인덱스 버퍼
	LPDIRECT3DINDEXBUFFER9	(*m_pIB)[ 9 ];						// 지형출력용 인덱스버퍼

	float					m_fAlpahMapWidth;					// 알파맵 Width , Height
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
	
	//--------------------- 재정의 함수 ---------------------

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

	// 버텍스 버퍼 생성
	virtual BOOL InitVBTile	();

	// 인덱스 버퍼 생성
	virtual BOOL InitIB		();

	// SLOD 인덱스 버퍼 생성
	virtual BOOL SetSLODIB	( int nSLODLevel );

	// SLOD 레벨 셋팅
	virtual void SetSLODLevel();

public:

	//	지형 Node 리턴
	NiNode* GetTerrainRoot();

	//-------------------------------------------------------
	
	// 픽킹 NiPick 생성
	virtual void CreatePick();

	//	인덱스 버퍼 관리자 셋팅
	void SetIndexBufferManager( NiDX9IndexBufferManager *pIBManager );

	//------------------------------------------------------------------
	//
	//						Geometry 관련 함수
	//
	//------------------------------------------------------------------


	//			지형 초기화
	BOOL		InitTerrain( NiRenderer *pRenderer );

	//			카메라 포인트 셋
	void		SetCamera( NiCamera *pCamera )	{ m_pCamera = pCamera; }

	//			SLOD 범위 지정
	void		SetSDLORANGE( float slodRange )	{ m_SLODRange = slodRange; }

	//			쉐이더 맵 개수 설정
	void		SetShderMapCount( int nCount )	{ m_nShaderMapCount = nCount; }

	//			현재 맵의 타일 갯수 셀 갯수
	int			GetCntTiles()					{ return m_nTileCount; }
	int			GetCntCells()					{ return m_nCellCount; }
	//			현재 지형의 가로 세로 크기
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
	
	//		인덱스를 이용하여 지형의 위치 벡터를 얻는다.
	void	GetVec3Terrain( int index, NiPoint3& vec ){ if( NULL != m_pHeightMapPosition )	vec = m_pHeightMapPosition[ index ]; }

	//		씬에 픽킹 관선을 쏜다.
	BOOL	PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction, NiPoint3 *PickPos, NiPoint3 *vPickNor, NiPoint3 *pPickedVtxs, 
		
		int &nPickedTileNum,  int &xTriIndex, int &yTriIndex, int &nPickPlace  );

	//		씬에 픽킹 관선을 쏜다.
	BOOL	PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction,NiPoint3& pkPickPoint );	//3번쨰 인자는 피킹점 리턴을 위함

	//		픽킹된 지형의 위치에 빌보드를 추가한다.
	BOOL	PickTerrainAndAddBillBoard( const NiPoint3& pos, const NiPoint3& direction );

	//		지형 절두체 Epsilion 셋팅
	void	SetFrustumEpsilion	( float fFrustumRange	)	{ if( NULL != m_pFrustum ) m_pFrustum->SetPlaneEpsilon( fFrustumRange ); }

	//		지형 SLOD 범위
	void	SetSlodRange		( float fSlodRange		)	{ m_SLODRange = fSlodRange; }

	//		바운딩 박스 그리기 여부
	void	SetDrawBB			( bool	bDraw			);

	//		정점의 노멀을 재구성
	void	CalculatorNormals();

			
	void	SetBillBoardChunk( int nXIndex, int nYIndex, BillBoardAtChunk* pChunk );

	//		데칼 적용
	BOOL	AttachDecalAtTile( const NiPoint3* vPickedPos, NiPoint3* vPickedNor, NiPoint3 *pPickedVtxs, const int &nPickedTileNum, const int &nXTriIndex, const int &nYTriIndex );

	//		픽킹된 오브젝트의 위치정보와 노멀을 이용하여 관련된 Tile의 DecalAtChunk에 해당 데칼을
	BOOL	AttachDecalOfStaticObject( const NiPoint3* pos, const  NiPoint3* dir, const int &nPickedObjectTileNum );

	//		해당 위치를 포함하는 타일의 인덱스를 얻는다.
	BOOL	GetTileIndex( const NiPoint3& pos, int &xIndex, int &yIndex );

	//		각타일들의 정점들로 전체 지형정보를 담는 m_pHeightMap를 셋팅
	void	SetHeightMapFromTiles();


	//------------------------------------------------------------------------------------
	//
	//						캐릭터 지형과 충돌 처리 함수 모음
	//
	//------------------------------------------------------------------------------------

	void	SetPlayer( NiNode *pPlayer );

	//		지형과 Object의 충돌 위치와 노멀을 얻는다.
	BOOL	GetHeightPos( NiPoint3& vPos, NiPoint3& vHeight, NiPoint3& vNor );

	//		캐릭터 위치에 해당하는 CollisionManager를 리턴한다.
	CollisionManager*	GetCollisionManager( NiPoint3& vPos );






	//------------------------------------------------------------------
	//
	//						Texture 관련 함수
	//
	//------------------------------------------------------------------

	//		TextureBledMaterial Craete
	BOOL	CreateTextureBlendMaterial();

	//		텍스쳐 생성
	BOOL	CreateTexture		( NiFixedString& strFileName,  BOOL bFirst = TRUE );	

	//		알파맵 생성 다이나믹 텍스쳐ㄴ
	BOOL	CreateAlphaMap		( NiDynamicTexturePtr &pApphaTex );

	//		알파맵 생성 소스 텍스쳐
	BOOL	CreateControlMap	( NiSourceTexturePtr &pSourceTex );

	//		현재 설정하여 사용중인 쉐이더 맵 갯수 
	int		GetShaderMapCount	(){	return m_nShaderMapCount; }

	//		블렌딩 텍스쳐 1개 추가
	BOOL	AddShaderMap( NiFixedString &fileName );

	//		전체 쉐이더 맵 스케일 조절
	BOOL	ShaderMapScaleTransForm( const NiPoint2& scaleRate );

	//		미리보기 표시를 위한 렌더타켓 설정
	void	CreateRenderTarget( NiRenderer *pRenderer );

	//		현재 알파맵에 브러쉬 크기만큼 그린다.
	void	DrawControlMapRGBA	( const NiPoint3& vPos, const NiPoint3& vDir );
	
	//		오브젝트의 바운디박스와 타일의 바운딩 박스를 비교해서 해당 타일의 컬리젼 그룹에 추가
//	void	IsCollisionOfTile( NifMesh *pMesh );
	void	IsCollisionOfTile( NiGeometry *pObject );

	//		로드 해서  해당 오브젝트를 해당 타일의 렌더링 그룹에 Attach
	void	SetRenderingGroup( NiNode *pObject, int &nTileNum, int &nIncludeType );

	//		로드 해서 해당 오브젝트를 해당 타일의 컬리전 Manager 그룹에 추가
	void	SetCollisionGroup( NiNode *pObject, const NiTransform& transform, int &nTileNum, int &nIncludeType );

	//		로드 해서 해당 오브젝트를 해당 타일의 컬리전 Manager 그룹에 추가
	void	SetCollisionGroupAboutMap( NiNode *pObject, int &nTileNum, int &nIncludeType );

	//		케릭터를 각 타일의 CollsionManager에 등록한다.
	void	SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager );

	void	SetCollisionTransform( const NiTransform &transform );

	void	SetRenderingTransform( const NiTransform &transform );
	
	
	//		현재 Draw Mode 셋팅 or 얻기
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