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
	m_fLastSendTime = 0.0f;				//���������� �޽����� ������ �ð�
	m_fDelay = 0.0f;					//��Ʈ��ũ ������ �ð�
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


//��Ʈ��ũ �Ŵ��� ����
bool NetworkManager::Create(bool server)
{
	NetworkManager*	pkNetwork = NiNew NetworkManager;
	assert(pkNetwork);

	m_pkNetworkManager = pkNetwork;

	m_bCaptain = server;

	m_StartEvent = CreateEvent(NULL,TRUE, FALSE,NULL);

	//��Ʈ��ũ ��ü �Ҵ�
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
	//���� ����
	if(m_pGameServer)
	{
		if(!m_pGameServer->Begin())
			return false;
#ifdef ZFLOG
//		ZFLog::g_sLog->Log("���� ����");
#endif
	}
	else		//Ŭ���̾�Ʈ ����
	{
		if(!m_pGameClient->Start())
			return false;
#ifdef ZFLOG
//		ZFLog::g_sLog->Log("Ŭ���̾�Ʈ ����");
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

	//���� ����
	if(m_pGameServer)
	{
		if(!m_pGameServer->End())
			return false;
	}
	else //Ŭ���̾�Ʈ ����
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

	//������ ���� �ٷ� �����̳ʿ� �߰�
	if(m_pGameServer)
	{
		CGameApp::mp_Appication->EnterCS();
		
		//���� ������ �����̳ʿ� ���� �߰�.
		m_vecUserInfo.push_back(m_SelfInfo);
		CGameApp::mp_Appication->LeaveCS();
	}
}
void NetworkManager::SetSendData(BYTE id)
{
	m_SelfInfo.userInfo.id = id;
		
	//Ŭ���̾�Ʈ�� ��� ������ self������ �����ؾ� �ϹǷ� �Ʒ����� ó��
	if(m_pGameClient)
	{
		CGameApp::mp_Appication->EnterCS();		
		//���� ������ �����̳ʿ� ���� �߰�.
		m_vecUserInfo.push_back(m_SelfInfo);
		CGameApp::mp_Appication->LeaveCS();
		
		struct user_info	packet1;
		packet1.info = m_SelfInfo.userInfo;
		packet1.size = sizeof(packet1);
		packet1.type = ADD_USER;
		m_pGameClient->SendData((unsigned char*)&packet1,packet1.size);

//		ZFLog::g_sLog->Log("���� : ADD_USER");
	}
}

//	��Ȱ��ȭ ���¿��� ������ ó��
bool NetworkManager::ProcessRespone( float fAccumTime)
{
	//=================================================================
	//		DEATH ������ ��� ��Ŷ�� ���� ó��  
	//		
	//	conv : ������ ���� �׾��ٴ� �޽����� ������ ������Ʈ ������ �ĺ��� ��ȿ
	//=================================================================
	BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
		
	// ������ Ÿ���� ����غ���.
	float fDeltaTime = fAccumTime - conPlayers[ nID ].userInfo.deathTime;

	//==================================================================
	//						UI ����
	//==================================================================
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
	pGameState->UpdateResponeProgress( fDeltaTime );
	//==================================================================

//	ZFLog::g_sLog->Log( " fDeltaTime = %f", fDeltaTime );
	// ������ �ð��� �����ȴٸ�
	if( fDeltaTime  >= RESPONE_TIME )
	{	
		conPlayers[ nID ].userInfo.state  = NORMAL;			// Ȱ��ȭ ��Ŵ			

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
		//						UI ����
		//==================================================================
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
		pGameState->UpdateResponeProgress( fDeltaTime );
		//==================================================================

		// ������ �ð��� �����ȴٸ�
		if( fDeltaTime  >= RESPONE_TIME )
		{				
			//�������� ���� ���� ��ų ĳ���� ID
			BYTE	resetID = iterPlayer->second.userInfo.id;
			iterPlayer->second.userInfo.state  = NORMAL;			// �ش� ĳ���� ���¸� ��ַ�	

			//ĳ���� ���� ����
			if(nID >= resetID) //���� ���� ������ ���� ������ ���̸� 
			{
				WorldManager::GetWorldManager()->GetCharacterManager()->GetCharacterIndex(resetID)->Reset();
			}
			else
				WorldManager::GetWorldManager()->GetCharacterManager()->GetCharacterIndex(resetID-1)->Reset();
				
			//���� ������ ������ ó��
			if(iterPlayer->first == nID)
			{
				//������ ��� �ð��� �������Ƿ� ���� ĳ������ ������ ��ġ�� ����
				// �������� �ϳ��� ���� �����̸� ������ġ���� ����
				if(  conPlayers[ nID ].userInfo.territory == nsPlayerInfo::TERRITORY_NO )
				{
					info.pos = WorldManager::GetWorldManager()->GetRespone()->GetStartPosition( 
						conPlayers[ nID ].userInfo.camp );					 
				}
				// �������� 1���� ������ �ش� �������� ������ ������ ��ġ ����
				else
				{
					info.pos = WorldManager::GetWorldManager()->GetRespone()->GetResponePosition( 
						conPlayers[ nID ].userInfo.camp, conPlayers[ nID ].userInfo.territory );
				}

				info.animation	= Animation_Enum::IDLE_RIFLE;
				info.Heading	= 0.0f;							
				info.Vertical	= 0.0f;						

				// ĳ���� �⺻ ���� ����( �Ѿ�, �ִϸ��̼�, ��ġ, ��Ÿ ���� ��� )
				WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter()->Reset( info.pos, fAccumTime );
			}
		}
	}	*/
	
	return false;
}

//	������ ó��
bool NetworkManager::ProcessTerritory( BYTE nCamp, BYTE nTerritory )
{

BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	BYTE nSelfCamp = conPlayers[ nID ].userInfo.camp;
	// ������ ����� ���� ������ �������� �ƴ϶��
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
	
	
	if(!m_pGameServer) //Ŭ���̾�Ʈ�� ���
	{
		m_pGameClient->SendDelayData((unsigned char*)&packet,packet.size);
	}
	else	//������ ���
	{
		PlayersMap::iterator iterPlayer;		

		//���ӵ� �����鿡�� ����
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
	
	//�ٸ� ���� ĳ���� �߰�
	NiMatrix3 rot;
	rot.MakeZRotation(info.userInfo.Heading);
	NiTransform kSpawn;
	kSpawn.m_Rotate = rot;
	kSpawn.m_Translate = info.userInfo.pos;
	kSpawn.m_fScale = 1.0f;

	WorldManager::GetWorldManager()->AddCharacter(info.userInfo.BranchOfMilitary,kSpawn);
}

//���� �����͸� ������Ʈ �Ѵ�.
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
//������ ID�� ã�� ����
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

//�߻� ������ �����Ѵ�.
void NetworkManager::SendFireData(const _PACKET_GUN_FIRE& gunFire)
{	 
	//������ ��� ��� Ŭ���̾�Ʈ�鿡�� ����
	if(m_pGameServer)
	{
		m_pGameServer->SendSelfGunfireData( (unsigned char*)&gunFire,gunFire.size);
	}
	else	//Ŭ���̾�Ʈ ó��
	{
		//������ ���� ������ ����
		m_pGameClient->SendData((unsigned char*)&gunFire,gunFire.size);
	}
}

//�߻� ��� ����
void NetworkManager::SendFireResult(const _PACKET_FIRE_RESULT& fireResult)	//�߻� ��� ����
{
	//��� Ŭ���̾�Ʈ�鿡�� ����
	if(m_pGameServer)
		m_pGameServer->SendSelfFireResult((unsigned char*)&fireResult,fireResult.size);
	else //Ŭ���̾�Ʈ ó��
		m_pGameClient->SendData((unsigned char*)&fireResult,fireResult.size);
}

void NetworkManager::SendSoundData(const _PACKET_SOUND_DATA& soundData)
{
	//���� �� ��� ��� Ŭ���̾�Ʈ�鿡�� ����
	if(m_pGameServer)
		m_pGameServer->SelfSoundData( (unsigned char*)&soundData,soundData.size);
	else	//Ŭ���̾�Ʈ = ������ ����
		m_pGameClient->SendData( (unsigned char*)&soundData,soundData.size);
}

//���� ���� ����
void NetworkManager::SendChangeStatus(const _PACKET_CHANGE_STATUS& changeStatus)
{
	//���� �ϰ�� �ش� Ŭ���̾�Ʈ���Ը� ����
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
	//��� ���� �߰�
	m_vecFireInfo.push_back(fireInfo);

	LeaveCriticalSection(&m_csFireInfo);
}

void NetworkManager::RegisterFireResult(FireResult& fireResult)
{
	EnterCriticalSection(&m_csFireResult);
	//��� ���� �߰�
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