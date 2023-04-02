#include "NetWorkManager.h"
#include "GameApp.h"
#include "WorldManager.h"
#include "CGamePlayState.h"

#ifdef ZFLOG
////#include "ZFLog.h"
#endif

NetworkManager*		NetworkManager::m_pkNetworkManager = NULL;		
CGameServer*		NetworkManager::m_pGameServer = NULL;
CGameClient*		NetworkManager::m_pGameClient = NULL;
HANDLE				NetworkManager::m_StartEvent = NULL;

std::string			NetworkManager::m_IP = std::string("");

bool				NetworkManager::m_bCaptain = false;	



NetworkManager::NetworkManager()
{
	m_pGameServer = NULL;
	m_pGameClient = NULL;

	m_nUniqID = 0;				
	m_fLastSendTime = 0.0f;				//마지막으로 메시지를 전송한 시간
	m_fDelay = 0.0f;					//네트워크 딜레이 시간
	m_bCaptain = false;

	InitializeCriticalSection(&m_csPlayers);
	InitializeCriticalSection(&m_csFireInfo);
	InitializeCriticalSection(&m_csFireResult);
}

NetworkManager::~NetworkManager()
{
	if(!m_vecUserInfo.empty())
		m_vecUserInfo.clear();

	DeleteCriticalSection(&m_csPlayers);
	DeleteCriticalSection(&m_csFireInfo);
	DeleteCriticalSection(&m_csFireResult);
}


//네트워크 매니저 생성
bool NetworkManager::Create(bool server)
{
	NetworkManager*	pkNetwork = NiNew NetworkManager;
	assert(pkNetwork);

	m_pkNetworkManager = pkNetwork;

	m_bCaptain = server;

	m_StartEvent = CreateEvent(NULL,TRUE, FALSE,NULL);

	//네트워크 객체 할당
	if(server)
		m_pGameServer = NiNew CGameServer;
	else
		m_pGameClient = NiNew CGameClient;

	return true;
}

bool NetworkManager::Destroy()
{
	GetInstance()->End();

	ResetEvent(m_StartEvent);

	if(m_pkNetworkManager)
		NiDelete m_pkNetworkManager;

	m_pkNetworkManager = NULL;

	return true;
}

bool NetworkManager::Start()
{
	//서버 시작
	if(m_pGameServer)
	{
		if(!m_pGameServer->Begin())
			return false;
#ifdef ZFLOG
//		ZFLog::g_sLog->Log("서버 시작");
#endif
	}
	else		//클라이언트 시작
	{
		if(!m_pGameClient->Start())
			return false;
#ifdef ZFLOG
//		ZFLog::g_sLog->Log("클라이언트 시작");
#endif
	}

	return true;
}

void NetworkManager::SetIP( std::string pIP )
{
	m_IP = pIP;
}

const char*	NetworkManager::GetIP()
{
	return m_IP.c_str();
}

bool NetworkManager::End()
{
	LockPlayers();
	if( !m_mapPlayers.empty() )
		m_mapPlayers.clear();
	UnLockPlayers();

	//서버 종료
	if(m_pGameServer)
	{
		if(!m_pGameServer->End())
			return false;
	}
	else //클라이언트 종료
	{
		if(!m_pGameClient->End())
			return false;
	}

	m_bCaptain = false;
	return true;
}

void NetworkManager::Reset(bool server)
{

}

NetworkManager* NetworkManager::GetInstance()
{
	return m_pkNetworkManager;
}

void NetworkManager::SetSelfData(const GAME_USER_INFO&  selfData)
{
	m_SelfInfo = selfData;	

	//서버의 경우는 바로 컨테이너에 추가
	if(m_pGameServer)
	{
		CGameApp::mp_Appication->EnterCS();
		
		//공유 데이터 컨테이너에 원소 추가.
		m_vecUserInfo.push_back(m_SelfInfo);
		CGameApp::mp_Appication->LeaveCS();
	}
}
void NetworkManager::SetSendData(BYTE id)
{
	m_SelfInfo.userInfo.id = id;
		
	//클라이언트의 경우 서버에 self정보를 전달해야 하므로 아래에서 처리
	if(m_pGameClient)
	{
		CGameApp::mp_Appication->EnterCS();		
		//공유 데이터 컨테이너에 원소 추가.
		m_vecUserInfo.push_back(m_SelfInfo);
		CGameApp::mp_Appication->LeaveCS();
		
		struct user_info	packet1;
		packet1.info = m_SelfInfo.userInfo;
		packet1.size = sizeof(packet1);
		packet1.type = ADD_USER;
		m_pGameClient->SendData((unsigned char*)&packet1,packet1.size);

//		ZFLog::g_sLog->Log("전송 : ADD_USER");
	}
}

//	비활성화 상태에서 리스폰 처리
bool NetworkManager::ProcessRespone( float fAccumTime)
{
	//=================================================================
	//		DEATH 상태일 경우 패킷및 로직 처리  
	//		
	//	conv : 서버로 부터 죽었다는 메시지를 받으면 스테이트 변경한 후부터 발효
	//=================================================================
	BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
		
	// 리스폰 타임을 계산해본다.
	float fDeltaTime = fAccumTime - conPlayers[ nID ].userInfo.deathTime;

	//==================================================================
	//						UI 갱신
	//==================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
	pGameState->UpdateResponeProgress( fDeltaTime );
	//==================================================================

//	ZFLog::g_sLog->Log( " fDeltaTime = %f", fDeltaTime );
	// 리스폰 시간이 충족된다면
	if( fDeltaTime  >= RESPONE_TIME )
	{	
		conPlayers[ nID ].userInfo.state  = NORMAL;			// 활성화 시킴			

		NetworkManager::GetInstance()->UnLockPlayers();
		return true;
	}
	
	NetworkManager::GetInstance()->UnLockPlayers();
	/*PlayersMap::iterator iterPlayer;

	for(iterPlayer = conPlayers.begin();iterPlayer!=conPlayers.end();++iterPlayer)
	{
		if(iterPlayer->second.userInfo.state == DEATH)
			continue;
		float fDeltaTime = fAccumTime - iterPlayer->second.userInfo.deathTime;
		//==================================================================
		//						UI 갱신
		//==================================================================
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
		pGameState->UpdateResponeProgress( fDeltaTime );
		//==================================================================

		// 리스폰 시간이 충족된다면
		if( fDeltaTime  >= RESPONE_TIME )
		{				
			//리스폰이 끝나 리셋 시킬 캐릭터 ID
			BYTE	resetID = iterPlayer->second.userInfo.id;
			iterPlayer->second.userInfo.state  = NORMAL;			// 해당 캐릭터 상태를 노멀로	

			//캐릭터 상태 변경
			if(nID >= resetID) //맞은 놈이 나보다 일찍 접속한 놈이면 
			{
				WorldManager::GetWorldManager()->GetCharacterManager()->GetCharacterIndex(resetID)->Reset();
			}
			else
				WorldManager::GetWorldManager()->GetCharacterManager()->GetCharacterIndex(resetID-1)->Reset();
				
			//현재 유저의 리스폰 처리
			if(iterPlayer->first == nID)
			{
				//리스폰 대기 시간이 끝났으므로 유저 캐릭터의 리스폰 위치로 리셋
				// 점령지가 하나도 없는 상태이면 시작위치에서 시작
				if(  conPlayers[ nID ].userInfo.territory == nsPlayerInfo::TERRITORY_NO )
				{
					info.pos = WorldManager::GetWorldManager()->GetRespone()->GetStartPosition( 
						conPlayers[ nID ].userInfo.camp );					 
				}
				// 점령지가 1개라도 있으면 해당 점령지와 연관된 리스폰 위치 적용
				else
				{
					info.pos = WorldManager::GetWorldManager()->GetRespone()->GetResponePosition( 
						conPlayers[ nID ].userInfo.camp, conPlayers[ nID ].userInfo.territory );
				}

				info.animation	= Animation_Enum::IDLE_RIFLE;
				info.Heading	= 0.0f;							
				info.Vertical	= 0.0f;						

				// 캐릭터 기본 사항 리셋( 총알, 애니메이션, 위치, 기타 정보 등등 )
				WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter()->Reset( info.pos, fAccumTime );
			}
		}
	}	*/
	
	return false;
}

//	점령지 처리
bool NetworkManager::ProcessTerritory( BYTE nCamp, BYTE nTerritory )
{

BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	BYTE nSelfCamp = conPlayers[ nID ].userInfo.camp;
	// 점령지 깃발이 나의 진영용 점령지가 아니라면
	if(  nSelfCamp != nCamp && (nSelfCamp != nsPlayerInfo::GEURRILLA0 || nSelfCamp != nsPlayerInfo::GEURRILLA1 ) )
	{
		if( m_pGameServer )
		{
			m_pGameServer->SendUpdateTerritory( nTerritory );
		}
		else
		{
			m_pGameClient->RequestTerritory(  nSelfCamp, nTerritory);
		}		
	}

	NetworkManager::GetInstance()->UnLockPlayers();

	return false;
}

void NetworkManager::UpdateSelfData(const UserInfo& selfInfo)
{
	struct user_info packet;
	packet.size = sizeof(packet);
	packet.type = UNIT_INFO;
	
	BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	conPlayers[ nID ].userInfo.animation	= selfInfo.animation;
	conPlayers[ nID ].userInfo.Heading		= selfInfo.Heading;
	conPlayers[ nID ].userInfo.pos			= selfInfo.pos;
	conPlayers[ nID ].userInfo.Vertical		= selfInfo.Vertical;
	conPlayers[ nID ].userInfo.state		= selfInfo.state;

	packet.info = conPlayers[ nID ].userInfo;
	
	
	if(!m_pGameServer) //클라이언트의 경우
	{
		m_pGameClient->SendDelayData((unsigned char*)&packet,packet.size);
	}
	else	//서버의 경우
	{
		PlayersMap::iterator iterPlayer;		

		//접속된 유저들에게 전송
		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			m_pGameServer->RegisterSendMsg( iterPlayer->second.pClientInfo, (UCHAR*)&packet, packet.size );
		}
	}
	NetworkManager::GetInstance()->UnLockPlayers();

}

void NetworkManager::AddUserData(const GAME_USER_INFO& userInfo)
{
	GAME_USER_INFO	info;

	info = userInfo;
	
	CGameApp::mp_Appication->EnterCS();
	
	m_vecUserInfo.push_back(info);
	CGameApp::mp_Appication->LeaveCS();
	
	//다른 유저 캐릭터 추가
	NiMatrix3 rot;
	rot.MakeZRotation(info.userInfo.Heading);
	NiTransform kSpawn;
	kSpawn.m_Rotate = rot;
	kSpawn.m_Translate = info.userInfo.pos;
	kSpawn.m_fScale = 1.0f;

	WorldManager::GetWorldManager()->AddCharacter(info.userInfo.BranchOfMilitary,kSpawn);
}

//유저 데이터를 업데이트 한다.
void NetworkManager::UpdateUserData(const UserInfo& selfInfo)
{	
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();	

	conPlayers[ selfInfo.id ].userInfo.animation = selfInfo.animation;
	conPlayers[ selfInfo.id ].userInfo.Heading	= selfInfo.Heading;
	conPlayers[ selfInfo.id ].userInfo.pos		= selfInfo.pos;
	conPlayers[ selfInfo.id ].userInfo.Vertical	= selfInfo.Vertical;
	conPlayers[ selfInfo.id ].userInfo.state	= selfInfo.state;
		
	NetworkManager::GetInstance()->UnLockPlayers();
}

/*
//유저를 ID로 찾아 제거
void NetworkManager::DeleteUser(DWORD id)
{
	VecUserInfo::iterator iter;

	CGameApp::mp_Appication->EnterCS();

	for(iter=m_vecUserInfo.begin();iter != m_vecUserInfo.end();++iter)
	{
		if(iter->userInfo.id == id)
		{
			m_vecUserInfo.erase(iter);
			break;
		}
	}
	CGameApp::mp_Appication->LeaveCS();
}
*/

//발사 정보를 전송한다.
void NetworkManager::SendFireData(const _PACKET_GUN_FIRE& gunFire)
{	 
	//서버의 경우 모든 클라이언트들에게 전송
	if(m_pGameServer)
	{
		m_pGameServer->SendSelfGunfireData( (unsigned char*)&gunFire,gunFire.size);
	}
	else	//클라이언트 처리
	{
		//서버로 전송 딜레이 없음
		m_pGameClient->SendData((unsigned char*)&gunFire,gunFire.size);
	}
}

//발사 결과 전송
void NetworkManager::SendFireResult(const _PACKET_FIRE_RESULT& fireResult)	//발사 결과 전송
{
	//모든 클라이언트들에게 전송
	if(m_pGameServer)
		m_pGameServer->SendSelfFireResult((unsigned char*)&fireResult,fireResult.size);
	else //클라이언트 처리
		m_pGameClient->SendData((unsigned char*)&fireResult,fireResult.size);
}

void NetworkManager::SendSoundData(const _PACKET_SOUND_DATA& soundData)
{
	//서버 일 경우 모든 클라이언트들에게 전송
	if(m_pGameServer)
		m_pGameServer->SelfSoundData( (unsigned char*)&soundData,soundData.size);
	else	//클라이언트 = 서버에 전송
		m_pGameClient->SendData( (unsigned char*)&soundData,soundData.size);
}

//상태 변경 전송
void NetworkManager::SendChangeStatus(const _PACKET_CHANGE_STATUS& changeStatus)
{
	//서버 일경우 해당 클라이언트에게만 전송
	if(m_pGameServer)
		m_pGameServer->SelfChangeStatus( (unsigned char*)&changeStatus,changeStatus.size);
	else
		m_pGameClient->SendData( (unsigned char*)&changeStatus,changeStatus.size);
}


void NetworkManager::SetID(BYTE id)
{
	m_nUniqID = id;
}

BYTE NetworkManager::GetID()
{
	return m_nUniqID;
}

BYTE NetworkManager::GetCamp()
{
	return m_SelfInfo.userInfo.camp;
}


void NetworkManager::ClearFireInfo()
{
	m_vecFireInfo.clear();
}
void NetworkManager::ClearFireResult()
{
	m_vecFireResult.clear();
}

void NetworkManager::RegisterFireInfo(FireInfo& fireInfo)
{
	EnterCriticalSection(&m_csFireInfo);
	//사격 정보 추가
	m_vecFireInfo.push_back(fireInfo);

	LeaveCriticalSection(&m_csFireInfo);
}

void NetworkManager::RegisterFireResult(FireResult& fireResult)
{
	EnterCriticalSection(&m_csFireResult);
	//사격 정보 추가
	m_vecFireResult.push_back(fireResult);

	LeaveCriticalSection(&m_csFireResult);
}

VecFireInfo&	NetworkManager::LockFireInfoVec()
{
	EnterCriticalSection(&m_csFireInfo);
	return m_vecFireInfo;
}
VecFireResult&	NetworkManager::LockFireResultVec()
{
	EnterCriticalSection(&m_csFireResult);
	return m_vecFireResult;
}

void NetworkManager::UnLockFireInfo()
{
	LeaveCriticalSection(&m_csFireInfo);
}
void NetworkManager::UnLockFireResult()
{
	LeaveCriticalSection(&m_csFireResult);
}


PlayersMap& NetworkManager::LockPlayers()
{
	EnterCriticalSection(&m_csPlayers);

	return m_mapPlayers;
}

void NetworkManager::UnLockPlayers()
{
	LeaveCriticalSection(&m_csPlayers);
}

DWORD NetworkManager::WaitComplete()
{
	DWORD ret; 
	ret = WaitForSingleObject( m_StartEvent, INFINITE );
	return ret;
}

void NetworkManager::ResetStartEvent()
{
	ResetEvent( m_StartEvent );
}

void	NetworkManager::CallStartEvent()
{
	SetEvent( m_StartEvent );
}

VecUserInfo& NetworkManager::GetVecUserInfo()
{
	return m_vecUserInfo;
}

BYTE	NetworkManager::GetCharacterIndex( BYTE nTeam, BYTE nJob )
{

	using namespace nsPlayerInfo;

	switch( nTeam )
	{
	case USA:

		switch( nJob )
		{
		case SOLIDIER:
			return BranchOfMilitary::US_Soldier;
			break;

		case SNIPER:
			return BranchOfMilitary::US_Sniper;
			break;

		case MEDIC:
			return BranchOfMilitary::US_Medic;
			break;

		case ASP:
			return BranchOfMilitary::US_ASP;
			break;
		}

		break;

	case EU:

		switch( nJob )
		{
		case SOLIDIER:
			return BranchOfMilitary::EU_Soldier;
			break;

		case SNIPER:
			return BranchOfMilitary::EU_Sniper;
			break;

		case MEDIC:
			return BranchOfMilitary::EU_Medic;
			break;

		case ASP:
			return BranchOfMilitary::EU_ASP;
			break;
		}

		break;

	case GEURRILLA0:
		return BranchOfMilitary::Gerilla;
		break;
	case GEURRILLA1:
		return BranchOfMilitary::Gerilla;
		break;
	}

	return 0;
}

BYTE NetworkManager::GetTeamType( BYTE nPos, bool bRoomSeatMove )
{	
	using namespace nsPlayerInfo;

	if( 0 <= nPos && nPos < MAX_TEAM_PLAYER )
		return nsPlayerInfo::USA;
	else if( MAX_TEAM_PLAYER <= nPos && nPos < MAX_TEAM_PLAYER*2 )
		return nsPlayerInfo::EU;
	else if( MAX_TEAM_PLAYER*2 == nPos)//|| MAX_TEAM_PLAYER*2+1 == nPos)
		return nsPlayerInfo::GEURRILLA0;
	else 
	{
		if(bRoomSeatMove) return nsPlayerInfo::GEURRILLA1;
		else			  return nsPlayerInfo::GEURRILLA0;
	}
	return 0;
}