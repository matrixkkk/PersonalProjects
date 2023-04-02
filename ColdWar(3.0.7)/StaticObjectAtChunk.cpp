#include "main.h"
#include "StaticObjectAtChunk.h"
//#include "ZFLog.h"



StaticObjectAtChunk::StaticObjectAtChunk()
{
	m_spRootStaticObject = NiNew NiNode();
	m_spRootForCollision = NiNew NiNode();

	m_spRootStaticObject->SetAppCulled( false );
	m_spRootStaticObject->Update( 0.0f );

	m_vecObject.clear();
}


//		Release
BOOL	StaticObjectAtChunk::ReleaseObject()
{

	if( !m_vecObject.empty() )
		m_vecObject.clear();

	if( m_spRootStaticObject )
	{
		for( unsigned int ui = 0; ui < m_spRootStaticObject->GetChildCount(); ui++ )
		{
			m_spRootStaticObject->DetachChild( m_spRootStaticObject->GetAt( ui ) );
		}

		m_spRootStaticObject = 0;
	}


	return TRUE;
}

//		NiNode Object ����
NiNode* StaticObjectAtChunk::GetRootObject()
{
	return m_spRootStaticObject;
}

void	StaticObjectAtChunk::AddStaticObject( NiNode *pObject, int type )
{
	pObject->SetAppCulled( false );

	// ���� �ٴ� ��ŷ�� Collision Group�� �߰�
	AddObjectForCollision( pObject );

	// �������� �׷쿡 ����
	if( type == ENTIRE_INCLUDE )
	{		
		pObject->SetAppCulled( true );
		const NiTransform &parentTrans =  pObject->GetParent()->GetWorldTransform();
		const NiTransform& combindTrans = parentTrans * pObject->GetLocalTransform();

		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;		
		NiAVObject *pCloneObject = (NiAVObject*)pObject->Clone(kCloning);

		NiAlphaProperty *pAlpha = (NiAlphaProperty*)pCloneObject->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();

			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestRef( 0x08 );
			pAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);

			pCloneObject->AttachProperty( pAlpha );
		}

		NiMaterialProperty *pMat = (NiMaterialProperty*)pCloneObject->GetProperty( NiProperty::MATERIAL );

		if( !pMat )
		{
			pMat = NiNew NiMaterialProperty();

			pMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
			pMat->SetAlpha(1.0f);//----------->�� ���� ���� �����ؼ� ������ ������� ��.
			pMat->SetShineness(10.0f);

			pCloneObject->AttachProperty( pMat );
		}

		pCloneObject->SetAppCulled( false );

		pCloneObject->SetTranslate( combindTrans.m_Translate );
		pCloneObject->SetRotate( combindTrans.m_Rotate );
		pCloneObject->SetScale( combindTrans.m_fScale );
		pCloneObject->Update(0.0f);
		pCloneObject->UpdateProperties();

		m_spRootStaticObject->AttachChild( pCloneObject );
		m_spRootStaticObject->Update( 0.0f );		
		m_spRootStaticObject->UpdateProperties();
		m_spRootStaticObject->UpdateWorldBound();

	}
	else
	{
		NiAlphaProperty *pAlpha = (NiAlphaProperty*)pObject->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();

			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestRef( 0x08 );
			pAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);

			pObject->AttachProperty( pAlpha );
		}

		m_vecObject.push_back( pObject );
	}
	
}

void	StaticObjectAtChunk::AddObjectForCollision( NiAVObject *pObject )
{
	if( m_spRootForCollision )
	{
		const NiTransform &parentTrans =  pObject->GetParent()->GetWorldTransform();
		const NiTransform& combindTrans = parentTrans * pObject->GetLocalTransform();

		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
		NiAVObject *pCloneObject = (NiAVObject*)pObject->Clone(kCloning);

		pCloneObject->SetTranslate( combindTrans.m_Translate );
		pCloneObject->SetRotate( combindTrans.m_Rotate );
		pCloneObject->SetScale( combindTrans.m_fScale );
		pCloneObject->Update( 0.0f );
		pCloneObject->UpdateProperties();
		pCloneObject->UpdateWorldBound();
		pCloneObject->SetAppCulled( true );

		m_spRootForCollision->AttachChild( pCloneObject );
		m_spRootForCollision->Update( 0.0f );
		m_spRootForCollision->UpdateWorldBound();	
	}
}

NiNode*	StaticObjectAtChunk::GetCollsionRoot()
{
	return m_spRootForCollision;
}

void	StaticObjectAtChunk::SetRenderingTransform( const NiTransform &transform )
{
	if( m_spRootStaticObject )
	{
		m_spRootStaticObject->SetTranslate( transform.m_Translate );
		m_spRootStaticObject->SetRotate( transform.m_Rotate );
		m_spRootStaticObject->SetScale( transform.m_fScale );

		m_spRootStaticObject->Update( 0.0f );
		m_spRootStaticObject->UpdateWorldBound();

	}

	if( m_spRootForCollision  )
	{
		m_spRootForCollision->SetTranslate( transform.m_Translate );
		m_spRootForCollision->SetRotate( transform.m_Rotate );
		m_spRootForCollision->SetScale( transform.m_fScale );

		m_spRootForCollision->Update( 0.0f );
		m_spRootForCollision->UpdateWorldBound();
	}
}

//		�ø�
void	StaticObjectAtChunk::SetAppCulled( bool bCull )
{
	std::vector< NiAVObject* >::iterator iterObject;

	for( iterObject = m_vecObject.begin(); iterObject != m_vecObject.begin(); ++iterObject )
	{
		(*iterObject)->SetAppCulled( bCull );
	}
	
}

//		���� ���� ������ ����
int	 StaticObjectAtChunk::GetCountObject()
{
	 return 0;
}

void	StaticObjectAtChunk::SetTileIndex( int xIndex, int yIndex )
{
	m_nTileXIndex = xIndex;
	m_nTileYIndex = yIndex;

}

void	StaticObjectAtChunk::GetTileIndex( int *xIndex, int *yIndex )
{
	*xIndex = m_nTileXIndex;
	*yIndex = m_nTileYIndex;
}


BOOL	StaticObjectAtChunk::PickRayAtObjects( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickpos, NiPoint3 *pickNor, NiPick *pPick, float *fDist )
{
	// �ʱ� �ٿ�� �ڽ��� ���ؼ��� ��ŷ ���� �ǽ�	
	BOOL bIntersect = FALSE;
	float fdis = FLT_MAX;

	pPick->RemoveTarget();
	
	pPick->SetTarget( m_spRootForCollision );
	
	

	std::vector< NiAVObject* > vecPickList;
	unsigned int uiCnt = m_spRootForCollision->GetChildCount();

	for(unsigned int i = 0 ; i < uiCnt; i++ )
	{
		pPick->RemoveTarget();
		pPick->SetTarget( m_spRootForCollision->GetAt( i ) );

		if( pPick->PickObjects( pos, dir ) )
		{
			NiPick::Results& pickResults = pPick->GetResults();
			NiPick::Record* pPickRecord = pickResults.GetAt(0);
			vecPickList.push_back( pPickRecord->GetAVObject() );
		}
	}

	pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );

	for( int i = 0; i < (int)vecPickList.size(); i++ )
	{
		pPick->RemoveTarget();
		pPick->SetTarget( vecPickList[ i ] );

		if( pPick->PickObjects( pos, dir ) )
		{
			// get pick results
			NiPick::Results& pickResults = pPick->GetResults();
			NiPick::Record* pPickRecord = pickResults.GetAt(0);

			fdis = pPickRecord->GetDistance();

//			ZFLog::g_sLog->Log( "��ŷ�� ������Ʈ %s , �Ÿ� = %f, ",  pPickRecord->GetAVObject()->GetName(), fdis ); 
			
			// get pick results		
			if( *fDist > fdis )
			{
				const NiFixedString &name = pPickRecord->GetAVObject()->GetName();
				*fDist = fdis;
				*pickpos = pPickRecord->GetIntersection();
				*pickNor = pPickRecord->GetNormal();
				bIntersect = TRUE;
			}
		}
	}

	pPick->RemoveTarget();

	if( bIntersect ) return TRUE;
	
	/*
	// ���� ���Ե� ������Ʈ�� ���ؼ� ��ŷ ���� �ǽ�
	if( pPick->PickObjects( pos, dir ) )
	{
		// get pick results
		
		float fdis = FLT_MAX, fMinDis = FLT_MAX;

		NiPick::Results& pickResults = pPick->GetResults();
	
//		ZFLog::g_sLog->Log( "���� �˻����� Ÿ�� ��ȣ %d ",  m_nTileYIndex * 16 +m_nTileXIndex ); 

		for( unsigned int i = 0; i < pickResults.GetEffectiveSize(); i++ )
		{
			NiPick::Record* pPickRecord = pickResults.GetAt(i);

			vecPickList.push_back( pPickRecord->GetAVObject() );

			const NiFixedString &name = pPickRecord->GetAVObject()->GetName();
//			ZFLog::g_sLog->Log( "�ٿ�� �浹�� ������Ʈ %s ",  name ); 
			
		}

		pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );
		pPick->SetSortType( NiPick::NO_SORT );
		pPick->SetPickType( NiPick::FIND_FIRST );

		for( int i = 0; i < (int)vecPickList.size(); i++ )
		{
			pPick->RemoveTarget();
			pPick->SetTarget( vecPickList[ i ] );
			
			if( pPick->PickObjects( pos, dir ) )
			{
				// get pick results
				NiPick::Results& pickResults = pPick->GetResults();
				NiPick::Record* pPickRecord = pickResults.GetAt(i);

				fdis = pPickRecord->GetDistance();

//				ZFLog::g_sLog->Log( "��ŷ�� ������Ʈ %s , �Ÿ� = %f, ",  pPickRecord->GetAVObject()->GetName(), fdis ); 
				
				// get pick results		
				if( fMinDis > fdis )
				{
					const NiFixedString &name = pPickRecord->GetAVObject()->GetName();
					fMinDis = fdis;
					*pickpos = pPickRecord->GetIntersection();
					*pickNor = pPickRecord->GetNormal();
					bIntersect = TRUE;
				}
			}
		}

		pPick->RemoveTarget();

		if( bIntersect ) 
		{
			*fDist = fMinDis;
			return TRUE;
		}
	}
	*/
	pPick->RemoveTarget();

	return FALSE;
}