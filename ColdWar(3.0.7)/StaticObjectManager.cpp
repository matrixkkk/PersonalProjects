#include "main.h"
#include "GameApp.h"
#include "StaticObjectManager.h"
#include "DecalManager.h"


StaticObjectManager	*StaticObjectManager::m_pStaticObjectManager	= NULL;


StaticObjectManager::StaticObjectManager()
{
	m_listMeshObject.clear();

	m_nObjectCount			= 0;
	m_pCurrentObject		= NULL;
	m_pPickedObject			= NULL;
	m_pTerrain				= NULL;

	m_pStaticObjectManager = this;

	m_spObjectRoot = NiTCreate<NiNode>();	

	//메인 씬에 어태치
	//와이어프레임 속성 추가
	
	m_spObjectRoot->SetAppCulled( false );
//	m_spObjectRoot->SetSelectiveUpdateFlags(bUpdate, true, bRigid);
	m_spObjectRoot->Update(0.0f);
	m_spObjectRoot->UpdateNodeBound();
	m_spObjectRoot->UpdateProperties();
	m_spObjectRoot->UpdateEffects();

	CreatePick();
}

StaticObjectManager::~StaticObjectManager()
{
	ReleaseObject();
}


BOOL StaticObjectManager::UpdateObject( float fElapsedTime  ) 
{
	return D3D_OK;
}



BOOL StaticObjectManager::ReleaseObject()
{

	if( !m_listMeshObject.empty() )
	{
		std::list< StaticObject* >::iterator iterObject;

		for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
		{
			if( (*iterObject)->GetRootObject() )
			{
				m_spObjectRoot->AttachChild( (*iterObject)->GetRootObject() );

				if( (*iterObject)->ReleaseObject() )
					NiDelete (*iterObject);
			}
		}

		m_listMeshObject.clear();
	}

	if( m_pPick != NULL )
	{
		NiDelete m_pPick;
		m_pPick = NULL;
	}

	m_spObjectRoot			= 0;
	m_nObjectCount			= 0;
	m_pCurrentObject		= NULL;
	m_pPickedObject			= NULL;
	m_pTerrain				= NULL;

	return TRUE;
}

StaticObjectManager* StaticObjectManager::GetManager()
{
	return	m_pStaticObjectManager;
}

StaticObjectManager*	StaticObjectManager::CreateManager()
{
	if( !m_pStaticObjectManager )
	{
		m_pStaticObjectManager = NiNew StaticObjectManager();
	}

	return m_pStaticObjectManager;

}

BOOL StaticObjectManager::IsManager()
{
	return m_pStaticObjectManager != NULL;
}

void StaticObjectManager::ReleaseManager()
{
	if( m_pStaticObjectManager )
	{
		NiDelete m_pStaticObjectManager;
		m_pStaticObjectManager = NULL;		
	}

}


void	StaticObjectManager::SetTerrain( Terrain *pTerrain )
{
	m_pTerrain = pTerrain;
}


//		MeshObject 하나 추가한다.
BOOL	StaticObjectManager::AddMeshObject(  const NiFixedString& strFileName )
{
	CGameApp::SetMediaPath( "./Data/Object/" );

	StaticObject *pStaticbject = NiNew StaticObject( strFileName );

	if( pStaticbject )
	{
		if( pStaticbject->LoadMesh() )
		{
//			m_pCurrentObject = pStaticbject;
			
			m_spObjectRoot->AttachChild( pStaticbject->GetRootObject() );

			m_spObjectRoot->Update(0.0f);
			m_spObjectRoot->UpdateNodeBound();
			m_spObjectRoot->UpdateProperties();
			m_spObjectRoot->UpdateEffects();

			m_listMeshObject.push_back( pStaticbject );

			++m_nObjectCount;

			return TRUE;
		}
	}

	return FALSE;
}

//		MeshObject 하나 추가한다.
void	StaticObjectManager::AddMeshObjectAtWorld( const NiPoint3& vPickPos )
{
	if( !m_pCurrentObject )
		return;

//	((GameApp*)CGameApp::GetApp())->m_pZFLog->Log( " 픽위치 x=%f, y=%f, z=%f\n", vPickPos.x, vPickPos.y, vPickPos.z  );

	m_pCurrentObject->AddMeshObjectAtWorld( vPickPos );

}

//		MeshObject 하나 삭제한다.
BOOL	StaticObjectManager::DeleteMeshObject( const NiFixedString& strFileName )
{

	if( m_listMeshObject.empty() )
		return FALSE;

	StaticObject* pStaticObject = GetMeshObject( strFileName );

	if( pStaticObject != NULL )
	{
		std::list< StaticObject* >::iterator iterObject;

		for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
		{
			if( pStaticObject == (*iterObject) )
			{
				m_spObjectRoot->DetachChild( pStaticObject->GetRootObject() );
				m_spObjectRoot->Update(0.0f);
				m_spObjectRoot->UpdateNodeBound();
				
				pStaticObject->ReleaseObject();
				NiDelete pStaticObject;
				pStaticObject = NULL;

				if( m_pCurrentObject == (*iterObject) )
					m_pCurrentObject = NULL;

				if( m_pPickedObject	== (*iterObject) ) 
					m_pPickedObject = NULL;

				m_listMeshObject.erase( iterObject );

				return TRUE;
			}
		}
	}

	return TRUE;
}

//		선택되어진 MeshObject의 Matrix 하나 제거
BOOL	StaticObjectManager::DeleteSelectedObject()
{
	if( m_pPickedObject != NULL )
	{
		if( m_pPickedObject->DeleteSelectedObject() )
		{
			m_pPickedObject = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

//		파일명과 일치되는 MeshObject를 찾는다.
StaticObject* StaticObjectManager::GetMeshObject( const NiFixedString& strFileName )
{
	std::list< StaticObject* >::iterator iterObject;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		NiFixedString &fileNmae = (*iterObject)->GetFileName();

		if( fileNmae.Equals( strFileName ) )
		{
			return (*iterObject);
		}
	}

	return NULL;
}

//		파일명과 일치되는 MeshObject를 찾아서 m_pCurrentObject에 셋팅
BOOL	StaticObjectManager::SetCurrentMeshObject(  const NiFixedString& strFileName )
{
	if( m_listMeshObject.empty() )
		return FALSE;

	m_pCurrentObject = GetMeshObject( strFileName );

	if( m_pCurrentObject != NULL )
		return TRUE;

	return FALSE;	
}

// 픽킹 NiPick 생성
void StaticObjectManager::CreatePick()
{
	m_pPick = NiNew NiPick;
    m_pPick->SetPickType(NiPick::FIND_FIRST);
	m_pPick->SetSortType(NiPick::NO_SORT);
    m_pPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
    m_pPick->SetFrontOnly(true);
    m_pPick->SetReturnTexture(true);
    m_pPick->SetReturnNormal(true);
    m_pPick->SetReturnColor(true);
	m_pPick->SetObserveAppCullFlag( false );
//    m_pPick->SetTarget(m_spScene);

}

//		오브젝트를 픽킹한다.
BOOL	StaticObjectManager::PickObject( const NiPoint3& pos, const NiPoint3& direction )
{
	if( m_listMeshObject.empty() )
		return FALSE;

	std::list< StaticObject* >::iterator iterObject;
	NiPoint3 vPickPos, vPickNormal;
	NiPoint3 vNearPickPos, vNearPickNormal;
	StaticObject* pSelectObject = NULL;
	float fDis = FLT_MAX, fDisMin = FLT_MAX;
	BOOL bCollision = FALSE;

	m_pPick->SetCoordinateType( NiPick::WORLD_COORDINATES );
	m_pPick->SetReturnNormal( true );
	m_pPick->SetObserveAppCullFlag( false );
	m_pPick->SetFrontOnly( true );

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		if( (*iterObject)->CheckCollisionWithRay( pos, direction, m_pPick, &vPickPos, &vPickNormal, &fDis ) )
		{
			if( fDisMin > fDis )
			{
				fDisMin			= fDis;
				bCollision		= TRUE;
				pSelectObject	= *iterObject;
				vNearPickPos	= vPickPos;
				vNearPickNormal = vPickNormal;
			}
		}
	}

	if( bCollision )
	{	
		m_pPickedObject = pSelectObject;

		int xIndex = 0, yIndex = 0;
/*
		if( m_pTerrain ) 
		{
			if( m_pTerrain->GetTileIndex( vNearPickPos, xIndex, yIndex ) )
			{
				DecalManager::GetManager()->GenerateDecal( &vNearPickPos, &vNearPickNormal, NULL, NULL, 
					DecalManager::BULLET_MARK, DecalManager::OUTDOOR, m_pTerrain->GetTile( xIndex, yIndex )->GetDecalAtChunk() );
				
			}
		}
*/
		std::for_each( m_listMeshObject.begin(),m_listMeshObject.end(), std::bind2nd( std::mem_fun( &StaticObject::SetDrawCollsionMesh ), FALSE ) );

		m_pPickedObject->SetDrawCollsionMesh( TRUE );

		if( m_pPickedObject->IsSelectedObject() )
		{
			if( m_pPickedObject->GetNifMesh() )
				m_pPickedObject->GetNifMesh()->SetWireFrame( true );
			
			const NiPoint3 &vPos = m_pPickedObject->GetNifMesh()->GetRootObject()->GetTranslate();

			return TRUE;
		}
	}

	return FALSE;
}

//		탄흔, 혈흔 등의 데칼을 오브젝트에 Attach 시킨다.
BOOL	StaticObjectManager::AttachDecal( const NiPoint3& pos, const NiPoint3& direction )
{

	// 바운딩 박스에 Ray가 충돌한 모든 타일들을 구한다.
	m_pPick->SetPickType( NiPick::FIND_ALL);
	m_pPick->SetSortType( NiPick::SORT );
	m_pPick->SetCoordinateType( NiPick::WORLD_COORDINATES );
	m_pPick->SetIntersectType( NiPick::BOUND_INTERSECT );
	m_pPick->SetReturnNormal( true );
	m_pPick->SetObserveAppCullFlag( false );

	m_pPick->SetTarget( m_spObjectRoot );

	if( m_pPick->PickObjects( pos, direction ) )
	{

	}

	return FALSE;
}

//		바운딩 박스 출력 여부
void	StaticObjectManager::SetDrawObjectBB( bool bDraw )
{
	std::for_each( m_listMeshObject.begin(), m_listMeshObject.end(), std::bind2nd( std::mem_fun( &StaticObject::SetDrawObjectBB ), bDraw ) );
}

//		선택되어진 MeshObject의 MatWorld가 있는지 여부
BOOL	StaticObjectManager::IsSelectedObject()
{
	if( m_pPickedObject != NULL )
	{
		if( m_pPickedObject->IsSelectedObject() )
			return TRUE;
	}

	return FALSE;
}

StaticObject*	StaticObjectManager::GetSelectedMatOfObj()	
{	
	return m_pPickedObject; 
}

//		World에 선택되어진 MeshObject을 NULL로 셋팅
void	StaticObjectManager::SetSelectedObjIsNULL()
{
	if( m_pPickedObject != NULL)
	{	
		m_pPickedObject->SetSelectedObjIsNULL();
	}
}

//		Object Dlg에서 입력된 World 좌표로 셋팅
void	StaticObjectManager::SetSelectedObjPos( const NiPoint3& vPos  )
{
	if( m_pPickedObject != NULL)
	{
		m_pPickedObject->SetSelectedObjPos( vPos );
	}
}


void	StaticObjectManager::Translation( float x, float y, float z )
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->Translation( x, y, z );
	}
}

void	StaticObjectManager::Scaling( float fDel )
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->Scaling( fDel );
	}

}

void	StaticObjectManager::RotationX( float fAngle )
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->RotationX( fAngle );
	}

}

void	StaticObjectManager::RotationY( float fAngle )
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->RotationY( fAngle );
	}

}

void	StaticObjectManager::RotationZ( float fAngle )
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->RotationZ( fAngle );
	}

}

void	StaticObjectManager::SetDefault()
{
	if( m_pPickedObject != NULL )
	{
		m_pPickedObject->SetDefault();
	}
}

/*
void	StaticObjectManager::SaveHeader( CArchive& ar )
{
	ar << (int)m_listMeshObject.size();	// 오브젝트 종류 갯수

	if( m_listMeshObject.empty() )
		return;

	std::list< StaticObject* >::iterator iterObject;	
	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		ar << (*iterObject)->GetObjectCount();
	}

}

void	StaticObjectManager::SaveData( CArchive& ar )
{
	if( m_listMeshObject.empty() )
		return;

	std::list< StaticObject* >::iterator iterObject;	
	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		(*iterObject)->SaveData( ar );
	}
}

void	StaticObjectManager::SaveNameData( std::ofstream& fwrite )
{
	std::list< StaticObject* >::iterator iterObject;	
	
	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		fwrite << CW2A((*iterObject)->GetFileName()) << "\n";
	}	
}
*/

void	StaticObjectManager::LoadData( FILE *file, std::list<int>& listEachofMeshCount )
{
	if( m_listMeshObject.empty() )
		return;

	std::list< StaticObject* >::iterator iterObj;
	StaticObject *meshObj = NULL;

	for( iterObj = m_listMeshObject.begin(); iterObj != m_listMeshObject.end(); ++iterObj )
	{
		(*iterObj)->LoadData( file, listEachofMeshCount.front() );
		listEachofMeshCount.pop_front();
	}	
}

void	StaticObjectManager::LoadNameData( FILE *file, std::list< int >& listEachofMeshCount, std::list<NiFixedString>& listFileName )
{
	int nCntMeshObjects = 0, nCntMesh = 0;

	m_pCurrentObject = NULL;

	char buf[10];

	// 오브젝트 종류 갯수
	fread( buf, 1, sizeof(int), file );
	memcpy( &nCntMeshObjects, buf, sizeof(int)  );

	if( nCntMeshObjects <= 0 )
		return;

	for( int i = 0; i < nCntMeshObjects; i++ )
	{
		fread( buf, 1, sizeof(int), file );
		memcpy( &nCntMesh, buf, sizeof(int)  );

		listEachofMeshCount.push_back( nCntMesh );
		NiFixedString &filename = listFileName.front();
//		NiFixedString strFileName = filename.c_str();

		AddMeshObject( filename );
		listFileName.pop_front();
	}

	
}

void	StaticObjectManager::SpaceDivisionForRendering( Terrain *pTerrain )
{

	std::list< StaticObject* >::iterator iterObject;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		(*iterObject)->SpaceDivisionForRendering( pTerrain );
	}
}

void	StaticObjectManager::SpaceDivisionForCollision( Terrain *pTerrain )
{

	std::list< StaticObject* >::iterator iterObject;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		(*iterObject)->SpaceDivisionForCollision( pTerrain );
	}
}

void		StaticObjectManager::SetCollisionTransform( Terrain *pTerrain )
{
	std::list< StaticObject* >::iterator iterObject;

	NiNode *pStartPosition = NULL;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		(*iterObject)->SetCollisionTransform( pTerrain );
	}
}

//			렌더링 용 Local Transform을 셋팅			
void		StaticObjectManager::SetRenderingTransform( Terrain *pTerrain )
{
	std::list< StaticObject* >::iterator iterObject;

	NiNode *pStartPosition = NULL;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		(*iterObject)->SetRenderingTransform( pTerrain );
	}
}

//			Nif 파일에 있는 "start" Node로 부터 케릭터 시작위치를 얻음
NiNode*		StaticObjectManager::GetCharacterStartPosition()
{
	std::list< StaticObject* >::iterator iterObject;

	NiNode *pStartPosition = NULL;

	for( iterObject = m_listMeshObject.begin(); iterObject != m_listMeshObject.end(); ++iterObject )
	{
		pStartPosition = (*iterObject)->GetCharacterStartPosition();

		if( pStartPosition )
			return pStartPosition;			
	}

	return NULL;

}