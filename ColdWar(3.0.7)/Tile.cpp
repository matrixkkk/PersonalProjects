#include "main.h"
#include "Tile.h"
#include "GameApp.h"
#include "BillBoardManager.h"


Tile::Tile()
{
	m_nLodLevel		= 0;		// SLOD 레벨
	m_nCellCount	= 0;		// 타일내 셀 갯수
	m_nVtxCount		= 0;		// 버텍스 갯수
	m_nWidthVtxCount= 0;		// 지형 전체 가로 버텍스 갯수
	m_nTriangles	= 0;		// 삼각형 갯수
	m_uiIndexCount	= 0;
	m_uiIBSize		= 0;
	m_nPixelSize	= 0;

	m_fPixelSize	= 0.0f;

	m_bDrawBB		= false;	// 바운딩 박스 Draw 여부
	m_bGetOldIB		= false;
	
	m_pBoundBox		= NULL;

	m_pIB			= NULL;		// 인덱스 버퍼
	m_pOldIB		= NULL;		// 초기 NiTriShape에 의해 생성된 인덱스 버퍼
	m_nXIndex		= -1;		// X, Y 인덱스 위치
	m_nYIndex		= -1;
	m_nCurrentShaderMap = -1;
		
	m_bitCrackType.reset();		// 4비트 0으로 초기화	

	m_pBillBoardAtChunk = NULL;	// 빌보드 청크

	m_pDecalAtChunk		= NULL; // 데칼 청크

	m_pStaticObjectAtChunk	= NULL; // 스태틱 오브젝트 청크

	m_pCollsionManager		= NULL;	// 충돌 관리
}

Tile::~Tile()
{

	m_spTileNode	= 0;
//	m_spTile		= 0;		// 지형 기하 
	m_bDrawBB		= FALSE;	// 바운딩 박스 Draw 여부
	m_pBoundBox		= NULL;

	m_bitCrackType.reset();		// 4비트 0으로 초기

}

void Tile::ReleaseObject(  NiNode *pRootNode )
{

	if( m_pStaticObjectAtChunk != NULL )
	{
		m_pStaticObjectAtChunk->ReleaseObject();
		NiDelete m_pStaticObjectAtChunk;
		m_pStaticObjectAtChunk = NULL;
	}

	if( m_pCollsionManager != NULL )
	{
		NiDelete m_pCollsionManager;
		m_pCollsionManager = NULL;
	}

	// 바운딩 박스
	if( NULL != m_pBoundBox )
	{
		if( m_pBoundBox->GetBoundObject() )
			pRootNode->DetachChild( m_pBoundBox->GetBoundObject() );

		m_pBoundBox->ReleaseObject();
		NiDelete m_pBoundBox;
		m_pBoundBox = NULL;
	}

	if( m_spTexture )
	{
		m_spTile->DetachProperty( m_spTexture ); 
		m_spTexture = 0;
	}

	// NiTriShape 객체
	if( m_spTileNode )
	{
		pRootNode->DetachChild( m_spTileNode );
	}

	if( m_spTile )
	{
		m_spTileNode->DetachChild( m_spTile );
	}

	m_nLodLevel		= 0;		// SLOD 레벨
	m_nCellCount	= 0;		// 타일내 셀 갯수
	m_nVtxCount		= 0;		// 버텍스 갯수
	m_nTriangles	= 0;		// 삼각형 갯수	
	m_bDrawBB		= FALSE;	// 바운딩 박스 Draw 여부
	m_pIB			= NULL;		
	m_pBillBoardAtChunk = NULL;
	m_pDecalAtChunk		= NULL;

	m_bitCrackType.reset();		// 4비트 0으로 초기화	
}

NiNode*		Tile::GetRootObject()
{
	return m_spTileNode;
}

BOOL Tile::CreateGeometry( int nCellCount,  int nTileCount, float fWidthSize, float fHeightSize, float fXPos, float fYPos, int xIndex, int yIndex, float fUVSpacing, float tU, float tV, NiNode *pRootNode, NiRenderer* pkRenderer )
{
	if( NULL == pRootNode )
		return FALSE;

	NiDX9RendererEx *pkRendererEx = (NiDX9RendererEx*)pkRenderer;
	
	// Min Max 구하기
	m_vMin = NiPoint3( fXPos, fYPos,				0.0f 					);
	m_vMax = NiPoint3( fXPos + fWidthSize,  fYPos + fHeightSize,	0.0f 	);

	m_nXIndex = xIndex;
	m_nYIndex = yIndex;
	m_fWidthSize = fWidthSize;
	m_fHeightSize = fHeightSize;

	

	// 바운딩 박스 생성
	NiAVObject *pAABBAVObject = NULL;
	m_pBoundBox = NiNew BoundBox( m_vMin, m_vMax );
	
	if( NULL != m_pBoundBox )
	{
		pAABBAVObject = m_pBoundBox->GenerateAABB( m_vMin, m_vMax );
		m_pBoundBox->SetAppCulled( true );
		if( !pAABBAVObject )
		{
			NiDelete m_pBoundBox;
			return FALSE;
		}
	}

	m_nCellCount		= nCellCount;
	m_nTileCount		= nTileCount;
	m_nVtxCount			= (nCellCount+1)*(nCellCount+1);	// 한 라인당 버텍스 갯수
	m_nTriangles		= nCellCount * nCellCount * 2;		// 삼각형 갯수
	m_nWidthVtxCount	= nCellCount * nTileCount + 1;
	
	m_nPixelSize		= m_nCellCount * 10;	// 셀	갯수
	m_fPixelSize		= (float)fWidthSize / (float)m_nPixelSize;

	float	vtxWidthSpacing		= fWidthSize  / nCellCount;
	float	vtxHeightSpacing	= fHeightSize / nCellCount;
	float	texUVSpacing		= 1.0f		  / nCellCount;

	// 타일 기하 정보 생성
	NiPoint3 *pVPosition	= NiNew NiPoint3[ m_nVtxCount ];
	NiPoint3 *pVNormal		= NiNew NiPoint3[ m_nVtxCount ];
	NiPoint2 *pUV			= NiNew NiPoint2[ m_nVtxCount ];
	NiColorA *pkColor		= NiNew NiColorA[ m_nVtxCount ];

	unsigned short *pUsTriIndex	= NiAlloc( unsigned short, m_nTriangles * 3 );	
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	if( !pUsTriIndex )
	{
		goto Exit;
	}

	int y, x;
	for( y = 0; y <= nCellCount; y++ )
	{
		for( x = 0; x <= nCellCount; x++ )
		{
			// Pos 셋팅
			
			pVPosition[  y * (nCellCount + 1) + x ].x = fXPos + (x*vtxWidthSpacing);
			pVPosition[  y * (nCellCount + 1) + x ].y = fYPos + (y*vtxHeightSpacing);
			pVPosition[  y * (nCellCount + 1) + x ].z = 0.0f;

			// 노멀 셋팅
			pVNormal[  y * (nCellCount + 1) + x ].x = 0.0f;
			pVNormal[  y * (nCellCount + 1) + x ].y = 0.0f;
			pVNormal[  y * (nCellCount + 1) + x ].z = 1.0f;

			// 노멀화
			pVNormal->Unitize();

			// 버텍스 컬러
			pkColor[ y * (nCellCount + 1) + x ].r = 1.0f;
			pkColor[ y * (nCellCount + 1) + x ].g = 0.0f;
			pkColor[ y * (nCellCount + 1) + x ].b = 0.0f;
			pkColor[ y * (nCellCount + 1) + x ].a = 1.0f;

			// 텍스쳐 UV 셋팅
			pUV[ y * (nCellCount + 1) + x ].x = x * texUVSpacing;
			pUV[ y * (nCellCount + 1) + x ].y = 1.0f - y * texUVSpacing;

		}
	}

	--x; --y;

	m_vCenterPos = ( pVPosition[ 0 ] + pVPosition[ x ] + pVPosition[ y * (nCellCount + 1) ] + pVPosition[ y * (nCellCount + 1) + x] ) / 4;

	
	//		3━━4
	//		┃  ┃
	//		┃  ┃
	//		0━━1
	// 와인딩 오더 : 반시계 방향
	// 4 3 0 ,      1 4 0

			// 3 -- 4 
			// | /
			// 0

			//      4
			//   /  |
			// 0 -- 1

	// 인텍스 버퍼 갑 셋팅 
	int nVtxCount = nCellCount + 1;
	int index = 0;

	for(  y = 0; y < nCellCount ; y++ )
	{
		for( x = 0; x < nCellCount ; x++ )
		{
			pUsTriIndex[ index++ ] = (y + 1) * nVtxCount + x + 1; 
			pUsTriIndex[ index++ ] = (y + 1) * nVtxCount + x;
			pUsTriIndex[ index++ ] = y * nVtxCount + x;
			
			pUsTriIndex[ index++ ] = y * nVtxCount + x + 1; 
			pUsTriIndex[ index++ ] = (y + 1) * nVtxCount + x + 1;
			pUsTriIndex[ index++ ] = y * nVtxCount + x;
		}
	}
	
	m_spTile = NiNew NiTriShapeEx( m_nVtxCount, pVPosition, pVNormal, NULL, pUV, 1, NiGeometryData::NBT_METHOD_NONE, m_nTriangles, /*NULL*/ pUsTriIndex);	

	if( NULL == m_spTile )
	{
		goto Exit;
	}
	
	m_spTile->CalculateNormals();		// 노멀 생성

	m_spTexture = NiNew NiTexturingProperty();
	m_spTexture->SetBaseFilterMode( NiTexturingProperty::FILTER_TRILERP );
	m_spTexture->SetBaseClampMode( NiTexturingProperty::CLAMP_MAX_MODES );
	m_spTexture->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
	

	m_spTile->AttachProperty( m_spTexture );

	m_spTileNode = NiNew NiNode();

	m_spTileNode->AttachChild( m_spTile );
	
	m_spTileNode->Update(0.0f);
	m_spTileNode->UpdateProperties();
	m_spTileNode->UpdateNodeBound();
	m_spTileNode->UpdateEffects();

	NiGeometryData *pkData = m_spTile->GetModelData();

	unsigned int flats = pkData->GetKeepFlags();
	pkData->SetKeepFlags( NiGeometryData::KEEP_ALL );

	// 전체 지형과 바운딩박스 최상위 루트의 자식으로 Attach
	pRootNode->AttachChild( m_spTileNode );
//	pRootNode->AttachChild( pAABBAVObject );

	return TRUE;

Exit:

	if( pVPosition )	NiDelete[] pVPosition;
	if( pVNormal )		NiDelete[] pVNormal;
	if( pUV )			NiDelete[] pUV;
	if( pkColor )		NiDelete[] pkColor;

	return FALSE;
}

float	Tile::GetDistance( const NiPoint3 &vPosition )
{
	NiPoint3 vecDiff = vPosition - m_vCenterPos;

	return NiAbs( vecDiff.Length() );
}

BOOL Tile::CreateTexture( NiFixedString& fileName )
{

	if( m_spTexture )
	{
//		m_spTile->AttachProperty( m_spTexture );

		CGameApp::SetMediaPath("Data/Texture/");

		m_spBaseTexture = NiSourceTexture::Create( CGameApp::ConvertMediaFilename( fileName ) );

		if( !m_spBaseTexture )
		{
			NiMessageBox( "CreateTexture NiSourceTexture::Create() Failed", "Failed" );
			return FALSE;
		}

		NiTextureTransform *pTransform = NiNew NiTextureTransform;	
		pTransform->SetScale( NiPoint2( 1.0f, 1.0f ) );

		m_spTexture->SetBaseTexture( m_spBaseTexture );
		m_spTexture->GetBaseMap()->SetTextureTransform( pTransform );

		// RGBA 조절 맵 생성
		if( !m_spControlMap )
		{
			if( !CreateControlMap( m_spControlMap ) )
				return FALSE;

			NiTexturingProperty::ShaderMap* pMap = NiNew NiTexturingProperty::ShaderMap;

			NiTextureTransform *pTransform = NiNew NiTextureTransform;
			pTransform->SetScale( NiPoint2( 1.0f, 1.0f ) );

			pMap->SetTextureTransform( pTransform );
			pMap->SetTexture( m_spControlMap );

			m_spTexture->SetShaderMap( 4, pMap );
			
		}

		m_spTile->UpdateProperties();			

		return TRUE;
	}

	return FALSE;
}

//		블렌딩 텍스쳐 1개 추가
BOOL	Tile::AddShaderMap( NiFixedString &fileName, int index )
{

	if( !m_spTexture || m_spTexture->GetShaderArrayCount() > 5 )
		return FALSE;
	
	// 텍스쳐 입히기 : Layer_00 픽섹 : R
	NiTexturingProperty::ShaderMap* pMap	= NiNew NiTexturingProperty::ShaderMap;
	NiTextureTransform *pTransform			= NiNew NiTextureTransform;

	pTransform->SetScale( NiPoint2( 1.0f, 1.0f ) );
	pMap->SetTextureTransform( pTransform );

	CGameApp::SetMediaPath( "Data/Texture/" );

	// 텍스쳐 불러오기
	NiSourceTexture* pTexture = NiSourceTexture::Create( CGameApp::ConvertMediaFilename( fileName ) ); 
	pMap->SetTexture( pTexture );
	
	m_spTexture->SetShaderMap( index, pMap );

	return TRUE;	
}


//		알파값을 조절할 텍스쳐 생성 - SourceTexture
BOOL	Tile::CreateControlMap	( NiSourceTexturePtr &pSourceTex )
{
	//m_nPixelSize = m_nCellCount * 10;	// 셀	갯수

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

//		현재 지정한 쉐이더 맵 스케일 조절
BOOL	Tile::ShaderMapScaleTransForm( const NiPoint2& scaleRate )
{
	// 텍스쳐 프로퍼티 생성X , 선택한 쉐이더 맵 없음, 쉐이더 맵 갯수 컨트롤 맵만 있을때
	if ( !m_spTexture || m_spTexture->GetShaderArrayCount() <= 1 )
		return FALSE;

	NiTexturingProperty::ShaderMap* pMap = NULL;
	NiTextureTransform *pTextureTrnas = NULL;

	for( unsigned int i = 0; i < m_spTexture->GetShaderArrayCount(); i++ )
	{
		pMap = m_spTexture->GetShaderMap( i );

		if( pMap )
		{
			pTextureTrnas =  pMap->GetTextureTransform();

			if( pTextureTrnas )
			{
				pTextureTrnas->SetScale( scaleRate );
			}
		}
	}

	return TRUE;
}

BOOL	Tile::ShaderMapScaleTransForm( const NiPoint2& scaleRate, int nTarget )
{
	if ( !m_spTexture || m_spTexture->GetShaderArrayCount() <= 1 )
		return FALSE;

	NiTexturingProperty::ShaderMap* pMap = NULL;
	NiTextureTransform *pTextureTrnas = NULL;

	pMap = m_spTexture->GetShaderMap( nTarget );

	if( pMap )
	{
		pTextureTrnas =  pMap->GetTextureTransform();

		if( pTextureTrnas )
		{
			pTextureTrnas->SetScale( scaleRate );
		}
	}
	else
	{
		NiMessageBox( " Tile::ShaderMapScaleTransForm() - pMap = NULL Failed", "Failed" );
		return FALSE;
	}

	return TRUE;

}

BOOL	Tile::CurrentShaderMapScaleTransForm( const NiPoint2& scaleRate )
{
	// 텍스쳐 프로퍼티 생성X , 선택한 쉐이더 맵 없음, 쉐이더 맵 갯수 컨트롤 맵만 있을때
	if ( !m_spTexture || m_spTexture->GetShaderArrayCount() <= 1 )
		return FALSE;

	NiTexturingProperty::ShaderMap* pMap = NULL;
	NiTextureTransform *pTextureTrnas = NULL;

	pMap = m_spTexture->GetShaderMap( m_nCurrentShaderMap );

	if( pMap )
	{
		pTextureTrnas =  pMap->GetTextureTransform();

		if( pTextureTrnas )
		{
			pTextureTrnas->SetScale( scaleRate );
			return TRUE;
		}
	}
	
	return FALSE;
}

//		지정한 인덱스에 해당하는 쉐이더 맵과 연관된 콘트롤 맵의 RGBA를 택하여 Reset;
BOOL	Tile::ResetShaderMap( int resetType )
{
	if( !m_spTexture || !m_spControlMap || m_nCurrentShaderMap == -1 )
		return FALSE;

	NiPixelData *pPixelData = m_spControlMap->GetSourcePixelData();
	unsigned char* pPixelBuf = pPixelData->GetPixels();

	if( !pPixelBuf )
		return FALSE;
	
	int count = 0;
	if( resetType == Terrain::ALPHA_00 ) 
	{
		for( unsigned int y = 0; y < m_nPixelSize; y++ )
		{
			for( unsigned int x = 0; x < m_nPixelSize; x++ )
			{
				pPixelBuf[ count * 4 + m_nCurrentShaderMap ] = 0x00;		// RGBA B			
				count++;
			}
		}
	}
	else
	{
		for( unsigned int y = 0; y < m_nPixelSize; y++ )
		{
			for( unsigned int x = 0; x < m_nPixelSize; x++ )
			{
				pPixelBuf[ count * 4 + m_nCurrentShaderMap ] = 0xFF;		// RGBA B			
				count++;
			}
		}
	}

	pPixelData->MarkAsChanged();

	return TRUE;
}

//			현재 선택한 텍스쳐의 알파값 조절
void	Tile::DrawControlMapRGBA( const NiPoint2& pickPixel, float fInsideLength, float fOutsideLength, int drawType )
{
	if( !m_spTexture )
		return;

	// 픽셀 데이타 얻어오기
	unsigned int iPitch = static_cast< unsigned int >( m_nPixelSize * 4 );
	NiPixelData *pPixelData =m_spControlMap->GetSourcePixelData();
	unsigned char* pPixelBuf = pPixelData->GetPixels();

	if( !pPixelBuf )return;	

	// 지형 대비 텍셀의 사이즈를 구한다.
	float	fLength		= 0.0f;
	float	fSmooth		= 0.0f;
	int		rectIndex	= 0;	

	float 		xAlphaPixel = 0;
	float		yAlphaPixel = 0;

	BYTE	newColor	= 0;
	NiPoint2 vecDist( 0.0f, 0.0f );
	NiPoint2 vecPixel;
	int xStartIndex = m_nXIndex * m_nPixelSize;
	int yStartIndex = m_nYIndex * m_nPixelSize;

	for( unsigned int y = 0; y < m_nPixelSize; y++ )
	{
		for( unsigned int x = 0; x < m_nPixelSize; x++ )
		{
			rectIndex = (y * iPitch) + x * 4;  // B, G, R, A
		
			vecPixel.x = static_cast<float>( ( x + xStartIndex ) * m_fPixelSize );
			vecPixel.y = static_cast<float>( ( y + yStartIndex ) * m_fPixelSize );
			vecDist = pickPixel - vecPixel;
			fLength = NiAbs( vecDist.Length() );
			
			switch( drawType )
			{

			case Terrain::TEXTURE_DRAW: // 알파텍스쳐 그리기

				if( fLength <= fInsideLength )// 안쪽 원보다 작으면	
				{
					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = 0xFF;			// RGBA A알파값 조정				

				}
				else if( fLength <= fOutsideLength )	// 안쪽 원보다 크고 바깥쪽 원보다 작은경우//( fInsideLength < fLength && fLength <= fOutsideLength  )	// 안쪽 원보다 크고 바깥쪽 원보다 작은경우
				{

					fLength -= fInsideLength;
					fSmooth = fOutsideLength - fInsideLength;

					BYTE nOldColor = pPixelBuf[ rectIndex + m_nCurrentShaderMap ];
					newColor = static_cast<BYTE>( (fSmooth - fLength) / fSmooth * 0xff);

					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = ( nOldColor < newColor ) ? newColor : nOldColor;

				}


				break;

			
			case Terrain::TEXTURE_ERASE: // 알파텍스쳐 지우기

				if(  fLength <= fInsideLength  )		// 작은 원보다 작으면
				{
					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = 0x00;			// RGBA A알파값 조정			
				}
				else if( fInsideLength < fLength && fLength <= fOutsideLength )	// 안쪽 원보다 크고 바깥쪽 원보다 작은경우
				{

					fLength -= fInsideLength;
					fSmooth = fOutsideLength - fInsideLength;

					BYTE nOldColor = pPixelBuf[ rectIndex + m_nCurrentShaderMap ];
					newColor = static_cast<BYTE>( (fSmooth - fLength) / fSmooth * 0xff);

					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = ( (nOldColor - newColor) < 0x00 ) ? 0x00 : (nOldColor - newColor);

				}

				break;
			}
		}
	}

	pPixelData->MarkAsChanged();

}


void	Tile::RenderObject(NiRenderer* pkRenderer)
{
	if( NULL != pkRenderer && m_spTile != NULL )
	{
		m_spTile->RenderImmediate( pkRenderer );
	}

	if( NULL != m_pBoundBox && m_bDrawBB )
	{
		m_pBoundBox->RenderImmediate( pkRenderer );
	}
}



void	Tile::SetOldIB()
{
	if( m_pOldIB )
	{
		m_spTile->SetIB( m_pOldIB, m_nTriangles * 3, m_nTriangles * 3 * sizeof(unsigned short) );
	}
}

void	Tile::RenderObject( NiRenderer* pkRenderer, D3DDevicePtr pkD3DDevice )
{

	assert(pkD3DDevice);
	//D3D_POINTER_REFERENCE(m_pkD3DDevice);
	pkD3DDevice->SetIndices( m_pIB );
	m_spTile->RenderImmediate( pkRenderer );

	if( NULL != m_pBoundBox && m_bDrawBB )
	{
		m_pBoundBox->RenderImmediate( pkRenderer );
	}
}


void Tile::SetIB( LPDIRECT3DINDEXBUFFER9 pIB, unsigned int uiIndexCount, unsigned int uiIBSize )
{ 
	if( !m_bGetOldIB )
		return;

	m_pIB			= pIB; 
	m_uiIndexCount	= uiIndexCount;
	m_uiIBSize		= uiIBSize;

	m_spTile->SetIB( pIB, uiIndexCount, uiIBSize );
}

void Tile::SetIB( LPDIRECT3DINDEXBUFFER9 pIB )
{
	m_pIB = pIB;
	m_spTile->SetIB( pIB );
}

void Tile::SetCulled( bool bCull )
{
	m_spTileNode->SetAppCulled( bCull );
	
	if( m_pStaticObjectAtChunk )	m_pStaticObjectAtChunk->SetAppCulled( bCull );

	if( m_pDecalAtChunk )			m_pDecalAtChunk->SetAppCulled( bCull );
}

void Tile::SetShaderMaterial( NiMaterial *pMaterial )
{
	m_spTile->ApplyAndSetActiveMaterial( pMaterial,  m_spTile->GetMaterialCount() );

	m_spTile->UpdateProperties();
	m_spTile->UpdateEffects();
}


void Tile::UpdateObject()
{
	// 4 Bit 값을 0 으로 초기화
	m_spTileNode->SetAppCulled( true );
	
	if( m_pStaticObjectAtChunk ) m_pStaticObjectAtChunk->SetAppCulled( true );

	m_bitCrackType.reset();	

	if( !m_bGetOldIB )
	{
		NiGeometryBufferData* pBufferData = (NiGeometryBufferData*)(m_spTile->GetModelData()->GetRendererData());

		if( pBufferData )
		{
			m_pOldIB = pBufferData->GetIB();

			if( m_pOldIB )
			{
				m_bGetOldIB = true;
				m_spTile->GetGeometryBufferData();
			}
		}		
	}	
}

//		빌보드 청크 생성 여부
BOOL	Tile::IsInitBillBoardAtChunk()
{
	return m_pBillBoardAtChunk != NULL;
}


//		빌보드 청크 생성
BOOL	Tile::GenerateBillBoardAtChunk()
{
	if( !m_pBillBoardAtChunk  )
	{
		m_pBillBoardAtChunk	= BillBoardManager::GetManager()->GenerateBillBoardAtChunk();

		if( m_pBillBoardAtChunk )
		{
			m_pBillBoardAtChunk->SetTileIndex( m_nXIndex, m_nYIndex );
			return TRUE;
		}
	}

	return FALSE;
}

//		빌보드 청크에 추가된 빌보드 오브젝트 픽킹
BOOL	Tile::PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *m_pPick )
{
	if( !m_pBillBoardAtChunk  )
		return FALSE;

	float fDis;
	if( m_pBillBoardAtChunk->PickBillBoard( pos, direction, m_pPick, &fDis) )
	{
		BillBoardManager::GetManager()->SetSlectBillBoardChunk( m_pBillBoardAtChunk );
		
		return TRUE;
	}

	return FALSE;
}

BOOL	Tile::IsInitDecalAtChunk()
{
	return m_pDecalAtChunk != NULL;
}

//		빌보드 청크 생성
BOOL	Tile::GenerateDecalAtChunk()
{
	if( !m_pDecalAtChunk  )
	{
		m_pDecalAtChunk	= DecalManager::GetManager()->GenerateDecalAtChunk();

		if( m_pDecalAtChunk )
		{

//			m_spTileNode->AttachChild( m_pDecalAtChunk->GetRootObject() );
//			m_spTileNode->Update( 0.0f );
//			m_spTileNode->UpdateProperties();
			
			m_pDecalAtChunk->SetTileIndex( m_nXIndex, m_nYIndex );
			return TRUE;
		}
	}

	return FALSE;
}


void	Tile::SetBillBoardChunk( BillBoardAtChunk *pBillBoard )
{
	m_pBillBoardAtChunk = pBillBoard;
}

void	Tile::SetDecalChunk( DecalAtChunk *pDecal )
{
	m_pDecalAtChunk = pDecal;
}

//		데칼 청크에 타입에 따라 빌보드 오브젝트 추가
BOOL	Tile::AttachDecalAtTile( const NiPoint3* vPickPos, const NiPoint3* vNormal, const NiPoint3* vTangent, const int *nHeightMapIndex, int nDecalType, int nDecalPlace )
{
	if( !m_pDecalAtChunk )
		return FALSE;

//	NiTriShape *pDecal = NULL;
	if( DecalManager::GetManager()->GenerateDecal( vPickPos, vNormal, vTangent, nHeightMapIndex, nDecalType, nDecalPlace, m_pDecalAtChunk ) )
		return TRUE;

//	if( !pDecal )
//		return FALSE;

//	if( m_pDecalAtChunk->AddDecal( pDecal ) )
//		return TRUE;

	return FALSE;

}

//		빌보드 청크에 타입에 따라 빌보드 오크젝트 추가
BOOL	Tile::AddBillBoardAtTile( const NiPoint3& vPos )
{
	if( !m_pBillBoardAtChunk )
		return FALSE;

	NiBillboardNodePtr pBillBoardNode;
	int  nType = 0;

	nType			= BillBoardManager::GetManager()->GetCurrentBillBoardType();
	pBillBoardNode	= BillBoardManager::GetManager()->GetCurrentBillBoardObject()->Clone();
	
	if( !pBillBoardNode )
	{
		NiMessageBox( "Tile::AddBillBoardAtTile() - pBillBoardNode = NULL", "Failed" );
		return FALSE;
	}

	if( m_pBillBoardAtChunk->AddBillBoard( vPos, pBillBoardNode, nType ) )
		return TRUE;

	return FALSE;

}


void Tile::SetDrawBB( bool bDraw )
{
	m_bDrawBB = bDraw;

	if( m_pBoundBox != NULL )
		m_pBoundBox->SetAppCulled( !bDraw );
}

/*
enum MapEnum
{
	BASE_INDEX,
	DARK_INDEX,
	DETAIL_INDEX,
	GLOSS_INDEX,
	GLOW_INDEX,
	BUMP_INDEX,
	NORMAL_INDEX,
	PARALLAX_INDEX,
	DECAL_BASE,
	SHADER_BASE,
	INDEX_MAX
};
*/

void Tile::SetTexture( NiTexture *pTexture )
{
	if( !m_spTexture )
	{
	// Get Texture property
		NiTexturingProperty* pkTextureProp = NiDynamicCast(NiTexturingProperty, m_spTile->GetProperty(NiTexturingProperty::GetType()));

		if( NULL != pkTextureProp )
		{
			NiTexturingProperty::Map* pkMap = pkTextureProp->GetMaps().GetAt( NiTexturingProperty::BASE_INDEX );	
			pkMap->SetTexture( pTexture );
		}
		else
		{
			m_spTexture = NiTCreate<NiTexturingProperty>();
			NiTexturingProperty::Map* pkMap = pkTextureProp->GetMaps().GetAt( NiTexturingProperty::BASE_INDEX );	
			pkMap->SetTexture( pTexture );
		}

	}
	else
	{
		NiTexturingProperty::Map* pkMap = m_spTexture->GetMaps().GetAt( NiTexturingProperty::BASE_INDEX ); 
		pkMap->SetTexture( pTexture );
	}
}

void Tile::SetTextureProperty( NiTexturingProperty *pTexturingProperty )
{
	m_spTexture = pTexturingProperty;
}

// SLOD 방향에 따라 비트값 셋팅       BOTTOM | RIGHT | TOP | LEFT
void Tile::SetBitCrackType( int slodType )
{
	switch( slodType )
	{
	case SLOD_LEFT:
		m_bitCrackType.set( SLOD_LEFT );
		break;

	case SLOD_TOP:
		m_bitCrackType.set( SLOD_TOP );
		break;

	case SLOD_RIGHT:
		m_bitCrackType.set( SLOD_RIGHT );
		break;

	case SLOD_BOTTOM:
		m_bitCrackType.set( SLOD_BOTTOM );
		break;	
	}
}

void Tile::GetCenterPos( NiPoint3 &vPos )
{
	NiBound kBound = m_spTile->GetWorldBound();

//	vPos  = m_spTile->GetWorldTranslate();

	vPos = kBound.GetCenter();

}


//------------------ 타일의 Min Max 값을 얻는다 ----------------------
void Tile::GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax )
//--------------------------------------------------------------------
{
	if( m_spTile != NULL && m_pBoundBox != NULL)
	{
		m_pBoundBox->GetMinMax( vMin, vMax );
	}
}


//------------------ Tile의 바운드 박스 충돌을 체크한다.------------------
BOOL Tile::CollisionBB( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *dis )
//------------------------------------------------------------------------
{


	pPick->RemoveTarget();
	pPick->SetTarget( m_spTile );
	pPick->SetFrontOnly( false );
	
	if( pPick->PickObjects( pos, dir ) )
	{
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*dis = NiAbs(pPickRecord->GetDistance());
		
		pPick->SetFrontOnly( true );
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->SetFrontOnly( true );
	pPick->RemoveTarget();

	return FALSE;
}


//------------------ Tile의 바운드 박스 충돌을 체크한다.------------------
BOOL Tile::CollisionTialeAntObjectBB(
									 const NiPoint3& pos, const NiPoint3& dir, 
									 const NiPoint2& pos1, const NiPoint2& pos2,
									 NiPick *pPick, float *fDistTile, float *fDisObject, int *nPickType 
									 )
//------------------------------------------------------------------------
{
	int nType = 0;

	pPick->RemoveTarget();
	pPick->SetTarget( m_spTile );
	pPick->SetFrontOnly( false );

	if( pPick->PickObjects( pos, dir ) )
	{
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*fDistTile = NiAbs(pPickRecord->GetDistance());		

		nType += Terrain::TILE_PICK ;
	}

	pPick->SetFrontOnly( true );
	pPick->RemoveTarget();

	// 2차원 픽킹 선분과 타일의 4개 선분과의 Intersection을 체크
	if( CheckIntersectionWithTile( pos1, pos2 ) )
	{
		*fDisObject = NiAbs( (m_vCenterPos - pos).Length() );

		nType += Terrain::OBJECT_PICK;
	}
	
	if( nType )
	{
		*nPickType = nType;
		return TRUE;
	}

	return FALSE;
}

BOOL Tile::CheckIntersectionWithTile( const NiPoint2& pos1, const NiPoint2& pos2 )
{
	NiPoint2 vIntersection;

	NiPoint2 LT( m_vMin.x, m_vMax.y );
	NiPoint2 RT( m_vMax.x, m_vMax.y );
	NiPoint2 LB( m_vMin.x, m_vMin.y );
	NiPoint2 RB( m_vMax.x, m_vMin.y );

	// 타일의 위에 선분
	if( GetIntersectPoint( LT, RT, pos1, pos2, &vIntersection ) )
		if( m_vMin.x <= vIntersection.x && vIntersection.x <= m_vMax.x  ) return TRUE;

	// 타일 아래 선분
	if( GetIntersectPoint( LB, RB, pos1, pos2, &vIntersection ) )
		if( m_vMin.x <= vIntersection.x && vIntersection.x <= m_vMax.x  ) return TRUE;

	// 타일 왼쪽 선분
	if( GetIntersectPoint( LB, LT, pos1, pos2, &vIntersection ) )
		if( m_vMin.y <= vIntersection.y && vIntersection.y <= m_vMax.y  ) return TRUE;

	// 타일 오른쪽 선분
	if( GetIntersectPoint( RB, RT, pos1, pos2, &vIntersection ) )
		if( m_vMin.y <= vIntersection.y && vIntersection.y <= m_vMax.y  ) return TRUE;

	return FALSE;

}

BOOL Tile::GetIntersectPoint(const NiPoint2& AP1, const NiPoint2& AP2, const NiPoint2& BP1, const NiPoint2& BP2, NiPoint2* IP) 
{ 
    float t; 
    float s;     
    float under = (BP2.y-BP1.y)*(AP2.x-AP1.x)-(BP2.x-BP1.x)*(AP2.y-AP1.y); 
    
	if(under == 0.0f) return FALSE; 
    
    float _t = (BP2.x-BP1.x)*(AP1.y-BP1.y) - (BP2.y-BP1.y)*(AP1.x-BP1.x); 
    float _s = (AP2.x-AP1.x)*(AP1.y-BP1.y) - (AP2.y-AP1.y)*(AP1.x-BP1.x);     
     
    t = _t/under; 
    s = _s/under;     

    if(t < 0.0f || t > 1.0f || s < 0.0f || s > 1.0f ) return FALSE; 
    
	if( _t == 0.0f && _s == 0.0f) return FALSE;     

    IP->x = AP1.x + t * (AP2.x-AP1.x); 
    IP->y = AP1.y + t * (AP2.y-AP1.y); 
     
    return true; 
}  
//------------------------------------------------
//
//	오브젝트와 현재 타일과의 바운딩 박스 충돌여부
//
//------------------------------------------------
BOOL Tile::IsTileArea( const NiPoint3& vPos )
{
	if( m_vMin.x <= vPos.x && m_vMin.y <= vPos.y &&
		m_vMax.x >= vPos.x && m_vMax.y >= vPos.y )
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------
//
//		오브젝트와 충돌한 지점의 위치와 노멀을 얻는다.
//
//------------------------------------------------------
BOOL Tile::GetHeightPos( NiPoint3& vPos, NiPoint3& vHeight, NiPoint3& vNormal, NiPick *pPick )
{
	vPos.z += 100.0f;
	NiPoint3 &vDir = -NiPoint3::UNIT_Z;

	pPick->SetTarget( m_spTile );

	if( pPick->PickObjects( vPos, vDir ) )
	{
		// get pick results
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		vHeight = vPos + vDir * NiAbs(pPickRecord->GetDistance());
		
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;
}

//------------------ 광선과 Tile 내 정점과 가장 가까운 충돌 위치 포착--------
BOOL Tile::GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, int *xIndex, int *yIndex )
//---------------------------------------------------------------------------
{

	if( m_spTile == NULL )
		return FALSE;

	pPick->SetTarget( m_spTile );

	if( pPick->PickObjects( pos, dir ) )
	{
		// get pick results
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*fDist = NiAbs(pPickRecord->GetDistance());
		unsigned short triIndex = pPickRecord->GetTriangleIndex() / 2;

		*pickpos	= pPickRecord->GetIntersection();

		*xIndex = triIndex % m_nCellCount; //너빈
		*yIndex = triIndex / m_nCellCount; //높이

		
		
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;
}

//		관리하고 있는 StaticObjectChunk에 있는 Object들에 대해서 Picking Ray 판정 실시
BOOL	Tile::PickRayAtObjectGroup( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *pickNor, NiPick *pPick, float *fDist )
{
	if( !m_pStaticObjectAtChunk )
		return FALSE;

	if( m_pStaticObjectAtChunk->PickRayAtObjects( pos, dir, pickpos, pickNor, pPick, fDist ) )
		return TRUE;

	return FALSE;
}

//------------------ 광선과 Tile 내 정점과 가장 가까운 충돌 위치 포착--------
BOOL Tile::GetPickPositionAntUV( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPick *pPick, float *fDist, NiPoint2 *fUV )
//---------------------------------------------------------------------------
{

	if( m_spTile == NULL )
		return FALSE;

	pPick->SetTarget( m_spTile );
	pPick->SetReturnTexture( true );

	if( pPick->PickObjects( pos, dir ) )
	{
		// get pick results
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*fDist		= NiAbs(pPickRecord->GetDistance());
		*pickpos	= pPickRecord->GetIntersection();
		*fUV		= pPickRecord->GetTexture();
				
		pPick->SetReturnTexture( false );
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->SetReturnTexture( false );
	pPick->RemoveTarget();

	return FALSE;
}

BOOL	Tile::GetPickPosition( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *vNormal, NiPoint3 *pVtx, NiPick *pPick, float *fDist, int &xIndex, int &yIndex )
{
	pPick->SetTarget( m_spTile );

	if( pPick->PickObjects( pos, dir ) )
	{
		unsigned short vtxIndex[3];
		// get pick results
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*fDist = NiAbs(pPickRecord->GetDistance());
		unsigned short triIndex = pPickRecord->GetTriangleIndex() / 2;
		
		pPickRecord->GetVertexIndices( vtxIndex[0], vtxIndex[1], vtxIndex[2] );
		*pickpos = pPickRecord->GetIntersection();
		*vNormal = pPickRecord->GetNormal();
		float fdis = pPickRecord->GetDistance();
		NiPoint3 pickPos = pos + dir * fdis;

		NiPoint3 *pVtxList = m_spTile->GetModelData()->GetVertices();
		
		for(int i = 0; i < 3; i++ ) 
			pVtx[ i ] = pVtxList[ vtxIndex[i] ];
	
		xIndex = triIndex % m_nCellCount; //너빈
		yIndex = triIndex / m_nCellCount; //높이
		
		
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;

}




//------------타일의 노멀및 정점 정보를 배열에 삽입-----------
void Tile::GetTileVtxData( int xPos, int zPos, int nCntWidthVtx, NiPoint3 *pHeightMapPosition, NiPoint3 *pHeightMapNormal )
//------------------------------------------------------------
{

	if( m_spTile == NULL )
		return;

	NiTriShapeData* pTileData = (NiTriShapeData*)(m_spTile->GetModelData());

	NiPoint3 *pVtxPos	= pTileData->GetVertices();
	NiPoint3 *pVtxNor	= pTileData->GetNormals();

	if( NULL != pVtxPos && NULL != pVtxNor )
	{
		for( int z = 0; z <=  m_nCellCount; z++ )
		{
			for( int x = 0; x <= m_nCellCount; x++ )
			{
				pHeightMapPosition[ ((zPos + z) * nCntWidthVtx) + (xPos + x) ]	= pVtxPos[ z * (m_nCellCount + 1) + x ];			
				pHeightMapNormal[ ((zPos + z) * nCntWidthVtx) + (xPos + x) ]	= pVtxNor[ z * (m_nCellCount + 1) + x ];
			}
		}
	}

	

}

//---------------------------- 타일의 정점에 새로운 노멀 삽입 -----------------------------
void Tile::SetNormalOfVtx( NiPoint3* spHeightMap )
//-----------------------------------------------------------------------------------------
{
	if( m_spTile == NULL )
		return;

//	NiPoint3 *pVtxPos = spHeightMap->GetNormals();

	NiTriShapeData* pTileData = (NiTriShapeData*)(m_spTile->GetModelData());

	NiPoint3 *pVtxNor	= pTileData->GetNormals();

	int index = 0;

	if( NULL != pVtxNor )
	{
		for( int y = 0; y <= m_nCellCount; y++ )
		{
			for( int x = 0; x <= m_nCellCount; x++ )
			{
				index = ((m_nYIndex * m_nCellCount + y) * m_nWidthVtxCount) + (m_nXIndex * m_nCellCount + x);
				pVtxNor[ y * (m_nCellCount + 1) + x ] = spHeightMap[ index ];
			}
		}

		pTileData->MarkAsChanged( NiGeometryData::NORMAL_MASK );
	}
}

//			스태틱 오브젝트 청크 생성 여부
BOOL	Tile::IsInitStaticAtChunk()
{
	return m_pStaticObjectAtChunk != NULL;
}


//			스태틱 오브젝트 청크 생성 
BOOL	Tile::GenerateStaticObjectAtChunk()
{
	if( !m_pStaticObjectAtChunk )
	{
		m_pStaticObjectAtChunk = NiNew StaticObjectAtChunk();

		if( m_pStaticObjectAtChunk )
		{
			m_pStaticObjectAtChunk->SetTileIndex( m_nXIndex, m_nYIndex );

			m_spTileNode->AttachChild( m_pStaticObjectAtChunk->GetRootObject() );
			m_spTileNode->Update(0.0f);
			m_spTileNode->UpdateProperties();

			return TRUE;
		}
	}

	return FALSE;
}

//			컬리전 Manager 생성 여부
BOOL		Tile::IsInitCollisionManger()
{
	return m_pCollsionManager != NULL;
}

//			컬리전 Manager 생성
BOOL		Tile::GenerateCollisionManger()
{
	
	if( !m_pCollsionManager )
	{
		m_pCollsionManager = NiNew CollisionManager();
		
		return TRUE;
	}

	return FALSE;

}

/*
BOOL Tile::IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, int &nTileNum, NifMesh *pMesh )
{
	if( !(	(m_vMax.x < vMin.x || m_vMin.x > vMax.x) ||	
			(m_vMax.y < vMin.y || m_vMin.y > vMax.y) ) )
	{

		if( !m_pStaticObjectAtChunk )
			GenerateStaticObjectAtChunk();

		pMesh->SetAppCulled( false );

		// 컬리젼 그룹 포함
		// 렌더링 최상위 Min, Max 추출
		NiNode *pCollision = (NiNode*)(pMesh->GetRootObject()->GetObjectByName( "Collision" ));
		
		NiPoint3 vColiMin, vColiMax;	
		unsigned int uiCntChild = pCollision->GetChildCount();

		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			if( NiIsKindOf( NiNode, pCollision->GetAt( 0 ) ) )
			{ 
				NiNode *pNDLCD_BN = (NiNode*)pCollision->GetAt( 0 );

				if( NiIsKindOf( NiGeometry, pNDLCD_BN->GetAt(0) ) )
				{
					NiAVObject *pShapeObject	= (NiAVObject*)pNDLCD_BN->GetAt( 0 );

					CGameApp::GetMinMax( (NiNode*)pShapeObject, &vColiMin, &vColiMax );

					if( !(	(m_vMax.x < vColiMin.x || m_vMin.x > vColiMax.x) || 
							(m_vMax.y < vColiMin.y || m_vMin.y > vColiMax.y)	) )
					{
						if( m_vMin.x <= vColiMin.x && vColiMax.x <= m_vMax.x && 
							m_vMin.y <= vColiMin.y && vColiMax.y <= m_vMax.y )
						{
							pMesh->AddCollisionGroup( ui, nTileNum, StaticObjectAtChunk::ENTIRE_INCLUDE );
							continue;
						}

						pMesh->AddCollisionGroup( ui, nTileNum, StaticObjectAtChunk::HALF_INCLUDE );
						continue;
					}
				}
			}
		}

		// 전체 포함
		if( m_vMin.x <= vMin.x && vMax.x <= m_vMax.x &&
			m_vMin.y <= vMin.y && vMax.y <= m_vMax.y )
		{
			m_pStaticObjectAtChunk->AddStaticObject( pMesh->GetRootObject(), StaticObjectAtChunk::ENTIRE_INCLUDE );

			pMesh->AddRenderingGroup( nTileNum, StaticObjectAtChunk::ENTIRE_INCLUDE );

			return TRUE;
		}

		m_pStaticObjectAtChunk->AddStaticObject( pMesh->GetRootObject(), StaticObjectAtChunk::HALF_INCLUDE );

		pMesh->AddRenderingGroup( nTileNum, StaticObjectAtChunk::HALF_INCLUDE );

		return TRUE;
	}

	return FALSE;
}
*/
BOOL	Tile::IsCollisionOfTile( const NiPoint3 &vMin, const NiPoint3 &vMax, NiGeometry *pMesh, int &nIncludeType )
{
	if( !(	(m_vMax.x < vMin.x || m_vMin.x > vMax.x) ||	
			(m_vMax.y < vMin.y || m_vMin.y > vMax.y) ) )
	{

		// 전체 포함
		if( m_vMin.x <= vMin.x && vMax.x <= m_vMax.x &&
			m_vMin.y <= vMin.y && vMax.y <= m_vMax.y )
		{
			 nIncludeType = StaticObjectAtChunk::ENTIRE_INCLUDE;
			 
			 return TRUE;
		}

		 nIncludeType = StaticObjectAtChunk::HALF_INCLUDE;
		 
		 return TRUE;

	}

	return FALSE;
}

//----------------------------------------------------------------------
//
//		오브젝트의 렌더링 포함 타일과 충돌박스 포함 타일을 결정한다.
BOOL Tile::SpaceDivisionRenderAndCollision( NifMesh *pMesh )
{
	
	NiNode *pObject = pMesh->GetRootObject();

	// 렌더링 최상위 Min, Max 추출
	NiNode *pRenderingGroup = (NiNode*)pObject->GetObjectByName( "RenderingGroup" );
	
	if( !pRenderingGroup ) return FALSE;

	NiPoint3 vMin, vMax;

	CGameApp::GetMinMax( pRenderingGroup, &vMin, &vMax );

	if( !(	(m_vMax.x < vMin.x || m_vMin.x > vMax.x) ||	
			(m_vMax.y < vMin.y || m_vMin.y > vMax.y) ) )
	{

		if( !m_pStaticObjectAtChunk )
			GenerateStaticObjectAtChunk();

		pMesh->SetAppCulled( false );

		//=================================================================================
		//									렌더링 그룹
		//=================================================================================
		for( unsigned int ui = 0; ui < pRenderingGroup->GetChildCount(); ui++ )
		{
			NiNode *pOjbect = (NiNode*)pRenderingGroup->GetAt( ui );
			
			CGameApp::GetMinMax( pOjbect, &vMin, &vMax );
			
			if( !(	(m_vMax.x < vMin.x || m_vMin.x > vMax.x) || 
					(m_vMax.y < vMin.y || m_vMin.y > vMax.y)	) )
			{
				if( m_vMin.x <= vMin.x && vMax.x <= m_vMax.x && 
					m_vMin.y <= vMin.y && vMax.y <= m_vMax.y )
				{
					pMesh->AddRenderingGroup( ui, m_nIndex, StaticObjectAtChunk::ENTIRE_INCLUDE );
					continue;
				}

				pMesh->AddRenderingGroup( ui, m_nIndex, StaticObjectAtChunk::HALF_INCLUDE );
				continue;
			}
		}

		//=================================================================================
		//									컬리젼 그룹
		//=================================================================================
		NiNode *pCollision = (NiNode*)(pMesh->GetRootObject()->GetObjectByName( "Collision" ));
		
		NiPoint3 vColiMin, vColiMax;
	
		// 출동박스를 포함하는 타일 정보를 추출
		unsigned int uiCntChild = pCollision->GetChildCount();
		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			if( NiIsKindOf( NiNode, pCollision->GetAt( 0 ) ) )
			{ 
				NiNode *pNDLCD_BN = (NiNode*)pCollision->GetAt( 0 );

				if( NiIsKindOf( NiGeometry, pNDLCD_BN->GetAt(0) ) )
				{
					NiNode *pShapeObject	= (NiNode*)pNDLCD_BN->GetAt( 0 );

					CGameApp::GetMinMax( pShapeObject, &vColiMin, &vColiMax );

					if( !(	(m_vMax.x < vColiMin.x || m_vMin.x > vColiMax.x) || 
							(m_vMax.y < vColiMin.y || m_vMin.y > vColiMax.y)	) )
					{
						if( m_vMin.x <= vColiMin.x && vColiMax.x <= m_vMax.x && 
							m_vMin.y <= vColiMin.y && vColiMax.y <= m_vMax.y )
						{
							pMesh->AddCollisionGroup( ui, m_nIndex, StaticObjectAtChunk::ENTIRE_INCLUDE );
							continue;
						}

						pMesh->AddCollisionGroup( ui, m_nIndex, StaticObjectAtChunk::HALF_INCLUDE );
						continue;
					}
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void Tile::SetRenderingGroup(  NiNode *pObject, int &nIncludeType ) 
{
	if( !IsInitStaticAtChunk() )
		GenerateStaticObjectAtChunk();

	m_pStaticObjectAtChunk->AddStaticObject( pObject, nIncludeType );
}

void Tile::SetCollisionGroup(  NiNode *pObject, const NiTransform& transform )
{
	if( !IsInitCollisionManger() )
		GenerateCollisionManger();

	m_pCollsionManager->AddCollidee( pObject, transform );
	
}

void Tile::SetCollisionGroupForMap(  NiNode *pObject )
{
	if( !IsInitCollisionManger() )
		GenerateCollisionManger();

		m_pCollsionManager->AddCollideeForMap( pObject );
}

void	Tile::SetCollisionTransform( const NiTransform &transform )
{
	if( m_pCollsionManager )
	{
		m_pCollsionManager->SetCollisionTransform( transform );
	}
}

void	Tile::SetRenderingTransform( const NiTransform &transform )
{
	if( m_pStaticObjectAtChunk )
	{
		m_pStaticObjectAtChunk->SetRenderingTransform( transform );
	}	
}

//		케릭터를 각 타일의 CollsionManager에 등록하고 초기화 한다.
BOOL	Tile::SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager )
{
	//컬리전 그룹 생성 여부
	if( !IsInitCollisionManger() )
	{
		return FALSE;
	}

	//컬리전 그룹 초기화
	if( !m_pCollsionManager->Initialize( pkCharacterManager->GetUserCharacter()->GetABVRoot(), pkCharacterManager->GetColliderVector() ) )
	{
		NiMessageBox( " SetCharacterAtCollisionGroup() - m_pCollsionManager->Initialize() is Failed", "Failed" );
		return FALSE;
	}

	// 컬리전 그룹에 Object Pick Root 셋팅
	if( m_pStaticObjectAtChunk )
	{
		m_pCollsionManager->SetPickTarget( m_pStaticObjectAtChunk->GetCollsionRoot() );
	}

	return TRUE;
}

CollisionManager*	Tile::GetCollisionManager()
{
	return m_pCollsionManager;
}

/*
BOOL Tile::Save( CArchive& ar )
{

	if( !m_spTile  )
	{
		NiMessageBox(" Tile::Save() - Failed", "Failed" );
		return FALSE;
	}

	NiTriShapeData *pTriModelData = (NiTriShapeData*)m_spTile->GetModelData();

	if( !pTriModelData )
		return FALSE;

	NiPoint3 *pVtxData = pTriModelData->GetVertices();
	NiPoint3 *pNorData = pTriModelData->GetNormals();
	NiPoint2 *pUVData  = pTriModelData->GetTextures();

	if( !pVtxData || !pNorData || !pUVData )
	{
		NiMessageBox(" Tile::Save() - pVtxData, pNorData, pUVData NULL", "Failed" );
		return FALSE;
	}

		// 텍스쳐 알파값 정보
	unsigned int iPitch = static_cast< unsigned int >( m_nPixelSize * 4 );
	NiPixelData *pPixelData =m_spControlMap->GetSourcePixelData();
	unsigned char* pPixelBuf = pPixelData->GetPixels();

	if( !pPixelBuf )
	{
		NiMessageBox(" Tile::Save() - pPixelBuf NULL", "Failed" );
		return FALSE;
	}

	if( NULL != m_pBoundBox )
		m_pBoundBox->ResetMinMax();

	for( int y = 0; y <= m_nCellCount; y++ )
	{
		for( int x = 0; x <= m_nCellCount; x++ )
		{			
			// Pos 셋팅
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].y ;
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].z ;			

			// 노멀 셋팅
			pNorData[  y * (m_nCellCount + 1) + x ].Unitize();
			ar << pNorData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pNorData[  y * (m_nCellCount + 1) + x ].y ;
			ar << pNorData[  y * (m_nCellCount + 1) + x ].z ;

			// 텍스쳐 UV 셋팅
			ar << pUVData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pUVData[  y * (m_nCellCount + 1) + x ].y ;

			m_pBoundBox->Include( pVtxData[  y * (m_nCellCount + 1) + x ] );
		}
	}

	// 바운딩 박스
	NiPoint3 min, max;
	m_pBoundBox->GetMinMax( &min, &max );
	m_pBoundBox->SetMinMax( min, max );

	ar << min.x << min.y << min.z ;
	ar << max.x << max.y << max.z ;

	// 픽셀 정보 저장
	ar << m_nPixelSize;

	int count = 0;
	for( unsigned int y = 0; y < m_nPixelSize; y++ )
	{
		for( unsigned int x = 0; x < m_nPixelSize; x++ )
		{
			ar << pPixelBuf[ count++ ];		// B
			ar << pPixelBuf[ count++ ];		// G
			ar << pPixelBuf[ count++ ];		// R
			ar << pPixelBuf[ count++ ];		// A	
		}
	}

	return TRUE;
}

*/
BOOL Tile::Load( FILE* file )
{
	if( !m_spTile  )
	{
		NiMessageBox(" Tile::Save() - Failed", "Failed" );
		return FALSE;
	}

	NiTriShapeData *pTriModelData = (NiTriShapeData*)m_spTile->GetModelData();

	if( !pTriModelData )
		return FALSE;
	
	NiPoint3 *pVtxData = pTriModelData->GetVertices();
	NiPoint3 *pNorData = pTriModelData->GetNormals();
	NiPoint2 *pUVData  = pTriModelData->GetTextures();

	if( !pVtxData || !pNorData || !pUVData )
	{
		NiMessageBox(" Tile::Save() - pVtxData, pNorData, pUVData NULL", "Failed" );
		return FALSE;
	}

	char buf[ 256 ];
	int index = 0;
	for( int y = 0; y <= m_nCellCount; y++ )
	{
		for( int x = 0; x <= m_nCellCount; x++ )
		{	
			index = y * (m_nCellCount + 1) + x;
			
			fread( buf, 1, sizeof(float) * 8, file );

			// Pos 셋팅
			memcpy( &pVtxData[  index ].x, buf						, sizeof(float));
			memcpy( &pVtxData[  index ].y, buf + sizeof(float)		, sizeof(float));
			memcpy( &pVtxData[  index ].z, buf + sizeof(float) * 2	, sizeof(float));

			// Normal 셋팅
			memcpy( &pNorData[  index ].x, buf + sizeof(float) * 3	, sizeof(float));
			memcpy( &pNorData[  index ].y, buf + sizeof(float) * 4	, sizeof(float));
			memcpy( &pNorData[  index ].z, buf + sizeof(float) * 5	, sizeof(float));

			// 텍스쳐 UV 셋팅
			memcpy( &pUVData[  index ].x, buf + sizeof(float) * 6	, sizeof(float));
			memcpy( &pUVData[  index ].y, buf + sizeof(float) * 7	, sizeof(float));
			
		}
	}	
		
	pTriModelData->MarkAsChanged( NiGeometryData::NORMAL_MASK	);
	pTriModelData->MarkAsChanged( NiGeometryData::VERTEX_MASK	);
	pTriModelData->MarkAsChanged( NiGeometryData::TEXTURE_MASK	);

	// 바운딩 박스 

	fread( buf, 1, sizeof(float) * 6, file );

	memcpy( &m_vMin.x, buf						, sizeof(float));
	memcpy( &m_vMin.y, buf + sizeof(float)		, sizeof(float));
	memcpy( &m_vMin.z, buf + sizeof(float) * 2	, sizeof(float));

	memcpy( &m_vMax.x, buf + sizeof(float) * 3	, sizeof(float));
	memcpy( &m_vMax.y, buf + sizeof(float) * 4	, sizeof(float));
	memcpy( &m_vMax.z, buf + sizeof(float) * 5	, sizeof(float));

	if( m_pBoundBox ) m_pBoundBox->SetMinMax( m_vMin, m_vMax );

	// 픽셀 정보 저장
	fread( buf, 1, sizeof(int) , file );

	memcpy( &m_nPixelSize, buf, sizeof(int));

	if( m_spControlMap )
	{
		// 텍스쳐 알파값 정보
		unsigned int iPitch = static_cast< unsigned int >( m_nPixelSize * 4 );
		NiPixelData *pPixelData = m_spControlMap->GetSourcePixelData();
		unsigned char* pPixelBuf = pPixelData->GetPixels();

		int count = 0;
		int size = sizeof(pPixelBuf);
		for( unsigned int y = 0; y < m_nPixelSize; y++ )
		{
			for( unsigned int x = 0; x < m_nPixelSize; x++ )
			{
				fread( buf, 1, sizeof(unsigned char) * 4 , file );

				memcpy( &pPixelBuf[ count++ ], buf + sizeof(unsigned char) * 0	, sizeof(unsigned char) );
				memcpy( &pPixelBuf[ count++ ], buf + sizeof(unsigned char) * 1	, sizeof(unsigned char) );
				memcpy( &pPixelBuf[ count++ ], buf + sizeof(unsigned char) * 2	, sizeof(unsigned char) );
				memcpy( &pPixelBuf[ count++ ], buf + sizeof(unsigned char) * 3	, sizeof(unsigned char) );
			}
		}
	}

	return TRUE;
}
