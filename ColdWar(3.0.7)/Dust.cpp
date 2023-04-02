#include "Dust.h"
#include "WorldManager.h"
#include "GameApp.h"


CDust::CDust()
{
	m_spDustTex			= 0;
	m_spDustRoot		= 0;
	m_pktTexTransForm	= 0;	
	m_fDelayTime	= 0.0f;
	m_fLastTime		= 0.0f;
	m_fLifeTime		= 0.0f;
	m_nXCurOffset	= 0;
	m_nYCurOffset	= 0;
	m_nXOffset		= 0;
	m_nYOffset		= 0;	

	m_fLastTime		= CGameApp::mp_Appication->GetAccumTime();
	m_fOffsetSpacing= 1.0f / static_cast<float>( m_nXOffset );

}

CDust::CDust( float fDelay, int xOffset, int yOffset)
{

	m_spDustTex			= 0;
	m_spDustRoot		= 0;
	m_pktTexTransForm	= 0;
	
	m_fDelayTime	= fDelay;
	m_fLastTime		= 0.0f;
	m_fLifeTime		= 0.0f;

	m_nXCurOffset	= 0;
	m_nYCurOffset	= 0;

	m_nXOffset		= xOffset;
	m_nYOffset		= yOffset;
	m_fLastTime		= CGameApp::mp_Appication->GetAccumTime();

	m_fOffsetSpacing= 1.0f / static_cast<float>( xOffset );		
}

CDust::~CDust()
{
	m_spDustRoot->DetachAllProperties();
	m_spDustRoot	= 0;
	m_spDustTex		= 0;
	
	m_pktTexTransForm = NULL;
}

void CDust::SetPosAndDir( const NiPoint3& vPos, const NiPoint3& vDir )
{

}

void CDust::SetTransform( const NiTransform& vTrnaform )
{

}

bool CDust::Initialize( char *pNifName, char *pTex )
{
	CGameApp::SetMediaPath("./Data/Object/");
	
	NiStream	kStream;

    // Load in the scenegraph for our world...
	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename( pNifName ) );

	unsigned int uiCount = kStream.GetObjectCount();
    assert(uiCount > 0 );  

	if( !bSuccess ) return false;
		
	m_spDustRoot = (NiNode*)kStream.GetObjectAt(0);

	if( !m_spDustRoot ) return false;

	NiTriShape *pShapeDust[3];
	::ZeroMemory( pShapeDust, sizeof(pShapeDust) );

	NiBillboardNode *pBill[3];
	::ZeroMemory( pBill, sizeof(pBill) );

	pBill[0] = (NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel01");
	pBill[1] = (NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel02");
	pBill[2] = (NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel03");

	pBill[0]->SetMode( NiBillboardNode::ALWAYS_FACE_CAMERA );
	pBill[1]->SetMode( NiBillboardNode::ALWAYS_FACE_CAMERA );
	pBill[2]->SetMode( NiBillboardNode::ALWAYS_FACE_CAMERA );

	pShapeDust[SMALL]		= (NiTriShape*)((NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel01"))->GetAt(0);
	pShapeDust[MEDIUM]		= (NiTriShape*)((NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel02"))->GetAt(0);
	pShapeDust[LARGE]		= (NiTriShape*)((NiBillboardNode*)m_spDustRoot->GetObjectByName("smokeLevel03"))->GetAt(0);

	// 0----------2
	// |	      |
	// |          |
	// |          |
	// |          |
	// 1----------3
	//---------------------------------------------------------
	//			Front Splash UV Set
	//---------------------------------------------------------	
	for( unsigned int ui = SMALL; ui <= LARGE; ui++ )
	{

		NiGeometryData *pModelData = pShapeDust[ ui ]->GetModelData();
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
		pUVData[ 1 ].y = m_fOffsetSpacing;

		pUVData[ 2 ].x = m_fOffsetSpacing;
		pUVData[ 2 ].y = 0.0f;

		pUVData[ 3 ].x = m_fOffsetSpacing;
		pUVData[ 3 ].y = m_fOffsetSpacing;

		pModelData->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
	}
	//---------------------------------------------------------


	//---------------------------------------------------------
	//			TextureTransform Create
	//---------------------------------------------------------

	m_pktTexTransForm = NiNew NiTextureTransform( 
		NiPoint2(0.0f,0.0f),								// kTranslate
		0.0f,												// fRotate
		NiPoint2(1.0f,1.0f),								// kScale
		NiPoint2(0.0f,0.0f),								// kCenter
		NiTextureTransform ::TransformMethod::MAX_TRANSFORM	// TransformMethod
		); 

	//---------------------------------------------------------
	//			Texturing Set
	//---------------------------------------------------------
	CGameApp::SetMediaPath("Data/Texture/");
	m_spDustTex	= NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(pTex));
	
	// 텍스쳐의 어드레스 모드
	m_spDustTex->SetBaseTextureTransform( m_pktTexTransForm );
	m_spDustTex->SetBaseClampMode(NiTexturingProperty::WRAP_S_WRAP_T);
	m_spDustTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	m_spDustTex->Update(0.0f);

	for( unsigned int ui = SMALL; ui <= LARGE; ui++ )
		pShapeDust[ ui ]->AttachProperty( m_spDustTex );

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

	m_spDustRoot->AttachProperty( pkAlpha );

	// 알파 조절
	NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
	pkMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	pkMat->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
	pkMat->SetShineness(10.0f);

	m_spDustRoot->AttachProperty( pkMat );

	m_spDustRoot->SetScale( DUST_DEFAULT_SCALE );
	m_spDustRoot->Update(0.0f);
	m_spDustRoot->UpdateProperties();

	return true;
}

void CDust::SetDefaultAttribute( float fDelay, float fSpacing, int xOffset, int yOffset )
{
	m_fDelayTime	= fDelay;
	m_nXOffset		= xOffset;
	m_nYOffset		= yOffset;
	m_fOffsetSpacing= fSpacing;

	m_pktTexTransForm = NiNew NiTextureTransform( 
		NiPoint2(0.0f,0.0f),								// kTranslate
		0.0f,												// fRotate
		NiPoint2(1.0f,1.0f),								// kScale
		NiPoint2(0.0f,0.0f),								// kCenter
		NiTextureTransform ::TransformMethod::MAX_TRANSFORM	// TransformMethod
	); 

	for( int i = SMALL; i <= LARGE; i++ )
	{
		NiBillboardNode *pBIll = (NiBillboardNode*)m_spDustRoot->GetAt(i);
		NiTexturingProperty *pTex = (NiTexturingProperty*)((NiTriShape*)pBIll->GetAt(0))->GetProperty( NiProperty::TEXTURING );
		
		if( pTex ) pTex->SetBaseTextureTransform( m_pktTexTransForm );

	}

	m_fLastTime		= CGameApp::mp_Appication->GetAccumTime();
}

void CDust::ClonDust( CDust *pClonDust )
{
	if( m_spDustRoot )
	{
		NiNode *pClonNode = pClonDust->GetObject();

		if( NULL == pClonNode )
		{
			NiCloningProcess kProcess;
			kProcess.m_eCopyType = NiObjectNET::COPY_EXACT;

			pClonNode = (NiNode*)m_spDustRoot->Clone(kProcess);

			pClonDust->SetObject( pClonNode );
			pClonDust->SetDefaultAttribute( m_fDelayTime, m_fOffsetSpacing, m_nXOffset, m_nYOffset );
		}
	}
}

void CDust::SetObject( NiNode *pObject )
{
	m_spDustRoot = pObject;

	m_spDustRoot->Update(0.0f);
	m_spDustRoot->UpdateProperties();
}

void CDust::SetDustTransform( const NiTransform &vTransform )
{
	m_spDustRoot->SetTranslate( vTransform.m_Translate );
	m_spDustRoot->SetRotate( vTransform.m_Rotate );
	m_spDustRoot->SetScale( vTransform.m_fScale );
	m_spDustRoot->Update(0.0f);

	m_fLifeTime		= 0;
	m_nXCurOffset	= 0;
	m_nYCurOffset	= 0;
	m_pktTexTransForm->SetTranslate( NiPoint2( 0.0f, 0.0f ) );

	m_fLastTime		= CGameApp::mp_Appication->GetAccumTime();
	

}

bool CDust::Update( float fAccumTime )
{
	float fDeltaTime	= fAccumTime - m_fLastTime;

	
	if( fDeltaTime > m_fDelayTime )
	{		
		m_fLifeTime += fDeltaTime;

		if( m_fLifeTime >= DUST_LIFE_TIME )
		{
			return true;
		}	

		if( m_nYCurOffset < m_nYOffset )
		{
			m_pktTexTransForm->SetTranslate( NiPoint2(	m_nXCurOffset * -m_fOffsetSpacing, m_nYCurOffset * m_fOffsetSpacing	) );								

			m_nXCurOffset++;

			if( m_nXCurOffset >= m_nXOffset )
			{
				m_nXCurOffset = 0;
				m_nYCurOffset++;				
			}		
		}		
		else
		{
		
			float fAlpha = 0.1f * m_fLifeTime;
//			ZFLog::g_sLog->Log("m_fLifeTime = %f, fAlpha = %f",m_fLifeTime, fAlpha );
			CGameApp::ChangeNodeAlpha( m_spDustRoot, 0.1f );
		}
		m_fLastTime	= fAccumTime;
	}
	
	return false;
}

void CDust::RenderClick(NiCamera* pkCamera)
{
	WorldManager::GetWorldManager()->PartialRender( pkCamera, m_spDustRoot );
}

