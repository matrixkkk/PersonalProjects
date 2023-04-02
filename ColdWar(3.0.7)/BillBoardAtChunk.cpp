#include "main.h"
#include "BillBoardAtChunk.h"
#include "BillBoardManager.h"
#include "GameApp.h"
#include "ZFlog.h"


BillBoardAtChunk::BillBoardAtChunk()
{
//	m_fDistanceApplyAlpha	= DISTANCE_APPLY_ALPHA;
	
	m_pCurrentBillBoard		= NULL;
	m_nTileXIndex = m_nTileYIndex = -1;
	
	m_BillBoardInfo.clear();

	m_spRootBillBoard = NiNew NiNode();	

	NiAlphaProperty *pAlpha = NiNew NiAlphaProperty();
	pAlpha->SetAlphaBlending(true);
	pAlpha->SetAlphaTesting(true);
	pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pAlpha->SetTestRef( 0x08 );
	pAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);

	NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
	pkMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	pkMat->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
	pkMat->SetShineness(10.0f);

	m_spRootBillBoard->AttachProperty( pAlpha );
	m_spRootBillBoard->AttachProperty( pkMat );

	m_spRootBillBoard->Update( 0.0 );
	m_spRootBillBoard->UpdateEffects();
	m_spRootBillBoard->UpdateProperties();

	m_spRootBillBoard->SetAppCulled( false );
	
}


//		Release
BOOL	BillBoardAtChunk::ReleaseObject()
{
	if( m_spRootBillBoard )
	{
		NiBillboardNodePtr spBillBoard;

		for( unsigned int ui = 0; ui < m_spRootBillBoard->GetChildCount(); ui++ )
		{
			if( NiIsKindOf( NiBillboardNode, m_spRootBillBoard->GetAt( ui ) ) )
			{
				NiAlphaPropertyPtr		spAlpha = (NiAlphaProperty*)	m_spRootBillBoard->GetAt( ui )->GetProperty( NiProperty::ALPHA		);
				NiWireframePropertyPtr	spWire	= (NiWireframeProperty*)m_spRootBillBoard->GetAt( ui )->GetProperty( NiProperty::WIREFRAME	);
				NiMaterialPropertyPtr	spMat	= (NiMaterialProperty*)	m_spRootBillBoard->GetAt( ui )->GetProperty( NiProperty::MATERIAL	);

				m_spRootBillBoard->GetAt( ui )->DetachAllProperties();

				if( spAlpha )	spAlpha = 0;
				if( spWire )	spWire = 0;
				if( spMat )		spWire = 0;

				spBillBoard = NiSmartPointerCast( NiBillboardNode, m_spRootBillBoard->DetachChild( m_spRootBillBoard->GetAt( ui ) ) );
				spBillBoard = 0;
			}
		}

		m_spRootBillBoard->Update( 0.0f );
	}

	return TRUE;
}


//------------ 카메라와 빌보드의 위치값을 이용하여 알파값 조정 -----------
void	BillBoardAtChunk::UpdateObject(  NiPoint3* vCameraPos )
{
	if( m_spRootBillBoard->GetAppCulled() ||
		m_spRootBillBoard->GetChildCount() <= 0 )
		return;

	NiPoint3 vCurrentCameraPos = *vCameraPos;
	NiPoint3 vObjectPos, vDiff;
	vCurrentCameraPos.z = 0.0f;

	float fAlpha = 0.0f;

	for( unsigned int ui = 0 ; ui < m_spRootBillBoard->GetChildCount(); ui++ )
	{		
		vObjectPos = m_spRootBillBoard->GetAt( ui )->GetWorldTranslate();//GetTranslate();
		vObjectPos.z = 0.0f;

		vDiff = vObjectPos - vCurrentCameraPos;
		float fDis = NiAbs( vDiff.Length() );

		
		if( fDis > START_DISTANCE_APPLY_ALPHA )
		{
			if( !m_spRootBillBoard->GetAt( ui )->GetAppCulled() )
				m_spRootBillBoard->GetAt( ui )->SetAppCulled(true);
		}
		else
		{
			if( m_spRootBillBoard->GetAt( ui )->GetAppCulled() )
				m_spRootBillBoard->GetAt( ui )->SetAppCulled( false );

			fAlpha = 1.0f - ((fDis - END_DISTANCE_APPLY_ALPHA) / END_DISTANCE_APPLY_ALPHA) ;
			
			if( fAlpha > 1.0f || fDis <= END_DISTANCE_APPLY_ALPHA ) fAlpha = 1.0f;
	
			CGameApp::ChangeNodeAlpha( m_spRootBillBoard->GetAt( ui ), fAlpha );		
		}
	}
}

//		알파값을 조정하여 보이게 될 기준 거리 셋팅
void	BillBoardAtChunk::SetDistanceApplyAlpha( float fDistance )
{
	m_fDistanceApplyAlpha = fDistance;
}

//		빌보드 1개 추가
BOOL	BillBoardAtChunk::AddBillBoard( const  NiPoint3& vPos, NiBillboardNode *pAddObject, int type )
{
	if( m_spRootBillBoard )
	{
	
		NiBillboardNodePtr spBill = pAddObject;
		spBill->SetTranslate( vPos );

		NiWireframeProperty *spWireFrame = NiTCreate<NiWireframeProperty>();
		spWireFrame->SetWireframe( false );
		spBill->AttachProperty( spWireFrame );

		m_spRootBillBoard->AttachChild( spBill );		
		m_spRootBillBoard->Update( 0.0f );
		m_spRootBillBoard->UpdateEffects();
		m_spRootBillBoard->UpdateProperties();
	
		m_BillBoardInfo.insert( std::make_pair(  type, pAddObject ) );

		return TRUE;
	}

	return FALSE;
}

//		빌보드 1개 제거
BOOL	BillBoardAtChunk::DeleteBillBoard( NiBillboardNode *pDeleteObject )
{
	if( m_spRootBillBoard )
	{

		std::multimap< int, NiBillboardNode*, std::greater<int> >::iterator iterBillBoard;

		for( iterBillBoard = m_BillBoardInfo.begin(); iterBillBoard != m_BillBoardInfo.end(); ++iterBillBoard )
		{
			if( pDeleteObject == iterBillBoard->second )
			{
				m_BillBoardInfo.erase( iterBillBoard );
				NiBillboardNodePtr spBillBoard = NiSmartPointerCast( NiBillboardNode, m_spRootBillBoard->DetachChild( pDeleteObject ) );
				spBillBoard = 0;

				m_spRootBillBoard->Update( 0.0f );	
				return TRUE;
			}
		}

		return FALSE;
	}
	
	return FALSE;
}

BOOL	BillBoardAtChunk::DeleteBillBoardType( int nIndex )
{
	if( m_spRootBillBoard )
	{

		NiBillboardNodePtr pBill;
		std::multimap< int, NiBillboardNode*, std::greater<int> >::iterator iterBillBoard;

		for( iterBillBoard = m_BillBoardInfo.begin(); iterBillBoard != m_BillBoardInfo.end(); )
		{
			if( nIndex == iterBillBoard->first )
			{
				pBill = NiSmartPointerCast( NiBillboardNode, m_spRootBillBoard->DetachChild( iterBillBoard->second ) );
				pBill = 0;		

				iterBillBoard = m_BillBoardInfo.erase( iterBillBoard );
			}
			else
			{
				++iterBillBoard;
			}
		}

		m_spRootBillBoard->Update( 0.0f );	

		return TRUE;
	}

	return FALSE;
}

//		컬링
void	BillBoardAtChunk::SetAppCulled( bool bCull )
{
	m_spRootBillBoard->SetAppCulled( bCull );
}

//		크기 조정
void	BillBoardAtChunk::SetCurrentScaling( int nType, float fScale )
{

	std::multimap< int, NiBillboardNode*, std::greater<int> >::iterator iterBill;

	for( iterBill = m_BillBoardInfo.begin(); iterBill != m_BillBoardInfo.end(); ++iterBill )
	{
		if( nType == iterBill->first )
		{
			iterBill->second->SetScale( fScale );
		}
	}

}


NiNode* BillBoardAtChunk::GetRootObject()
{
	return m_spRootBillBoard;
}

//		현재 존의 빌보드 갯수
int		BillBoardAtChunk::GetCountBillBoard()
{
	return static_cast< int >( m_spRootBillBoard->GetChildCount() );
}

BOOL	BillBoardAtChunk::IsSelectedBillBoard()
{
	return m_pCurrentBillBoard != NULL;
}

void	BillBoardAtChunk::SetNullSelectedBillBoard()
{
	m_pCurrentBillBoard = NULL;
}

BOOL	BillBoardAtChunk::DeleteSelectedBillBoard()
{
	if( IsSelectedBillBoard() )
	{
		NiBillboardNodePtr pBill;
		pBill = NiSmartPointerCast( NiBillboardNode, m_spRootBillBoard->DetachChild( m_pCurrentBillBoard ) );
		pBill = 0;

		m_pCurrentBillBoard = NULL;
		m_spRootBillBoard->Update( 0.0f );

		return TRUE;
	}

	return FALSE;

}

void	BillBoardAtChunk::SetTileIndex( int xIndex, int yIndex )
{
	m_nTileXIndex = xIndex;
	m_nTileYIndex = yIndex;
}

void	BillBoardAtChunk::GetTileIndex( int *xIndex, int *yIndex )
{
	*xIndex = m_nTileXIndex;
	*yIndex = m_nTileYIndex;
}

/*
//		파일 저장, 로드
void	BillBoardAtChunk::SaveData( CArchive& ar )
{
	// 존에 존재하는 빌보드 갯수
	ar <<  static_cast<int>( m_spRootBillBoard->GetChildCount() );	

	if( !static_cast<int>( m_spRootBillBoard->GetChildCount() ) )
		return;

	NiPoint3 vBillPos;
	std::multimap< int, NiBillboardNode*, std::greater<int> >::iterator iterBillBoard;

	ar << m_nTileXIndex << m_nTileYIndex;

	for( iterBillBoard = m_BillBoardInfo.begin(); iterBillBoard != m_BillBoardInfo.end(); ++iterBillBoard )
	{
		vBillPos = iterBillBoard->second->GetTranslate();
		ar <<  iterBillBoard->first << vBillPos.x << vBillPos.y << vBillPos.z;
	}
}
*/

void	BillBoardAtChunk::LoadData(  FILE *file, std::map<int, BillBoardObject*>& ObjectInfo )
{
	// 존에 존재하는 빌보드 갯수
	int nBillBoardCount = 0;
	int nType = 0;
	NiPoint3 vPos;

	char buf[20];
	::ZeroMemory( buf, sizeof(buf) );

	fread( buf, 1, sizeof(int) * 3, file );

	memcpy( &nBillBoardCount, buf, sizeof(int) );
	
	memcpy( &m_nTileXIndex, buf + sizeof(int)		, sizeof(int) );
	memcpy( &m_nTileYIndex, buf + sizeof(int) * 2	, sizeof(int) );
	
	for( int i = 0; i < nBillBoardCount; i++ )
	{

		fread( buf, 1, sizeof(int) + sizeof(float)*3, file );

		memcpy( &nType	, buf									, sizeof(int)	);
	
		memcpy( &vPos.x	, buf + sizeof(int)						, sizeof(float) );
		memcpy( &vPos.y	, buf + sizeof(int) + sizeof(float)		, sizeof(float) );
		memcpy( &vPos.z	, buf + sizeof(int) + sizeof(float)*2	, sizeof(float) );

		NiBillboardNode *pBillBoard = ObjectInfo[ nType ]->Clone();

		AddBillBoard( vPos, pBillBoard, nType );

		SetCurrentScaling( nType, ObjectInfo[ nType ]->GetScale() );
	}		
}


NiBillboardNode* BillBoardAtChunk::GetSelectedBillBoard()
{
	return m_pCurrentBillBoard;
}


//		빌보드 픽킹
BOOL	BillBoardAtChunk::PickBillBoard( const  NiPoint3& pos, const  NiPoint3& direction, NiPick *pPick, float *fDis )
{

	if( !pPick || m_spRootBillBoard->GetChildCount() < 1 )
		return FALSE;

	NiWireframeProperty *pWire = NULL;

	if( NiIsKindOf( NiAVObject, m_spRootBillBoard ) )
	{
		pPick->SetTarget( (NiAVObject*)m_spRootBillBoard );
		pPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );
		pPick->SetPickType( NiPick::FIND_ALL );
		pPick->SetSortType( NiPick::SORT );

		if( pPick->PickObjects( pos, direction ) )
		{
			// get pick results
			NiPick::Results& pickResults = pPick->GetResults();
			NiPick::Record* pPickRecord = pickResults.GetAt(0);
			
			NiAVObject *pObject = pPickRecord->GetAVObject();
			NiNode		*pNode = pPickRecord->GetParent();		

			*fDis = pPickRecord->GetDistance();

			if( NiIsKindOf( NiBillboardNode, pNode ) )
			{
				if( BillBoardManager::GetManager()->GetSelectedBillBoardNode() )
				{
					pWire = (NiWireframeProperty*)(BillBoardManager::GetManager()->GetSelectedBillBoardNode())->GetProperty( NiWireframeProperty::GetType() );

					if( pWire ) pWire->SetWireframe( false );
				}

				pWire = (NiWireframeProperty*)((NiBillboardNode*)pNode)->GetProperty( NiWireframeProperty::GetType() );

				m_pCurrentBillBoard = (NiBillboardNode*)pNode;

				BillBoardManager::GetManager()->SetSlectBillBoardNode(  (NiBillboardNode*)pNode );

				if( pWire ) pWire->SetWireframe( true );
			}
		
			pPick->SetPickType( NiPick::FIND_FIRST );
			pPick->SetSortType( NiPick::NO_SORT );
			pPick->RemoveTarget();
			return TRUE;
		}

		pPick->SetPickType( NiPick::FIND_FIRST );
		pPick->SetSortType( NiPick::NO_SORT );
		pPick->RemoveTarget();
	}

	return FALSE;
}
