#include "main.h"
#include "StaticObject.h"
#include "GameApp.h"


StaticObject::StaticObject()
{
	m_spObjects			= NULL;
	m_pSelectedObject	= NULL;
	m_pOriginMesh		= NULL;

//	m_strFileName		= _T("");
	m_usObjectCnt		= 0;
	m_nObjectType		= 0;
	
	m_bDrawBB			= FALSE;
	m_bPickDrawMesh		= FALSE;

	m_spObjects = NiTCreate<NiNode>();
	
	m_spObjects->SetAppCulled( false );

	m_spObjects->Update(0.0f);
	m_spObjects->UpdateNodeBound();
	m_spObjects->UpdateProperties();
	m_spObjects->UpdateEffects();
	
}

StaticObject::StaticObject( const NiFixedString& strObjectName )
{
	m_spObjects			= NULL;
	m_pSelectedObject	= NULL;
	m_pOriginMesh		= NULL;	

	m_strFileName		= strObjectName;
	m_usObjectCnt		= 0;
	m_nObjectType		= 0;

	m_bDrawBB			= FALSE;
	m_bPickDrawMesh		= FALSE;

	m_spObjects = NiTCreate<NiNode>();

	m_spObjects->SetAppCulled( false );

	m_spObjects->Update(0.0f);
	m_spObjects->UpdateNodeBound();
	m_spObjects->UpdateProperties();
	m_spObjects->UpdateEffects();
}

StaticObject::~StaticObject()
{
	ReleaseObject();	
}

BOOL StaticObject::ReleaseObject()
{
	if( !m_listNifMeshs.empty() )
	{
		std::list< NifMesh* >::iterator iterMesh;

		for( iterMesh = m_listNifMeshs.begin(); iterMesh != m_listNifMeshs.end(); iterMesh++ )
		{
			if( (*iterMesh)->GetRootObject() )
				m_spObjects->DetachChild( (*iterMesh)->GetRootObject() );

			NiDelete (*iterMesh);
		}

		m_listNifMeshs.clear();
	}

	if( m_spObjects != NULL )
	{
		m_spObjects = 0;
	}

	if( m_pOriginMesh )
	{
		NiDelete m_pOriginMesh;
		m_pOriginMesh = NULL;
	}
	
	m_pSelectedObject = NULL;

	return TRUE;
}

void	StaticObject::SetDrawObjectBB( bool bDraw )
{
	m_bDrawBB = bDraw;
	std::for_each( m_listNifMeshs.begin(), m_listNifMeshs.end(), std::bind2nd( std::mem_fun( &NifMesh::DrawBB ), bDraw ) );
}

NiNode*	StaticObject::GetRootObject()
{
	return m_spObjects;
}	

//		컬링
void	StaticObject::SetAppCulled( bool bCull )
{
	if( m_spObjects )
		m_spObjects->SetAppCulled( bCull );
}

//		원본 메쉬 로드
BOOL	StaticObject::LoadMesh()
{
	if( !m_pOriginMesh )
	{
		m_pOriginMesh = NiNew NifMesh( m_strFileName );		
		m_pOriginMesh->GenerateMesh();
		m_pOriginMesh->GetMinMax( &m_vMin, &m_vMax );	

		NiNode		*pObjectGroup = NULL;
		NiNode		*pRenderingGroup = NULL;
		NiNode		*pCollisionGroup = NULL;
		
		// 오브젝트 타일 결정
		NiNode *pObject = m_pOriginMesh->GetRootObject();
		pObjectGroup = (NiNode*)pObject->GetObjectByName( "ObjectGroup" );
		pRenderingGroup = (NiNode*)pObject->GetObjectByName( "RenderingGroup" );
		pCollisionGroup = (NiNode*)pObject->GetObjectByName( "Collision" );

		if( pCollisionGroup && pObjectGroup )
		{
			m_nObjectType = MAP;
		}
		else if( pCollisionGroup && pRenderingGroup )
		{
			m_nObjectType = STATIC_WITH_COLLISION;
		}
		else
		{
			m_nObjectType = STATIC_ONLY_RENDERING;
		}

		return TRUE;
	}
	
	return FALSE;
}

//		바운딩 박스와 충돌 체크 가장 가까운 MeshObject 선별한다.
BOOL	StaticObject::CheckCollisionBBWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *fDis )
{

	return FALSE;
}

//		메쉬 충돌 체크
BOOL	StaticObject::CheckCollisionWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, NiPoint3* vPickPos, NiPoint3 *vNormal, float *fDis )
{
	if( !m_spObjects || m_listNifMeshs.empty() )
		return FALSE;

	BOOL bCollision = FALSE;
	float fMinDis	= FLT_MAX;
	NiPoint3 vTempPickPos, vTempPickNormal;
	std::list< NifMesh* >::iterator iterMesh;

	pPick->SetPickType( NiPick::FIND_FIRST );
	//pPick->SetIntersectType( NiPick::BOUND_INTERSECT);

	for( iterMesh = m_listNifMeshs.begin(); iterMesh != m_listNifMeshs.end(); ++iterMesh )
	{
		
		pPick->SetIntersectType( NiPick::BOUND_INTERSECT );
		if( (*iterMesh)->RayCollision( pos, dir, vPickPos, vNormal, pPick, fDis ) )
		{
			*fDis = FLT_MAX;

			pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );			
			if( (*iterMesh)->RayCollision( pos, dir, vPickPos, vNormal, pPick, fDis ) )
			{
				if( fMinDis > *fDis )
				{
					fMinDis			= *fDis;					
					vTempPickPos	= *vPickPos;
					vTempPickNormal = *vNormal;
					m_pSelectedObject = (*iterMesh);
					bCollision		= TRUE;
				}
			}
		}
	}

	if( bCollision )
	{
		*fDis		= fMinDis;
		*vPickPos	= vTempPickPos;
		*vNormal	= vTempPickNormal;
	}
	else
	{
		m_pSelectedObject = NULL;
	}

		
	return bCollision;
}

//		메쉬 Object 월드에 추가
void	StaticObject::AddMeshObjectAtWorld( const NiPoint3& pos )
{

	if( !m_usObjectCnt && !m_pOriginMesh )
	{
		m_pOriginMesh = NiNew NifMesh( m_strFileName );
		m_pOriginMesh->GenerateMesh();
		m_pOriginMesh->GetMinMax( &m_vMin, &m_vMax );
		m_pOriginMesh->SetPosition( pos );		

		m_listNifMeshs.push_back( m_pOriginMesh );

		m_spObjects->AttachChild( m_pOriginMesh->GetRootObject() );
		m_spObjects->Update( 0.0f );
		m_spObjects->UpdateNodeBound();
		m_spObjects->UpdateProperties();
		m_spObjects->UpdateEffects();
		
	}
	else
	{
		NifMesh *pNewMesh = NiNew NifMesh();

		pNewMesh->ClonObject( m_pOriginMesh->GetRootObject() );
		pNewMesh->SetName( m_strFileName );
		pNewMesh->GenerateAABB( m_vMin, m_vMax );
		pNewMesh->SetPosition( pos );
		
		NiBound kBound = pNewMesh->GetRootObject()->GetWorldBound();
		
		m_listNifMeshs.push_back( pNewMesh );

		m_spObjects->AttachChild( pNewMesh->GetRootObject() );
		m_spObjects->Update( 0.0f );
		m_spObjects->UpdateNodeBound();
		m_spObjects->UpdateProperties();
		m_spObjects->UpdateEffects();
	}

	++m_usObjectCnt;
}


//		선택되어진 MeshObject의 Matrix 하나 제거
BOOL	StaticObject::DeleteSelectedObject()
{
	if( !m_pSelectedObject || m_listNifMeshs.empty() )
		return FALSE;

	std::list< NifMesh* >::iterator iterMesh;
	for( iterMesh = m_listNifMeshs.begin(); iterMesh != m_listNifMeshs.end(); ++iterMesh )
	{
		if( m_pSelectedObject == (*iterMesh) )
		{
			m_listNifMeshs.erase( iterMesh );
			break;
		}
	}	

	NiAVObject *pObject = m_pSelectedObject->GetRootObject();	
	m_spObjects->DetachChild( pObject );

	m_pSelectedObject->ReleaseObject();
	NiDelete m_pSelectedObject;
	m_pSelectedObject = NULL;

	--m_usObjectCnt;

	return FALSE;
}

//		World에 선택되어진 MeshObject을 NULL로 셋팅
void	StaticObject::SetSelectedObjIsNULL()
{
	if( NULL != m_pSelectedObject )
	{
		m_pSelectedObject->SetWireFrame( false );
		m_pSelectedObject = NULL;
	}
}

NifMesh* StaticObject::GetNifMesh()
{ 
	return m_pSelectedObject; 
}

//		Object Dlg에서 입력된 World 좌표로 셋팅
void	StaticObject::SetSelectedObjPos( const NiPoint3& vPos )
{

	if( m_pSelectedObject )
		m_pSelectedObject->SetPosition( vPos );
}

//	
void	StaticObject::Translation( float x, float y, float z )
{
	if( m_pSelectedObject )
		m_pSelectedObject->Translation( x, y, z );

}

void	StaticObject::Scaling( float fDel )
{
	if( m_pSelectedObject )
		m_pSelectedObject->Scaling( fDel );
}

void	StaticObject::RotationX( float fAngle )
{
	if( m_pSelectedObject )
		m_pSelectedObject->RotationX( fAngle );
}

void	StaticObject::RotationY( float fAngle )
{
	if( m_pSelectedObject )
		m_pSelectedObject->RotationY( fAngle );
}

void	StaticObject::RotationZ( float fAngle )
{
	if( m_pSelectedObject )
		m_pSelectedObject->RotationZ( fAngle );
}

void	StaticObject::SetDefault()
{
	if( m_pSelectedObject )
		m_pSelectedObject->SetDefault();
}

int		StaticObject::GetObjectCount()
{
	if( m_listNifMeshs.empty() )
		return 0;

	return static_cast<int>( m_listNifMeshs.size() );
}

/*
void	StaticObject::SaveData( CArchive& ar )
{
	std::list< NifMesh* >::iterator iterMesh;
	
	for( iterMesh = m_listNifMeshs.begin(); iterMesh != m_listNifMeshs.end(); ++iterMesh )
	{
		(*iterMesh)->SaveData( ar );
	}
}
*/


void	StaticObject::SpaceDivisionForRendering( Terrain *pTerrain )
{
	std::list< NifMesh* >::iterator iterObject;

	for( iterObject = m_listNifMeshs.begin(); iterObject != m_listNifMeshs.end(); ++iterObject )
	{

		switch( m_nObjectType )
		{
		case MAP:

			(*iterObject)->SpaceDivisionForRenderingFromMapData( pTerrain );

			break;

		case STATIC_WITH_COLLISION:

			(*iterObject)->SpaceDivisionForRendering( pTerrain );

			break;  

		case STATIC_ONLY_RENDERING:

			(*iterObject)->SpaceDivisionForRendering( pTerrain );

			break;

		}
		/*
		if( (*iterObject)->IsMapData( ) )
		{
			(*iterObject)->SpaceDivisionForRenderingFromMapData( pTerrain );
		}
		else
		{
			(*iterObject)->SpaceDivisionForRendering( pTerrain );
		}
		*/
	}

}



//		컬리전 용 그룹으로 공간 분할
void	StaticObject::SpaceDivisionForCollision( Terrain *pTerrain )
{
	std::list< NifMesh* >::iterator iterObject;

	for( iterObject = m_listNifMeshs.begin(); iterObject != m_listNifMeshs.end(); ++iterObject )
	{

		switch( m_nObjectType )
		{
		case MAP:

			(*iterObject)->SpaceDivisionForCollisionFromMapData( pTerrain );

			break;

		case STATIC_WITH_COLLISION:

			(*iterObject)->SpaceDivisionForCollision( pTerrain );


			break;  

		case STATIC_ONLY_RENDERING:

	//		(*iterObject)->SpaceDivisionForRendering( pTerrain );

			break;

		}

		/*
		if( (*iterObject)->IsMapData( ) )
		{
			(*iterObject)->SpaceDivisionForCollisionFromMapData( pTerrain );
		}
		else
		{
			(*iterObject)->SpaceDivisionForCollision( pTerrain );
		}
		*/
	}

}

//			Nif 파일에 있는 "start" Node로 부터 케릭터 시작위치를 얻음
NiNode*		StaticObject::GetCharacterStartPosition()
{
	std::list< NifMesh* >::iterator iterObject;

	for( iterObject = m_listNifMeshs.begin(); iterObject != m_listNifMeshs.end(); ++iterObject )
	{
		if( (*iterObject)->IsMapData( ) )
		{
			NiNode *pObject = NULL;
			pObject = (*iterObject)->GetCharacterStartPosition();

			return pObject;
		}
	}

	return NULL;
}

//		
void	StaticObject::SetCollisionTransform( Terrain *pTerran )
{
	std::list< NifMesh* >::iterator iterObject;

	for( iterObject = m_listNifMeshs.begin(); iterObject != m_listNifMeshs.end(); ++iterObject )
	{
		if( (*iterObject)->IsMapData( ) )
		{
			(*iterObject)->SetCollisionTransform( pTerran );
		}
	}
}

void	StaticObject::SetRenderingTransform( Terrain *pTerrain )
{
	std::list< NifMesh* >::iterator iterObject;

	for( iterObject = m_listNifMeshs.begin(); iterObject != m_listNifMeshs.end(); ++iterObject )
	{
		if( (*iterObject)->IsMapData( ) )
		{
			(*iterObject)->SetRenderingTransform( pTerrain );
		}
	}
}


void StaticObject::LoadData( FILE *file, int nCntMeshInfo )
{

	if( !m_pOriginMesh || !m_spObjects )
	{
		NiMessageBox( " StaticObject::LoadData - m_pOriginMesh = NULL", "Failed" );
		return;
	}

	NiPoint3 vMin, vMax;
	NifMesh *pNewMesh = NULL;


	for( int i = 0; i < nCntMeshInfo; i++ )
	{
		pNewMesh = NiNew NifMesh();

		pNewMesh->ClonObject( m_pOriginMesh->GetRootObject() );
		pNewMesh->SetName( m_strFileName );

		pNewMesh->LoadData( file );
		
		m_spObjects->AttachChild( pNewMesh->GetRootObject() );
		m_listNifMeshs.push_back( pNewMesh );	

	}	

	m_spObjects->Update( 0.0f );
	m_spObjects->UpdateNodeBound();
	m_spObjects->UpdateProperties();
	m_spObjects->UpdateEffects();
}