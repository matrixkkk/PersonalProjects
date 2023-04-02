#include "main.h"
#include "BillBoardManager.h"



BillBoardManager *BillBoardManager::m_pBillBoardManager	= NULL;

int BillBoardInfo::m_nCountBillBoardOBject = 0;

BillBoardManager::BillBoardManager()
{
	m_pCamera = NULL;
	m_pPick	  = NULL;
	m_pSelectedBillBoardChunk = NULL;

	m_pCurrnetBillBoardInfo = NULL;
	m_pSelectedBillBoardNode =NULL;

	m_pBillBoardManager = this;
	m_listBillBoardChunk.clear();
	m_listBillBoardObject.clear();

	m_spBillBoardRoot = NiTCreate<NiNode>();
	m_spBillBoardRoot->SetName("BillBoardManager");
	m_spBillBoardRoot->Update(0.0f);
	m_spBillBoardRoot->UpdateEffects();
	m_spBillBoardRoot->UpdateProperties();

	CreatePick();
}

BillBoardManager::~BillBoardManager()
{
	ReleaseObject();

	m_pCamera = NULL;
	m_pPick	  = NULL;

	m_pSelectedBillBoardChunk	= NULL;
	m_pSelectedBillBoardNode	= NULL;
	m_pCurrnetBillBoardInfo		= NULL;
}

BOOL	BillBoardManager::IsSelectedBillBoardChunk()
{
	return m_pSelectedBillBoardChunk != NULL;
}

void BillBoardManager::SetNullBillBoardChunk()
{
	m_pSelectedBillBoardChunk  = NULL;
}

void	BillBoardManager::SetSlectBillBoardNode( NiBillboardNode *pNode )
{
	m_pSelectedBillBoardNode = pNode;
}

BOOL	BillBoardManager::IsSelectedBillBoardNode()
{
	return m_pSelectedBillBoardNode != NULL;
}

void	BillBoardManager::SetNullBillBoardNode()
{
	if( m_pSelectedBillBoardNode )
	{

		NiWireframeProperty *pWire = (NiWireframeProperty*)m_pSelectedBillBoardNode->GetProperty( NiProperty::WIREFRAME );

		if( pWire )
			pWire->SetWireframe( false );

		m_pSelectedBillBoardNode = NULL;
	}
	
}

BOOL	BillBoardManager::PickBillBoard( const NiPoint3& pos, const NiPoint3& dir )
{
	if( m_listBillBoardChunk.empty() )
		return FALSE;
	
	float fMinDis = FLT_MAX;
	float fdis = FLT_MAX;
	BOOL bPick = FALSE;

	std::list<BillBoardAtChunk*>::iterator iterBill;
	BillBoardAtChunk* pSelectedBillBoard = NULL;
	
	for( iterBill = m_listBillBoardChunk.begin(); iterBill != m_listBillBoardChunk.end(); ++iterBill )
	{
		if( (*iterBill)->PickBillBoard( pos, dir, m_pPick, &fdis ) )
		{
			if( fMinDis > fdis )
			{
				fMinDis = fdis;
				pSelectedBillBoard = (*iterBill);
				bPick = TRUE;
			}
		}
	}

	if( bPick )
	{
		NiWireframeProperty *pWire  = NULL;

		if( pSelectedBillBoard->IsSelectedBillBoard() )
		{
			// 이전에 선택댄 빌버드 버이게 하기
			if( m_pSelectedBillBoardNode )
			{
				pWire = (NiWireframeProperty*)m_pSelectedBillBoardNode->GetProperty( NiWireframeProperty::GetType() );
				pWire->SetWireframe( false );
			}

			// 최근 선택댄  빌버드 와이어			
			m_pSelectedBillBoardNode  = pSelectedBillBoard->GetSelectedBillBoard();

			pWire = (NiWireframeProperty*)m_pSelectedBillBoardNode->GetProperty( NiWireframeProperty::GetType() );

			if( pWire ) pWire->SetWireframe( true );

			m_pSelectedBillBoardChunk = pSelectedBillBoard;

			return TRUE;
		}
	}

	return FALSE; 
}

NiBillboardNode* BillBoardManager::GetSelectedBillBoardNode()
{
	return m_pSelectedBillBoardNode;
}

BOOL	BillBoardManager::DeleteSelectedBillBoard()
{
	if( m_pSelectedBillBoardNode )
	{
		NiNode *pNode = m_pSelectedBillBoardNode->GetParent();

		if( pNode )
		{
			std::list< BillBoardAtChunk* >::iterator iterChunk;
			for( iterChunk = m_listBillBoardChunk.begin(); iterChunk != m_listBillBoardChunk.end(); ++iterChunk )
			{
				if( (*iterChunk)->DeleteBillBoard( m_pSelectedBillBoardNode ) )
				{
					m_pSelectedBillBoardNode = NULL;
					m_pSelectedBillBoardChunk = NULL;
					return TRUE;
				}
			}
		}
	}
	return FALSE;

}

BillBoardManager* BillBoardManager::GetManager()
{
	return m_pBillBoardManager;
}

BillBoardManager* BillBoardManager::CreateManager()
{
	if( !m_pBillBoardManager )
	{
		m_pBillBoardManager = NiNew BillBoardManager();		
	}

	return m_pBillBoardManager;
}

BOOL	BillBoardManager::IsManager()
{
	return m_pBillBoardManager != NULL;
}

void	BillBoardManager::SetSlectBillBoardChunk( BillBoardAtChunk* pBill )
{
	m_pSelectedBillBoardChunk = pBill;
}

BillBoardAtChunk*	BillBoardManager::GetSelectedBillBoardChunk()
{
	return m_pSelectedBillBoardChunk;
}

void BillBoardManager::ReleaseManager()
{	
	if( m_pBillBoardManager )
	{
		NiDelete m_pBillBoardManager;
		m_pBillBoardManager = NULL;
		
	}
}

//		빌보드 오브젝트 종류 변경
BOOL	BillBoardManager::SetCurrentBillBoardObject( const NiFixedString& fileName )
{
	std::list< BillBoardInfo >::iterator iterBill;
	
	for( iterBill = m_listBillBoardObject.begin(); iterBill != m_listBillBoardObject.end(); ++iterBill )
	{
		if( iterBill->m_pBillBoardObject->GetFileName() == fileName )
		{
			m_pCurrnetBillBoardInfo = &(*iterBill);
			return TRUE;
		}
	}

	return FALSE;

}

//		현재 선택되어진 빌보드 오브젝트
BillBoardObject* BillBoardManager::GetCurrentBillBoardObject()
{
	if( m_pCurrnetBillBoardInfo != NULL )
		return m_pCurrnetBillBoardInfo->m_pBillBoardObject;

	return NULL;
}

int	BillBoardManager::GetCurrentBillBoardType()
{
	if( m_pCurrnetBillBoardInfo != NULL )
		return m_pCurrnetBillBoardInfo->m_nIndex;

	return -1;
}


// 픽킹 NiPick 생성
void BillBoardManager::CreatePick()
{
	m_pPick = NiNew NiPick;
    m_pPick->SetPickType(NiPick::FIND_FIRST);
	m_pPick->SetSortType(NiPick::NO_SORT);
    m_pPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
    m_pPick->SetFrontOnly(false);
    m_pPick->SetReturnTexture(true);
    m_pPick->SetReturnNormal(true);
    m_pPick->SetReturnColor(true);
//    m_pPick->SetTarget(m_spScene);

}


NiNode* BillBoardManager::GetObject()
{
	return m_spBillBoardRoot;
}

void	BillBoardManager::SetCamera( NiCamera *pCamera )
{
	m_pCamera = pCamera;
}

BOOL	BillBoardManager::UpdateObject( float fElapsedTime  )
{

	if( m_listBillBoardChunk.empty() )
		return FALSE;

	std::list< BillBoardAtChunk* >::iterator iterBill;

	NiPoint3 vCameraPos = m_pCamera->GetWorldTranslate();

	std::for_each( m_listBillBoardChunk.begin(), m_listBillBoardChunk.end(), std::bind2nd( std::mem_fun( &BillBoardAtChunk::UpdateObject ), &vCameraPos ) );

	return TRUE;
}

BOOL	BillBoardManager::ReleaseObject()
{

	if( !m_listBillBoardObject.empty() )
	{
		std::list< BillBoardInfo >::iterator iterBill;

		for( iterBill = m_listBillBoardObject.begin(); iterBill != m_listBillBoardObject.end(); ++iterBill )
		{
			if( iterBill->m_pBillBoardObject->ReleaseObject() )
				NiDelete iterBill->m_pBillBoardObject;
		}

		m_listBillBoardObject.clear();

	}
	if( !m_listBillBoardChunk.empty() )
	{
		std::list< BillBoardAtChunk* >::iterator iterBill;

		for( iterBill = m_listBillBoardChunk.begin(); iterBill != m_listBillBoardChunk.end(); ++iterBill )
		{
			if( (*iterBill)->ReleaseObject() )
				NiDelete (*iterBill);
		}

		m_listBillBoardChunk.clear();
	}

	if( m_spBillBoardRoot ) m_spBillBoardRoot = 0;

	BillBoardInfo::m_nCountBillBoardOBject = 0;
		
	return TRUE;
}

//		빌보드 오브젝트 추가
BOOL	BillBoardManager::AddBillBoard( NiFixedString& fileName )
{
	BillBoardInfo stBillBoardInfo;
	stBillBoardInfo.m_pBillBoardObject = NiNew BillBoardObject();
	
	if( stBillBoardInfo.m_pBillBoardObject->GenerateBillBoard( fileName ) )
	{
		stBillBoardInfo.m_nIndex = BillBoardInfo::m_nCountBillBoardOBject++;		

		m_listBillBoardObject.push_back( stBillBoardInfo );		

		return TRUE;
	}
	else
	{
		if( stBillBoardInfo.m_pBillBoardObject->ReleaseObject() )
			NiDelete stBillBoardInfo.m_pBillBoardObject;

		NiMessageBox( "BillBoardManager::AddBillBoard() Failed", "Failed" );
	}

	return FALSE;
}

//		빌보드 오브젝트 제거
BOOL	BillBoardManager::DeleteBillBoard( const NiFixedString& fileName )
{
	if( m_listBillBoardObject.empty() )
		return FALSE;

	std::list< BillBoardInfo >::iterator iterBill; 

	for( iterBill = m_listBillBoardObject.begin(); iterBill != m_listBillBoardObject.end(); ++iterBill )
	{
		if( iterBill->m_pBillBoardObject->GetFileName() == fileName )
		{
			if( iterBill->m_pBillBoardObject->ReleaseObject() )
			{
				NiDelete iterBill->m_pBillBoardObject;
				iterBill->m_pBillBoardObject = NULL;

				if( !m_listBillBoardChunk.empty() )
				{
					std::for_each( m_listBillBoardChunk.begin(), m_listBillBoardChunk.end(), 
						std::bind2nd( std::mem_fun( &BillBoardAtChunk::DeleteBillBoardType ), iterBill->m_nIndex ) );
				}

				m_listBillBoardObject.erase( iterBill );

//				--BillBoardInfo::m_nCountBillBoardOBject;

				return TRUE;
			}
		}
	}
	
	return FALSE;
}


BillBoardAtChunk* BillBoardManager::GenerateBillBoardAtChunk()
{
	BillBoardAtChunk *pBillBoardChunk = NiNew BillBoardAtChunk();

	m_spBillBoardRoot->AttachChild( pBillBoardChunk->GetRootObject() );	
	
	m_spBillBoardRoot->Update(0.0f);
	m_spBillBoardRoot->UpdateProperties();

	m_listBillBoardChunk.push_back( pBillBoardChunk );

	return pBillBoardChunk;
}

//		현재 선택되어진 빌보드 크기 조정
void	BillBoardManager::SetCurrentScaling( float fScale )
{
	
	if( m_listBillBoardChunk.empty() || !m_pCurrnetBillBoardInfo)
		return;

//	m_pCurrnetBillBoardInfo->m_fScaling = fScale;
	m_pCurrnetBillBoardInfo->m_pBillBoardObject->SetScale( fScale );

	std::list< BillBoardAtChunk* >::iterator iterBill;

	for( iterBill = m_listBillBoardChunk.begin(); iterBill != m_listBillBoardChunk.end(); ++iterBill )
	{
		(*iterBill)->SetCurrentScaling( m_pCurrnetBillBoardInfo->m_nIndex, fScale );
	}
}

//		알파값 적용 거리 변경 셋팅
void	BillBoardManager::SetDistanceApplyAlpha( float fAlpha )
{
	m_fAlphaDistance = fAlpha;

	if( m_listBillBoardChunk.empty() )
		return;	

	std::for_each( m_listBillBoardChunk.begin(), m_listBillBoardChunk.end(),
		std::bind2nd( std::mem_fun( &BillBoardAtChunk::SetDistanceApplyAlpha ), fAlpha ) );

}

float	BillBoardManager::GetCurrentScaling()
{
	if( !m_pCurrnetBillBoardInfo )
		return 0.0f;

//	return m_pCurrnetBillBoardInfo->m_fScaling;
	return m_pCurrnetBillBoardInfo->m_pBillBoardObject->GetScale();
}
/*
	
void	BillBoardManager::SaveHeader( CArchive& ar )
{

}
	

void	BillBoardManager::SaveData( CArchive& ar )
{	
	// 빌보드 종류
	ar << static_cast<int>( m_listBillBoardObject.size() );

	if( m_listBillBoardObject.empty() )
		return;

	// 청크 알파
	ar << m_fAlphaDistance;
	
	std::list< BillBoardInfo >::iterator iterInfo;
	for(  iterInfo = m_listBillBoardObject.begin(); iterInfo != m_listBillBoardObject.end(); ++iterInfo )
	{
		ar << iterInfo->m_nIndex;
		ar << iterInfo->m_pBillBoardObject->GetScale();
	}

	// 청크 갯수
	ar << static_cast<int>( m_listBillBoardChunk.size() );

	if( m_listBillBoardChunk.empty() )
		return;

	std::list< BillBoardAtChunk* >::iterator iterBill;

	for( iterBill = m_listBillBoardChunk.begin(); iterBill != m_listBillBoardChunk.end(); ++iterBill )
	{
		(*iterBill)->SaveData( ar );
	}
	
}


void	BillBoardManager::SaveNameData( std::ofstream& fwrite )
{
	if( m_listBillBoardObject.empty() )
		return;

	std::list< BillBoardInfo >::iterator iterBillInfo;

	for( iterBillInfo = m_listBillBoardObject.begin(); iterBillInfo != m_listBillBoardObject.end(); ++iterBillInfo )
	{
		fwrite << CW2A(iterBillInfo->m_pBillBoardObject->GetFileName()) << "\n";
	}
}

*/

void	BillBoardManager::LoadNameData( FILE *file,  std::map<int, BillBoardObject*>& ObjectInfo, std::list<NiFixedString>& listFileName )
{

	char buf[20];
	::ZeroMemory( buf, sizeof(buf) );

	int nCntMeshObjects = 0, nCntMesh = 0;

	fread( buf, 1, sizeof(int) + sizeof(float), file );

	// 오브젝트 종류 갯수
	memcpy( &nCntMeshObjects	, buf				, sizeof(int)	);

	if( nCntMeshObjects <= 0 )
		return;

	// 알파값
	memcpy( &m_fAlphaDistance	, buf + sizeof(int)	, sizeof(float) );

	if( !nCntMeshObjects  )
		return;

	for( int i = 0; i < nCntMeshObjects; i++ )
	{
		NiFixedString &strFileName = listFileName.front();
//		NiFixedString fileName = strFileName.c_str();
		AddBillBoard( strFileName );
		listFileName.pop_front();
	}

	std::list< BillBoardInfo >::iterator iterInfo;
	int count = 0;
	float fAlpha;
	for(  iterInfo = m_listBillBoardObject.begin(); iterInfo != m_listBillBoardObject.end(); ++iterInfo )
	{
		fread( buf, 1, (sizeof(int) + sizeof(float)), file );

		memcpy( &iterInfo->m_nIndex	, buf				, sizeof(int)	);
		memcpy( &fAlpha	, buf + sizeof(int)				, sizeof(float)	);
		
		iterInfo->m_pBillBoardObject->SetScale( fAlpha );

		ObjectInfo.insert( std::make_pair( iterInfo->m_nIndex, iterInfo->m_pBillBoardObject ) );
	}

	BillBoardInfo::m_nCountBillBoardOBject = m_listBillBoardObject.back().m_nIndex;
	BillBoardInfo::m_nCountBillBoardOBject++;

	m_pCurrnetBillBoardInfo			= NULL;
	m_pSelectedBillBoardChunk		= NULL;
	m_pSelectedBillBoardNode		= NULL;

}


void	BillBoardManager::LoadData( FILE *file, std::map<int, BillBoardObject*>& ObjectInfo, Terrain *pTerrain )
{
	if( m_listBillBoardObject.empty() )
		return;

	// 청크 갯수
	int nChunkCount;

	BillBoardAtChunk *pChunk = NULL;
	int xIndex, yIndex;
	
	char buf[5];
	::ZeroMemory( buf, sizeof(buf) );

	fread( buf, 1, sizeof(int), file );
	memcpy( &nChunkCount, buf, sizeof(int) );

	for( int i =0; i < nChunkCount; i++ )
	{
		pChunk = GenerateBillBoardAtChunk();

		if(	pChunk )
		{
			pChunk->LoadData( file, ObjectInfo );
			pChunk->GetTileIndex( &xIndex, &yIndex );
			pTerrain->SetBillBoardChunk( xIndex, yIndex, pChunk );
		}
	}
}


BillBoardObject*	BillBoardManager::GetBillBoardObject( int nType )
{
	std::list< BillBoardInfo >::iterator iterBill;
	
	for( iterBill = m_listBillBoardObject.begin(); iterBill != m_listBillBoardObject.end(); ++iterBill )
	{
		if( iterBill->m_nIndex == nType )
			return iterBill->m_pBillBoardObject;
	}

	return NULL;
}








