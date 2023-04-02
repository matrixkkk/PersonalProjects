#include "CTerritory.h"
#include "GameApp.h"
#include "WorldManager.h"
#include "CharacterManager.h"
#include "NetWorkManager.h"
//#include "ZFLog.h"

CTerritory* CTerritory::m_pTerritory = NULL;

CTerritory::CTerritory()
{
	m_pCharacter = NULL;
	::ZeroMemory( m_nStateTerritory, sizeof(m_nStateTerritory) );
	

	for( int i = nsPlayerInfo::TERRITORY_1; i <= nsPlayerInfo::TERRITORY_3; i++ )
		m_nStateTerritory[ i ] = NEUTRAL;

	m_fBarWidth		= 0.08f;
	m_fBarHeight	= 0.02f;
	m_fXSpacing		= 0.015f;
	m_fStartX		= 0.35f;
	m_fStartY		= 0.02f;
}


CTerritory::~CTerritory()
{
	for( int i = nsPlayerInfo::TERRITORY_1; i <= nsPlayerInfo::TERRITORY_3; i++ )
	{
		NiNodePtr spTmp;
		if( m_mapCollision.GetAt( i, spTmp ) ) spTmp = 0;
	}
	m_mapCollision.RemoveAll();

	for( int i = USA_TERR; i <= NEUTRAL; i++ )
	{
		NiNodePtr spTmp;
		if( m_mapTerritorys.GetAt( i, spTmp ) ) spTmp = 0;		
	}
	m_mapTerritorys.RemoveAll();

	if( m_mapTerritoryPos.empty() )
		m_mapTerritoryPos.clear();

	for( int i = 0; i < 3; i++ )
	{
		m_pKTerritoryState[i]->DetachAllProperties();
		m_pKTerritoryState[i] = 0;
	}

	m_spTerritoryStateBar = 0;
	m_spUSA = 0;
	m_spEU = 0;

	if( m_spRoot )
	{
		m_spRoot = 0;
	}

	::ZeroMemory( m_nStateTerritory, sizeof(m_nStateTerritory) );

}


CTerritory* CTerritory::GetInstance()
{
	if( !m_pTerritory  )
	{
		m_pTerritory = NiNew CTerritory();		

		return m_pTerritory;
	}

	return m_pTerritory;
}


void CTerritory::Destroy()
{
	if( m_pTerritory )
	{
		NiDelete m_pTerritory;
		m_pTerritory = NULL;
	}
}

bool	CTerritory::Initialize()
{

	m_spRoot = NiNew NiNode();


	WorldManager::GetWorldManager()->AttachLight1AffectedNode( m_spRoot );
	WorldManager::GetWorldManager()->AttachLight2AffectedNode( m_spRoot );

	//===========================================================================
	//		점령지 로드 및 생성
	//===========================================================================
	AddTerritory( USA_TERR,	"flag_USA.nif"		);
	AddTerritory( EU_TERR,	"flag_EU.nif"		);
	AddTerritory( NEUTRAL,	"flag_NETRAL.nif"	);
	//===========================================================================

	//===========================================================================
	//		점령지 위치 지정
	//===========================================================================
	m_mapTerritoryPos.insert( std::make_pair( nsPlayerInfo::TERRITORY_1, NiPoint3( 65.45f,  29.9f, -0.0f ) ) );
	m_mapTerritoryPos.insert( std::make_pair( nsPlayerInfo::TERRITORY_2, NiPoint3(-46.45f, 40.96f, 3.98f ) ) );
	m_mapTerritoryPos.insert( std::make_pair( nsPlayerInfo::TERRITORY_3, NiPoint3(  9.62f, -62.0f, 4.28f ) ) );
	//===========================================================================

	//===========================================================================
	//		충돌용
	//===========================================================================
	NiNodePtr spTempCollision;
	m_mapTerritorys.GetAt( USA_TERR, spTempCollision );

	for( int i = nsPlayerInfo::TERRITORY_1; i <= nsPlayerInfo::TERRITORY_3; i++ )
	{
		NiCloningProcess kProcess;
		kProcess.m_eCopyType = NiObjectNET::COPY_EXACT;
		NiNodePtr pClone = (NiNode*)spTempCollision->Clone(kProcess);

		pClone->SetTranslate( m_mapTerritoryPos[ i ] );
		pClone->Update( 0.0f );
		pClone->UpdateWorldBound();		

		m_mapCollision.SetAt( i, pClone );

	}
	
	//m_pCharacter		= WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();

	//===========================================================================
	//	초기 중립 깃발로 셋팅	
	//===========================================================================
	for( int iTerritory = nsPlayerInfo::TERRITORY_1; iTerritory <= nsPlayerInfo::TERRITORY_3; iTerritory++ )
	{
		NiNodePtr spTerritory;
		m_mapTerritorys.GetAt( NEUTRAL, spTerritory );
		
		NiCloningProcess kProcess;
		kProcess.m_eCopyType = NiObjectNET::COPY_EXACT;

		NiNodePtr pClone = (NiNode*)spTerritory->Clone(kProcess);
		pClone->SetTranslate( m_mapTerritoryPos[ iTerritory ] );
		pClone->Update( 0.0f );
		pClone->UpdateProperties();
		pClone->UpdateWorldBound(); 

		m_spRoot->SetAt( iTerritory, pClone );
		m_spRoot->Update( 0.0f );
		m_spRoot->UpdateWorldBound();
		m_spRoot->UpdateProperties();
		m_spRoot->UpdateEffects();
	}
	
	m_spRoot->Update( 0.0f );
	m_spRoot->UpdateWorldBound();
	m_spRoot->UpdateProperties();
	m_spRoot->UpdateEffects();


	//==============================================================================
	//		UI 생성
	//==============================================================================

	m_fBarWidth		= 0.08f;
	m_fBarHeight	= 0.02f;
	m_fXSpacing		= 0.015f;
	m_fStartX		= 0.35f;
	m_fStartY		= 0.02f;

	m_spTerritoryStateBar = NiNew NiScreenElements( NiNew NiScreenElementsData(false,false,1) ); 
	m_spTerritoryStateBar->Insert(4); 
	m_spTerritoryStateBar->SetRectangle( 0, m_fStartX, m_fStartY, 0.3f, 0.05f ); 
	m_spTerritoryStateBar->SetTextures( 0, 0,  0.0f, 0.0f, 1.0f, 1.0f ); 
	
	CGameApp::SetMediaPath("Data/Texture/");
	NiTexturingProperty* pkTex = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("territory_statebar.tga"));

	// 텍스쳐의 어드레스 모드
	pkTex->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	pkTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	m_spTerritoryStateBar->AttachProperty(pkTex);

	// 알파
	NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty;
	pkAlpha->SetAlphaBlending(true);
	pkAlpha->SetAlphaTesting(true);
	pkAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pkAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pkAlpha->SetTestRef(0x00);
	pkAlpha->SetTestMode(NiAlphaProperty::TEST_GREATER);

	m_spTerritoryStateBar->AttachProperty( pkAlpha );

	m_spTerritoryStateBar->Update(0.0f);
	m_spTerritoryStateBar->UpdateProperties();
	m_spTerritoryStateBar->UpdateEffects();

	//==================================================================================

	//==================================================================================
	m_spUSA = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("USA_bar.tga") );
	m_spEU  = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("EU_bar.tga") );

	// 텍스쳐의 어드레스 모드
	m_spUSA->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	m_spUSA->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	m_spEU->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	m_spEU->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);

	for( int i = 0; i< 3; i++ )
	{
		m_pKTerritoryState[i] = NiNew NiScreenElements( NiNew NiScreenElementsData( false,false,1) ); 
		m_pKTerritoryState[i]->Insert(4);
		m_pKTerritoryState[i]->SetTextures( 0, 0,  0.0f, 0.0f, 1.0f, 1.0f ); 	
		m_pKTerritoryState[i]->SetRectangle(0,m_fStartX + m_fXSpacing*(i+1) + m_fBarWidth*i, m_fStartY + 0.012f, m_fBarWidth, m_fBarHeight );
		m_pKTerritoryState[i]->AttachProperty( pkAlpha );

		m_pKTerritoryState[i]->Update(0.0f);
		m_pKTerritoryState[i]->UpdateBound();
		m_pKTerritoryState[i]->UpdateProperties();
		m_pKTerritoryState[i]->UpdateEffects();
	}

	return true;
}

void CTerritory::RenderScreenItem()
{
	NiRenderer *pRenderer = NiRenderer::GetRenderer();

	if( !pRenderer ) return;
	
	m_spTerritoryStateBar->RenderImmediate( pRenderer );

	for( int i = 0 ; i < 3; i++ )
		m_pKTerritoryState[ i ]->RenderImmediate( pRenderer );
}

NiNode* CTerritory::GetObject()
{ 
	return m_spRoot; 
}

void CTerritory::Update( float fElapsedTime )
{
	m_pCharacter		= WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();

 	NiNodePtr spTerritory;
	const NiBound& kCharBound = m_pCharacter->GetCharRoot()->GetWorldBound();
	
	for( int iTerritory = nsPlayerInfo::TERRITORY_1; iTerritory <= nsPlayerInfo::TERRITORY_3; iTerritory++ )
	{		
		m_mapCollision.GetAt( iTerritory, spTerritory );

		const NiBound& kTerritoryBound = spTerritory->GetWorldBound();
		
		if( NiBound::TestIntersect( 0.0f, kCharBound, NiPoint3(0,0,0), kTerritoryBound, NiPoint3(0,0,0) ) )
		{
			NetworkManager::GetInstance()->ProcessTerritory( m_nStateTerritory[iTerritory], iTerritory );
//			ZFLog::g_sLog->Log( " %d지역 충돌 점령 요청", iTerritory+1 );
			break; 

		}
	}
}

void CTerritory::UpdateTerritory( BYTE nCamp, BYTE nNewTerritory )
{
	PossessionTerritory( nCamp, nNewTerritory );	
}

void CTerritory::RobeTerritory( BYTE nPoseCamp,  BYTE nRobeTerritory )
{
	m_nStateTerritory[ nRobeTerritory ]	= nPoseCamp;

	// 빼앗긴 점령지의 깃발 정보를 제거
	NiAVObjectPtr spPrevious = m_spRoot->DetachChildAt( nRobeTerritory ); 
	spPrevious= 0;

	// 상대편 깃발 정보 생성
	NiNode *spTerritory = ClonTerritory( nPoseCamp );	

	// 뺏앗긴 점령지의 위치값으로 셋팅
	spTerritory->SetTranslate( m_mapTerritoryPos[ nRobeTerritory ] );
	spTerritory->Update(0.0f);
	spTerritory->UpdateProperties();
	spTerritory->UpdateWorldBound();

	// 빼앗긴 점령지의 적군 깃발 셋팅
	m_spRoot->SetAt( nRobeTerritory, spTerritory );
	m_spRoot->Update(0.0f);
	m_spRoot->UpdateProperties();
	m_spRoot->UpdateWorldBound();
	m_spRoot->UpdateEffects();
}

void CTerritory::PossessionTerritory( BYTE nCamp, BYTE nTerritory )
{
	//========== 사운드 데이터 만듬 ============
	BYTE dwSelfID = (BYTE)NetworkManager::GetInstance()->GetID();
	BYTE  currentCamp;		//현재 유저의 진영

	//현재 유저의 진영을 가져온다.
	PlayersMap conMap = NetworkManager::GetInstance()->LockPlayers();
	currentCamp = conMap[ dwSelfID ].userInfo.camp;
	NetworkManager::GetInstance()->UnLockPlayers();

	//점령 사운드 설정
	DWORD eSound;
	if(currentCamp == nCamp)
		eSound = eSoundCode::SOUND_WAV_CAPTUREFLAG;
	else
		eSound = eSoundCode::SOUND_WAV_LOSTFLAG;
	FMOD::Channel* pChannel;
	CSoundManager::GetInstance()->Play2D(eSound,pChannel);

	//==========================================

	m_nStateTerritory[ nTerritory ] = nCamp;

	// 점령한 팀의 깃발 생성
	NiNode *spTerritory = ClonTerritory( nCamp );

	// 점령지의 위치로 Transform
	spTerritory->SetTranslate( m_mapTerritoryPos[ nTerritory ] );
	spTerritory->Update(0.0f);
	spTerritory->UpdateProperties();
	spTerritory->UpdateWorldBound();
	
	// 이전 깃발 정보를 제거
	NiAVObjectPtr spPrevious =m_spRoot->DetachChildAt( nTerritory ); 
	spPrevious = 0 ;

	// 점령 팀의 깃발로 셋팅
	m_spRoot->SetAt( nTerritory, spTerritory );

	m_spRoot->Update(0.0f);
	m_spRoot->UpdateProperties();
	m_spRoot->UpdateWorldBound();
	m_spRoot->UpdateEffects();

	//==========================================================================
	//				UI	변경
	//==========================================================================

	if( m_pKTerritoryState[ nTerritory ]->GetProperty( NiProperty::TEXTURING ) )
		m_pKTerritoryState[ nTerritory ]->RemoveProperty( NiProperty::TEXTURING );

	if( nCamp == nsPlayerInfo::USA )
		m_pKTerritoryState[ nTerritory ]->AttachProperty( m_spUSA );
	else
		m_pKTerritoryState[ nTerritory ]->AttachProperty( m_spEU );

	m_pKTerritoryState[ nTerritory ]->UpdateProperties();

	//==========================================================================
}


NiNode*  CTerritory::ClonTerritory( BYTE nCamp )
{
	NiCloningProcess kCloning;
	kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
	NiNodePtr spTerritory;
	

	if(	!m_mapTerritorys.GetAt( nCamp - nsPlayerInfo::USA, spTerritory ) )
	{
		return NULL;
	}

	NiNode *pClone = (NiNode*)spTerritory->Clone(kCloning);

	return pClone;
}


void CTerritory::AddTerritory( int nCamp, char *pName )
{
	NiStream kStream;
	CGameApp::SetMediaPath( "./Data/Object/" );

	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename(pName) );

	if (!bSuccess)
	{
		NiMessageBox("StaticObjectDecalFactory::AddDecalShapeAtMap() Nif file could not be loaded!", "NIF Error");
		return;
	}

	NiNodePtr spTerritory =  (NiNode*)((NiNode*)kStream.GetObjectAt(0))->GetObjectByName("root");

	m_mapTerritorys.SetAt( nCamp, spTerritory );
	
}

void CTerritory::Render(NiCamera* pkCamera)
{
	NiRenderer *pRenderer = NiRenderer::GetRenderer();

	WorldManager::GetWorldManager()->PartialRender( pkCamera, m_spRoot);
}


