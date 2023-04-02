#include "main.h"
#include "Tile.h"
#include "GameApp.h"
#include "BillBoardManager.h"


Tile::Tile()
{
	m_nLodLevel		= 0;		// SLOD ����
	m_nCellCount	= 0;		// Ÿ�ϳ� �� ����
	m_nVtxCount		= 0;		// ���ؽ� ����
	m_nWidthVtxCount= 0;		// ���� ��ü ���� ���ؽ� ����
	m_nTriangles	= 0;		// �ﰢ�� ����
	m_uiIndexCount	= 0;
	m_uiIBSize		= 0;
	m_nPixelSize	= 0;

	m_fPixelSize	= 0.0f;

	m_bDrawBB		= false;	// �ٿ�� �ڽ� Draw ����
	m_bGetOldIB		= false;
	
	m_pBoundBox		= NULL;

	m_pIB			= NULL;		// �ε��� ����
	m_pOldIB		= NULL;		// �ʱ� NiTriShape�� ���� ������ �ε��� ����
	m_nXIndex		= -1;		// X, Y �ε��� ��ġ
	m_nYIndex		= -1;
	m_nCurrentShaderMap = -1;
		
	m_bitCrackType.reset();		// 4��Ʈ 0���� �ʱ�ȭ	

	m_pBillBoardAtChunk = NULL;	// ������ ûũ

	m_pDecalAtChunk		= NULL; // ��Į ûũ

	m_pStaticObjectAtChunk	= NULL; // ����ƽ ������Ʈ ûũ

	m_pCollsionManager		= NULL;	// �浹 ����
}

Tile::~Tile()
{

	m_spTileNode	= 0;
//	m_spTile		= 0;		// ���� ���� 
	m_bDrawBB		= FALSE;	// �ٿ�� �ڽ� Draw ����
	m_pBoundBox		= NULL;

	m_bitCrackType.reset();		// 4��Ʈ 0���� �ʱ�

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

	// �ٿ�� �ڽ�
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

	// NiTriShape ��ü
	if( m_spTileNode )
	{
		pRootNode->DetachChild( m_spTileNode );
	}

	if( m_spTile )
	{
		m_spTileNode->DetachChild( m_spTile );
	}

	m_nLodLevel		= 0;		// SLOD ����
	m_nCellCount	= 0;		// Ÿ�ϳ� �� ����
	m_nVtxCount		= 0;		// ���ؽ� ����
	m_nTriangles	= 0;		// �ﰢ�� ����	
	m_bDrawBB		= FALSE;	// �ٿ�� �ڽ� Draw ����
	m_pIB			= NULL;		
	m_pBillBoardAtChunk = NULL;
	m_pDecalAtChunk		= NULL;

	m_bitCrackType.reset();		// 4��Ʈ 0���� �ʱ�ȭ	
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
	
	// Min Max ���ϱ�
	m_vMin = NiPoint3( fXPos, fYPos,				0.0f 					);
	m_vMax = NiPoint3( fXPos + fWidthSize,  fYPos + fHeightSize,	0.0f 	);

	m_nXIndex = xIndex;
	m_nYIndex = yIndex;
	m_fWidthSize = fWidthSize;
	m_fHeightSize = fHeightSize;

	

	// �ٿ�� �ڽ� ����
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
	m_nVtxCount			= (nCellCount+1)*(nCellCount+1);	// �� ���δ� ���ؽ� ����
	m_nTriangles		= nCellCount * nCellCount * 2;		// �ﰢ�� ����
	m_nWidthVtxCount	= nCellCount * nTileCount + 1;
	
	m_nPixelSize		= m_nCellCount * 10;	// ��	����
	m_fPixelSize		= (float)fWidthSize / (float)m_nPixelSize;

	float	vtxWidthSpacing		= fWidthSize  / nCellCount;
	float	vtxHeightSpacing	= fHeightSize / nCellCount;
	float	texUVSpacing		= 1.0f		  / nCellCount;

	// Ÿ�� ���� ���� ����
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
			// Pos ����
			
			pVPosition[  y * (nCellCount + 1) + x ].x = fXPos + (x*vtxWidthSpacing);
			pVPosition[  y * (nCellCount + 1) + x ].y = fYPos + (y*vtxHeightSpacing);
			pVPosition[  y * (nCellCount + 1) + x ].z = 0.0f;

			// ��� ����
			pVNormal[  y * (nCellCount + 1) + x ].x = 0.0f;
			pVNormal[  y * (nCellCount + 1) + x ].y = 0.0f;
			pVNormal[  y * (nCellCount + 1) + x ].z = 1.0f;

			// ���ȭ
			pVNormal->Unitize();

			// ���ؽ� �÷�
			pkColor[ y * (nCellCount + 1) + x ].r = 1.0f;
			pkColor[ y * (nCellCount + 1) + x ].g = 0.0f;
			pkColor[ y * (nCellCount + 1) + x ].b = 0.0f;
			pkColor[ y * (nCellCount + 1) + x ].a = 1.0f;

			// �ؽ��� UV ����
			pUV[ y * (nCellCount + 1) + x ].x = x * texUVSpacing;
			pUV[ y * (nCellCount + 1) + x ].y = 1.0f - y * texUVSpacing;

		}
	}

	--x; --y;

	m_vCenterPos = ( pVPosition[ 0 ] + pVPosition[ x ] + pVPosition[ y * (nCellCount + 1) ] + pVPosition[ y * (nCellCount + 1) + x] ) / 4;

	
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

	// ���ؽ� ���� �� ���� 
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
	
	m_spTile->CalculateNormals();		// ��� ����

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

	// ��ü ������ �ٿ���ڽ� �ֻ��� ��Ʈ�� �ڽ����� Attach
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

		// RGBA ���� �� ����
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

//		���� �ؽ��� 1�� �߰�
BOOL	Tile::AddShaderMap( NiFixedString &fileName, int index )
{

	if( !m_spTexture || m_spTexture->GetShaderArrayCount() > 5 )
		return FALSE;
	
	// �ؽ��� ������ : Layer_00 �ȼ� : R
	NiTexturingProperty::ShaderMap* pMap	= NiNew NiTexturingProperty::ShaderMap;
	NiTextureTransform *pTransform			= NiNew NiTextureTransform;

	pTransform->SetScale( NiPoint2( 1.0f, 1.0f ) );
	pMap->SetTextureTransform( pTransform );

	CGameApp::SetMediaPath( "Data/Texture/" );

	// �ؽ��� �ҷ�����
	NiSourceTexture* pTexture = NiSourceTexture::Create( CGameApp::ConvertMediaFilename( fileName ) ); 
	pMap->SetTexture( pTexture );
	
	m_spTexture->SetShaderMap( index, pMap );

	return TRUE;	
}


//		���İ��� ������ �ؽ��� ���� - SourceTexture
BOOL	Tile::CreateControlMap	( NiSourceTexturePtr &pSourceTex )
{
	//m_nPixelSize = m_nCellCount * 10;	// ��	����

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

//		���� ������ ���̴� �� ������ ����
BOOL	Tile::ShaderMapScaleTransForm( const NiPoint2& scaleRate )
{
	// �ؽ��� ������Ƽ ����X , ������ ���̴� �� ����, ���̴� �� ���� ��Ʈ�� �ʸ� ������
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
	// �ؽ��� ������Ƽ ����X , ������ ���̴� �� ����, ���̴� �� ���� ��Ʈ�� �ʸ� ������
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

//		������ �ε����� �ش��ϴ� ���̴� �ʰ� ������ ��Ʈ�� ���� RGBA�� ���Ͽ� Reset;
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

//			���� ������ �ؽ����� ���İ� ����
void	Tile::DrawControlMapRGBA( const NiPoint2& pickPixel, float fInsideLength, float fOutsideLength, int drawType )
{
	if( !m_spTexture )
		return;

	// �ȼ� ����Ÿ ������
	unsigned int iPitch = static_cast< unsigned int >( m_nPixelSize * 4 );
	NiPixelData *pPixelData =m_spControlMap->GetSourcePixelData();
	unsigned char* pPixelBuf = pPixelData->GetPixels();

	if( !pPixelBuf )return;	

	// ���� ��� �ؼ��� ����� ���Ѵ�.
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

			case Terrain::TEXTURE_DRAW: // �����ؽ��� �׸���

				if( fLength <= fInsideLength )// ���� ������ ������	
				{
					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = 0xFF;			// RGBA A���İ� ����				

				}
				else if( fLength <= fOutsideLength )	// ���� ������ ũ�� �ٱ��� ������ �������//( fInsideLength < fLength && fLength <= fOutsideLength  )	// ���� ������ ũ�� �ٱ��� ������ �������
				{

					fLength -= fInsideLength;
					fSmooth = fOutsideLength - fInsideLength;

					BYTE nOldColor = pPixelBuf[ rectIndex + m_nCurrentShaderMap ];
					newColor = static_cast<BYTE>( (fSmooth - fLength) / fSmooth * 0xff);

					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = ( nOldColor < newColor ) ? newColor : nOldColor;

				}


				break;

			
			case Terrain::TEXTURE_ERASE: // �����ؽ��� �����

				if(  fLength <= fInsideLength  )		// ���� ������ ������
				{
					pPixelBuf[ rectIndex + m_nCurrentShaderMap ] = 0x00;			// RGBA A���İ� ����			
				}
				else if( fInsideLength < fLength && fLength <= fOutsideLength )	// ���� ������ ũ�� �ٱ��� ������ �������
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
	// 4 Bit ���� 0 ���� �ʱ�ȭ
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

//		������ ûũ ���� ����
BOOL	Tile::IsInitBillBoardAtChunk()
{
	return m_pBillBoardAtChunk != NULL;
}


//		������ ûũ ����
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

//		������ ûũ�� �߰��� ������ ������Ʈ ��ŷ
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

//		������ ûũ ����
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

//		��Į ûũ�� Ÿ�Կ� ���� ������ ������Ʈ �߰�
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

//		������ ûũ�� Ÿ�Կ� ���� ������ ��ũ��Ʈ �߰�
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

// SLOD ���⿡ ���� ��Ʈ�� ����       BOTTOM | RIGHT | TOP | LEFT
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


//------------------ Ÿ���� Min Max ���� ��´� ----------------------
void Tile::GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax )
//--------------------------------------------------------------------
{
	if( m_spTile != NULL && m_pBoundBox != NULL)
	{
		m_pBoundBox->GetMinMax( vMin, vMax );
	}
}


//------------------ Tile�� �ٿ�� �ڽ� �浹�� üũ�Ѵ�.------------------
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


//------------------ Tile�� �ٿ�� �ڽ� �浹�� üũ�Ѵ�.------------------
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

	// 2���� ��ŷ ���а� Ÿ���� 4�� ���а��� Intersection�� üũ
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

	// Ÿ���� ���� ����
	if( GetIntersectPoint( LT, RT, pos1, pos2, &vIntersection ) )
		if( m_vMin.x <= vIntersection.x && vIntersection.x <= m_vMax.x  ) return TRUE;

	// Ÿ�� �Ʒ� ����
	if( GetIntersectPoint( LB, RB, pos1, pos2, &vIntersection ) )
		if( m_vMin.x <= vIntersection.x && vIntersection.x <= m_vMax.x  ) return TRUE;

	// Ÿ�� ���� ����
	if( GetIntersectPoint( LB, LT, pos1, pos2, &vIntersection ) )
		if( m_vMin.y <= vIntersection.y && vIntersection.y <= m_vMax.y  ) return TRUE;

	// Ÿ�� ������ ����
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
//	������Ʈ�� ���� Ÿ�ϰ��� �ٿ�� �ڽ� �浹����
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
//		������Ʈ�� �浹�� ������ ��ġ�� ����� ��´�.
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

//------------------ ������ Tile �� ������ ���� ����� �浹 ��ġ ����--------
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

		*xIndex = triIndex % m_nCellCount; //�ʺ�
		*yIndex = triIndex / m_nCellCount; //����

		
		
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;
}

//		�����ϰ� �ִ� StaticObjectChunk�� �ִ� Object�鿡 ���ؼ� Picking Ray ���� �ǽ�
BOOL	Tile::PickRayAtObjectGroup( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *pickNor, NiPick *pPick, float *fDist )
{
	if( !m_pStaticObjectAtChunk )
		return FALSE;

	if( m_pStaticObjectAtChunk->PickRayAtObjects( pos, dir, pickpos, pickNor, pPick, fDist ) )
		return TRUE;

	return FALSE;
}

//------------------ ������ Tile �� ������ ���� ����� �浹 ��ġ ����--------
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
	
		xIndex = triIndex % m_nCellCount; //�ʺ�
		yIndex = triIndex / m_nCellCount; //����
		
		
		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;

}




//------------Ÿ���� ��ֹ� ���� ������ �迭�� ����-----------
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

//---------------------------- Ÿ���� ������ ���ο� ��� ���� -----------------------------
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

//			����ƽ ������Ʈ ûũ ���� ����
BOOL	Tile::IsInitStaticAtChunk()
{
	return m_pStaticObjectAtChunk != NULL;
}


//			����ƽ ������Ʈ ûũ ���� 
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

//			�ø��� Manager ���� ����
BOOL		Tile::IsInitCollisionManger()
{
	return m_pCollsionManager != NULL;
}

//			�ø��� Manager ����
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

		// �ø��� �׷� ����
		// ������ �ֻ��� Min, Max ����
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

		// ��ü ����
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

		// ��ü ����
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
//		������Ʈ�� ������ ���� Ÿ�ϰ� �浹�ڽ� ���� Ÿ���� �����Ѵ�.
BOOL Tile::SpaceDivisionRenderAndCollision( NifMesh *pMesh )
{
	
	NiNode *pObject = pMesh->GetRootObject();

	// ������ �ֻ��� Min, Max ����
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
		//									������ �׷�
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
		//									�ø��� �׷�
		//=================================================================================
		NiNode *pCollision = (NiNode*)(pMesh->GetRootObject()->GetObjectByName( "Collision" ));
		
		NiPoint3 vColiMin, vColiMax;
	
		// �⵿�ڽ��� �����ϴ� Ÿ�� ������ ����
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

//		�ɸ��͸� �� Ÿ���� CollsionManager�� ����ϰ� �ʱ�ȭ �Ѵ�.
BOOL	Tile::SetCharacterAtCollisionGroup( CharacterManager *pkCharacterManager )
{
	//�ø��� �׷� ���� ����
	if( !IsInitCollisionManger() )
	{
		return FALSE;
	}

	//�ø��� �׷� �ʱ�ȭ
	if( !m_pCollsionManager->Initialize( pkCharacterManager->GetUserCharacter()->GetABVRoot(), pkCharacterManager->GetColliderVector() ) )
	{
		NiMessageBox( " SetCharacterAtCollisionGroup() - m_pCollsionManager->Initialize() is Failed", "Failed" );
		return FALSE;
	}

	// �ø��� �׷쿡 Object Pick Root ����
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

		// �ؽ��� ���İ� ����
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
			// Pos ����
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].y ;
			ar << pVtxData[  y * (m_nCellCount + 1) + x ].z ;			

			// ��� ����
			pNorData[  y * (m_nCellCount + 1) + x ].Unitize();
			ar << pNorData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pNorData[  y * (m_nCellCount + 1) + x ].y ;
			ar << pNorData[  y * (m_nCellCount + 1) + x ].z ;

			// �ؽ��� UV ����
			ar << pUVData[  y * (m_nCellCount + 1) + x ].x ;
			ar << pUVData[  y * (m_nCellCount + 1) + x ].y ;

			m_pBoundBox->Include( pVtxData[  y * (m_nCellCount + 1) + x ] );
		}
	}

	// �ٿ�� �ڽ�
	NiPoint3 min, max;
	m_pBoundBox->GetMinMax( &min, &max );
	m_pBoundBox->SetMinMax( min, max );

	ar << min.x << min.y << min.z ;
	ar << max.x << max.y << max.z ;

	// �ȼ� ���� ����
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

			// Pos ����
			memcpy( &pVtxData[  index ].x, buf						, sizeof(float));
			memcpy( &pVtxData[  index ].y, buf + sizeof(float)		, sizeof(float));
			memcpy( &pVtxData[  index ].z, buf + sizeof(float) * 2	, sizeof(float));

			// Normal ����
			memcpy( &pNorData[  index ].x, buf + sizeof(float) * 3	, sizeof(float));
			memcpy( &pNorData[  index ].y, buf + sizeof(float) * 4	, sizeof(float));
			memcpy( &pNorData[  index ].z, buf + sizeof(float) * 5	, sizeof(float));

			// �ؽ��� UV ����
			memcpy( &pUVData[  index ].x, buf + sizeof(float) * 6	, sizeof(float));
			memcpy( &pUVData[  index ].y, buf + sizeof(float) * 7	, sizeof(float));
			
		}
	}	
		
	pTriModelData->MarkAsChanged( NiGeometryData::NORMAL_MASK	);
	pTriModelData->MarkAsChanged( NiGeometryData::VERTEX_MASK	);
	pTriModelData->MarkAsChanged( NiGeometryData::TEXTURE_MASK	);

	// �ٿ�� �ڽ� 

	fread( buf, 1, sizeof(float) * 6, file );

	memcpy( &m_vMin.x, buf						, sizeof(float));
	memcpy( &m_vMin.y, buf + sizeof(float)		, sizeof(float));
	memcpy( &m_vMin.z, buf + sizeof(float) * 2	, sizeof(float));

	memcpy( &m_vMax.x, buf + sizeof(float) * 3	, sizeof(float));
	memcpy( &m_vMax.y, buf + sizeof(float) * 4	, sizeof(float));
	memcpy( &m_vMax.z, buf + sizeof(float) * 5	, sizeof(float));

	if( m_pBoundBox ) m_pBoundBox->SetMinMax( m_vMin, m_vMax );

	// �ȼ� ���� ����
	fread( buf, 1, sizeof(int) , file );

	memcpy( &m_nPixelSize, buf, sizeof(int));

	if( m_spControlMap )
	{
		// �ؽ��� ���İ� ����
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
