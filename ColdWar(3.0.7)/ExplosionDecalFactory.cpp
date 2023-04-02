#include "main.h"
#include "ExplosionDecalFactory.h"
#include "GameApp.h"

ExplosionDecalFactory::ExplosionDecalFactory( int nCntRect )
{
	
	m_nCntRect= nCntRect;

	m_nCntVtx = (m_nCntRect + 1) * (m_nCntRect + 1);
	m_nCtnTri = m_nCntRect * m_nCntRect * 2;

	CGameApp::SetMediaPath("Data/Object/Decal/");

	m_spTexture = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename( "Decal.tga" ) );
	m_spTexture->SetBaseFilterMode( NiTexturingProperty::FILTER_TRILERP );
	m_spTexture->SetBaseClampMode( NiTexturingProperty::CLAMP_MAX_MODES );
	m_spTexture->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );

	m_spAlpha = NiNew NiAlphaProperty();
	m_spAlpha->SetAlphaBlending(true);
	m_spAlpha->SetAlphaTesting(true);
	m_spAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	m_spAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	m_spAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	m_spMaterial = NiNew NiMaterialProperty;
	m_spMaterial->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	m_spMaterial->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
	m_spMaterial->SetShineness(10.0f);
	

}

BOOL	ExplosionDecalFactory::Init( )
{
	
	return TRUE;
}

ExplosionDecalFactory::~ExplosionDecalFactory()
{
	if( m_spTexture || m_spAlpha || m_spMaterial )
	{
		m_spAlpha = 0;
		m_spMaterial = 0;
		m_spTexture = 0;
	}
}



BOOL	ExplosionDecalFactory::GenerateDecal( const NiPoint3 *vertex, const NiPoint3 *normal, std::vector<int>& vecDecalTri, DecalAtChunk *pDecalAtChunk )
{
	
	if( vecDecalTri.empty() )
		return FALSE;

	int count = 0;
	std::vector< int >::iterator iterTri;

	NiPoint3 *pVtx = NiNew NiPoint3[ m_nCntVtx ];
	NiPoint3 *pNor = NiNew NiPoint3[ m_nCntVtx ];
	NiPoint2 *pUV  = NiNew NiPoint2[ m_nCntVtx ];
	unsigned short *pUsTriIndex	= NiAlloc( unsigned short, m_nCtnTri * 3 );

	::ZeroMemory( pVtx, sizeof(pVtx) );
	::ZeroMemory( pNor, sizeof(pNor) );
	::ZeroMemory( pUV, sizeof(pUV) );	
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );

	

	for( iterTri = vecDecalTri.begin(); iterTri != vecDecalTri.end(); ++iterTri )
	{	
		int & index = *iterTri;
		pVtx[ count ] = vertex[ index ];
		pNor[ count ] = normal[ index ];

		count++;
	}

	float fWidthUV = 1.0f / m_nCntRect;
	int nCntWidthVtx = m_nCntRect + 1;
	int nRectIndex = 0;

	for( int y = 0; y < nCntWidthVtx; y++ )
	{
		for( int x= 0; x <nCntWidthVtx; x++ )
		{	
			nRectIndex = y * 4 + x;

			// 텍스쳐 UV 셋팅
			pUV[ nRectIndex ].x = x * fWidthUV;
			pUV[ nRectIndex ].y = 1.0f - y * fWidthUV;
		}
	}

	// 인텍스 버퍼 갑 셋팅 
	int index = 0;	

	for(  int y = 0; y < m_nCntRect ; y++ )
	{
		for( int x = 0; x < m_nCntRect ; x++ )
		{
			pUsTriIndex[ index++ ] = (y + 1) * nCntWidthVtx + x + 1; 
			pUsTriIndex[ index++ ] = (y + 1) * nCntWidthVtx + x;
			pUsTriIndex[ index++ ] = y * nCntWidthVtx + x;
			
			pUsTriIndex[ index++ ] = y * nCntWidthVtx + x + 1; 
			pUsTriIndex[ index++ ] = (y + 1) * nCntWidthVtx + x + 1;
			pUsTriIndex[ index++ ] = y * nCntWidthVtx + x;
		}
	}	

	NiTriShapePtr spExplosionDecalShape = NiNew NiTriShape( m_nCntVtx, pVtx, pNor, NULL, pUV, 1, NiGeometryData::NBT_METHOD_NONE, m_nCtnTri, pUsTriIndex);	

	NiTexturingPropertyPtr	spTexture	= (NiTexturingProperty*)m_spTexture->CreateClone( m_kCloning );
	NiAlphaPropertyPtr		spAlpha		= (NiAlphaProperty*)m_spAlpha->CreateClone( m_kCloning );
	NiMaterialPropertyPtr	spMaterial	= (NiMaterialProperty*)m_spMaterial->CreateClone( m_kCloning );

	spExplosionDecalShape->AttachProperty( spTexture );
	spExplosionDecalShape->AttachProperty( spAlpha );
	spExplosionDecalShape->AttachProperty( spMaterial );

	spExplosionDecalShape->Update(0.0f);
	spExplosionDecalShape->UpdateProperties();

	// 해당 청크에 삽입
	if( pDecalAtChunk->AddDecal( spExplosionDecalShape ) )
		return TRUE;



	
	return FALSE;
}