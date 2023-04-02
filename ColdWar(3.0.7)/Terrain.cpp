#include "main.h"
#include "Terrain.h"
#include "GameApp.h"
#include "CameraManager.h"
#include "WorldManager.h"
#include "NiCollisionUtils.h"

unsigned int Terrain::ms_uiAlphaPreviewW = 32;
unsigned int Terrain::ms_uiAlphaPreviewH = 32;

Terrain *Terrain::ms_pTerrain	=	NULL;

int	BlendTextureInfo::m_nCntBlendTexture = 0;


Terrain::Terrain( int nTileCount, int nCellCount, float fWidthSize, float fHeightSize )
{

	//--------------------------------------------------------------------------------
	//					Terrain ���� ���� ����
	//--------------------------------------------------------------------------------

	// ���� ���� ���� �ʱ�ȭ
	m_nCellCount	= nCellCount;						// ��	����
	m_nTileCount	= nTileCount;						// Ÿ�� ����
	m_nTriangles	= (m_nCellCount * m_nTileCount) * (m_nCellCount * m_nTileCount) * 2;	// ��ü �ﰢ�� ����	
	m_nTotalVtxs	= ((m_nTileCount * m_nCellCount) + 1) * ((m_nTileCount * m_nCellCount) + 1);
	m_nWidthVtxCount= m_nCellCount * m_nTileCount + 1;	// ���� ���ؽ� ����
	m_nWidthRect	= m_nCellCount * m_nTileCount;		// ���� �� ����
	m_fWidthSize	= fWidthSize;						// �� ���� ũ��
	m_fHeightSize	= fHeightSize;						// �� ���� ũ��
	m_SLODRange		= fWidthSize / 4.0f;				// SDLO ���� 

	m_fUVSpacing	= 1.0f / (float)nTileCount;

	m_fVtxWidthSpacing	= m_fWidthSize / ( m_nTileCount * m_nCellCount );
	m_fVtxHeightSpacing = m_fHeightSize / ( m_nTileCount * m_nCellCount );

	m_fWidthSizeOfTile  = static_cast<float>( m_fWidthSize / (float)nTileCount );		// Ÿ�� �ϳ��� ���� ����
	m_fHeightSizeOfTile = static_cast<float>( m_fHeightSize / (float)m_nTileCount );	// Ÿ�� �ϳ��� ���� ����
	
	//--------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------
	//					Texture ���� ���� ����
	//--------------------------------------------------------------------------------

	// ���ĸ� Width , Height
	m_fAlpahMapWidth	= fWidthSize	* 2 ;
	m_fAlpahMapHeight	= fHeightSize	* 2 ;

	m_nCurrentDrawMode	= -1;											// �����ؽ��� Draw ���
	m_nCntCheckPixel	= (int)((m_nCellCount * m_nCellCount) / 10) ;	// �����ؽ��� ��ŷ�� �ȼ��κ��� �˻��� �ȼ��� �ִ� ����
	m_fInsideLength		= m_fWidthSize / 30.0f;							// �����ؽ��� �׸��� ���� ���� �ִ� ������ ũ��
	m_fOutsideLength	= m_fWidthSize / 10.0f;							// �����ؽ��� �׸��� �ٱ��� ���� �ִ� ������ ũ��	

	m_nPixelSize = m_nCellCount * 10;	// ��	����

	m_fPixelSize = (float)m_fWidthSizeOfTile / (float)m_nPixelSize;
	m_nShaderMapCount	= 0;											// TextureBlendMaterial ShaderMap ����
	m_nCurrentShaderMap = -1;											// ������ ���̴� �� �ε��� 0 = R, 1 = G, 2 = B, 3 = A


	//--------------------------------------------------------------------------------

									
	//--------------------------------------------------------------------------------
	//					���� Class ���� ���� ����
	//--------------------------------------------------------------------------------

	// ���� Ŭ���� ������ �ʱ�ȭ
	m_pkD3DDevice			= NULL;
	m_pkD3DTexture			= NULL;

	m_pCamera				= NULL;
	m_pQuadTree				= NULL;
	m_pFrustum				= NULL;	
	m_pHeightMapPosition	= NULL;
	m_pHeightMapNormal		= NULL;
	m_pUsTriIndex			= NULL;

	m_pTileBoundColliInfo		= new BoundColliInfo[ static_cast<int>(m_nWidthRect * m_nWidthRect / 2) ];
	::ZeroMemory( m_pTileBoundColliInfo, sizeof(m_pTileBoundColliInfo) );

	m_pOjbectBoundColliInfo		= new BoundColliInfo[ static_cast<int>(m_nWidthRect * m_nWidthRect / 2) ];
	::ZeroMemory( m_pOjbectBoundColliInfo, sizeof(m_pOjbectBoundColliInfo) );

	m_pPick					= NULL;
	m_eFormat				= D3DFMT_UNKNOWN;

	m_pPlayerNode			= NULL;

	//--------------------------------------------------------------------------------	

	// ī�޶� ȸ�� ���
	NiMatrix3 kRotX, kRotZ;
	kRotX.MakeXRotation( -NI_HALF_PI );
	kRotZ.MakeZRotation( NI_HALF_PI );
	
	m_matCameraRot = kRotZ * kRotX;

}

void Terrain::CreatePick()
{
    m_pPick = NiNew NiPick;
    m_pPick->SetPickType(NiPick::FIND_FIRST);
	m_pPick->SetSortType(NiPick::NO_SORT);
    m_pPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
    m_pPick->SetFrontOnly(false);
    m_pPick->SetReturnTexture(false);
    m_pPick->SetReturnNormal(false);
    m_pPick->SetReturnColor(false);
//    m_pPick->SetTarget(m_spScene);
}

Terrain::~Terrain()
{
	D3D_POINTER_RELEASE(m_pkD3DDevice);
}

Terrain*	Terrain::CreateManager( int nTileCount, int nCellCount, float fWidthSize, float fHeightSize )
{

	if( !ms_pTerrain )
	{
		ms_pTerrain = NiNew Terrain( nTileCount, nCellCount, fWidthSize, fHeightSize );		
	}

	return ms_pTerrain;

}

Terrain*	Terrain::GetManager()
{
	return ms_pTerrain;
}

BOOL		Terrain::IsManager()
{
	return ms_pTerrain != NULL;
}

void		Terrain::ReleaseManager()
{
	if( ms_pTerrain )
	{
		ms_pTerrain->ReleaseObject();

		NiDelete ms_pTerrain;
		ms_pTerrain = NULL;		
	}
}

//---------------------HeightMap ���� ��� ������ġ ����------------------------------
void Terrain::SetHeightMap()
//------------------------------------------------------------------------------------
{
	float xOffset, zOffset;
	float xDim, zDim;	
	
	m_pHeightMapPosition	= NiNew NiPoint3[ m_nTotalVtxs ];
	m_pHeightMapNormal		= NiNew NiPoint3[ m_nTotalVtxs ];

	m_pUsTriIndex			= new unsigned int[ m_nTriangles * 3 ];//NiAlloc( unsigned short, m_nTriangles * 3 );
	::ZeroMemory( m_pUsTriIndex, sizeof(m_pUsTriIndex) );
		
	xOffset		= -m_fHeightSize * 0.5f;	// ����
	zOffset		= -m_fWidthSize  * 0.5f;	// �ʺ�

	xDim = static_cast<float>( m_fHeightSize / ( m_nCellCount * m_nTileCount) );
	zDim = static_cast<float>( m_fWidthSize  / ( m_nCellCount * m_nTileCount) );

	int nCount = (m_nCellCount * m_nTileCount);
	int index;
	for( int x = 0; x <= nCount ; x++ )
	{
		for( int z = 0; z <= nCount ; z++ )
		{
			index = x * m_nWidthVtxCount + z;

			m_pHeightMapPosition[ index ].x	= zOffset + ( z * zDim );
			m_pHeightMapPosition[ index ].y	= xOffset + ( x * xDim );
			m_pHeightMapPosition[ index ].z	= 0.0f;

			m_pHeightMapNormal[ index ].x = 0.0f;
			m_pHeightMapNormal[ index ].y = 0.0f;
			m_pHeightMapNormal[ index ].z = 1.0f;

			m_pHeightMapNormal[ index ].Unitize();
		}
	}

	index = 0;
	for(  int y = 0; y < m_nWidthRect ; y++ )
	{
		for( int x = 0; x < m_nWidthRect ; x++ )
		{
			m_pUsTriIndex[ index++ ] = (y + 1)	* m_nWidthVtxCount + x + 1; 
			m_pUsTriIndex[ index++ ] = (y + 1)	* m_nWidthVtxCount + x;
			m_pUsTriIndex[ index++ ] = y		* m_nWidthVtxCount + x;
			
			m_pUsTriIndex[ index++ ] = y		* m_nWidthVtxCount + x + 1; 
			m_pUsTriIndex[ index++ ] = (y + 1)	* m_nWidthVtxCount + x + 1;
			m_pUsTriIndex[ index++ ] = y		* m_nWidthVtxCount + x;
		}
	}
/*
	m_pHeightData = NiNew NiTriShapeData( nTotalVtx, pHeightMapPosition, pHeightMapNormal, NULL, NULL, 1, NiGeometryData::NBT_METHOD_NONE , m_nTriangles, pUsTriIndex );

	m_pHeightMapPosition	= m_pHeightData->GetVertices();
	m_pHeightMapNormal		= m_pHeightData->GetNormals();
*/

}

//---------------���� ���� �� �ʱ�ȭ, ����Ʈ��, ����ü ����---------------------------
BOOL Terrain::InitTerrain( NiRenderer *pRenderer )
//------------------------------------------------------------------------------------
{
	if( NULL == pRenderer )
		return FALSE;
	
	m_pRenderer = (NiDX9RendererEx*)pRenderer;

	m_pkD3DDevice = m_pRenderer->GetD3DDevice();	

	assert(m_pkD3DDevice);
	D3D_POINTER_REFERENCE(m_pkD3DDevice);

	// Prepare scene for UpdateSelected
    bool bUpdate = true;
    bool bRigid = false;    

	m_spTerrainRoot = NiTCreate<NiNode>();	

	if( m_spTerrainRoot == NULL )
		return FALSE;

	// ��ü �� ���� ����
	SetHeightMap();

	// SLOD �ε��� ���� ����
	if( !InitIB() )
	{
		::MessageBox(NULL, " Create InitIB() Failed", "Failed", MB_OK );
		return FALSE;
	}

	// Ÿ�Ϻ� ���ؽ� ���� ����
	if( !InitVBTile() )
	{
		::MessageBox(NULL, " Create InitVBTile() Failed", "Failed", MB_OK );
		return FALSE;
	}	

	// ����Ʈ�� ����
	if( NULL == m_pQuadTree )
	{
		m_pQuadTree = NiNew QuadTree( this );
		m_pQuadTree->GnerateQuadTree( this );
	}

	// ����ü ����
	if( NULL == m_pFrustum )
	{
		m_pFrustum = NiNew FrustumEx( CameraManager::Get()->GetCamera() );		
	}

	// ��ŷ ���� ����
	if( NULL == m_pPick ) CreatePick();

	// �⺻ �ؽ��� ��� ���� �� ����
/*
	if( !m_spTex )
	{
		m_spTex = NiNew NiTexturingProperty();
		m_spTex->SetBaseFilterMode( NiTexturingProperty::FILTER_TRILERP );
		m_spTex->SetBaseClampMode( NiTexturingProperty::CLAMP_MAX_MODES );
		m_spTex->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );

		m_spTerrainRoot->AttachProperty( m_spTex ); 
	}
*/
	// ���̴� �ؽ��� ���÷����� ���� CreateTextureBlendMaterial�� ���� �� ����
	if( CreateTextureBlendMaterial() )
	{
	}

	
	//	m_spTerrainRoot->SetSelectiveUpdateFlags(bUpdate, true, bRigid);
	m_spTerrainRoot->Update(0.0f);
	m_spTerrainRoot->UpdateNodeBound();
	m_spTerrainRoot->UpdateProperties();
	m_spTerrainRoot->UpdateEffects();
	


	return TRUE;
}

//----------- Ÿ�ϴ� VertexBuffer ���� �� �������� ����---------------
BOOL Terrain::InitVBTile()
//--------------------------------------------------------------------
{
	Tile* tempTile = NULL;
	float xTileSize, yTileSize;
	float xStartPos, yStartPos;
	float xPos, yPos;
	float tUOffset, tVOffset;
	float tUStartPos, tVStartPos;
	float tUVSpacing;

	xStartPos = -(m_fWidthSize / 2.0f);	// �ʺ�
	yStartPos = -(m_fHeightSize/ 2.0f);	// ����	

	xTileSize = m_fHeightSize / m_nTileCount;
	yTileSize = m_fWidthSize / m_nTileCount;

	// ���ĸ� �ؽ��� UV Ÿ�Ϻ� ����
	tUOffset = 1.0f / m_nTileCount;
	tVOffset = 1.0f / m_nTileCount;	

	tUVSpacing = tUOffset / m_nCellCount;

	for( int y = 0; y < m_nTileCount; y++ )		// ����
	{
		for( int x = 0; x < m_nTileCount; x++ ) // �ʺ�
		{
			xPos = xStartPos + ( x * xTileSize );
			yPos = yStartPos + ( y * yTileSize );

			tUStartPos = tUOffset * x;	
			tVStartPos = 1.0f - (tVOffset * y);
			
			tempTile = NiNew Tile();
			tempTile->CreateGeometry( m_nCellCount, m_nTileCount, xTileSize , yTileSize, xPos, yPos, x, y, tUVSpacing, tUStartPos, tVStartPos , m_spTerrainRoot, m_pRenderer );

			m_vecTiles.push_back( tempTile );
		}
	}

	// ���� ����
	NiAlphaProperty *pAlpha = NiNew NiAlphaProperty();
	pAlpha->SetAlphaBlending(true);
	pAlpha->SetAlphaTesting(true);
	pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	m_spTerrainRoot->AttachProperty( pAlpha );

	/*
	CGameApp::SetMediaPath( "Data/Texture/" );



	// ���ؽ� �÷� ����

	NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty;
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
	m_spTerrainRoot->AttachProperty( pkVCProp );
	

	NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("a.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
	pkTexProp->SetBaseClampMode( NiTexturingProperty::WRAP_S_WRAP_T );
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);

	*/

//	((GameApp*)CGameApp::GetApp())->SetMaterialColor( NiColor( 0.7f, 0.7f, 0.7f ), NiColor( 0.7f, 0.7f, 0.7f ), NiColor( 0.0f, 0.0f, 0.0f ), 0.5f );

	
	//	((GameApp*)CGameApp::GetApp())->SetMaterialProperty( m_spTerrainRoot );

	return TRUE;
}

//------------------------SLOD�� ���� �ε��� ���� ����--------------------------------
BOOL Terrain::InitIB()
//------------------------------------------------------------------------------------
{

	int nMaxLevel = 1;
	int tempCellCount = m_nCellCount;

	// Max SLOD Level ���
	while( tempCellCount != 1)
	{
		tempCellCount /= 2;
		nMaxLevel++;
	}

	m_maxSLODLevel = nMaxLevel;
	
	m_pIB = new LPDIRECT3DINDEXBUFFER9[ nMaxLevel ][ 9 ];	

	for( int i = 0; i < nMaxLevel; i++ )
	{
		if( !SetSLODIB( i ) )
		{
			::MessageBox(NULL, " Create InitVBTile() Failed", "Failed", MB_OK );
			return FALSE;
		}
	}	

	return TRUE;
}

// ���� 0 �Ͻ� ���� ����
// ������ �������� �ﰢ�� ������ ������
//------------------------SLOD�� ���� �ε��� ���� ����--------------------------------
BOOL Terrain::SetSLODIB( int nSLODLevel )
//------------------------------------------------------------------------------------
{
	if( m_pRenderer == NULL )
		return FALSE;

	NiDX9IndexBufferManager* pkIBManager = NULL;		// ���ؽ� ���� ������ 
	
	unsigned short *index = NULL;
	unsigned int	usIBSize = 0;
	unsigned short rectIndex[9];
	int count = 0;	
	bool bCrack = false;

	pkIBManager = m_pRenderer->GetIBManager();

	if( NULL == pkIBManager )
		return FALSE;

	int vtxSpacing	= (int)pow( (double)2, (double)nSLODLevel );				// ������ ���ؽ� ���� ����
	int vtxCenter	= vtxSpacing / 2;											// ũ�� ������ ���� Cell �߾� ��ġ
	int cntSLODCell = m_nCellCount / (int)pow( (double)2, (double)nSLODLevel );		// ������ ���� or ���� �� ����

	// TriangleList Default Size ������ �ﰢ�� ����;
	int nTriangles  = (m_nCellCount / (int)pow((double)2, (double)nSLODLevel))*(m_nCellCount / (int)pow((double)2, (double)nSLODLevel)) * 2;
	int nCntTriangles;

	for( int nCrack = 0; nCrack< 9; nCrack++ )
	{
		nCntTriangles = nTriangles;		
		
		// ���� 0�϶� ũ���� �����Ƿ� NULL�� ����
		if( nSLODLevel == 0 && nCrack >= 1 )
		{
			m_vecIBInfos.push_back( IBInfo( nCntTriangles * 3, nCntTriangles * 3 * sizeof(short)));
			m_pIB[ nSLODLevel ][ nCrack ] = NULL;
			continue;
		}

		// ũ���� �߰��Ǵ� �ﰢ�� ����
		if( nCrack >= SLOD_LEFT && nCrack <= SLOD_BOTTOM )
		{
			nCntTriangles += (m_nCellCount / (int)pow( (double)2, (double)nSLODLevel ) * 3);
		}
		
		if( nCrack >= SLOD_LEFT_BOTTOM && nCrack <= SLOD_RIGHT_BOTTOM )
		{
			nCntTriangles += (m_nCellCount / (int)pow( (double)2, (double)nSLODLevel ) * 3 * 2) - 2;
		}

		index = new unsigned short[ nCntTriangles * 3 ];			
		count = 0;

		for( int z = 0; z < cntSLODCell ; z++ )
		{
			for( int x = 0; x < cntSLODCell; x++ )
			{
				bCrack = false;

				rectIndex[ 0 ] = (z * vtxSpacing * (m_nCellCount+1))				+ ( x		* vtxSpacing );
				rectIndex[ 1 ] = (z * vtxSpacing * (m_nCellCount+1))				+ ( x		* vtxSpacing + vtxCenter);
				rectIndex[ 2 ] = (z * vtxSpacing * (m_nCellCount+1))				+ ((x+1)	* vtxSpacing );

				rectIndex[ 3 ] = ((z * vtxSpacing + vtxCenter) * (m_nCellCount+1))	+ ( x		* vtxSpacing );
				rectIndex[ 4 ] = ((z * vtxSpacing + vtxCenter) * (m_nCellCount+1))	+ ( x		* vtxSpacing + vtxCenter);
				rectIndex[ 5 ] = ((z * vtxSpacing + vtxCenter) * (m_nCellCount+1))	+ ((x+1)	* vtxSpacing );

				rectIndex[ 6 ] = ((z+1) * vtxSpacing * (m_nCellCount+1))			+ ( x		* vtxSpacing );
				rectIndex[ 7 ] = ((z+1) * vtxSpacing * (m_nCellCount+1))			+ ( x		* vtxSpacing + vtxCenter);
				rectIndex[ 8 ] = ((z+1) * vtxSpacing * (m_nCellCount+1))			+ ((x+1)	* vtxSpacing );

				//		6����7����8
				//	    ��  ��   ��
				//		��  ��   ��
				//		3����4����5
				//		��  ��   ��
				//		��  ��   ��
				//		0����1����2

				//		3����4
				//		��  ��
				//		��  ��
				//		0����1
				// ���ε� ���� : �ݽð� ����
				// 4 3 0 ,      1 4 0

				// 3 -- 4 
				// | /
				// 0

				//      4
				//   /  |
				// 0 -- 1
				
				switch( nCrack )
				{

				case SLOD_LEFT:													//
																				//
					if( x == 0 )												//
					{															//
						index[ count++ ] = rectIndex[ 4 ];						//
						index[ count++ ] = rectIndex[ 0 ];						//
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 3 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 3 ];
						index[ count++ ] = rectIndex[ 0 ];

						bCrack = true;
					}
					else
						bCrack = false;

					break;

				case SLOD_TOP:

					if( z == cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];
						
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];
						
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 7 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 7 ];
						
						bCrack = true;
					}
					else
						bCrack = false;

					break;

				case SLOD_RIGHT:

					if( x == cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 5 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 5 ];
					
						bCrack = true;
					}
					else
						bCrack = false;

					break;
				
				case SLOD_BOTTOM:

					if( z == 0 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 1 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 1 ];
					
						bCrack = true;
					}
					else
						bCrack = false;

					break;

				case SLOD_LEFT_BOTTOM:

					//��
					//��
					//��
					//�����
					//
					// ���ο�
					if( z == 0 && x != 0)
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 1 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 1 ];
					
						bCrack = true;
					}
					// ���ο�
					else if( x == 0 && z != 0)
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 3 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 3 ];
					
						bCrack = true;
					}
					else if( x == 0 && z == 0 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 1 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 1 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 3 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 3 ];

						bCrack = true;
					}
					else 
						bCrack = false;



					break;

				case SLOD_RIGHT_TOP:
				
					//�����
					//      ��
					//      ��
					//      ��
					//

					// ���� ��
					if( z == cntSLODCell - 1 && x != cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 7 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 7 ];
					
						bCrack = true;
					}
					// ���� ��
					else if( x == cntSLODCell - 1 && z != cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 5 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 5 ];
					
						bCrack = true;
					}
					else if( x == cntSLODCell - 1 && z == cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 7 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 7 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 5 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 5 ];

						bCrack = true;
					}
					else 
						bCrack = false;


					break;

				case SLOD_LEFT_TOP:

					//�����
					//��
					//��
					//��
					//

					// ���� ��
					if( z == cntSLODCell - 1 && x != 0 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 7 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 7 ];
					
						bCrack = true;
					}
					// ���� ��
					else if( x == 0 && z != cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 3 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 3 ];
					
						bCrack = true;
					}
					else if( x == 0 && z == cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 3 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 3 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 7 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 7 ];

						bCrack = true;
					}
					else 
						bCrack = false;

					break;

				case SLOD_RIGHT_BOTTOM:

					//      ��
					//      ��
					//      ��
					//�����
					//

					// ���� ��
					if( z == 0 && x != cntSLODCell - 1 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 1 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 1 ];
					
						bCrack = true;
					}
					// ���� ��
					else if( x == cntSLODCell - 1 && z != 0 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 5 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 5 ];
					
						bCrack = true;
					}
					else if( x == cntSLODCell - 1 && z == 0 )
					{
						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 6 ];
						index[ count++ ] = rectIndex[ 0 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 8 ];
						index[ count++ ] = rectIndex[ 6 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 5 ];
						index[ count++ ] = rectIndex[ 8 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 2 ];
						index[ count++ ] = rectIndex[ 5 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 1 ];
						index[ count++ ] = rectIndex[ 2 ];

						index[ count++ ] = rectIndex[ 4 ];
						index[ count++ ] = rectIndex[ 0 ];
						index[ count++ ] = rectIndex[ 1 ];

						bCrack = true;
					}
					else 
						bCrack = false;

					break;

				case SLOD_DEFAULT:

					index[ count++ ] = rectIndex[ 8 ];
					index[ count++ ] = rectIndex[ 6 ];
					index[ count++ ] = rectIndex[ 0 ];
					
					index[ count++ ] = rectIndex[ 2 ];
					index[ count++ ] = rectIndex[ 8 ];
					index[ count++ ] = rectIndex[ 0 ];

					bCrack = true;

					break;
				}

				if( !bCrack )
				{
					index[ count++ ] = rectIndex[ 8 ];
					index[ count++ ] = rectIndex[ 6 ];
					index[ count++ ] = rectIndex[ 0 ];
					
					index[ count++ ] = rectIndex[ 2 ];
					index[ count++ ] = rectIndex[ 8 ];
					index[ count++ ] = rectIndex[ 0 ];
				}
			}
		}
		
		//CraeteIndexBuffer - ù��° ���� �ﰢ�� ���� * �ﰢ���� �ε��� ����(3) * �ε��� ������(2Byte);
		usIBSize = nCntTriangles * 3 * sizeof(unsigned short);
		
		m_pIB[ nSLODLevel ][ nCrack ] = pkIBManager->PackBuffer( index, nCntTriangles * 3, nCntTriangles * 3, NULL, usIBSize, D3DPOOL_MANAGED,  D3DUSAGE_WRITEONLY );
		
		m_vecIBInfos.push_back( IBInfo( nCntTriangles * 3, usIBSize ) );

		if( NULL == m_pIB[ nSLODLevel ][ nCrack ] )
		{
			delete[] index;
			return FALSE;
		}

		if( NULL != index )
		{
			delete[] index;
		}
	}

	return TRUE;
}


//---------------- SLOD ������ Crack Ÿ���� ���� ----------------
void Terrain::SetSLODLevel()
//---------------------------------------------------------------
{
	if( m_vecValidTilesNum.empty() )
		return;

	NiPoint3 vTileCenterPos;		
	NiPoint3 vCameraPos, vDiff;
	float fLen = FLT_MAX;	
	int xOffset, zOffset;
	int slodLevel = 0;	
	int crackType;

	std::vector< int >::iterator iterTilesNum;

	if( m_pPlayerNode )
	{
		vCameraPos = m_pPlayerNode->GetTranslate();
	//	vCameraPos = m_matCameraRot * vCameraPos;
		vCameraPos.z = 0.0f;
	}
	
	// -----------------------Ÿ�Ϻ��� ������ ����------------------------------------------------------ 

	for( iterTilesNum = m_vecValidTilesNum.begin(); iterTilesNum !=m_vecValidTilesNum.end(); ++iterTilesNum )
	{
		vTileCenterPos = m_vecTiles[ *iterTilesNum ]->GetCenterPos();		
		vTileCenterPos.z =0.0f;
		m_vecTiles[ *iterTilesNum ]->SetCulled( false );
//		ZFLog::g_sLog->Log( "Ÿ�� ���� : %d ��",  (int)m_vecValidTilesNum.size() );


		vDiff = vCameraPos - vTileCenterPos;
		fLen = NiAbs(vDiff.Length());

		for( int i =0; i < m_maxSLODLevel; i++ )
		{
			if( i * m_SLODRange <= fLen )
				m_vecTiles[ *iterTilesNum ]->SetLodLevel( i );
			else
				break;	
		}			
	}

	// -----------------------Ÿ�Ϻ��� �ڽ��� ���� Ÿ���� CrackType Bit ���� ����-----------------------

	for( iterTilesNum = m_vecValidTilesNum.begin(); iterTilesNum !=m_vecValidTilesNum.end(); ++iterTilesNum )
	{
		slodLevel = m_vecTiles[ *iterTilesNum ]->GetLodLevel();

		
		// Ÿ���� x, z ��ġ���� ��´�.
		xOffset		= (*iterTilesNum) % m_nTileCount;
		zOffset		= (*iterTilesNum) / m_nTileCount;

		// Left Tile �˻�     BOTTOM | RIGHT | TOP | LEFT 0 0 0 0
		if( xOffset - 1 >= 0 )
		{		
			// ���� Ÿ���� ���� Ÿ�Ϻ��� ������ ũ����
			if( m_vecTiles[ zOffset * m_nTileCount + (xOffset - 1) ]->GetLodLevel() > slodLevel )
				m_vecTiles[ zOffset * m_nTileCount + (xOffset - 1) ]->SetBitCrackType( SLOD_RIGHT - 1);			
		}

		// Right Tile �˻�
		if( xOffset + 1 <= m_nTileCount - 1 )
		{
			if( m_vecTiles[ zOffset * m_nTileCount + (xOffset + 1) ]->GetLodLevel() > slodLevel )
				m_vecTiles[ zOffset * m_nTileCount + (xOffset + 1) ]->SetBitCrackType( SLOD_LEFT - 1);			
		}

		// Top Tile �˻�
		if( zOffset + 1 <= m_nTileCount - 1 )
		{
			if( m_vecTiles[ (zOffset + 1) * m_nTileCount + xOffset ]->GetLodLevel() > slodLevel )
				m_vecTiles[ (zOffset + 1) * m_nTileCount + xOffset ]->SetBitCrackType( SLOD_BOTTOM - 1);			
		}

		// Bottom Tile �˻�
		if( zOffset - 1 >= 0 )
		{
			if( m_vecTiles[ (zOffset - 1) * m_nTileCount + xOffset ]->GetLodLevel() > slodLevel )
				m_vecTiles[ (zOffset - 1) * m_nTileCount + xOffset ]->SetBitCrackType( SLOD_TOP - 1);			
		}

	}

	
	for( iterTilesNum = m_vecValidTilesNum.begin(); iterTilesNum !=m_vecValidTilesNum.end(); ++iterTilesNum )
	{
		slodLevel = m_vecTiles[ *iterTilesNum ]->GetLodLevel();

		switch( m_vecTiles[ *iterTilesNum ]->GetBitSetValue() ) 
		{
		// ��Ʈ���� 0�̸� �⺻ ũ����
			
		//  1��° bit LEFT
		//  2��° bit TOP
		//  3��° bit RIGHT
		//  4��° bit BOTTOM

//----------------------------------------------------------
		case 0 :
			crackType = SLOD_DEFAULT;
			break;

		case 1 :
			crackType = SLOD_LEFT;
			break;

		case 2 :
			crackType = SLOD_TOP;
			break;

		case 4 :
			crackType = SLOD_RIGHT;
			break;

		case 8 :
			crackType = SLOD_BOTTOM;
			break;
//----------------------------------------------------------
		case 3 :
			crackType = SLOD_LEFT_TOP;
			break;

		case 6 :
			crackType = SLOD_RIGHT_TOP;
			break;

		case 9 :
			crackType = SLOD_LEFT_BOTTOM;
			break;	

		case 12 :
			crackType = SLOD_RIGHT_BOTTOM;
			break;

//----------------------------------------------------------
		}
		
		IBInfo &tempIBInfo = m_vecIBInfos[ 9 * slodLevel + crackType ];

		m_vecTiles[ *iterTilesNum ]->SetIB( m_pIB[ slodLevel ][ crackType ], tempIBInfo.m_uiIndexCount, tempIBInfo.m_usIBSize );

	}	

}

//		��ŷ�� ������ ��ġ vector�� ��´�.
BOOL	Terrain::PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction, NiPoint3 *vPickPos, NiPoint3 *vPickNor, NiPoint3 *pPickedVtxs,
								
								int &nPickedTileNum, int &xTriIndex, int &yTriIndex, int &nPickPlace  )
{
	BOOL bTilePick	= FALSE, bObjectPick = FALSE;
	float fDist		= FLT_MAX;
//	float fMinDist	= FLT_MAX;
	float fDistForTile = FLT_MAX;
	float fDistForObject = FLT_MAX;
	int nTileIndex = 0;
	int nCntTiles = 0, nCntObjetGroups = 0;
	unsigned short min;
	NiPoint3 vTilePickPos, vTilePickNor, vObjectPickPos, vObjectPisNor;
	NiPoint3 pVtx[3];// pMinVtx[3];

	BoundColliInfo stTmpCollision;
	::ZeroMemory( m_pTileBoundColliInfo, sizeof(m_pTileBoundColliInfo) );
	::ZeroMemory( m_pOjbectBoundColliInfo, sizeof(m_pOjbectBoundColliInfo) );
	

//	std::vector< int > vecTileNum;
	std::vector< int >::iterator iterTileNum;		


	//------------------------------------------------------------------------------------------------------------
	//
	//						Triangle ��ŷ Data ����
	//
	//------------------------------------------------------------------------------------------------------------

	// �ٿ�� �ڽ��� Ray�� �浹�� ��� Ÿ�ϵ��� ���Ѵ�.
	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );
	m_pPick->SetReturnNormal( true );
	m_pPick->SetFrontOnly( false );


	for( iterTileNum = m_vecValidTilesNum.begin(); iterTileNum != m_vecValidTilesNum.end(); ++iterTileNum )
	{
		nTileIndex = *iterTileNum;
		// ���� �������� ���� ���� ������ ���ؼ��� �˻縦 �ǽ��Ѵ�.
		if( m_vecTiles[ nTileIndex]->GetDistance( pos ) <= PICKING_LIMIT_DISTANCE )
		{
			// �ʱ� �ٿ�� ���������� �˻縦 �ǽ��Ͽ� ���� �浹�� Ÿ�ϵ� ����Ʈ ����
			if( m_vecTiles[ nTileIndex ]->CollisionBB( pos, direction, m_pPick, &fDist ) )
			{
				// �Ÿ������� ������ ���� �ڷᱸ���� �߰��Ѵ�. .
				m_pTileBoundColliInfo[ nCntTiles ].m_usTileNum = *iterTileNum;
				m_pTileBoundColliInfo[ nCntTiles ].m_fDistance = fDist;
				++nCntTiles;
			}

			// ������Ʈ �׷��� �ִ� ��쿡�� ����
			if( m_vecTiles[ nTileIndex ]->IsInitStaticAtChunk() )
			{
				m_pOjbectBoundColliInfo[ nCntObjetGroups ].m_usTileNum = *iterTileNum;
				m_pOjbectBoundColliInfo[ nCntObjetGroups ].m_fDistance = fDist;
				++nCntObjetGroups;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------
	//
	//												Ÿ�� ��ŷ
	//
	//------------------------------------------------------------------------------------------------------------

	// �浹�� Ÿ���� �ִٸ�
	if( nCntTiles )
	{
		// ���� ���� �Ͽ� �浹�� �ٿ�� �ڽ� Ÿ�ϵ��� �����Ѵ�.
		for( unsigned short i = 0; i < nCntTiles - 1; i++ )
		{
			min = i;
			for( unsigned short j = i+1; j < nCntTiles; j++ )
			{
				if( m_pTileBoundColliInfo[ min ].m_fDistance > m_pTileBoundColliInfo[ j ].m_fDistance )
				{
					min = j;
				}
			}

			stTmpCollision					=  m_pTileBoundColliInfo[ min ];
			m_pTileBoundColliInfo[ min ]	= m_pTileBoundColliInfo[ i ]; 
			m_pTileBoundColliInfo[ i ]		= stTmpCollision; 

		}

		m_pPick->SetFrontOnly( true );
		m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );
		m_pPick->SetCoordinateType( NiPick::MODEL_COORDINATES );

//		int xIndex, zIndex;
		// ���� ����� Ÿ�ϵ�� ���� �浹 ���� üũ�ؼ� �浹�Ǹ� �ش� Ÿ���� ���� �浹 Ÿ���̴�.
		for( unsigned short i = 0; i < nCntTiles; i++ )
		{
			// ù��° �浹�� Ÿ���� ��ġ������ �����ؼ� ����
			if( m_vecTiles[ m_pTileBoundColliInfo[ i ].m_usTileNum ]->GetPickPosition( pos, direction, &vTilePickPos, &vTilePickNor,  
				pPickedVtxs, m_pPick, &fDistForTile, xTriIndex, yTriIndex ) )
			{
				nPickedTileNum = m_pTileBoundColliInfo[ i ].m_usTileNum;
				bTilePick = TRUE;
				break;
			}
		}
	}


	//------------------------------------------------------------------------------------------------------------
	//
	//								������Ʈ �׷쿡 ���� ��ŷ
	//
	//------------------------------------------------------------------------------------------------------------

	if( nCntObjetGroups )
	{	
		m_pPick->SetFrontOnly( true );
		m_pPick->SetCoordinateType( NiPick::WORLD_COORDINATES );
		
		// ���� ���� �Ͽ� �浹�� �ٿ�� �ڽ� Ÿ�ϵ��� �����Ѵ�.
		for( unsigned short i = 0; i < nCntObjetGroups - 1; i++ )
		{
			min = i;
			for( unsigned short j = i+1; j < nCntObjetGroups; j++ )
			{
				if( m_pOjbectBoundColliInfo[ min ].m_fDistance > m_pOjbectBoundColliInfo[ j ].m_fDistance )
				{
					min = j;
				}
			}

			stTmpCollision					= m_pOjbectBoundColliInfo[ min ];
			m_pOjbectBoundColliInfo[ min ]	= m_pOjbectBoundColliInfo[ i ]; 
			m_pOjbectBoundColliInfo[ i ]	= stTmpCollision; 
		}

		
		// ���� ����� Ÿ�ϵ�� ���� �浹 ���� üũ�ؼ� �浹�Ǹ� �ش� Ÿ���� ���� �浹 Ÿ���̴�.
		for( unsigned short i = 0; i < nCntObjetGroups; i++ )
		{
			// ù��° �浹�� Ÿ���� ��ġ������ �����ؼ� ����
			if( m_vecTiles[ m_pOjbectBoundColliInfo[ i ].m_usTileNum ]->PickRayAtObjectGroup( pos, direction, &vObjectPickPos, &vObjectPisNor, m_pPick, &fDistForTile ) )
			{
				bObjectPick = TRUE;
				break;
			}
		}
	}

	m_pPick->SetReturnNormal( false );
	//----------------------------------------------------------------------------------------------------
	//
	//	����, ������Ʈ�� ���� Decal�� �ٸ��� �����ϹǷ� �Ÿ��� ���� ��Ȯ�� �����Ͽ� ó���Ѵ�.
	//
	//	DecalManager::OUTDOOR = ����(Terrain) , DecalManager::STATIC_OBJECT = ������Ʈ
	//----------------------------------------------------------------------------------------------------
	if( bTilePick && bObjectPick )
	{
		if( fDistForTile > fDistForTile )
		{
			*vPickPos = vTilePickPos;
			*vPickNor = vTilePickNor;
			nPickPlace = DecalManager::OUTDOOR;
		}
		else
		{
			*vPickPos = vObjectPickPos;
			*vPickNor = vObjectPisNor;
			nPickPlace = DecalManager::STATIC_OBJECT;
		}

		return TRUE;
	}
	else if( bTilePick && !bObjectPick )
	{
		*vPickPos = vTilePickPos;
		*vPickNor = vTilePickNor;
		nPickPlace = DecalManager::OUTDOOR;

		return TRUE;
	}
	else if( !bTilePick && bObjectPick )
	{
		*vPickPos = vObjectPickPos;
		*vPickNor = vObjectPisNor;
		nPickPlace = DecalManager::STATIC_OBJECT;

		return TRUE;
	}

	return FALSE;

	/*
	NiPoint3 vPickPos, vNearPickPos;
	int xIndex, zIndex;

	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );

	for( iterTileNum = vecTileNum.begin(); iterTileNum != vecTileNum.end(); ++iterTileNum )
	{
		if( m_vecTiles[ *iterTileNum ]->GetPickPosition( pos, direction, &vPickPos, m_pPick, &fDist, &xIndex, &zIndex ) )
		{
			if( fMinDist > fDist )
			{
				fMinDist	 = fDist;
				vNearPickPos = vPickPos;
				bPick		 = TRUE;
			}
		}
	}
	*/
	// ���� ����� ��ġ ����
//	*PickPos = vNearPickPos;

//	return bPick;
}

//		���� ��ŷ ������ ���.
BOOL	Terrain::PickRayAtScene( const NiPoint3& pos, const NiPoint3& direction,NiPoint3& pkPickPoint )
{

	const NiPoint3 &vCharPos = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter()->GetCharRoot()->GetTranslate();

	BOOL bTilePick		= FALSE, bObjectPick = FALSE;
	float fDist			= FLT_MAX, fDist1 = FLT_MAX;
	float fDistForTile	= FLT_MAX;
	float fDistForObject = FLT_MAX;
	int nTileIndex		= 0;
	int nCntTiles = 0, nCntObjetGroups = 0;
	unsigned short min;
	NiPoint3 vTilePickPos, vTilePickNor, vObjectPickPos, vObjectPisNor, pVtx[3]; ;
	
	NiPoint2 startPos( pos.x, pos.y ), endPos( pos.x + direction.x * PICKING_LIMIT_DISTANCE, pos.y + direction.y * PICKING_LIMIT_DISTANCE );

	// pMinVtx[3];
	int nPickedTileNum	= 0, nPickedObjectTileNum = 0;
	int xTriIndex = 0, yTriIndex = 0;
	
	BoundColliInfo stTmpCollision;
	::ZeroMemory( m_pTileBoundColliInfo, sizeof(m_pTileBoundColliInfo) );
	::ZeroMemory( m_pOjbectBoundColliInfo, sizeof(m_pOjbectBoundColliInfo) );
	

	std::vector< int >::iterator iterTileNum;	

	// �ٿ�� �ڽ��� Ray�� �浹�� ��� Ÿ�ϵ��� ���Ѵ�.
	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetIntersectType( NiPick::BOUND_INTERSECT );
	m_pPick->SetCoordinateType( NiPick::WORLD_COORDINATES );
	m_pPick->SetReturnNormal( false );

	//------------------------------------------------------------------------------------------------------------
	//
	//						Triangle ��ŷ Data ����
	//
	//------------------------------------------------------------------------------------------------------------
	float fdis;
	int nType = 0;
	for( iterTileNum = m_vecValidTilesNum.begin(); iterTileNum != m_vecValidTilesNum.end(); ++iterTileNum )
	{
		nTileIndex = *iterTileNum;
		// ���� �������� ���� ���� ������ ���ؼ��� �˻縦 �ǽ��Ѵ�.
		fdis = m_vecTiles[ nTileIndex]->GetDistance( vCharPos );

		if( NiAbs(m_vecTiles[ nTileIndex]->GetDistance( vCharPos )) <= PICKING_LIMIT_DISTANCE )
		{
			nType = 0;
			// �ʱ� �ٿ�� ���������� �˻縦 �ǽ��Ͽ� ���� �浹�� Ÿ�ϵ� ����Ʈ ����
			if( m_vecTiles[ nTileIndex ]->CollisionTialeAntObjectBB( pos, direction, startPos, endPos, m_pPick, &fDist, &fDist1, &nType ) )
			{
				if( nType == TILE_PICK  || nType == OBJECT_TILE_PICK)
				{
					m_pTileBoundColliInfo[ nCntTiles ].m_usTileNum = *iterTileNum;
					m_pTileBoundColliInfo[ nCntTiles ].m_fDistance = fDist;
					++nCntTiles;
				}
			
				if( nType >= OBJECT_PICK )
				{
					if( m_vecTiles[ nTileIndex ]->IsInitStaticAtChunk() )
					{
						m_pOjbectBoundColliInfo[ nCntObjetGroups ].m_usTileNum = *iterTileNum;
						m_pOjbectBoundColliInfo[ nCntObjetGroups ].m_fDistance = fDist1;
						++nCntObjetGroups;
					}
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------------------
	//
	//												Ÿ�� ��ŷ
	//
	//------------------------------------------------------------------------------------------------------------

	m_pPick->SetReturnNormal( true );

	// �浹�� �ٿ�� Ÿ���� �ִٸ�
	if( nCntTiles )
	{
		// ���� ���� �Ͽ� �浹�� �ٿ�� �ڽ� Ÿ�ϵ��� �����Ѵ�.
		for( unsigned short i = 0; i < nCntTiles - 1; i++ )
		{
			min = i;
			for( unsigned short j = i+1; j < nCntTiles; j++ )
			{
				if( m_pTileBoundColliInfo[ min ].m_fDistance > m_pTileBoundColliInfo[ j ].m_fDistance )
				{
					min = j;
				}
			}

			stTmpCollision					= m_pTileBoundColliInfo[ min ];
			m_pTileBoundColliInfo[ min ]	= m_pTileBoundColliInfo[ i ]; 
			m_pTileBoundColliInfo[ i ]		= stTmpCollision; 
		}

		m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );

		// ���� ����� Ÿ�ϵ�� ���� �浹 ���� üũ�ؼ� �浹�Ǹ� �ش� Ÿ���� ���� �浹 Ÿ���̴�.
		for( unsigned short i = 0; i < nCntTiles; i++ )
		{
			// ù��° �浹�� Ÿ���� ��ġ������ �����ؼ� ����
			if( m_vecTiles[ m_pTileBoundColliInfo[ i ].m_usTileNum ]->GetPickPosition( pos, direction, &vTilePickPos, &vTilePickNor,  
				pVtx, m_pPick, &fDistForTile, xTriIndex, yTriIndex ) )
			{
				nPickedTileNum = m_pTileBoundColliInfo[ i ].m_usTileNum;
				bTilePick = TRUE;
				break;
			}
		}
	}

	
	
	//------------------------------------------------------------------------------------------------------------
	//
	//								������Ʈ �׷쿡 ���� ��ŷ
	//
	//------------------------------------------------------------------------------------------------------------

	if( nCntObjetGroups )
	{
		m_pPick->SetFrontOnly( true);
		m_pPick->SetCoordinateType( NiPick::WORLD_COORDINATES );
		m_pPick->SetObserveAppCullFlag( false );
		m_pPick->SetIntersectType( NiPick::BOUND_INTERSECT );
		m_pPick->SetSortType( NiPick::NO_SORT );
		m_pPick->SetPickType( NiPick::FIND_FIRST );

		// ���� ���� �Ͽ� �浹�� �ٿ�� �ڽ� Ÿ�ϵ��� �����Ѵ�.
		for( unsigned short i = 0; i < nCntObjetGroups - 1; i++ )
		{
			min = i;

			for( unsigned short j = i+1; j < nCntObjetGroups; j++ )
			{
				if( m_pOjbectBoundColliInfo[ min ].m_fDistance > m_pOjbectBoundColliInfo[ j ].m_fDistance )
				{
					min = j;
				}
			}

			stTmpCollision					= m_pOjbectBoundColliInfo[ min ];
			m_pOjbectBoundColliInfo[ min ]	= m_pOjbectBoundColliInfo[ i ]; 
			m_pOjbectBoundColliInfo[ i ]	= stTmpCollision; 
		}

		// ���� ����� Ÿ�ϵ�� ���� �浹 ���� üũ�ؼ� �浹�Ǹ� �ش� Ÿ���� ���� �浹 Ÿ���̴�.
		for( unsigned short i = 0; i < nCntObjetGroups; i++ )
		{
			// ù��° �浹�� Ÿ���� ��ġ������ �����ؼ� ����
			if( m_vecTiles[ m_pOjbectBoundColliInfo[ i ].m_usTileNum ]->PickRayAtObjectGroup( pos, direction, &vObjectPickPos, &vObjectPisNor, m_pPick, &fDistForObject ) )
			{
				nPickedObjectTileNum = m_pOjbectBoundColliInfo[ i ].m_usTileNum;
				bObjectPick = TRUE;
				break;
			}
		}
	}

	m_pPick->SetReturnNormal( false );
	//----------------------------------------------------------------------------------------------------
	//
	//	����, ������Ʈ�� ���� Decal�� �ٸ��� �����ϹǷ� �Ÿ��� ���� ��Ȯ�� �����Ͽ� ó���Ѵ�.
	//
	//	DecalManager::OUTDOOR = ����(Terrain) , DecalManager::STATIC_OBJECT = ������Ʈ
	//----------------------------------------------------------------------------------------------------

	if( bTilePick && bObjectPick )
	{
		if( fDistForObject > fDistForTile )
		{
			AttachDecalAtTile( &vTilePickPos, &vTilePickNor, pVtx, nPickedTileNum, xTriIndex, yTriIndex );  
			CDustManager::GetInstance()->AddDust( vTilePickPos, vTilePickNor );
			pkPickPoint = vTilePickPos;	//�浹�� ����
		}
		else
		{
			AttachDecalOfStaticObject( &vObjectPickPos, &vObjectPisNor, nPickedObjectTileNum );
			CDustManager::GetInstance()->AddDust( vObjectPickPos, vObjectPisNor );
			pkPickPoint = vObjectPickPos;	//�浹�� ����
		}

		return TRUE;
	}
	else if( bTilePick && !bObjectPick )
	{
		AttachDecalAtTile( &vTilePickPos, &vTilePickNor, pVtx, nPickedTileNum, xTriIndex, yTriIndex );
		CDustManager::GetInstance()->AddDust( vTilePickPos, vTilePickNor );
		pkPickPoint = vTilePickPos;	//�浹�� ����
		
		return TRUE;
	}
	else if( !bTilePick && bObjectPick )
	{
		AttachDecalOfStaticObject( &vObjectPickPos, &vObjectPisNor, nPickedObjectTileNum );
		CDustManager::GetInstance()->AddDust( vObjectPickPos, vObjectPisNor );
		pkPickPoint = vObjectPickPos;	//�浹�� ����

		return TRUE;
	}

	return FALSE;
}

//		��ŷ�� ������ ��ġ�� �����带 �߰��Ѵ�.
BOOL	Terrain::PickTerrainAndAddBillBoard( const NiPoint3& pos, const NiPoint3& direction )
{
	BOOL bPick		= FALSE;
	float fDist		= FLT_MAX;
	float fMinDist	= FLT_MAX; 

	std::vector< int > vecTileNum;
	std::vector< int >::iterator iterTileNum;	

	// �ٿ�� �ڽ��� Ray�� �浹�� ��� Ÿ�ϵ��� ���Ѵ�.
	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetIntersectType( NiPick::BOUND_INTERSECT );

	for( iterTileNum = m_vecValidTilesNum.begin(); iterTileNum != m_vecValidTilesNum.end(); ++iterTileNum )
	{
		if( m_vecTiles[ *iterTileNum ]->CollisionBB( pos, direction, m_pPick, &fDist ) )
		{
			vecTileNum.push_back( *iterTileNum );
		}
	}

	// �浹�� Ÿ�ϵ��߿� ���� ����� Ÿ���� Vertex Locking �۾��� ���Ͽ� ���������� ���� ����� Ÿ���� ��ȣ�� ���Ѵ�.
	NiPoint3 vPickPos, vNearPickPos;
	int xIndex, zIndex;
	int selectedTileNum = -1;

	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );

	for( iterTileNum = vecTileNum.begin(); iterTileNum != vecTileNum.end(); ++iterTileNum )
	{
		if( m_vecTiles[ *iterTileNum ]->GetPickPosition( pos, direction, &vPickPos, m_pPick, &fDist, &xIndex, &zIndex ) )
		{
			if( fMinDist > fDist )
			{
				fMinDist	 = fDist;
				vNearPickPos = vPickPos;
				bPick		 = TRUE;
				selectedTileNum = *iterTileNum;
			}
		}
	}
	
	if( bPick &&  selectedTileNum != -1 )
	{
	//		������ ûũ�� Ÿ�Կ� ���� ������ ��ũ��Ʈ �߰�
		if( !m_vecTiles[ selectedTileNum ]->IsInitBillBoardAtChunk() )
		{
			if( m_vecTiles[ selectedTileNum ]->GenerateBillBoardAtChunk() )
			{
				if( !m_vecTiles[ selectedTileNum ]->PickBillBoard( pos, direction, m_pPick ) )
				{
					if( m_vecTiles[ selectedTileNum ]->AddBillBoardAtTile( vNearPickPos ) )
						return TRUE;
				}
			}
		}
		else
		{
			if( !m_vecTiles[ selectedTileNum ]->PickBillBoard( pos, direction, m_pPick ) )
			{
				if( m_vecTiles[ selectedTileNum ]->AddBillBoardAtTile( vNearPickPos ) )
					return TRUE;
			}
		}	
	}
	else
	{
		
		BillBoardManager::GetManager()->PickBillBoard( pos, direction );
	}
	
	return FALSE;	
}


//-----------------------------------------------------------------------------------------
//
//					������ Object�� �浹 ��ġ�� ����� ��´�.
//
//-----------------------------------------------------------------------------------------
BOOL Terrain::GetHeightPos( NiPoint3& vPos,  NiPoint3& vHeight, NiPoint3& vNormal )
{
	if( !m_pPick ) return FALSE;

	NiPoint3 &vRealPos = NiPoint3( vPos.x + m_fWidthSize / 2, vPos.y + m_fHeightSize / 2, vPos.z );

	NiPoint3 vPosition = vPos;
	vPosition.z += 100.0f;
	if( vPosition.z < 0.0f )
		vPosition.z *= -1.0f;
	// ���� ������ ��ġ�� �̿��Ͽ� �浹���θ� ������ Ÿ���� �ε����� �����Ѵ�.
//	int xIndex = static_cast<int>( vRealPos.x / m_fWidthSizeOfTile  );
//	int yIndex = static_cast<int>( vRealPos.y / m_fHeightSizeOfTile );

	int xIndex = static_cast<int>( vRealPos.x / m_fVtxWidthSpacing  );
	int yIndex = static_cast<int>( vRealPos.y / m_fVtxHeightSpacing );

	int triindex[3];

	triindex[0] = (yIndex + 1)	* m_nWidthVtxCount + (xIndex + 1);
	triindex[1] = (yIndex + 1)	* m_nWidthVtxCount + (xIndex);
	triindex[2] = (yIndex)		* m_nWidthVtxCount + (xIndex);
	//NiPoint3 kIntersect;
    float fLineParam, fTriParam1, fTriParam2;

	if( NiCollisionUtils::IntersectTriangle( vPosition, -NiPoint3::UNIT_Z,
		m_pHeightMapPosition[ triindex[0] ],
		m_pHeightMapPosition[ triindex[1] ],
		m_pHeightMapPosition[ triindex[2] ], 
		true, 
		vHeight, fLineParam, fTriParam1, fTriParam2 ) )
	{
		
		NiPoint3 v0, kV1;
        
        v0 = m_pHeightMapPosition[triindex[1]] - m_pHeightMapPosition[triindex[0]];
        kV1 = m_pHeightMapPosition[triindex[2]] - m_pHeightMapPosition[triindex[0]];
		vNormal = v0.Cross(kV1);

		vNormal.Unitize();

		return TRUE;
	}

	triindex[0] = (yIndex)		* m_nWidthVtxCount + (xIndex + 1);
	triindex[1] = (yIndex + 1)	* m_nWidthVtxCount + (xIndex + 1);
	triindex[2] = (yIndex)		* m_nWidthVtxCount + (xIndex);

	if( NiCollisionUtils::IntersectTriangle( vPosition, -NiPoint3::UNIT_Z,
		m_pHeightMapPosition[ triindex[0] ],
		m_pHeightMapPosition[ triindex[1] ],
		m_pHeightMapPosition[ triindex[2] ], 
		true, 
		vHeight, fLineParam, fTriParam1, fTriParam2 ) )
	{
		
		NiPoint3 v0, kV1;
        
        v0 = m_pHeightMapPosition[triindex[1]] - m_pHeightMapPosition[triindex[0]];
        kV1 = m_pHeightMapPosition[triindex[2]] - m_pHeightMapPosition[triindex[0]];
        vNormal = v0.Cross(kV1);

		vNormal.Unitize();

		return TRUE;
	}

	/*

	Tile *pTile = m_vecTiles[ y * m_nTileCount + x ];

	if( m_vecTiles[ y * m_nTileCount + x ]->IsTileArea( vPos ) )
	{
		// �浹���θ� üũ�Ѵ�.
			if( pTile->GetHeightPos( vPos, vHeight, vNormal, m_pPick ) )
				return TRUE;

	}

	
/*
	int yMin = yIndex - 1 >= 0 ? yIndex - 1 : yIndex ;
	int yMax = yIndex + 1 <= m_nTileCount - 1 ? yIndex + 1 : yIndex;

	int xMin = xIndex - 1 >= 0 ? xIndex - 1 : xIndex ;
	int xMax = xIndex + 1 <= m_nTileCount - 1 ? xIndex + 1 : xIndex;

	// �ش� �ε����� �¿�� Ÿ�ϵ��� �����Ͽ� �浹���� �����Ѵ�.
	for( int y =  yMin; y <= yMax ; y++ )
	{
		for( int x = xMin; x <= xMax; x++ )
		{
			Tile *pTile = m_vecTiles[ y * m_nTileCount + x ];

			// Min Max�� ������ ��ġ�� �ִ������� �˻��Ѵ�.
			if( pTile->IsTileArea( vPos ) )
			{
				// �浹���θ� üũ�Ѵ�.
				if( pTile->GetHeightPos( vPos, vHeight, vNormal, m_pPick ) )
					return TRUE;
			}			
		}
	}		
*/
	return FALSE;
}

//-----------------------------------------------------------------------------------------
//
//					ĳ���� ��ġ�� �ش��ϴ� CollisionManager�� �����Ѵ�.
//
//-----------------------------------------------------------------------------------------
CollisionManager*	Terrain::GetCollisionManager( NiPoint3& vPos )
{
	if( !m_pPick ) return FALSE;

	NiPoint3 &vRealPos = NiPoint3( vPos.x + m_fWidthSize / 2, vPos.y + m_fHeightSize / 2, vPos.z );

	int xIndex = static_cast<int>( vRealPos.x / m_fWidthSizeOfTile  );
	int yIndex = static_cast<int>( vRealPos.y / m_fHeightSizeOfTile );

	return m_vecTiles[ yIndex * m_nTileCount + xIndex ]->GetCollisionManager();
}





//		�ش� ��ġ�� �����ϴ� Ÿ���� �ε����� ��´�.
BOOL	Terrain::GetTileIndex( const NiPoint3& vPos, int &xIndex, int &yIndex )
{

	if( !m_pPick ) return FALSE;

	NiPoint3 &vRealPos = NiPoint3( vPos.x + m_fWidthSize / 2, vPos.y + m_fHeightSize / 2,  0.0f );

	// ���� ������ ��ġ�� �̿��Ͽ� �浹���θ� ������ Ÿ���� �ε����� �����Ѵ�.
	xIndex = static_cast<int>( vRealPos.x / m_fWidthSizeOfTile  );
	yIndex = static_cast<int>( vRealPos.y / m_fHeightSizeOfTile );


	// Min Max�� ������ ��ġ�� �ִ������� �˻��Ѵ�.
	if(  m_vecTiles[ yIndex * m_nTileCount + xIndex ]->IsTileArea( vPos ) )
	{
		return TRUE;
	}			

	return FALSE;
}

//		�ٿ�� �ڽ� �׸��� ����
void Terrain::SetDrawBB	( bool	bDraw	)
{
	if( m_vecTiles.empty() )
		return ;

	std::for_each( m_vecTiles.begin(), m_vecTiles.end(), std::bind2nd( std::mem_fun( &Tile::SetDrawBB ), bDraw ) );
}

//--------------------------------- �ؽ��� ���� �Լ� ------------------------------------------------

//		TextureBledMaterial Craete
BOOL Terrain::CreateTextureBlendMaterial()
{
	// ���̴��� ������ �Ѵ�.
	NiMaterial *pkShaderMaterial			= NiMaterialLibrary::CreateMaterial("TextureBlendMaterial");	

	TextureBlendMaterial *pBlendMaterial	= reinterpret_cast< TextureBlendMaterial* >( pkShaderMaterial );
	
	//�� Ÿ�� ������ ���̴��� �����Ѵ�.
	std::for_each( m_vecTiles.begin(), m_vecTiles.end(), std::bind2nd( std::mem_fun( &Tile::SetShaderMaterial ), pBlendMaterial ) );

	return TRUE;
}


//---------------------- �ؽ��� ���� ------------------------------------

BOOL Terrain::CreateTexture( NiFixedString& strFileName,  BOOL bFirst )
//-----------------------------------------------------------------------
{
	// �⺻ �ؽ��� ����

	if( m_vecTiles.empty() )
		return FALSE;

	m_strBaseTexName = strFileName;

	std::vector< Tile* >::iterator iterTile;

	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->CreateTexture( strFileName );
	}

	return TRUE;
}

BOOL	Terrain::CreateControlMap( NiSourceTexturePtr &pSourceTex )
{
	m_nPixelSize = m_nCellCount * 10;	// ��	����

	m_fPixelSize = (float)m_fWidthSizeOfTile / (float)m_nPixelSize;

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout	= NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped		= NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt		= NiTexture::FormatPrefs::BINARY;

	NiPixelData *pPixelData = NiNew NiPixelData( m_nPixelSize, m_nPixelSize, NiPixelFormat::BGRA8888 );

	pSourceTex = NiSourceTexture::Create( pPixelData, kPrefs );

	if( !pSourceTex )
	{
		NiMessageBox( "CreateControlMap - NiSourceTexture::Create() Failed", "Failed" );
		return FALSE;
	}

	if( pSourceTex->GetStatic() )
		pSourceTex->SetStatic( !pSourceTex->GetStatic() );

	pPixelData = pSourceTex->GetSourcePixelData();
	unsigned char* pPixelBuf = pPixelData->GetPixels();

	if( !pPixelBuf )
		return FALSE;	

	int count = 0;
	for( unsigned int y = 0; y < m_nPixelSize; y++ )
	{
		for( unsigned int x = 0; x < m_nPixelSize; x++ )
		{
			pPixelBuf[ count++ ] = 0x00;	// B
			pPixelBuf[ count++ ] = 0x00;	// G
			pPixelBuf[ count++ ] = 0x00;	// R
			pPixelBuf[ count++ ] = 0x00;	// A			

		}
	}

	pPixelData->MarkAsChanged();

	return TRUE;
}



//---------------------- ���ĸ� ���� ------------------------------------
BOOL Terrain::CreateAlphaMap( NiDynamicTexturePtr &pTexture )								
//-----------------------------------------------------------------------
{
	m_nPixelSize = m_nCellCount * 10;	// ��	����

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout	= NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped		= NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt		= NiTexture::FormatPrefs::SMOOTH;

	// Try to get a format that matches the source for top speed
	pTexture = NiDynamicTexture::Create(m_nPixelSize, m_nPixelSize, kPrefs );

	if( pTexture != NULL )
	{
		int iPitch = 0;
		BYTE* pPtr = (BYTE*)(pTexture->Lock(iPitch));

		assert(pPtr != NULL);
		assert(iPitch == static_cast<int>( m_nPixelSize * 4 ) );
		
		int count = 0;

		for( unsigned int y = 0; y < m_nPixelSize; y++ )
		{
			for( unsigned int x = 0; x < m_nPixelSize; x++ )
			{
					pPtr[ count++ ] = 0x00;		// RGBA B
					pPtr[ count++ ] = 0x00;		// RGBA G
					pPtr[ count++ ] = 0x00;		// RGBA R
					pPtr[ count++ ] = 0x00;		// RGBA A
			}
		}

		if( !pTexture->UnLock() )
		{
			NiMessageBox( "BOOL Terrain::CreateAlphaMap( AlphaTexture&	apphaTex  - Texture Unlock Failed", "Failed" );
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}


//		���� �ؽ��� 1�� �߰�
BOOL	Terrain::AddShaderMap( NiFixedString &fileName )
{

	if( m_vecTiles.empty() )
		return FALSE;

	std::vector< Tile* >::iterator iterTile;

	for( int i = 0; i < BLEND_TEX_COUNT; i++ )
	{
		if( m_vecBlendTextureInfo[ i ].m_nTexureIndex == -1 )
		{
			switch( i )
			{
			case BLEND_TEX_R :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_B;
				break;

			case BLEND_TEX_G :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_G;
				break;

			case BLEND_TEX_B :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_R;
				break;

			case BLEND_TEX_A :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_A;
				break;
			}

			m_vecBlendTextureInfo[ i ].m_strTexName = fileName;

//			m_spTex->SetShaderMap( i, pMap );
//			m_spTex->Update( 0.0f );

			m_nCurrentShaderMap = i;

			++m_nShaderMapCount;

			for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
			{
				(*iterTile)->AddShaderMap( fileName, m_nCurrentShaderMap );
			}

			return TRUE;
		}
	}

	/*
	if( !m_spTex || m_spTex->GetShaderArrayCount() > 5 )
		return FALSE;
	
	// �ؽ��� ������ : Layer_00 �ȼ� : R
	NiTexturingProperty::ShaderMap* pMap	= NiNew NiTexturingProperty::ShaderMap;
	NiTextureTransform *pTransform			= NiNew NiTextureTransform;

	pTransform->SetScale( NiPoint2( 1.0f, 1.0f ) );
	pMap->SetTextureTransform( pTransform );

	CGameApp::SetMediaPath( "Data/Texture/" );

	// �ؽ��� �ҷ�����
	NiSourceTexture* pTexture = NiSourceTexture::Create( CGameApp::ConvertMediaFilename( CW2A( fileName.GetBuffer() ) ) ); 
	pMap->SetTexture( pTexture );

	for( int i = 0; i < BLEND_TEX_COUNT; i++ )
	{
		if( m_vecBlendTextureInfo[ i ].m_nTexureIndex == -1 )
		{
			switch( i )
			{
			case BLEND_TEX_R :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_B;
				break;

			case BLEND_TEX_G :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_G;
				break;

			case BLEND_TEX_B :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_R;
				break;

			case BLEND_TEX_A :

				m_vecBlendTextureInfo[ i ].m_nTexureIndex = BLEND_TEX_A;
				break;
			}

			m_vecBlendTextureInfo[ i ].m_strTexName = fileName;

			m_spTex->SetShaderMap( i, pMap );
			m_spTex->Update( 0.0f );

			m_nCurrentShaderMap = i;

			++m_nShaderMapCount;

			return TRUE;
		}
	}
	*/
	return TRUE;
}

//		���� ������ ���̴� �� ������ ����
BOOL	Terrain::ShaderMapScaleTransForm( const NiPoint2& scaleRate )
{
	// �ؽ��� ������Ƽ ����X , ������ ���̴� �� ����, ���̴� �� ���� ��Ʈ�� �ʸ� ������
//	if ( !m_spTex || m_nCurrentShaderMap == -1 || m_spTex->GetShaderArrayCount() <= 1 )
//		return FALSE;

	if( m_vecTiles.empty() )
		return FALSE;
	
	std::vector< Tile* >::iterator iterTile;

	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->ShaderMapScaleTransForm( scaleRate );
	}

	return TRUE;
/*
	NiTexturingProperty::ShaderMap* pMap = m_spTex->GetShaderMap( m_nCurrentShaderMap );
	if( !pMap )
		return FALSE;

	NiTextureTransform *pTextureTrnas =  pMap->GetTextureTransform();
	if( pTextureTrnas )
	{
		pTextureTrnas->SetScale( scaleRate );
		m_spTex->Update( 0.0f );

		return TRUE;
	}
*/
	return FALSE;
}

//		��Ÿ�ϵ��� ������� ��ü ���������� ��� m_pHeightMap�� ����
void Terrain::SetHeightMapFromTiles()
{

	if( m_vecTiles.empty() )
		return;

	std::vector< Tile* >::iterator iterTile;

	int zPos	= 0, xPos = 0;
	int xCount	= 0;
	int nCntWidthVtx = m_nTileCount * m_nCellCount + 1;

	// Ÿ�ϵ�κ��� ������ ������ ���´�.
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		if( xCount >= m_nTileCount )
		{
			xPos	= 0; 
			xCount	= 0;
			++zPos;			
		}

		(*iterTile)->GetTileVtxData( (xPos * m_nCellCount), (zPos * m_nCellCount), nCntWidthVtx, m_pHeightMapPosition, m_pHeightMapNormal );

		++xPos;
		++xCount;
	}
}


//		������ ����� �籸��
void Terrain::CalculatorNormals()
{
	if( m_vecTiles.empty() )
		return;

	unsigned int *pTriIndex = &m_pUsTriIndex[0];

    for ( int i = 0; i < m_nTriangles; i++)
    {
        // get indices to triangle vertices
        unsigned int i0 = *pTriIndex++;
        unsigned int i1 = *pTriIndex++;
        unsigned int i2 = *pTriIndex++;

        // compute unit length triangle normal
        NiPoint3 e1 = m_pHeightMapPosition[i1] - m_pHeightMapPosition[i0];
        NiPoint3 e2 = m_pHeightMapPosition[i2] - m_pHeightMapPosition[i1];
        NiPoint3 normal = e1.Cross(e2);
        NiPoint3::UnitizeVector(normal);

        // update the running sum of normals at the various vertices
        m_pHeightMapNormal[i0] += normal;
        m_pHeightMapNormal[i1] += normal;
        m_pHeightMapNormal[i2] += normal;
    }
    
    NiPoint3::UnitizeVectors(m_pHeightMapNormal, m_nTotalVtxs, sizeof(m_pHeightMapNormal[0]));

//	m_pHeightData->MarkAsChanged( NiGeometryData::VERTEX_MASK	);
//	m_pHeightData->CalculateNormals();

	// �籸���� ����� �ٽ� ������ Ÿ�ϵ��� ���� ��ְ��� �����Ѵ�.
	std::vector< Tile* >::iterator iterTile;
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{		
		(*iterTile)->SetNormalOfVtx( m_pHeightMapNormal );
	}
}



void	Terrain::SetBillBoardChunk( int nXIndex, int nYIndex, BillBoardAtChunk* pChunk )
{
	if( m_vecTiles.empty() )
		return;

	int nTileNum = nYIndex * m_nCellCount + nXIndex;

	m_vecTiles[ nTileNum ]->SetBillBoardChunk( pChunk );
}


//		���� ���ĸʿ� �귯�� ũ�⸸ŭ �׸���.
void	Terrain::DrawControlMapRGBA	( const NiPoint3& vPos, const NiPoint3& vDir )
{

	if( m_nCurrentShaderMap == -1 || !m_pPick )
		return;		

	float fDist		= FLT_MAX;
	float fMinDist	= FLT_MAX; 
	std::vector< int > vecTileNum;
	std::vector< int >::iterator iterTileNum;	

	// �ٿ�� �ڽ��� Ray�� �浹�� ��� Ÿ�ϵ��� ���Ѵ�.
	m_pPick->SetPickType( NiPick::FIND_FIRST );
	m_pPick->SetSortType( NiPick::NO_SORT );
	m_pPick->SetIntersectType( NiPick::BOUND_INTERSECT );
	m_pPick->SetFrontOnly( true );

	for( iterTileNum = m_vecValidTilesNum.begin(); iterTileNum != m_vecValidTilesNum.end(); ++iterTileNum )
	{
		if( m_vecTiles[ *iterTileNum ]->CollisionBB( vPos, vDir, m_pPick, &fDist ) )
		{
			vecTileNum.push_back( *iterTileNum );
		}
	}

	if( vecTileNum.empty() )
		return;

	// �浹�� Ÿ�ϵ��߿� ���� ����� Ÿ���� Vertex Locking �۾��� ���Ͽ� ���������� ���� ����� Ÿ���� ��ȣ�� ���Ѵ�.
	// �� Ÿ�ϳ��� ���� x, y �ε����� ���´�.
	int	nNearTileNum = 0;						// ��ŷ�� Ÿ�� ��ȣ
	NiPoint2 fTexUV;
	NiPoint2 fPickedTexUV;
	NiPoint3 vPickPos, vNearPickPos;

	m_pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );

	for( iterTileNum = vecTileNum.begin(); iterTileNum != vecTileNum.end(); ++iterTileNum )
	{
		if( m_vecTiles[ *iterTileNum ]->GetPickPositionAntUV( vPos, vDir, &vPickPos, m_pPick, &fDist, &fTexUV ) )
		{
			if( fMinDist > fDist )
			{
				fMinDist	 = fDist;
				vNearPickPos = vPickPos;
				nNearTileNum = *iterTileNum; 
				
				fPickedTexUV = fTexUV;
			}
		}
	}

	float fUSpacing = fTexUV.x / (float)(m_nTileCount);
	float fVSpacing = (1.0f - fTexUV.y) / (float)(m_nTileCount);

	//	���ĸʿ� ��ŷ�� ������ ��Ī�Ǵ� Pixel�� ��ġ�� ã�´�.
	//	��ŷ�� Ÿ���� x, y �ε���
	int xOffset = nNearTileNum % m_nTileCount;
	int yOffset = nNearTileNum / m_nTileCount;

	float fRealUCoord = fUSpacing + xOffset * m_fUVSpacing;
	float fRealVCoord = (fVSpacing + yOffset * m_fUVSpacing);

	int texPosX = static_cast<int>(m_nPixelSize * m_nTileCount * fRealUCoord); //+ xOffset * m_nPixelSize;
	int texPosY = static_cast<int>(m_nPixelSize * m_nTileCount * fRealVCoord); //+ yOffset * m_nPixelSize;
	float fTexelSize = static_cast<float>(1.0f / static_cast<float>(m_nCellCount * 10));	// ���� ��� �ؼ��� ����� ���Ѵ�.

	NiPoint3 vMin, vMax;
	int rectLeft = 0, rectRight = 0, rectTop = 0, rectBottom = 0;		

	// ������ ���� Ÿ�ϵ� �˻�
	int cnt = 1;
	while( xOffset + cnt <= m_nTileCount - 1 )
	{		
		m_vecTiles[ yOffset * m_nTileCount + (xOffset + cnt) ]->GetMinMax( &vMin, &vMax );
		
		if( (NiAbs(vMin.x - vNearPickPos.x ) <= m_fOutsideLength) || (NiAbs(vMax.x - vNearPickPos.x ) <= m_fOutsideLength) )
			rectRight++;
		else
			break;
		
		cnt++;		
	}
	// ���� ���� Ÿ�ϵ� �˻�
	cnt = 1;
	while( xOffset - cnt >= 0 )
	{		
		m_vecTiles[ yOffset * m_nTileCount + (xOffset - cnt) ]->GetMinMax( &vMin, &vMax );
		
		if( (NiAbs(vMin.x - vNearPickPos.x ) <= m_fOutsideLength) || (NiAbs(vMax.x - vNearPickPos.x ) <= m_fOutsideLength) )
			rectLeft++;
		else
			break;
		
		cnt++;	
	}
	// �Ʒ��� ���� Ÿ�ϵ� �˻�
	cnt = 1;
	while( yOffset - cnt >= 0 )
	{		
		m_vecTiles[ (yOffset - cnt ) * m_nTileCount + xOffset ]->GetMinMax( &vMin, &vMax );
		
		if( (NiAbs(vMin.y - vNearPickPos.y ) <= m_fOutsideLength ) || (NiAbs(vMax.y - vNearPickPos.y ) <= m_fOutsideLength ) )
			rectBottom++;
		else
			break;
		cnt++;
	}
	// ���� ���� Ÿ�ϵ� �˻�
	cnt = 1;
	while( yOffset + cnt <= m_nTileCount - 1 )
	{		
		m_vecTiles[ (yOffset + cnt ) * m_nTileCount + xOffset ]->GetMinMax( &vMin, &vMax );

		if( (NiAbs(vMin.y - vNearPickPos.y ) <= m_fOutsideLength ) || (NiAbs(vMax.y - vNearPickPos.y ) <= m_fOutsideLength ) )
			rectTop++;
		else
			break;

		cnt++;
	}

	int nStart_yIndex	= yOffset - rectBottom;
	int nEnd_yIndex		= yOffset + rectTop;
	int nStart_xIndex	= xOffset - rectLeft;
	int nEnd_xIndex		= xOffset + rectRight;

	//------------ Ÿ�ϵ��� �Ÿ��� ���� �������� �ִ� �������� Taransform �Ѵ�. -------------

	// ��ŷ�� ������ �ȼ� ��ġ ����
	NiPoint2 vPickPixel( static_cast<float>( texPosX * m_fPixelSize), static_cast<float>( texPosY * m_fPixelSize) );

	for( int y = nStart_yIndex; y <= nEnd_yIndex; y++ )
	{
		for( int x = nStart_xIndex; x <= nEnd_xIndex; x++ )
		{
			m_vecTiles[ y * m_nTileCount + x ]->DrawControlMapRGBA( vPickPixel, m_fInsideLength, m_fOutsideLength, m_nCurrentDrawMode );
		}
	}
}


BOOL Terrain::UpdateObject( float fElapsedTime  )
{	
	// Ÿ�� ��ȣ�� ��� ���� Vector �����̳� Clear
	m_vecValidTilesNum.clear();	
	
	// �������� �ø� ���� ����
	m_pFrustum->GenerateFrustum();
	
	// ����Ʈ���� �ø�
	if( m_pQuadTree != NULL ) m_pQuadTree->Update( this, m_pFrustum, m_vecValidTilesNum );

	// Ÿ�ϵ� ������Ʈ
	std::for_each( m_vecTiles.begin(), m_vecTiles.end(), std::mem_fun( &Tile::UpdateObject ) );
	
	// SLOD �������� ����
	SetSLODLevel();

	return TRUE;
}


//------------------------��� ���� ���� Release -----------------------------
BOOL Terrain::ReleaseObject()
//----------------------------------------------------------------------------
{

	m_nPixelSize		= 0;
	m_nShaderMapCount	= 0;	// TextureBlendMaterial ShaderMap ����
	m_nCurrentShaderMap = -1;	// ������ ���̴� �� �ε��� 0 = R, 1 = G, 2 = B, 3 = A

	if( m_pTileBoundColliInfo != NULL )
	{
		delete[] m_pTileBoundColliInfo;
		m_pTileBoundColliInfo = NULL;
	}

	if( m_pHeightMapPosition != NULL )
	{
		NiDelete[] m_pHeightMapPosition;
		m_pHeightMapPosition = NULL;
	}

	if( m_pHeightMapNormal != NULL )
	{
		NiDelete[] m_pHeightMapNormal;
		m_pHeightMapNormal = NULL;
	}	

	if( m_pPlayerNode != NULL )
	{
		m_pPlayerNode = NULL;
	}

	if( m_pUsTriIndex != NULL )
	{
		delete[] m_pUsTriIndex;
		m_pUsTriIndex = NULL;
	}	

	std::vector< Tile* >::iterator tileIter;
	Tile* tempTile = NULL;
	
	if (! m_vecTiles.empty() )
	{
		for( tileIter = m_vecTiles.begin(); tileIter != m_vecTiles.end(); ++tileIter )
		{
			tempTile = (*tileIter);			
			tempTile->SetCulled( false );
			tempTile->SetOldIB();
			tempTile->ReleaseObject( m_spTerrainRoot );
			NiDelete tempTile;
		}
	}

	//	�ε������� Release
	if( m_pIB )
	{
		for( int i = 0; i < m_maxSLODLevel; i++ )
		{
			for( int crack = 0 ; crack < 9; crack++ )
			{
				if( NULL != m_pIB[ i ][ crack ] )
				{
					NiDX9RendererEx::ReleaseIBResource( m_pIB[ i ][ crack ] );
					m_pIB[ i ][ crack ] = NULL;
				}
			}
		}

		m_pIB = NULL;
	}
	
	if( !m_vecValidTilesNum.empty() )
	{
		m_vecValidTilesNum.clear();
	}

	if( !m_vecIBInfos.empty() )
	{
		m_vecIBInfos.clear();
	}	

	// ����Ʈ�� Release
	if( NULL != m_pQuadTree )
	{
		NiDelete m_pQuadTree;
		m_pQuadTree = NULL;
	}

	//	����ü Release
	if( NULL != m_pFrustum )
	{
		NiDelete m_pFrustum;
		m_pFrustum = NULL;
	}

	if( m_spBaseTexture != NULL  )
	{
		int a = m_spBaseTexture->GetRefCount();
		m_spBaseTexture = 0;		
	}

	if( NULL != m_pPick )
	{
		NiDelete m_pPick;
		m_pPick = NULL;
	}

//	if( m_spTex )
//	{
//		m_spTerrainRoot->DetachProperty( m_spTex ); 
//		m_spTex = 0;
//	}

	if( m_spTerrainRoot )
	{
		m_spTerrainRoot = 0;
	}

	
	//������ ��� �ؽ��Ŀ� ���ĸ��� Release()
//	ReleaseAllTextureAndAlpha();


	return TRUE;
}

void	Terrain::SetPlayer( NiNode *pPlayer )
{
	m_pPlayerNode = pPlayer;
}
//----------------AlphaMap UI�� ���� ���� ����--------------
void Terrain::CreateRenderTarget( NiRenderer *pRenderer )
{
    m_spRenderTexture = NiRenderedTexture::Create( PICK_UI_SIZE, PICK_UI_SIZE, pRenderer);
    assert(m_spRenderTexture);

    m_spRenderTargetGroup = NiRenderTargetGroup::Create( m_spRenderTexture->GetBuffer(), pRenderer, true, true);

//    NiD3DRenderedTextureData* pkRenderData = (NiD3DRenderedTextureData*)m_spRenderTexture->GetRendererData();

//    m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());

    D3DSURFACE_DESC kSurfDesc;

    m_pkD3DTexture->GetLevelDesc(0, &kSurfDesc);

    //  We also need to create a texture that we can lock and read for 
    //  getting the pixel color out...

    m_eFormat = kSurfDesc.Format;
    m_pkD3DDevice->CreateTexture(kSurfDesc.Width, kSurfDesc.Height, 1, 0, kSurfDesc.Format, D3DPOOL_SYSTEMMEM, &m_pkD3DReadTexture, 0);

    assert(m_pkD3DReadTexture);
}




NiNode* Terrain::GetTerrainRoot() 
{
	return m_spTerrainRoot;
}


//		������ �����ؽ��ĸ� �������·� ����

//    NiD3DRenderedTextureData* pkRenderData = (NiD3DRenderedTextureData*)m_spRenderTexture->GetRendererData();

//    m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());




//		��Į ����
BOOL	Terrain::AttachDecalAtTile( const NiPoint3* vPickedPos, NiPoint3* vPickedNor, NiPoint3 *pPickedVtxs, 

								   const int &nPickedTileNum, const int &nXTriIndex, const int &nYTriIndex )
{
	// ��ŷ�� Ÿ���� X, Y �ε���
	int xOffset = nPickedTileNum % m_nTileCount;	// �ʺ�
	int yOffset = nPickedTileNum / m_nTileCount;	// ����
	NiPoint3 vInterSect;

	vPickedNor->Unitize();

	float fPlaneConstant = vPickedPos->Dot( NiPoint3::UNIT_Z );
	NiPlane plane( NiPoint3::UNIT_Z, fPlaneConstant );
	
	int negativeCount = 0;
	bool negative[3];
	BOOL bFindInter = FALSE;
	for (int i = 0; i < 3; i++)
	{
		///-- ���� ������ �������� ������ ������ ������ ����� ���ʿ� ���ϰԵȴ�.
		bool neg = NiPlane::NEGATIVE_SIDE == plane.WhichSide( pPickedVtxs[ i ] ) ? true : false;

		negative[ i ] = neg;
		negativeCount += neg;
	}

	if (negativeCount != 0) 
	{
		int count = 0;
		for (int b = 0; b < 3; b++)
		{
			// c is the index of the previous vertex
			// c �� ���� ���ؽ��� �ε����̴�.
			int c = (b != 0) ? b - 1 : 3 - 1;

			if (negative[b])	// �ش���ؽ��� ����� �����ʿ� �ְ�..
			{
				if (!negative[c])  //�������ؽ��� ��� �ٱ��� ������
				{
					// Current vertex is on positive side of plane,
					// but previous vertex is on negative side.
					const NiPoint3 & v1 = pPickedVtxs[c];
					const NiPoint3 & v2 = pPickedVtxs[b];

					const NiPoint3 &planeNormal = plane.GetNormal();						 

					///-- DotProduct(plane, v1) - v1���� ������ ��������
					///-- (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
					///-- v1���� v2 ��� ��������
					float t = plane.Distance( v1 ) / ( planeNormal.x * (v1.x - v2.x) + planeNormal.y * (v1.y - v2.y) + planeNormal.z * (v1.z - v2.z));
					///-- ������ �浹����
					vInterSect = v1 * (1.0F - t) + v2 * t;

					bFindInter = TRUE;
				}
			}				
		}
	}

	NiPoint3 vTangent = NiPoint3::UNIT_X;
	
	///-- ���⼭�� �ٱ����� �������� ������ �浹������ ���Ѵ�.
	if(bFindInter) 	vTangent = vInterSect - *vPickedPos;

	vTangent.Unitize();

	// ��ü Height ���� rect �ε���
	// Ÿ�� ���� ������ ���� * ( Ÿ�� ���� * Ÿ���� Y�ε��� + ���� Y �ε��� ) + Ÿ�� ���� * Ÿ���� X�ε��� + ���� X �ε���
	//		3����4
	//		��  ��
	//		��  ��
	//		0����1
	//		^
	//	������ �ε��� ���´�. ������ �߽����� ��Į���� ���� ���� �ﰢ�� �������� ���´�.
	
	
	// Ÿ���� x,y �ε����� Ÿ�ϳ� �ﰢ�� Index�� �̿��Ͽ� ��ü HeightMap������ ���̰��� ���´�
	int nHeightIndex = m_nWidthVtxCount * ( m_nTileCount * yOffset + nYTriIndex ) + ( m_nTileCount * xOffset + nXTriIndex );

	//------------------ 

	if( nPickedTileNum != -1 )
	{
	//		��Į ûũ�� Ÿ�Կ� ���� ������ ��ũ��Ʈ �߰�
		if( !m_vecTiles[ nPickedTileNum ]->IsInitDecalAtChunk() )
		{
			if( m_vecTiles[ nPickedTileNum ]->GenerateDecalAtChunk() )
			{
				if( m_vecTiles[ nPickedTileNum ]->AttachDecalAtTile( vPickedPos, vPickedNor, &vTangent, &nHeightIndex, DecalManager::BULLET_MARK, DecalManager::OUTDOOR ) )
						return TRUE;

//				if( m_vecTiles[ nPickedTileNum ]->AttachDecalAtTile( NULL, NULL, NULL, &nHeightIndex, DecalManager::BOMB_MARK, DecalManager::OUTDOOR ) )
//						return TRUE;

				//DecalManager::GetManager()->GenerateDecal( &vNearPickPos, &vNormal, &vTangent, &nHeightIndex, 
				//DecalManager::BULLET_MARK, DecalManager::OUTDOOR, m_vecTiles[ nNearTileNum ]->GetDecalAtChunk() );
				
			//	if( DecalManager::GetManager()->GenerateExplosionAtTerrain( &nHeightIndex, m_vecTiles[ nNearTileNum ]->GetDecalAtChunk() ) )
			//		return TRUE;
			}
		}
		else
		{
			if( m_vecTiles[ nPickedTileNum ]->AttachDecalAtTile( vPickedPos, vPickedNor, &vTangent, &nHeightIndex, DecalManager::BULLET_MARK, DecalManager::OUTDOOR ) )
				return TRUE;

//			if( m_vecTiles[ nPickedTileNum ]->AttachDecalAtTile( NULL, NULL, NULL, &nHeightIndex, DecalManager::BOMB_MARK, DecalManager::OUTDOOR ) )
//				return TRUE;


		//	if( DecalManager::GetManager()->GenerateExplosionAtTerrain( &nHeightIndex, m_vecTiles[ nNearTileNum ]->GetDecalAtChunk() ) )
		//		return TRUE;
		}	
	}

	
//	DecalManager::GetManager()->AttachDecal( vNearPickPos, vNormal, vTangent, nHeightIndex, DecalManager::BULLET_MARK, DecalManager::OUTDOOR );

	return TRUE;
}

//		��ŷ�� ������Ʈ�� ��ġ������ ����� �̿��Ͽ� ���õ� Tile�� DecalAtChunk�� �ش� ��Į��
BOOL	Terrain::AttachDecalOfStaticObject( const NiPoint3* pos, const  NiPoint3* dir, const int &nPickedObjectTileNum )
{
	if(  !m_vecTiles[ nPickedObjectTileNum ]->IsInitDecalAtChunk() )
	{
		m_vecTiles[ nPickedObjectTileNum ]->GenerateDecalAtChunk();
	}		

	if( DecalManager::GetManager()->GenerateDecal( pos, dir, NULL, NULL, 
		DecalManager::BULLET_MARK, DecalManager::STATIC_OBJECT, m_vecTiles[ nPickedObjectTileNum ]->GetDecalAtChunk() ) )
		return TRUE;

	return FALSE;
}

Tile*	Terrain::GetTile( const  int& xIndex, const int& yIndex )
{
	return m_vecTiles[ yIndex * m_nTileCount + xIndex ];
}


NiPoint3*	Terrain::GetTerrainVertices()
{
	return m_pHeightMapPosition;
}

NiPoint3*	Terrain::GetTerrainNormals()
{
	return m_pHeightMapNormal;
}

NiPoint3&	Terrain::GetVtxPosition( unsigned int nVtxIndex )
{
	return m_pHeightMapPosition[ nVtxIndex ];
}

NiPoint3&	Terrain::GetVtxNormal( unsigned int nVtxIndex )
{
	return m_pHeightMapNormal[ nVtxIndex ];
}


//		��ŷ �������� ���� ��Į�� Ŭ���� �ϴµ� ����� �������� ���´�.
void	Terrain::GetDecalTriIndex( const int *nHeightMapIndex,  const float *fLimitWidth, const float *fLimitHeight, std::vector< DecalTriIndex >& vecDecalTri )
{

	int nCntWidthRect	= static_cast<int>( (*fLimitWidth / m_fVtxWidthSpacing)  );
	int nCntHeightRect	= static_cast<int>( (*fLimitHeight / m_fVtxHeightSpacing)  );

	int xIndex = *nHeightMapIndex % m_nWidthVtxCount;
	int yIndex = *nHeightMapIndex / m_nWidthVtxCount;

	int nStart_XIndex, nEnd_XIndex, nStart_YIndex, nEnd_YIndex;  
	
	nStart_XIndex = xIndex - nCntWidthRect;
	if( nStart_XIndex < 0 )
		nStart_XIndex = 0;

	nEnd_XIndex =  xIndex + nCntWidthRect;
	if( nEnd_XIndex > m_nWidthRect )
		nEnd_XIndex = m_nWidthRect;

	nStart_YIndex = yIndex - nCntHeightRect;
	if( nStart_YIndex < 0 )
		nStart_YIndex = 0;

	nEnd_YIndex =  yIndex + nCntHeightRect;
	if( nEnd_YIndex > m_nWidthRect )
		nEnd_YIndex = m_nWidthRect;


	DecalTriIndex decalTri;

	for( int y = nStart_YIndex; y <= nEnd_YIndex; y++ )
	{
		for( int x = nStart_XIndex; x <= nEnd_XIndex; x++ )
		{

				//		3����4
				//		��  ��
				//		��  ��
				//		0����1
				// ���ε� ���� : �ݽð� ����
				// 4 3 0 ,      1 4 0

						// 3 -- 4 
						// | /
						// 0

						//      4
						//   /  |
						// 0 -- 1


			decalTri.index[0] = (y+1)	* m_nWidthVtxCount + (x+1);
			decalTri.index[1] = (y+1)	* m_nWidthVtxCount + x;
			decalTri.index[2] = y		* m_nWidthVtxCount + x;

			vecDecalTri.push_back( decalTri );

			decalTri.index[0]	= (y)	* m_nWidthVtxCount + (x+1);
			decalTri.index[1]	= (y+1)	* m_nWidthVtxCount + (x+1);
			decalTri.index[2]	= y		* m_nWidthVtxCount + x;

			vecDecalTri.push_back( decalTri );
		}
	}
}

//		��ŷ �������� ���� ��Į�� Ŭ���� �ϴµ� ����� �������� ���´�.
void	Terrain::GetDecalTriIndex( const int *nHeightMapIndex,  const int &nCntWidthRect, const int &nCntHeightRect, std::vector< int >& vecVtxIndex )
{

	int xIndex = *nHeightMapIndex % m_nWidthVtxCount;
	int yIndex = *nHeightMapIndex / m_nWidthVtxCount;

	int nStart_XIndex, nEnd_XIndex, nStart_YIndex, nEnd_YIndex;  
	
	nStart_XIndex = xIndex - nCntWidthRect;
	if( nStart_XIndex < 0 )
		nStart_XIndex = 0;

	nEnd_XIndex =  xIndex + nCntWidthRect;
	if( nEnd_XIndex > m_nWidthRect )
		nEnd_XIndex = m_nWidthRect;

	nStart_YIndex = yIndex - nCntHeightRect;
	if( nStart_YIndex < 0 )
		nStart_YIndex = 0;

	nEnd_YIndex =  yIndex + nCntHeightRect;
	if( nEnd_YIndex > m_nWidthRect )
		nEnd_YIndex = m_nWidthRect;

	for( int y = nStart_YIndex; y <= nEnd_YIndex + 1; y++ )
	{
		for( int x = nStart_XIndex; x <= nEnd_XIndex + 1; x++ )
		{

				//		3����4
				//		��  ��
				//		��  ��
				//		0����1
				// ���ε� ���� : �ݽð� ����
				// 4 3 0 ,      1 4 0

						// 3 -- 4 
						// | /
						// 0

						//      4
						//   /  |
						// 0 -- 1

			vecVtxIndex.push_back(  y * m_nWidthVtxCount + x );
		}
	}
}

//		������Ʈ�� �ٿ��ڽ��� Ÿ���� �ٿ�� �ڽ��� ���ؼ� �ش� Ÿ���� �ø��� �׷쿡 �߰�
/*
void	Terrain::IsCollisionOfTile( NifMesh *pMesh )
{

	NiNode *pObject = pMesh->GetRootObject();

	// ������ �ֻ��� Min, Max ����
	NiNode *pRenderingGroup = (NiNode*)pObject->GetObjectByName( "RenderingGroup" );
	
	if( !pRenderingGroup ) return ;

	NiPoint3 vMin, vMax;

	CGameApp::GetMinMax( pRenderingGroup, &vMin, &vMax );

	for( int i = 0; i < (int)m_vecTiles.size(); i++ )
	{
		m_vecTiles[ i ]->IsCollisionOfTile( vMin, vMax, i, pMesh );		
	}

}
*/

//		�ε� �ؼ�  �ش� ������Ʈ�� �ش� Ÿ���� ������ �׷쿡 Attach
void	Terrain::SetRenderingGroup( NiNode *pObject, int &nTileNum, int &nIncludeType )
{
	m_vecTiles[ nTileNum ]->SetRenderingGroup( pObject, nIncludeType );
}

//		�ε� �ؼ� �ش� ������Ʈ�� �ش� Ÿ���� �ø��� Manager �׷쿡 �߰�
void	Terrain::SetCollisionGroup( NiNode *pObject, const NiTransform& transform, int &nTileNum, int &nIncludeType )
{
	m_vecTiles[ nTileNum ]->SetCollisionGroup( pObject, transform );
}

void	Terrain::SetCollisionGroupAboutMap( NiNode *pObject, int &nTileNum, int &nIncludeType )
{
	m_vecTiles[ nTileNum ]->SetCollisionGroupForMap( pObject );
}

void	Terrain::SetCollisionTransform( const NiTransform &transform )
{
	std::vector< Tile* >::iterator iterTile;
	
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->SetCollisionTransform( transform );
	}
}

void	Terrain::SetRenderingTransform( const NiTransform &transform )
{
	std::vector< Tile* >::iterator iterTile;
	
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->SetRenderingTransform( transform );
	}
}

//		�ɸ��͸� �� Ÿ���� CollsionManager�� ����Ѵ�.
void	Terrain::SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager )
{
	std::vector< Tile* >::iterator iterTile;
	
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->SetCharacterAtCollisionGroup( pkCharacterManager );
	}
}

void	Terrain::IsCollisionOfTile( NiGeometry *pObject )
{
	NiPoint3 vMin, vMax;

	CGameApp::GetMinMax( (NiAVObject*)pObject, &vMin, &vMax );

	std::vector<int> vecTileNums;
	int nIncludeType = 0;

	for( int i = 0; i < (int)m_vecTiles.size(); i++ )
	{
		if( m_vecTiles[ i ]->IsCollisionOfTile( vMin, vMax, pObject, nIncludeType ) )
		{
			vecTileNums.push_back( i );
			vecTileNums.push_back( nIncludeType );
		}
	}
	
	int *pTileList = NULL;
	if( vecTileNums.empty() )
		pTileList = NULL;
	else
	{
		pTileList = new int[ (int)vecTileNums.size() ];
		std::vector<int>::iterator iterTile;
		int count = 0;
		for( iterTile = vecTileNums.begin(); iterTile != vecTileNums.end(); ++iterTile )
		{
			pTileList[ count++ ] =  *iterTile;
		}
	}


	NiIntegersExtraData	*pTileDatas = NiNew NiIntegersExtraData( vecTileNums.size(), pTileList );

	pObject->AddExtraData( "TileNums", pTileDatas );

}


//--------------------------------------------------------------------------------
//					Header ���� Save
//--------------------------------------------------------------------------------
/*
void Terrain::SaveHeader( CArchive& ar )
{
	ar << m_nCellCount;							// Ÿ�� �ϳ��� ������ (������) ���� ����
	ar << m_nTileCount;							// ��ü Terrain Tile ����
	ar << m_fWidthSize;							// ��ü Terrain ���� ����
	ar << m_fHeightSize;						// ��ü Terrain ���� ����
	ar << m_SLODRange;							// SLOD ���� ���� ����
	ar << m_nShaderMapCount;					// ���� �ؽ��� ����
	ar << m_pFrustum->GetPlaneEpsilon();		// �������� �ø� ����
	ar << m_pCamera->GetViewFrustum().m_fFar;	// �������� Far ����
}


//--------------------------------------------------------------------------------
//					Terrain Vtx ���� Save
//--------------------------------------------------------------------------------
void Terrain::SaveData( CArchive& ar )
{	
	if( m_vecTiles.empty() )
		return;

	// ���������� �����Ѵ�.
	std::vector< Tile* >::iterator iterTile;
	for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
	{
		(*iterTile)->Save( ar );
	}

	if ( m_nShaderMapCount > 0 )
	{
		// ���̴� �� ������
		for( int i = 0; i < m_nShaderMapCount; i++ )
		{
			ar	<< m_vecBlendTextureInfo[ i ].m_nTexureIndex;
			ar	<< m_vecBlendTextureInfo[ i ].m_fTextureScale.x 
				<< m_vecBlendTextureInfo[ i ].m_fTextureScale.y;
		}
	}
}



void Terrain::SaveNameData( std::ofstream& fwrite )
{
	// ���̽� �ؽ��� �̸�
	fwrite << CW2A(m_strBaseTexName) << "\n";

	if( m_nShaderMapCount <= 0 )
		return;
	
	// ���� �ؽ��Ŀ� ���̴� �ؽ���	
	for( int i = 0; i < m_nShaderMapCount; i++  )
	{
		fwrite << CW2A(m_vecBlendTextureInfo[ i ].m_strTexName) << "\n";
	}	
		
}
*/

void Terrain::LoadData( FILE* file )
{
	for( int i = 0; i < (int)m_vecTiles.size(); i++ )
	{
		m_vecTiles[ i ]->Load( file );
	}

	SetHeightMapFromTiles();

	// ���̴� �� ������
	char buf[20];
	NiPoint2 vScale;
	std::vector< Tile* >::iterator iterTile;

	for( int i = 0; i < m_nShaderMapCount; i++ )
	{
		fread( buf, 1, sizeof(short), file );

		memcpy( &m_vecBlendTextureInfo[ i ].m_nTexureIndex		, buf					, sizeof(short)  );

		fread( buf, 1, sizeof(float)*2, file );

		memcpy( &(m_vecBlendTextureInfo[ i ].m_fTextureScale.x)	, buf 					, sizeof(float));
		memcpy( &(m_vecBlendTextureInfo[ i ].m_fTextureScale.y)	, buf + sizeof(float)	, sizeof(float));

		vScale.x = m_vecBlendTextureInfo[ i ].m_fTextureScale.x;
		vScale.y = m_vecBlendTextureInfo[ i ].m_fTextureScale.y;
		
		for( iterTile = m_vecTiles.begin(); iterTile != m_vecTiles.end(); ++iterTile )
		{
			(*iterTile)->AddShaderMap( m_vecBlendTextureInfo[ i ].m_strTexName, i );
			(*iterTile)->ShaderMapScaleTransForm( vScale, i );
		}		
	}
}


void Terrain::LoadNameData( int &nCnt, std::list<NiFixedString>& listFileName )
{

	for( int i = 0 ; i < nCnt; i++ )
	{
//		std::string &stFileName = listFileName.front();
		m_vecBlendTextureInfo[ i ].m_strTexName = listFileName.front();	
		listFileName.pop_front();		
	}
}

