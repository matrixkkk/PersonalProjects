#include "main.h"
#include "DecalManager.h"
#include "GameApp.h"


DecalManager *DecalManager::m_pDecaldManager	= NULL;


DecalManager::DecalManager()
{

	m_pDecaldManager = this;
	m_fLastUpdateTime	= 0.0f;

	m_spDecalRoot = NiTCreate<NiNode>();
	m_spDecalRoot->SetName("DecalManager");

	NiWireframeProperty *pWire = NiNew NiWireframeProperty();
	pWire->SetWireframe( true );
	m_spDecalRoot->AttachProperty( pWire );

	m_spDecalRoot->Update(0.0f);
	m_spDecalRoot->UpdateEffects();
	m_spDecalRoot->UpdateProperties();

	m_spDecalRoot->SetAppCulled( false );

	// ���� ź�� ���� ������ Factory ����
	m_pBulletDecalFactory = NiNew BulletDecalFactory();

	// ���� ���� ���� ������ Factory ����
	m_pExplosionDecalFactory = NiNew ExplosionDecalFactory( 3 );

	// ����ƽ ������Ʈ ���� ��Į ������ Factory ����
	m_pStaticObjectDecalFactory = NiNew StaticObjectDecalFactory();

	// ���� Decal ������ Shape ����
	m_pStaticObjectDecalFactory->Init();


	m_nCntExplosionLimitRect = 1;
}

DecalManager::~DecalManager()
{
	ReleaseObject();
	
}

void	DecalManager::SetWireFrame( bool bWire )
{
	NiWireframeProperty *pWire = (NiWireframeProperty*)m_spDecalRoot->GetProperty( NiProperty::WIREFRAME );

	if( pWire )
		pWire->SetWireframe( bWire );
}


NiNode* DecalManager::GetObject()
{
	return m_spDecalRoot;
}

BOOL	DecalManager::UpdateObject( float fElapsedTime  )
{
	if( m_listDecalChunks.empty() )
		return FALSE;

	float fDelTime = fElapsedTime - m_fLastUpdateTime;

	std::for_each( m_listDecalChunks.begin(), m_listDecalChunks.end(), 
		std::bind2nd( std::mem_fun( &DecalAtChunk::UpdateObject ), fDelTime ) );

	m_fLastUpdateTime = fElapsedTime;
	return TRUE;
}

BOOL	DecalManager::ReleaseObject()
{
	if( !m_listDecalObjects.empty() )
	{
		std::vector< DecalObject >::iterator iterBill;

		for( iterBill = m_listDecalObjects.begin(); iterBill != m_listDecalObjects.end(); ++iterBill )
		{
			iterBill->ReleaseObject();
		}

		m_listDecalObjects.clear();

	}
	if( !m_listDecalChunks.empty() )
	{
		std::list< DecalAtChunk* >::iterator iterBill;

		for( iterBill = m_listDecalChunks.begin(); iterBill != m_listDecalChunks.end(); ++iterBill )
		{
			if( (*iterBill)->ReleaseObject() )
				NiDelete (*iterBill);
		}

		m_listDecalChunks.clear();
	}

	if( m_spDecalRoot ) m_spDecalRoot = 0;

	m_fLastUpdateTime = 0.0f;

	if( m_pBulletDecalFactory )
	{
		NiDelete m_pBulletDecalFactory;
		m_pBulletDecalFactory = NULL;
	}

	if( m_pExplosionDecalFactory )
	{
		NiDelete m_pExplosionDecalFactory;
		m_pExplosionDecalFactory = NULL;
	}

	if( m_pStaticObjectDecalFactory )
	{
		NiDelete m_pStaticObjectDecalFactory;
		m_pStaticObjectDecalFactory = NULL;
	}

//	if( m_spBulletTexture )
//		m_spBulletTexture = 0;



	return TRUE;
}

DecalManager* DecalManager::CreateManager()
{

	if( !m_pDecaldManager )
	{
		m_pDecaldManager = NiNew DecalManager();		
	}

	return m_pDecaldManager;

}

//		��Į ������ ���� ������ ����
void	DecalManager::SetTerrain( Terrain *pTerrain )
{
	m_pTerrain = pTerrain;
}


DecalManager* DecalManager::GetManager()
{
	return m_pDecaldManager;
}

BOOL	DecalManager::IsManager()
{
	return m_pDecaldManager != NULL;
}

void DecalManager::ReleaseManager()
{
	
	if( m_pDecaldManager )
	{
		NiDelete m_pDecaldManager;
		m_pDecaldManager = NULL;		
	}

}

// ���� ��Į�� ���̴� �ؽ��� ���� ����
BOOL	DecalManager::Intialize()
{	

	/*
	CGameApp::SetMediaPath("Data/Object/Decal");

	NiSourceTexturePtr spDecalSourceTex = NiSourceTexture::Create(  CGameApp::ConvertMediaFilename( "Decal.tga" ) );

	if( !spDecalSourceTex )
	{
		NiMessageBox(" ź��� ��Į �ؽ��� ���� ���� DecalManager::Intialize() -  NiSourceTexture::Create() ", "Faield" );
		return FALSE;
	}

	m_spBulletTexture = NiNew NiTexturingProperty();
	m_spBulletTexture->SetBaseTexture( spDecalSourceTex );
	
*/
	return FALSE;
}

BOOL DecalManager::AddObject( const NiFixedString& fileName, int nType )
{
	DecalObject stDecalObject;

	if( stDecalObject.GenerateDecal( fileName, nType ) )
	{
		m_listDecalObjects.push_back( stDecalObject );

		return TRUE;
	}

	return TRUE;
}

//		��Į �����Ʈ ����
BOOL DecalManager::DeleteObject( const NiFixedString& fileName )
{
	if( m_listDecalObjects.empty() )
		return FALSE;

	std::vector< DecalObject >::iterator iterObject;

	for( iterObject = m_listDecalObjects.begin(); iterObject != m_listDecalObjects.end(); ++iterObject )
	{
		if( iterObject->GetFileName()  == fileName )
		{
			iterObject->ReleaseObject();
			m_listDecalObjects.erase( iterObject );

			return TRUE;
		}
	}

	return FALSE;
}

//		ûũ�� ��Į�� ����ġ
/*
BOOL	DecalManager::AddDecalAtChunk(  DecalAtChunk* pDecal, int nType )
{
	NiTriShapePtr spCloneDecal = m_listDecalObjects[ nType ].Clone();

	if( spCloneDecal )
	{
		pDecal->AddDecal( spCloneDecal );
		return TRUE;
	}

	return FALSE;
}
*/
BOOL	DecalManager::RenderObject( NiRenderer *pkRenderer )
{


	return TRUE;
}

//		��Į�� Scene�� �����Ѵ�.
BOOL	DecalManager::GenerateDecal( const NiPoint3* vCenter, const NiPoint3* vNormal, const  NiPoint3* vTangent, 
									
									const int *nHeightMapIndex, int nDecalType, int nDecalPlace, DecalAtChunk *pDecalChunk )	
{
	NiTriShape *pDecal = NULL;

	switch( nDecalType )
	{
	case BULLET_MARK:
		// ��

		switch( nDecalPlace )
		{

		case OUTDOOR:
			// Terrain

			if( GenerateBooldAtTerrain( vCenter, vNormal, vTangent, nHeightMapIndex, pDecalChunk ) )
				return TRUE;
			
//			if( NULL != pDecal )
//				return pDecal;

			break;

		case STATIC_OBJECT:

			if( GenerateDecalForStaticobject( vCenter, vNormal, pDecalChunk, nDecalType ) )
				return TRUE;

			break;
		}
		
		break;

	case BOMB_MARK:
		
		switch( nDecalPlace )
		{

		case OUTDOOR:

			if( GenerateExplosionAtTerrain( nHeightMapIndex, pDecalChunk ) )
				return TRUE;
		
			break;
		}

		break;


	}

	return FALSE;
}

BOOL DecalManager::GenerateBooldAtTerrain( const NiPoint3* vCenter, const NiPoint3* vNormal, const  NiPoint3* vTangent, const int *nHeightMapIndex, DecalAtChunk *pDecalChunk )
{
	float fWidth = g_fBulletMarkSize;
	float fHeight = g_fBulletMarkSize;

	NiPoint3 *pVtx = NULL, *pNor = NULL;

	m_vecDecalTriIndex.clear();

	if( m_pTerrain )
	{
		m_pTerrain->GetDecalTriIndex( nHeightMapIndex, &fWidth, &fHeight, m_vecDecalTriIndex );

		pVtx = m_pTerrain->GetTerrainVertices();
		pNor = m_pTerrain->GetTerrainNormals();
	}
		
	if( !m_vecDecalTriIndex.empty() )
	{
		if( m_pBulletDecalFactory )
		{
			m_pBulletDecalFactory->Init( vCenter, vNormal, vTangent, fWidth, fHeight, 0.1f );
			m_pBulletDecalFactory->ClipMesh( m_vecDecalTriIndex, pVtx, pNor );
			
			if( m_pBulletDecalFactory->GenerateDecal( pDecalChunk, *vNormal ) )
				return TRUE;
		}
	}

	return FALSE;

}

BOOL DecalManager::GenerateExplosionAtTerrain( const int *nHeightMapIndex, DecalAtChunk *pDecalChunk )
{
	float fWidth = 0.5f;
	float fHeight = 0.5f;
	NiPoint3 *pVtx = NULL, *pNor = NULL;

	m_vecVtxIndex.clear();

	if( m_pTerrain )
	{
		m_pTerrain->GetDecalTriIndex( nHeightMapIndex, m_nCntExplosionLimitRect, m_nCntExplosionLimitRect, m_vecVtxIndex );

		pVtx = m_pTerrain->GetTerrainVertices();
		pNor = m_pTerrain->GetTerrainNormals();
	}
		
	if( !m_vecVtxIndex.empty() )
	{
		if( m_pExplosionDecalFactory )
		{
			if( m_pExplosionDecalFactory->GenerateDecal( pVtx, pNor, m_vecVtxIndex, pDecalChunk ) )
				return TRUE;
		}
	}

	return FALSE;

}

//		��Į ûũ ����
DecalAtChunk*	DecalManager::GenerateDecalAtChunk()
{
	DecalAtChunk *pDecalChunk = NiNew DecalAtChunk();

	m_spDecalRoot->AttachChild( pDecalChunk->GetRootObject() );	
	
	m_spDecalRoot->Update(0.0f);
	m_spDecalRoot->UpdateProperties();
	m_spDecalRoot->UpdateWorldBound();
	m_spDecalRoot->UpdateEffects();

	m_listDecalChunks.push_back( pDecalChunk );

	return pDecalChunk;

}

BOOL	DecalManager::GenerateDecalForStaticobject( const NiPoint3 *vPos, const NiPoint3 *vNormal, DecalAtChunk *pDecalChunk, int &nDecalType )
{

	if( m_pStaticObjectDecalFactory )
	{
		if( m_pStaticObjectDecalFactory->GenerateDecal( vPos, vNormal, StaticObjectDecalFactory::BRICK_DECAL, pDecalChunk ) )
			return TRUE;
	}
	

	return FALSE;
}