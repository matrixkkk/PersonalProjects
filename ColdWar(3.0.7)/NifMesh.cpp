#include "main.h"
#include "NifMesh.h"
#include "GameApp.h"
#include "Terrain.h"


NifMesh::NifMesh()
{
	m_spObject		= NULL;
	m_pBoundBox		= NULL;

	m_strFileName	= "";
	m_bDrawBB		= false;
	m_bWireFrame	= false;
}

NifMesh::NifMesh( const NiFixedString &fileName )
{

	m_spObject		= NULL;
	m_pBoundBox		= NULL;

	m_strFileName	= fileName;
	m_bDrawBB		= false;
	m_bWireFrame	= false;

}

NifMesh::~NifMesh()
{
	ReleaseObject();
}

NiNode*		NifMesh::GenerateMesh()
{
	CGameApp::SetMediaPath("./Data/Object/");
	
	NiStream	kStream;
	NiBound		kBound;
	NiPoint3	kCenter;
	float		fRadius;

    // Load in the scenegraph for our world...
	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename( m_strFileName ) );

	unsigned int uiCount = kStream.GetObjectCount();
    assert(uiCount > 0 );    
    
	// Find root of scene graph and the animated camera
    for (unsigned int i = 0; i < uiCount; i++)
    {
        NiObject* pkObject = kStream.GetObjectAt(i);

        if (NiIsKindOf(NiNode, pkObject))
        {
            // Set first node as root
            m_spObject = (NiNode*)pkObject;
			const NiFixedString &kName = m_spObject->GetName();
			break;
		}
    }
	assert (m_spObject);

	if( bSuccess )
	{

		NiAlphaProperty *pAlpha = (NiAlphaProperty*)m_spObject->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();
			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

			m_spObject->AttachProperty( pAlpha );
		}

		m_spObject->Update(0.0f);
		m_spObject->UpdateNodeBound();
		m_spObject->UpdateProperties();

		kBound  = m_spObject->GetWorldBound();		
		kCenter = kBound.GetCenter();
		fRadius = kBound.GetRadius();

		m_vMin.x = kCenter.x - fRadius;
		m_vMin.y = kCenter.y - fRadius;
		m_vMin.z = kCenter.z - fRadius;

		m_vMax.x = kCenter.x + fRadius;	
		m_vMax.y = kCenter.y + fRadius;
		m_vMax.z = kCenter.z + fRadius;

		if( !m_pBoundBox )
		{
			m_pBoundBox			= NiNew BoundBox( m_vMin, m_vMax );
			NiAVObject *pBound	= m_pBoundBox->GenerateAABB( m_vMin, m_vMax );

			m_spObject->AttachChild( pBound );
		}
	}

	NiWireframeProperty *pWire = (NiWireframeProperty*)m_spObject->GetProperty( NiProperty::WIREFRAME );

	if( !pWire )
	{
		NiWireframePropertyPtr spWireFrame = NiTCreate<NiWireframeProperty>();
		spWireFrame->SetWireframe( false );
		m_spObject->AttachProperty( spWireFrame );
	}

	m_spObject->Update(0.0f);
	m_spObject->UpdateNodeBound();
	m_spObject->UpdateProperties();
	m_spObject->UpdateEffects();

	return m_spObject;
}

BOOL	NifMesh::GenerateAABB( const NiPoint3& vMin, const NiPoint3& vMax )
{
	if( !m_pBoundBox )
	{
		m_pBoundBox = NiNew BoundBox( vMin, vMax );
		NiAVObject *pBound = m_pBoundBox->GenerateAABB( vMin, vMax );

		m_spObject->AttachChild( pBound );

		m_spObject->Update(0.0f);
		m_spObject->UpdateNodeBound();
		m_spObject->UpdateProperties();
		m_spObject->UpdateEffects();

		return TRUE;
	}

	return FALSE;
}

void	NifMesh::SetWireFrame( bool bWire )
{
	if( m_spObject )
	{
		NiWireframeProperty* pkWireframeProp = (NiWireframeProperty*)m_spObject->GetProperty( NiWireframeProperty::GetType()) ;
		pkWireframeProp->SetWireframe( bWire );
	
		m_spObject->Update(0.0f);
		m_spObject->UpdateProperties();
	}

}

BOOL	NifMesh::GenerateAABB()
{
	if( !m_pBoundBox )
	{
		m_pBoundBox = NiNew BoundBox( m_vMin, m_vMax );
		m_pBoundBox->GenerateAABB( m_vMin, m_vMax );

		return TRUE;
	}

	return FALSE;
}

void	NifMesh::SetName( const NiFixedString& strName )
{
	m_strFileName = strName;
}

void	NifMesh::GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax )
{
	*vMin = m_vMin;
	*vMax = m_vMax;
}

void	NifMesh::SetMinMax( const NiPoint3& vMin, const NiPoint3& vMax )
{
	m_vMin = vMin;
	m_vMax = vMax;
}


BOOL	NifMesh::ReleaseObject()
{
	if( m_spObject )
	{
//		NiDelete m_spObject;
		m_spObject = 0;
	}
	
	if( m_pBoundBox )
	{
		NiDelete m_pBoundBox;
		m_pBoundBox = NULL;
	}

	return TRUE;
}

NiNode*		NifMesh::ClonObject( NiNode *pObject )
{
	if( !m_spObject )
	{

//		NiObjectNET::SetDefaultCopyType(NiObjectNET::COPY_EXACT);		// 이름 값을 똑같이 복사
		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;

		m_spObject = (NiNode*)pObject->Clone(kCloning);
/*
		NiAlphaProperty *pAlpha = (NiAlphaProperty*)m_spObject->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();
			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

			m_spObject->AttachProperty( pAlpha );
			m_spObject->Update( 0.0f );
			m_spObject->UpdateProperties();
		}
*/
	}

	return m_spObject;
}

NiNode*		NifMesh::ClonObject( NifMesh *pNifMehs )
{
	if( !m_spObject )
	{
//		NiObjectNET::SetDefaultCopyType(NiObjectNET::COPY_EXACT);		// 이름 값을 똑같이 복사
		
		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
//		kCloning.m_cAppendChar = '%';

		NiNode *pAVObject = pNifMehs->GetRootObject();

		m_spObject = (NiNode*)pAVObject->Clone(kCloning);
	}

	return m_spObject;
	
}

NiNode*		NifMesh::GetRootObject()
{
	return m_spObject;
}

void	NifMesh::SetObject( NiNode* pNode )
{
	m_spObject = pNode;
}

void	NifMesh::SetAppCulled( bool bCull )
{
	m_spObject->SetAppCulled( bCull );
	
	if( m_bDrawBB )
		m_pBoundBox->SetAppCulled( bCull );
}

void	NifMesh::DrawBB( bool bDraw )
{
	if( m_pBoundBox ) m_pBoundBox->SetAppCulled( !bDraw );
}

BOOL	NifMesh::RayCollision( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *vPickPos, NiPoint3 *vNormal, NiPick *pPick, float *fDist )
{

	if( m_spObject == NULL )
		return FALSE;

	unsigned short triIndex = 0;
	unsigned short vtxIndex[3];
	NiPoint3 vLook, vPos;

	pPick->SetTarget( m_spObject );

	if( pPick->PickObjects( pos, dir ) )
	{
		// get pick results
		NiPick::Results& pickResults = pPick->GetResults();
		NiPick::Record* pPickRecord = pickResults.GetAt(0);

		*fDist = NiAbs(pPickRecord->GetDistance());
		unsigned short triIndex = pPickRecord->GetTriangleIndex() / 2;
		pPickRecord->GetVertexIndices( vtxIndex[0], vtxIndex[1], vtxIndex[2] );

//		*vPickPos = pos + dir * (*fDist);
		*vPickPos	= pPickRecord->GetIntersection();
		*vNormal	= pPickRecord->GetNormal();

		pPick->RemoveTarget();

		return TRUE;
	}

	pPick->RemoveTarget();

	return FALSE;

}

BOOL	NifMesh::RayCollisionBB( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *dis )
{
	if( NULL == m_pBoundBox || NULL == pPick)
		return FALSE;

	if( m_pBoundBox->RayCollision( pos, dir, pPick, dis ) )
		return TRUE;
		
	return FALSE;
}

void	NifMesh::Translation( float x, float y, float z )
{
	NiPoint3 vMoveUnit( x, y, z );
	NiPoint3 kObjectPos = m_spObject->GetTranslate();	

	kObjectPos	+= vMoveUnit;

	if( m_spObject )
	{
		m_spObject->SetTranslate( kObjectPos );
		m_spObject->Update( 0.0f );
	}

}

void	NifMesh::Scaling( float fDel )
{
	if( m_spObject )
	{
		float fScale = m_spObject->GetScale() + fDel ;

		if( fScale <= 0.0f )
			fScale = NiAbs( fScale );

		m_spObject->SetScale( fScale );
		m_spObject->Update( 0.0f );
	}

}

void	NifMesh::RotationX( float fAngle )
{
	NiMatrix3 kRotX;
	kRotX.MakeXRotation( fAngle );

	if( m_spObject )
	{
		m_spObject->SetRotate( kRotX * m_spObject->GetRotate() );
		m_spObject->Update( 0.0f );
	}

	
}

void	NifMesh::RotationY( float fAngle )
{
	NiMatrix3 kRotY;
	kRotY.MakeYRotation( fAngle );

	if( m_spObject )
	{
		m_spObject->SetRotate( kRotY * m_spObject->GetRotate() );
		m_spObject->Update( 0.0f );
	}


}

void	NifMesh::RotationZ( float fAngle )
{
	NiMatrix3 kRotZ;
	kRotZ.MakeZRotation( fAngle );

	if( m_spObject )
	{
		m_spObject->SetRotate( kRotZ * m_spObject->GetRotate() );
		m_spObject->Update( 0.0f );
	}

}

void	NifMesh::SetDefault()
{

}

void	NifMesh::SetPosition( const NiPoint3& vPos )
{
	if( m_spObject )
		m_spObject->SetTranslate( vPos );

	if( m_pBoundBox )
		m_pBoundBox->SetPosition( vPos );
}

void	NifMesh::AddCollisionGroup( unsigned int &objectIndex, int &index, BYTE type )
{
	stCollisionIncludeInfo stIncludeInfo;

	stIncludeInfo.m_nObjectIndex	= (BYTE)objectIndex;
	stIncludeInfo.m_nTileNum		= (BYTE)index;
	stIncludeInfo.m_nIncludeType	= type;

	m_vecInculdeCollsionGroup.push_back( stIncludeInfo );
}

void	NifMesh::AddRenderingGroup( unsigned int &objectIndex, int &index, BYTE type )
{
	stRenderingIncludeInfo stIncludeInfo;

	stIncludeInfo.m_nObjectIndex	= (BYTE)objectIndex;
	stIncludeInfo.m_nTileNum		= (BYTE)index;
	stIncludeInfo.m_nIncludeType	= type;

	m_vecInculdeRenderingGroup.push_back( stIncludeInfo );

}

void	NifMesh::ResetRenderingAndCollsionGroup()
{
	if( !m_vecInculdeRenderingGroup.empty() )
		m_vecInculdeRenderingGroup.clear();

	if( !m_vecInculdeCollsionGroup.empty() )
		m_vecInculdeCollsionGroup.clear();
}

void	NifMesh::ResetRenderingGroup()
{
	if( !m_vecInculdeRenderingGroup.empty() )
		m_vecInculdeRenderingGroup.clear();
}

void	NifMesh::ResetCollsionGroup()
{
	if( !m_vecInculdeCollsionGroup.empty() )
		m_vecInculdeCollsionGroup.clear();
}

void	NifMesh::CalculateWorldMinMax( NiPoint3 *vMin, NiPoint3 *vMax )
{
	NiPoint3 vTmpMin, vTmpMax;

	float fRadius = 0.0f;

	vTmpMin.x = vTmpMin.y = FLT_MIN;
	vTmpMax.x = vTmpMax.y = FLT_MIN;

	// 렌더링 최상위 Min, Max 추출
	NiNode *pRenderingGroup = (NiNode*)m_spObject->GetObjectByName( "RenderingGroup" );

	const NiBound &kBound = pRenderingGroup->GetWorldBound();
	const NiPoint3 &kCenter = kBound.GetCenter();

	unsigned int uiCntChild = pRenderingGroup->GetChildCount();

	for( unsigned int ui = 0; ui < uiCntChild; ui++ )
	{
		if( NiIsKindOf( NiNode, pRenderingGroup->GetAt( ui ) ) )
		{
			NiNode *pObjects = (NiNode*)pRenderingGroup->GetAt( ui );

			for( unsigned int uj = 0; uj < pObjects->GetChildCount(); uj++ )
			{	
				NiGeometry *pShapeData =  (NiGeometry*)pObjects->GetAt( uj );

				const NiBound &kBound = pShapeData->GetWorldBound();
				const NiPoint3 &kCenter = kBound.GetCenter();
				fRadius = kBound.GetRadius();

				vMin->x = kCenter.x - fRadius;
				vMin->y = kCenter.y - fRadius;

				vMax->x = kCenter.x + fRadius;	
				vMax->y = kCenter.y + fRadius;

				if( vMin->x > vTmpMin.x )
					vTmpMin.x = vMin->x;

				if( vMin->y > vTmpMin.y )
					vTmpMin.y = vMin->y;

				if( vMax->x > vTmpMax.x )
					vTmpMax.x = vMax->x;

				if( vMax->y > vTmpMax.y )
					vTmpMax.y = vMax->y;				
			}
		}
		else
		{
			NiGeometry *pObject = (NiGeometry*)pRenderingGroup->GetAt( ui );

			const NiBound &kBound = pObject->GetWorldBound();
			const NiPoint3 &kCenter = kBound.GetCenter();
			fRadius = kBound.GetRadius();	

			vMin->x = kCenter.x - fRadius;
			vMin->y = kCenter.y - fRadius;

			vMax->x = kCenter.x + fRadius;	
			vMax->y = kCenter.y + fRadius;

			if( vMin->x > vTmpMin.x )
				vTmpMin.x = vMin->x;

			if( vMin->y > vTmpMin.y )
				vTmpMin.y = vMin->y;

			if( vMax->x > vTmpMax.x )
				vTmpMax.x = vMax->x;

			if( vMax->y > vTmpMax.y )
				vTmpMax.y = vMax->y;	

		}
	}

	vMin->x = vTmpMin.x;
	vMin->y = vTmpMin.y;

	vMax->x = vTmpMax.x;
	vMax->y = vTmpMax.y;
}

BOOL	NifMesh::IsMapData()
{
	if( m_spObject )
	{
		NiNode *pCollision = NULL;
		NiNode *pObjectGroup = NULL;

		pCollision = (NiNode*)m_spObject->GetObjectByName( "Collision" );
		pObjectGroup = (NiNode*)m_spObject->GetObjectByName( "ObjectGroup" );

		if( pCollision && pObjectGroup )
			return TRUE;

		return FALSE;
	}

	return FALSE;

}

//-----------------------------------------------------
//
//		맵 테이타 타일 바운딩 박스의 의해 공간 분할
//		Collision		= 충돌 체크용 바운딩 박스
//		ObjectGroup		= 렌더링용 Object 모음
//
//-----------------------------------------------------

void	NifMesh::SpaceDivision( Terrain *pTerran )
{
	if( !pTerran || !m_spObject )
		return;

	if( m_spObject )
	{

		NiStream kStream;
		NiPoint3 vMin, vMax;

		NiNode *pObjectGroup	= (NiNode*)m_spObject->GetObjectByName( "ObjectGroup" );
		NiNode *pCollision		= (NiNode*)m_spObject->GetObjectByName( "Collision" );

		unsigned int uiCntChild = pObjectGroup->GetChildCount();

		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			if( NiIsKindOf( NiNode, pObjectGroup->GetAt( ui ) ) )
			{
				NiNode *pObjects = (NiNode*)pObjectGroup->GetAt( ui );

				for( unsigned int uj = 0; uj < pObjects->GetChildCount(); uj++ )
				{	
					NiGeometry *pShapeData =  (NiGeometry*)pObjects->GetAt( uj );

					pTerran->IsCollisionOfTile( pShapeData );
				}
			}
			else
			{
				NiGeometry *pObject = (NiGeometry*)pObjectGroup->GetAt( ui );

				pTerran->IsCollisionOfTile( pObject );			
			}
		}

		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			NiNode *pObjects = (NiNode*)pObjectGroup->GetAt( ui );

			NiGeometry *pShapeData =  (NiGeometry*)pObjects->GetAt( 0 );

			pTerran->IsCollisionOfTile( pShapeData );			
		}
	}
}


//-----------------------------------------------------
//
//		맵 테이타 타일 바운딩 박스의 의해 공간 분할
//		Collision		= 충돌 체크용 바운딩 박스
//		ObjectGroup		= 렌더링용 Object 모음
//
//-----------------------------------------------------
void	NifMesh::SpaceDivisionForRenderingFromMapData( Terrain *pTerran )
{

	if( m_spObject )
	{

		NiNode *pRenderingGroup = (NiNode*)m_spObject->GetObjectByName( "ObjectGroup" );
		
		const NiTransform& transRoot = m_spObject->GetLocalTransform();
		const NiTransform& transRenderGroup = pRenderingGroup->GetLocalTransform();
		NiTransform combinedTransform = transRoot * transRenderGroup;

		unsigned int uiCntChild = pRenderingGroup->GetChildCount();
		unsigned int uiTileNums = 0;
		int *pTileListInfo;

		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			if( NiIsKindOf( NiNode, pRenderingGroup->GetAt( ui ) ) )
			{
				NiNode *pObjects = (NiNode*)pRenderingGroup->GetAt( ui );

				NiIntegersExtraData *pTileList = (NiIntegersExtraData*)pObjects->GetExtraData( "TileNums" );

				//----------------- TileNums Info -------------------
				//	타일번호, 포함타입( 부분 포함, 완전 포함 )
				//---------------------------------------------------
				pTileList->GetArray( uiTileNums, pTileListInfo );

				
				for( unsigned int uo = 0; uo < uiTileNums / 2; uo++ )
				{						
					pTerran->SetRenderingGroup( pObjects, pTileListInfo[ 2 * uo ], pTileListInfo[ 2 * uo + 1 ] );
				}

			}
			else
			{
				NiAVObject *pObject = (NiAVObject*)pRenderingGroup->GetAt( ui );

				pObject->SetAppCulled( true );

				NiIntegersExtraData *pTileList = (NiIntegersExtraData*)pObject->GetExtraData( "TileNums" );
				//----------------- TileNums Info -------------------
				//	타일번호, 포함타입( 부분 포함, 완전 포함 )
				//---------------------------------------------------
				pTileList->GetArray( uiTileNums, pTileListInfo );

				for( unsigned int uo = 0; uo < uiTileNums / 2; uo++ )
				{						
					pTerran->SetRenderingGroup( (NiNode*)pObject, pTileListInfo[ 2 * uo ], pTileListInfo[ 2 * uo + 1 ] );
				}
			}
		}
	}
}


void NifMesh::SpaceDivisionForRendering( Terrain *pTerran )
{
	std::vector< stRenderingIncludeInfo >::iterator iterInfo;

	NiNode *pRenderingGroup = (NiNode*)m_spObject->GetObjectByName("RenderingGroup");

	if( !pRenderingGroup ) return;

	for( iterInfo = m_vecInculdeRenderingGroup.begin(); iterInfo != m_vecInculdeRenderingGroup.end(); ++iterInfo )
	{
		int nTilNum = iterInfo->m_nTileNum;
		int nType = iterInfo->m_nIncludeType;

		NiNode *pNode = (NiNode*)pRenderingGroup->GetAt( iterInfo->m_nObjectIndex );

		if( !pNode ) continue;

		pTerran->SetRenderingGroup( pNode, nTilNum, nType );
	}	
}

void	NifMesh::SpaceDivisionForCollision( Terrain *pTerran )
{
	NiNode *pCollisionGroup = (NiNode*)m_spObject->GetObjectByName( "Collision" );

	const NiTransform& transform = m_spObject->GetLocalTransform();

	if( !pCollisionGroup ) return;

	std::vector< stCollisionIncludeInfo >::iterator iterInfo;

	for( iterInfo = m_vecInculdeCollsionGroup.begin(); iterInfo != m_vecInculdeCollsionGroup.end(); ++iterInfo )
	{
		int nTilNum = iterInfo->m_nTileNum;
		int nType = iterInfo->m_nIncludeType;

		NiNode *pNode = (NiNode*)pCollisionGroup->GetAt( iterInfo->m_nObjectIndex );
		
		if( !pNode ) continue;

		pTerran->SetCollisionGroup( pCollisionGroup, transform, nTilNum, nType );
	}
}

//		Nif 맵 파일의 Extra Data를 이용하여 해당 타일의 CollisionManager에 추가		
void	NifMesh::SpaceDivisionForCollisionFromMapData( Terrain *pTerran )
{
	if( m_spObject )
	{
		NiNode *pCollisionGroup = (NiNode*)m_spObject->GetObjectByName( "Collision" );

		const NiTransform &vRootTransform		= m_spObject->GetLocalTransform();
		const NiTransform &vColiRootTransform	= pCollisionGroup->GetLocalTransform();

		const NiTransform &vTransformForCollision = vRootTransform * vColiRootTransform;

		unsigned int uiCntChild = pCollisionGroup->GetChildCount();
		unsigned int uiTileNums = 0;
		int *pTileListInfo;

		for( unsigned int ui = 0; ui < uiCntChild; ui++ )
		{
			if( NiIsKindOf( NiNode, pCollisionGroup->GetAt( ui ) ) )
			{
				NiNode *pObjects = (NiNode*)pCollisionGroup->GetAt( ui );

				if( NiIsKindOf( NiNode, pObjects->GetAt( 0 ) ) )
				{ 
					NiNode *pNDLCD_BN = (NiNode*)pObjects->GetAt( 0 );

					if( NiIsKindOf( NiGeometry, pNDLCD_BN->GetAt(0) ) )
					{
						NiAVObject *pShapeObject =  (NiAVObject*)pNDLCD_BN->GetAt( 0 );

						pShapeObject->SetAppCulled( true );

						NiIntegersExtraData *pTileList = (NiIntegersExtraData*)pShapeObject->GetExtraData( "TileNums" );

						pTileList->GetArray( uiTileNums, pTileListInfo );
					
						//----------------- TileNums Info -------------------
						//	타일번호, 포함타입( 부분 포함, 완전 포함 )
						//---------------------------------------------------
						for( unsigned int uo = 0; uo < uiTileNums / 2; uo++ )
						{						
							pTerran->SetCollisionGroup( pObjects, vTransformForCollision, pTileListInfo[ 2 * uo ], pTileListInfo[ 2 * uo + 1 ] );
						}
					}
				}
			}
		}
	}	
}


//			Nif 파일에 있는 "start" Node로 부터 케릭터 시작위치를 얻음
NiNode*	NifMesh::GetCharacterStartPosition()
{
	if( m_spObject )
	{
		NiNode *pStartPosition =  NULL;
		
		pStartPosition = (NiNode*)m_spObject->GetObjectByName( "start" );

		if( pStartPosition )
			return pStartPosition;
	}

	return NULL;
}

void	NifMesh::SetCollisionTransform( Terrain *pTerran )
{
	if( m_spObject )
	{
		NiNode *pCollisionGroup = (NiNode*)m_spObject->GetObjectByName( "Collision" );

		const NiTransform &vRootTransform		= m_spObject->GetLocalTransform();
		const NiTransform &vColiRootTransform	= pCollisionGroup->GetLocalTransform();

		const NiTransform &vTransformForCollision = vRootTransform * vColiRootTransform;

		pTerran->SetCollisionTransform( vTransformForCollision );

		m_spObject->Update(0.0f);

	}

}

void	NifMesh::SetRenderingTransform( Terrain *pTerran )
{
	if( m_spObject )
	{
		NiNode *pRenderingGroup = (NiNode*)m_spObject->GetObjectByName( "ObjectGroup" );

		const NiTransform &vRootTransform		= m_spObject->GetLocalTransform();
		const NiTransform &vColiRootTransform	= pRenderingGroup->GetLocalTransform();

		const NiTransform &vTransformForRendering = vRootTransform * vColiRootTransform;

		pTerran->SetRenderingTransform( vTransformForRendering );

		m_spObject->Update(0.0f);

	}
}

/*
void	NifMesh::SaveData( CArchive& ar )
{
	const NiPoint3	&vPos	= m_spObject->GetTranslate();
	const NiMatrix3 &matRot = m_spObject->GetRotate();
	float fScale			= m_spObject->GetScale();
	NiPoint3 matCol;

	ar << vPos.x;
	ar << vPos.y;
	ar << vPos.z;

	for( unsigned int ui = 0; ui < 3; ui++ )
	{
		matRot.GetCol( ui, matCol );
		ar << matCol.x;
		ar << matCol.y;
		ar << matCol.z;
	}

	if( fScale < 0.0f )
		fScale = NiAbs( fScale );

	ar << fScale;
}
*/
void	NifMesh::LoadData( FILE *file )
{

	if( !m_spObject )
	{
		NiMessageBox(" NifMesh::LoadData - m_spObject = NULL! ", "Failed" );
		return;
	}

	NiPoint3	vPos;
	NiMatrix3	matRot;
	NiPoint3	matCol;
	float		fScale;
	int			nCnt;

	char buf[15];

	fread( buf, 1, sizeof(float) * 3, file );

	memcpy( &vPos.x	, buf						, sizeof(float)  );
	memcpy( &vPos.y	, buf + sizeof(float)		, sizeof(float)  );
	memcpy( &vPos.z	, buf + sizeof(float) * 2	, sizeof(float)  );
		
	for( unsigned int ui = 0; ui < 3; ui++ )
	{	
		fread( buf, 1, sizeof(float) * 3, file );

		memcpy( &matCol.x	, buf + sizeof(float) * 0	, sizeof(float)  );
		memcpy( &matCol.y	, buf + sizeof(float) * 1	, sizeof(float)  );
		memcpy( &matCol.z	, buf + sizeof(float) * 2	, sizeof(float)  );

		matRot.SetCol( ui, matCol );
	}
	
	fread( buf, 1, sizeof(float), file );

	memcpy( &fScale	, buf, sizeof(float)  );

	if( fScale < 0.0f )
		fScale = NiAbs( fScale );
	
	m_vecInculdeCollsionGroup.clear();
	m_vecInculdeRenderingGroup.clear();

	//===================================================================
	//							렌더링 그룹
	//===================================================================

	// 포함 정보
	fread( buf, 1, sizeof(int), file );
	memcpy( &nCnt, buf, sizeof(int)  );

	if( nCnt )
	{
		stRenderingIncludeInfo stIncludeInfo;

		for( int i = 0; i < nCnt; i++ )
		{
			fread( buf, 1, sizeof(WORD) + sizeof(BYTE)*2, file );

			memcpy( &stIncludeInfo.m_nObjectIndex	, buf								, sizeof(BYTE)  );
			memcpy( &stIncludeInfo.m_nTileNum		, buf + sizeof(BYTE)				, sizeof(WORD)  );
			memcpy( &stIncludeInfo.m_nIncludeType	, buf + sizeof(BYTE) + sizeof(WORD) , sizeof(BYTE)  );

			m_vecInculdeRenderingGroup.push_back( stIncludeInfo );
		}
	}

	//===================================================================
	//							컬리전 그룹
	//===================================================================
	// 포함 정보
	fread( buf, 1, sizeof(int), file );
	memcpy( &nCnt, buf, sizeof(int)  );

	if( nCnt )
	{
		stCollisionIncludeInfo stIncludeInfo;

		for( int i = 0; i < nCnt; i++ )
		{
			fread( buf, 1, sizeof(WORD) + sizeof(BYTE)*2, file );

			memcpy( &stIncludeInfo.m_nObjectIndex	, buf								, sizeof(BYTE)  );
			memcpy( &stIncludeInfo.m_nTileNum		, buf + sizeof(BYTE)				, sizeof(WORD)  );
			memcpy( &stIncludeInfo.m_nIncludeType	, buf + sizeof(BYTE) + sizeof(WORD) , sizeof(BYTE)  );

			m_vecInculdeCollsionGroup.push_back( stIncludeInfo );
		}
	}

	if( m_spObject )
	{	
		NiWireframeProperty *pWire = (NiWireframeProperty *)m_spObject->GetProperty( NiProperty::WIREFRAME );
		if( pWire )
		{
			if( pWire->GetWireframe() )
				pWire->SetWireframe( false );
		}

		NiAlphaProperty *pAlpha = (NiAlphaProperty*)m_spObject->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();
			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestRef( 0x08 );
			pAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);
			m_spObject->AttachProperty( pAlpha );
		}

		m_spObject->SetTranslate( vPos );
		m_spObject->SetRotate( matRot );
		m_spObject->SetScale( fScale );

		m_spObject->Update( 0.0f );
		m_spObject->UpdateProperties();
		m_spObject->UpdateWorldBound();
	}
}
