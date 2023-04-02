#include "main.h"
#include "CMuzelSplash.h"
#include "GameApp.h"
//#include "ZFLog.h"

CMuzelSplash::CMuzelSplash( float fDelay, int xOffset0, int yOffset0, int xOffset1, int yOffset1 )
{
	m_spSplash[ FRONT ]		= 0;
	m_spSplash[ HORIZON ]	= 0;
	m_spSplash[ VERTICAL ]	= 0;

	::ZeroMemory( m_pktTexTransForm, sizeof(m_pktTexTransForm) );
	::ZeroMemory( m_nXCurOffset, sizeof(m_nXCurOffset) );
	::ZeroMemory( m_nYCurOffset, sizeof(m_nYCurOffset) );	
	
	m_nXOffset[ FRONT ]	= xOffset0;   
	m_nYOffset[ FRONT ]	= yOffset0;   
	m_nXOffset[ HORIZON ] = xOffset1; 
	m_nXOffset[ HORIZON ] = yOffset1; 

	m_fOffsetSpacing[ FRONT ]	= 1.0f / static_cast<float>(xOffset0);;
	m_fOffsetSpacing[ HORIZON ]	= 1.0f / static_cast<float>(xOffset1);;

	m_fDelayTime		= fDelay;	
	m_fLastTime			= 0.0f;
	m_bActive			= false;
}

CMuzelSplash::~CMuzelSplash()
{
	m_spSplash[ FRONT ]->DetachAllProperties();
	m_spSplash[ FRONT ]		= 0;
	m_spSplash[ HORIZON ]->DetachAllProperties();
	m_spSplash[ HORIZON ]	= 0;
	m_spSplash[ VERTICAL ]->DetachAllProperties();
	m_spSplash[ VERTICAL ]	= 0;

	if( m_pktTexTransForm )
	{ 
		::ZeroMemory( m_pktTexTransForm, sizeof(m_pktTexTransForm) );
	}
}

bool CMuzelSplash::Initialize( NiNode* pCharObject, char *pTex0, char *pTex1 )
{	
	
	/*
	// 모델의 로컬 좌표만 복사되어 부모의 영향을 받지 않아서 이걸로 하기는 좀 이상함.
	m_spSplash[ FRONT ]		= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("FrontPlane"))->GetAt(0)->Clone();
	m_spSplash[ HORIZON ]	= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("VerticalPlane"))->GetAt(0)->Clone();
	m_spSplash[ VERTICAL ]	= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("HorizonPlane"))->GetAt(0)->Clone();
	*/

	m_spSplash[ FRONT ]		= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("FrontPlane"))->GetAt(0);
	m_spSplash[ HORIZON ]	= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("VerticalPlane"))->GetAt(0);
	m_spSplash[ VERTICAL ]	= (NiTriStrips*)((NiNode*)pCharObject->GetObjectByName("HorizonPlane"))->GetAt(0);

	if( !m_spSplash[ FRONT ]	|| 
		!m_spSplash[ HORIZON ]	||
		!m_spSplash[ VERTICAL ]	)
		return false;

	// NITriStrips Vtx Indexing			
	// 0----------2
	// |	      |
	// |          |
	// |          |
	// |          |
	// 1----------3

	//---------------------------------------------------------
	//			Front Splash UV Set
	//---------------------------------------------------------	
	NiGeometryData *pModelData = m_spSplash[ FRONT ]->GetModelData();
	NiPoint2 *pUVData = pModelData->GetTextureSet(0);
																	
	if( NULL == pUVData )											
	{
		if( pModelData->GetConsistency() == NiGeometryData::STATIC )
		{
			pModelData->SetConsistency( NiGeometryData::MUTABLE );
		}

		pModelData->CreateTextures( true, 1 );						
		pUVData = pModelData->GetTextures();
	}

	pUVData[ 0 ].x = 0.0f;
	pUVData[ 0 ].y = 0.0f;

	pUVData[ 1 ].x = 0.0f;
	pUVData[ 1 ].y = m_fOffsetSpacing[ FRONT ];

	pUVData[ 2 ].x = m_fOffsetSpacing[ FRONT ];
	pUVData[ 2 ].y = 0.0f;

	pUVData[ 3 ].x = m_fOffsetSpacing[ FRONT ];
	pUVData[ 3 ].y = m_fOffsetSpacing[ FRONT ];

	pModelData->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
	//---------------------------------------------------------


	//---------------------------------------------------------
	//			Horison Splash UV Set
	//---------------------------------------------------------

	for( unsigned int i = HORIZON; i <= VERTICAL; i++ )
	{
		pModelData = m_spSplash[ i ]->GetModelData();
		pUVData = pModelData->GetTextureSet(0);

		if( NULL == pUVData )											
		{
			if( pModelData->GetConsistency() == NiGeometryData::STATIC )
			{
				pModelData->SetConsistency( NiGeometryData::MUTABLE );
			}

			pModelData->CreateTextures( true, 1 );						
			pUVData = pModelData->GetTextures();
		}

/*
		pUVData[ 0 ].x = 0.0f;
		pUVData[ 0 ].y = 0.0f;

		pUVData[ 1 ].x = 0.0f;
		pUVData[ 1 ].y = m_fOffsetSpacing[ HORIZON ];

		pUVData[ 2 ].x = m_fOffsetSpacing[ HORIZON ];
		pUVData[ 2 ].y = 0.0f;

		pUVData[ 3 ].x = m_fOffsetSpacing[ HORIZON ];
		pUVData[ 3 ].y = m_fOffsetSpacing[ HORIZON ];
*/

		pUVData[ 0 ].x = 0.0f;
		pUVData[ 0 ].y = m_fOffsetSpacing[ HORIZON ];

		pUVData[ 1 ].x = m_fOffsetSpacing[ HORIZON ];
		pUVData[ 1 ].y = m_fOffsetSpacing[ HORIZON ];

		pUVData[ 2 ].x = 0.0f;
		pUVData[ 2 ].y = 0.0f;

		pUVData[ 3 ].x = m_fOffsetSpacing[ HORIZON ];
		pUVData[ 3 ].y = 0.0f;
		

		pModelData->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
	}
	//---------------------------------------------------------

	
	//---------------------------------------------------------
	//			TextureTransform Create
	//---------------------------------------------------------
	
	m_pktTexTransForm[ FRONT ] = NiNew NiTextureTransform( 
		NiPoint2(0.0f,0.0f),								// kTranslate
		0.0f,												// fRotate
		NiPoint2(1.0f,1.0f),								// kScale
		NiPoint2(0.0f,0.0f),								// kCenter
		NiTextureTransform ::TransformMethod::MAX_TRANSFORM	// TransformMethod
		); 

	m_pktTexTransForm[ HORIZON ] = NiNew NiTextureTransform( 
		NiPoint2(0.0f,0.0f),								// kTranslate
		0.0f,												// fRotate
		NiPoint2(1.0f,1.0f),								// kScale
		NiPoint2(0.0f,0.0f),								// kCenter
		NiTextureTransform ::TransformMethod::MAX_TRANSFORM	// TransformMethod
		); 
	//---------------------------------------------------------


	//---------------------------------------------------------
	//			Texturing Set
	//---------------------------------------------------------
	CGameApp::SetMediaPath("Data/Texture/");
	//m_spTexture = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("MuzelSplash.tga"));
	NiTexturingProperty *pFront	= NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(pTex0));
	NiTexturingProperty *pHV	= NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(pTex1));
	
	// 텍스쳐의 어드레스 모드
	pFront->SetBaseTextureTransform( m_pktTexTransForm[ FRONT ] );
	pFront->SetBaseClampMode(NiTexturingProperty::WRAP_S_WRAP_T);
	pFront->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pFront->Update(0.0f);
	
	m_spSplash[ FRONT ]->AttachProperty(pFront);

	pHV->SetBaseTextureTransform( m_pktTexTransForm[ HORIZON ] );
	pHV->SetBaseClampMode(NiTexturingProperty::WRAP_S_WRAP_T);
	pHV->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	pHV->Update(0.0f);
	
	m_spSplash[ HORIZON ]->AttachProperty(pHV);
	m_spSplash[ VERTICAL ]->AttachProperty(pHV);
	//---------------------------------------------------------


	//---------------------------------------------------------
	//			Alpha, Stencil Property Create & Set
	//---------------------------------------------------------
	NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty;
	pkAlpha->SetAlphaBlending(true);
	pkAlpha->SetAlphaTesting(true);
	pkAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pkAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pkAlpha->SetTestRef(0x00);
	pkAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);

	NiStencilProperty* pStencil = NiNew NiStencilProperty;
	pStencil->SetDrawMode( NiStencilProperty::DrawMode::DRAW_BOTH );

	for( unsigned int i = FRONT; i <= VERTICAL; i++ )
	{
		m_spSplash[i]->AttachProperty(pkAlpha);
		m_spSplash[i]->AttachProperty(pStencil);

		m_spSplash[i]->UpdateProperties();
		m_spSplash[i]->UpdateNodeBound();   // Bound설정
		m_spSplash[i]->Update(0.0f);

		m_spSplash[i]->SetAppCulled( true );
	}

	return true;
}

void CMuzelSplash::Update( float fAccumTime, bool bUpdate )
{
	/*float fDeltaTime	= fAccumTime - m_fLastTime;

	if( fDeltaTime > m_fDelayTime )
	{
		m_fLastTime	= fAccumTime;

		for( int i = FRONT; i <= HORIZON; i++ )
		{
			if( m_nXCurOffset[ i ] >= m_nXOffset[ i ] )
			{
				m_nXCurOffset[ i ] = 0;
				m_nYCurOffset[ i ]++;
				
				if( m_nYCurOffset[ i ] >= m_nYOffset[ i ] )
				{
					m_nYCurOffset[ i ] = 0;
				}
			}

			m_pktTexTransForm[ i ]->SetTranslate( 
				NiPoint2(	m_nXCurOffset[ i ] * -m_fOffsetSpacing[ i ], 
							m_nYCurOffset[ i ] * m_fOffsetSpacing[ i ]	) );	

			m_nXCurOffset[ i ]++;
		}		
	}

	if( !m_bActive )
	{
		for( int  i = FRONT; i<= VERTICAL; i++ )
			m_spSplash[i]->SetAppCulled( bUpdate );

		m_bActive = true;
	}
	*/
	float m_fCurrenTime = NiGetCurrentTimeInSec();
	
	float fDeltaTime	= m_fCurrenTime - m_fLastTime;

	if( fDeltaTime > m_fDelayTime )
	{
		m_fLastTime	= m_fCurrenTime;

		for( int i = FRONT; i <= HORIZON; i++ )
		{
			if( m_nXCurOffset[ i ] >= m_nXOffset[ i ] )
			{
				m_nXCurOffset[ i ] = 0;
				m_nYCurOffset[ i ]++;
				
				if( m_nYCurOffset[ i ] >= m_nYOffset[ i ] )
				{
					m_nYCurOffset[ i ] = 0;
				}
			}

			m_pktTexTransForm[ i ]->SetTranslate( 
				NiPoint2(	m_nXCurOffset[ i ] * -m_fOffsetSpacing[ i ], 
							m_nYCurOffset[ i ] * m_fOffsetSpacing[ i ]	) );	

			m_nXCurOffset[ i ]++;
		}		
	}

	if( !m_bActive )
	{
		for( int  i = FRONT; i<= VERTICAL; i++ )
			m_spSplash[i]->SetAppCulled( bUpdate );

		m_bActive = true;
	}
}

void	CMuzelSplash::RenderClick()
{
	NiRenderer *pRenderer = NiRenderer::GetRenderer();

	if( NULL == pRenderer || !m_bActive )
		return;
	
	for( int i = FRONT; i <= VERTICAL; i++ )
	{
		m_spSplash[i]->RenderImmediate( pRenderer );
		m_spSplash[i]->SetAppCulled( true );
	}

	m_bActive = false;
}


