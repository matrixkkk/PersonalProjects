#include "GameServer.h"
#include "GameApp.h"
#include "NetworkManager.h"
#include "CharacterManager.h"
#include "CStateManager.h"
#include "CGameRoomState.h"
#include "CGameResultState.h"
#include "CGamePlayState.h"
#include "CTerritory.h"
#include "strconv.h"


#ifdef NET_TEST
//#include "ZFLog.h"
#endif

CGameServer::CGameServer() :
m_nCurNetState( nsPlayerInfo::NET_ROOM_STATE )
{
	/*
	m_nGlobal_id = 1;

	idx = 0;	

	::ZeroMemory( m_nPlayerPosAtRoom, sizeof(m_nPlayerPosAtRoom) );
	m_nPlayerPosAtRoom[ 0 ] = TRUE;

	m_stCaptain.userInfo.id = 0;
	m_stCaptain.userInfo.m_nPosAtRoom = 0;

	InitializeCriticalSection(&m_csPlayers);
	InitializeCriticalSection(&m_csPosAtRoom);
	*/
}

CGameServer::~CGameServer()
{
	
}

// ��������
bool CGameServer::Begin()		
{
	using namespace nsPlayerInfo;
	m_nGlobal_id = 1;

	idx = 0;	

	m_bCntInitPlyers = 0;

	::ZeroMemory( m_nPlayerPosAtRoom, sizeof(m_nPlayerPosAtRoom) );
	::ZeroMemory( m_nStateTerritory, sizeof(m_nStateTerritory) );
	
	m_nPlayerPosAtRoom[ 0 ] = TRUE;

	m_stCaptain.userInfo.id					= 0;	
	m_stCaptain.userRoomInfo.m_nPosAtRoom	= 0;
	m_stCaptain.userRoomInfo.m_nJob			= SOLIDIER;
	m_stCaptain.userRoomInfo.m_nStateAtRoom = READY;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	conPlayers.insert( std::make_pair( m_stCaptain.userInfo.id, m_stCaptain ) );
	NetworkManager::GetInstance()->UnLockPlayers();

	// �ʱ� ������ �κ� �� ����
	m_nCurNetState	= NET_ROOM_STATE;

	InitializeCriticalSection(&m_csStateTerritory);
	InitializeCriticalSection(&m_csUSA_Territory);
	InitializeCriticalSection(&m_csEU_Territory);
	InitializeCriticalSection(&m_csPosAtRoom);

	m_fTerritoryTime = NiGetCurrentTimeInSec();

	CNetSystemServer::Begin();

	return true;
}

// ��������
bool CGameServer::End()		
{

	if( !m_listUSA_Territory.empty() )
		m_listUSA_Territory.clear();

	if( !m_listEU_Territory.empty() )
		m_listEU_Territory.clear();

	::ZeroMemory( m_nPlayerPosAtRoom, sizeof(m_nPlayerPosAtRoom) );

	DeleteCriticalSection(&m_csStateTerritory);
	DeleteCriticalSection(&m_csUSA_Territory);
	DeleteCriticalSection(&m_csEU_Territory);
	DeleteCriticalSection(&m_csPosAtRoom);

	CNetSystemServer::End();

	return true;
}


bool CGameServer::ConnectProcess(CLIENTINFO *pClientInfo)
{
	GAME_USER_INFO	info;
	DWORD	dwType = 0;
	info.pClientInfo = pClientInfo;
	info.userInfo.id = m_nGlobal_id++;	//m_nGlobal_id = 1;

	//�����ڿ��� ID ����
	struct give_id packet1;
	packet1.size = sizeof(packet1);
	packet1.type = GIVE_ID;
	packet1.id = info.userInfo.id;

	RegisterSendMsg(pClientInfo,(unsigned char*)&packet1,packet1.size );
	
#ifdef NET_TEST
	char temp[128];
	sprintf_s(temp,sizeof(temp),"��Ŷ Ÿ�� = GIVE_ID ������ = %d",packet1.size);
//	ZFLog::g_sLog->Log(temp);
#endif
	
	return true;
}

void CGameServer::SetCaptaionID( CEGUI::String& strName )//const wchar_t *pID )
{
	m_stCaptain.userRoomInfo.m_strID = strName;
}

bool CGameServer::DisConnectProcess(CLIENTINFO *pClientInfo)
{


	if( m_nCurNetState == nsPlayerInfo::NET_ROOM_STATE )
	{
//		EnterCriticalSection(&m_csPlayers);
		

		//---------------------------------------------------------------------------------------	
		//						������ ������ ���� ID�� �ڷᱸ������ ����
		//---------------------------------------------------------------------------------------	

//		PlayersMap	conPlayers;
		PlayersMap::iterator iterPlayer;
		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		BYTE nID = 0;
		BYTE nPosAtRoom = 0;
		CEGUI::String strID;

		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{

			if( iterPlayer->second.pClientInfo == pClientInfo )
			{
				nID				= iterPlayer->second.userInfo.id;
				nPosAtRoom		= iterPlayer->second.userRoomInfo.m_nPosAtRoom;
				strID			= iterPlayer->second.userRoomInfo.m_strID;

				break;
			}
		}

		const char *pStr = strID.c_str();

		conPlayers.erase(iterPlayer);

		//---------------------------------------------------------------------------------------	
		//						�濡 ���� �����鿡�� ���� �޽��� ����
		//---------------------------------------------------------------------------------------	
		if( !conPlayers.empty() )
		{
			//PlayersMap::iterator iterPlayer;
			PACKET_DISCONNECT_PLAYER_AT_ROOM stPacket;

			stPacket.nID = nID;

			for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			{						
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size);		
			}
		}		
		
		EnterCriticalSection(&m_csPosAtRoom);		
		m_nPlayerPosAtRoom[ nPosAtRoom ] = FALSE;		
		LeaveCriticalSection(&m_csPosAtRoom);

		//---------------------------------------------------------------------------------------	
		//						������ UI ����
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		CGameRoomState* pRoomState = (CGameRoomState*)rStateManager.GetStateClass("GameRoom");

		if( pRoomState )
		{
			pRoomState->SetEmptyStaticText( nPosAtRoom );
			pRoomState->RemovePlayerList( strID, nID );
		}
		
//		LeaveCriticalSection(&m_csPlayers);

		NetworkManager::GetInstance()->UnLockPlayers();
	}
	//	�����߿� ������ �� ó��
	else
	{


	}
		/*
		GAME_USER_INFO userInfo;

		int id;

		CGameApp::mp_Appication->EnterCS();
		
		vector<GAME_USER_INFO>& vecUsers = NetworkManager::GetInstance()->GetVecUserInfo();
		
		//������ ���� ������ ID�� ĳ���� ������ ���������Ϳ��� ����
		for(unsigned int i=1;i<vecUsers.size();i++)
		{
			if(vecUsers[i].pClientInfo->socketClient == pClientInfo->socketClient)
			{
				id = vecUsers[i].userInfo.id;
				NetworkManager::GetInstance()->DeleteUser(id);
				
				char temp[128];
				sprintf_s(temp,sizeof(temp),"ID : %d �� ��������",id);
//				ZFLog::g_sLog->Log(temp);
				break;
			}
		}
		CGameApp::mp_Appication->LeaveCS();
		
		//�ٸ� �����ڵ鿡�� ���� ������ �˸��� ��Ŷ ��ŷ
		struct another_disconnect packet1;
		packet1.size = sizeof(packet1);
		packet1.type = ANOTHER_DISCONNECT;
		packet1.id = id;

		CGameApp::mp_Appication->EnterCS();
		
		//�ٸ� �����ڵ鿡�� ���������� �˸�
		for(unsigned int i=0;i<vecUsers.size();i++)
		{
			//�������� ���� ����
			RegisterSendMsg( vecUsers[i].pClientInfo, (unsigned char*)&packet1, packet1.size );
		}
		CGameApp::mp_Appication->LeaveCS();	
		*/
	return true;
}

void CGameServer::ProcessAddUserInfo(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	GAME_USER_INFO	user;
	struct user_info* pPacket = (struct user_info*)pBuf;

	user.pClientInfo = pClientInfo;
	user.userInfo = pPacket->info;

	CGameApp::mp_Appication->EnterCS();
	
	//���� ���� ������ ���ӵ� ��� Ŭ���̾�Ʈ���� ����
	vector<GAME_USER_INFO>& vecUsers = NetworkManager::GetInstance()->GetVecUserInfo();
	
	for(unsigned int i=1;i<vecUsers.size();i++)
	{
		struct user_info* pPacket1 = (struct user_info*)pBuf;
		RegisterSendMsg( vecUsers[i].pClientInfo, (unsigned char*)&pPacket, pPacket->size );
	}

	//���� �����ڿ��� ������������ ������ ����
	for(unsigned int i=0;i<vecUsers.size();i++)
	{
		struct user_info packet;
			
		GAME_USER_INFO gameUser = vecUsers[i];
		
		packet.info = gameUser.userInfo;
		packet.type = ADD_USER;
		packet.size = sizeof(packet);

		RegisterSendMsg(pClientInfo,(unsigned char*)&packet,packet.size);

	}	

	//��Ʈ��ũ �Ŵ����� ���� ��� �߰�
	NetworkManager::GetInstance()->AddUserData(user);
	CGameApp::mp_Appication->LeaveCS();		
}

void CGameServer::ProcessUpdateUser(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	struct user_info* info = (user_info*)pBuf;			
	struct user_info sendInfo;

	sendInfo.info = info->info;
	sendInfo.size = info->size;
	sendInfo.type = UNIT_INFO;
		
	//���� ������ ������ ������Ʈ �Ѵ�.
	NetworkManager::GetInstance()->UpdateUserData(info->info);
	
	//��� �÷��̾�鿡�� ������ ������ ����	
	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayer = NetworkManager::GetInstance()->LockPlayers();

	for( iterPlayer = ++(conPlayer.begin()); iterPlayer != conPlayer.end(); ++iterPlayer )
	{
		RegisterSendMsg( iterPlayer->second.pClientInfo, (unsigned char*)&sendInfo, sendInfo.size);
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

//�߻� ���� ó��
void CGameServer::ProcessGunFire(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	struct _PACKET_GUN_FIRE* pPacket = (_PACKET_GUN_FIRE*)pBuf;

	BYTE id = (BYTE)pPacket->fireInfo.damageUserID;
	
	//�ѿ� ���� ������� ���� ���� ����
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;
	
	//���������� ó��
	NetworkManager::GetInstance()->RegisterFireInfo(pPacket->fireInfo);
	
	//�ٸ� Ŭ���̾�Ʈ��� ��� �������� ����

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}	

	NetworkManager::GetInstance()->UnLockPlayers();	
}
//�߻� ��� ó��
void CGameServer::ProcessFireResult(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	struct _PACKET_FIRE_RESULT* pPacket = (_PACKET_FIRE_RESULT*)pBuf;

	//��� �����ڿ��� ����
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}
	NetworkManager::GetInstance()->UnLockPlayers();
	
	//���� ��ü ó��
	NetworkManager::GetInstance()->RegisterFireResult(pPacket->fireResult);
}

//���� ������ ó��
void CGameServer::ProcessSoundData(const unsigned char* pBuf,CLIENTINFO *pClientInfo)
{
	struct _PACKET_SOUND_DATA* pPacket = (_PACKET_SOUND_DATA*)pBuf;

	//��� �����ڿ��� ����
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}
	NetworkManager::GetInstance()->UnLockPlayers();

	//���� ��ü ó��
	NiTransform		kTrans	  = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter()->GetCharRoot()->GetWorldTransform(); 
		
	CSoundManager::GetInstance()->RegisterSound(pPacket->eSoundCode,pPacket->kSoundSpot,kTrans,pPacket->b2DSound);
}

//���� ���� �޽��� ó��
void CGameServer::ProcessChangeStatus(const unsigned char* pBuf,CLIENTINFO* pClientInfo)	//���� ���� �޽��� ó��
{
	//�� �޽����� �÷��̾��� ���¸� ���� �ϴ� ���̹Ƿ� ��� Ŭ���̾�Ʈ���� ���� �ʿ䰡 ����
	//������ ������ �� �����Ӹ��� �÷��̾��� ���¸� �������ֱ� �����̴�(�������� ������带 ����)
	struct _PACKET_CHANGE_STATUS* pPacket = (_PACKET_CHANGE_STATUS*)pBuf;

	BYTE desID = pPacket->id;
	CLIENTINFO* pDestClientInfo;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	pDestClientInfo = conPlayers[desID].pClientInfo;				//�ٲ� �÷��̾��� ID�� Client������ ������
	RegisterSendMsg(pDestClientInfo,(UCHAR*)pPacket,pPacket->size);	//�� �÷��̾�Ը� ����	

	NetworkManager::GetInstance()->UnLockPlayers();
}

bool CGameServer::PacketProcess(const UCHAR* pBuf,CLIENTINFO* pClientInfo)
{	

	switch( m_nCurNetState )
	{
	case nsPlayerInfo::NET_ROOM_STATE:
		return PacketProcessAtRoom( pBuf, pClientInfo );
		break;

	case nsPlayerInfo::NET_GAME_STATE:
		return PacketProcessAtGame( pBuf, pClientInfo );
		break;
	}
	
	return true;
}

bool	CGameServer::PacketProcessAtRoom(const UCHAR* pBuf,CLIENTINFO* pClientInfo)
{
	
	DWORD	dwType = 0;

	//��Ŷ�� Ÿ���� �о�´�.
	dwType = (int)pBuf[1];

	switch( dwType )
	{
	case CTOS_CONNECT_ROOM:

		ProcessLogin( pBuf, pClientInfo );
		break;

	case CTOS_CHAT_IN_ROOM:
		
		ProcessChatMsg( pBuf, pClientInfo );
		break;

	case CTOS_GAME_READY:

		ProcessReady( pBuf, pClientInfo );
		break;

	case CTOS_CHANGE_TEAM:

		ProcessChangeTeam( pBuf, pClientInfo );
		break;

	case CTOS_CHANGE_JOB:
		
		ProcessChangeJob( pBuf, pClientInfo );
		break;

	case CTOS_GAME_JOIN:

		ProcessGameJoin( pBuf, pClientInfo );
		break;
	}

	return true;
}


bool	CGameServer::ProcessLogin( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{

	using namespace nsPlayerInfo;
	// ��Ŷ
	LPACKET_CONNECT_ROOM pPacket	= (LPACKET_CONNECT_ROOM)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 �÷��̾� ����
	//---------------------------------------------------------------------------------------	
	GAME_USER_INFO	stPlayer;
	stPlayer.pClientInfo				= pClientInfo;
	stPlayer.userInfo.id				= pPacket->nID;
	stPlayer.userRoomInfo.m_nPosAtRoom	= GetEmptyPosAtRoom(); 
	stPlayer.userRoomInfo.m_nStateAtRoom= DEFAULT;
	stPlayer.userRoomInfo.m_nJob		= SOLIDIER;
	stPlayer.userRoomInfo.m_strID		= (CEGUI::utf8*)CONV_MBCS_UTF8(pPacket->strGameID);
	
//	PlayersMap				conPlayers;


	PACKET_ADD_PLAYER		stPacket;
	stPacket.nID			= pPacket->nID;
	stPacket.nPosAtRoom		= stPlayer.userRoomInfo.m_nPosAtRoom;
	stPacket.nStateAtRoom	= DEFAULT;
	stPacket.nJob			= SOLIDIER;
	strcpy_s( stPacket.strGameID,  USER_ID_LEN, pPacket->strGameID );	
	
	PlayersMap::iterator	iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();		

	//---------------------------------------------------------------------------------------	
	//						���� �����鿡�� �ű� ���� ���� �۽�
	//---------------------------------------------------------------------------------------		
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{						
		RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size);		
	}
	//---------------------------------------------------------------------------------------	
	//						�ű� �������� ���� ������ ���� �۽�
	//---------------------------------------------------------------------------------------	
	for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
	{	
		stPacket.nID			= iterPlayer->second.userInfo.id;
		stPacket.nPosAtRoom		= iterPlayer->second.userRoomInfo.m_nPosAtRoom;
		stPacket.nStateAtRoom	= iterPlayer->second.userRoomInfo.m_nStateAtRoom;
		stPacket.nJob			= iterPlayer->second.userRoomInfo.m_nJob;
		strcpy_s( stPacket.strGameID,  USER_ID_LEN, CONV_UTF8_MBCS(iterPlayer->second.userRoomInfo.m_strID.c_str()) );

		RegisterSendMsg( pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size);		
	}

	//---------------------------------------------------------------------------------------	
	//						�ű� �����ڸ� �ڷᱸ���� �߰�
	//---------------------------------------------------------------------------------------
	conPlayers.insert( std::make_pair( pPacket->nID, stPlayer ) );

	//---------------------------------------------------------------------------------------	
	//						�ű� �������� �濡���� ��ġ, ����, ������ ����
	//---------------------------------------------------------------------------------------

	PACKET_INIT_INFO stNewPlayer;
	stNewPlayer.nID				= pPacket->nID;
	stNewPlayer.nJob			= SOLIDIER;
	stNewPlayer.nStateAtRoom	= DEFAULT;
	stNewPlayer.nPosAtRoom		= stPlayer.userRoomInfo.m_nPosAtRoom;

	RegisterSendMsg( pClientInfo, reinterpret_cast<UCHAR*>(&stNewPlayer), stNewPlayer.size);		

	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	CGameRoomState* pRoomState = (CGameRoomState*)rStateManager.GetStateClass("GameRoom");

	if( pRoomState )
	{
		pRoomState->SetStaticText( stPlayer.userRoomInfo.m_nPosAtRoom, stPlayer.userRoomInfo.m_nJob, stPlayer.userRoomInfo.m_strID );
		pRoomState->InsertPlayerList( stPlayer.userRoomInfo.m_strID,  stPlayer.userInfo.id );
	}

	NetworkManager::GetInstance()->UnLockPlayers();
//	LeaveCriticalSection(&m_csPlayers);

	return true;
}

bool	CGameServer::ProcessChatMsg( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// ��Ŷ
	LPACKET_CHATMSG_AT_ROOM pPacket	= (LPACKET_CHATMSG_AT_ROOM)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 ������ �÷��̾�鿡�� ä�� �޽��� ����
	//---------------------------------------------------------------------------------------	
	
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
		}
	}	

	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->AddChatText( CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8(pPacket->strMsg) ) );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

bool	CGameServer::ProcessReady( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// ��Ŷ
	LPACKET_GAME_READY pPacket	= (LPACKET_GAME_READY)pBuf;
	BYTE nPos;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	nPos = conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom;
	conPlayers[ pPacket->nID ].userRoomInfo.m_nStateAtRoom = pPacket->nState;

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
		}
	}	

	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticTextAlpha( nPos, pPacket->nState );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

//	�Ϲ� ������ �� ��ü �޼��� ó��
bool	CGameServer::ProcessChangeTeam( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// ��Ŷ
	LPACKET_CHANGE_TEAM pPacket	= (LPACKET_CHANGE_TEAM)pBuf;
	BYTE nNewPos = 0;
	BYTE nOldPos = 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	// ���� ��ġ
	nOldPos = conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom;

	// �ű���� �ϴ� ������ �ű� �� �ִٸ�
	if( SearchEmptyPos( pPacket->nTeam, nOldPos, &nNewPos ) )
	{
		pPacket->nTeam = nNewPos;		
		
		conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom = nNewPos;

		EnterCriticalSection(&m_csPosAtRoom);
		m_nPlayerPosAtRoom[ nNewPos ] = TRUE; 
		m_nPlayerPosAtRoom[ nOldPos ] = FALSE;
		LeaveCriticalSection(&m_csPosAtRoom);

		if( !conPlayers.empty() )
		{
			PlayersMap::iterator	iterPlayer;
			for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			{						
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
			}
		}

		//---------------------------------------------------------------------------------------	
		//						������ UI ����
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->ChangePos( nOldPos, nNewPos );
	}

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}


// ������ �ڽ��� �ڸ��� �ٲܶ�
void CGameServer::ChangePos( BYTE nTeam )
{
	BYTE nNewPos = 0;
	BYTE nOldPos = 0;

	nOldPos = m_stCaptain.userRoomInfo.m_nPosAtRoom;

	if( SearchEmptyPos( nTeam, nOldPos, &nNewPos ) )
	{		
		m_stCaptain.userRoomInfo.m_nPosAtRoom = nNewPos;

		PACKET_CHANGE_TEAM stPacket;
		stPacket.nTeam = nNewPos;
		stPacket.nID = m_stCaptain.userInfo.id;		
		
		EnterCriticalSection(&m_csPosAtRoom);
		m_nPlayerPosAtRoom[ nNewPos ] = TRUE; 
		m_nPlayerPosAtRoom[ nOldPos ] = FALSE;
		LeaveCriticalSection(&m_csPosAtRoom);

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
		
		conPlayers[ m_stCaptain.userInfo.id ].userRoomInfo.m_nPosAtRoom = nNewPos;

		if( !conPlayers.empty() )
		{
			PlayersMap::iterator	iterPlayer;
			for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			{						
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
			}
		}

		//---------------------------------------------------------------------------------------	
		//						������ UI ����
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->ChangePos( nOldPos, nNewPos );

		NetworkManager::GetInstance()->UnLockPlayers();
	}	
}

bool CGameServer::ProcessChangeJob( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// ��Ŷ
	LPACKET_CHANGE_JOB pPacket	= (LPACKET_CHANGE_JOB)pBuf;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	// ���� ��ġ
	conPlayers[ pPacket->nID ].userRoomInfo.m_nJob = pPacket->nJob;

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
		}
	}
	
	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticText(
		conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom, pPacket->nJob, conPlayers[ pPacket->nID ].userRoomInfo.m_strID );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

//������ �����͸� ������Ʈ
void	CGameServer::ProcessChatMsgInGame(const unsigned char* pBuf,CLIENTINFO* pClientInfo)	
{
	// ��Ŷ
	LPACKET_CHATMSG_AT_GAME pPacket	= (LPACKET_CHATMSG_AT_GAME)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 ������ �÷��̾�鿡�� ä�� �޽��� ����
	//---------------------------------------------------------------------------------------	
	
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
		}
	}	

	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->AddChatText( CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8(pPacket->strMsg) ) );

	NetworkManager::GetInstance()->UnLockPlayers();

}

bool CGameServer::ProcessGameJoin( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	LPACKET_GAME_JOIN pPacket = (LPACKET_GAME_JOIN)pBuf;

	PlayersMap::iterator	iterPlayer;
	BYTE					nCntPlayers = 1;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	conPlayers[ pPacket->nID ].userRoomInfo.m_bInitialize = true;	
	
	// ��� ���� �Ͽ����� üũ
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{						
		if( iterPlayer->second.userRoomInfo.m_bInitialize )
			nCntPlayers++;
	}	

	// ��� ���� �Ͽ��ٸ�
	if( nCntPlayers == static_cast<BYTE>(conPlayers.size()) )
	{		
		// �κ� ���¿��� ���� ���·� State ��ȯ
		m_nCurNetState = nsPlayerInfo::NET_GAME_STATE;

		// ���� ���� ȭ������ ��ȯ �� ���� ����		
		NetworkManager::GetInstance()->CallStartEvent();
	}

	NetworkManager::GetInstance()->UnLockPlayers();
	
	
	return true;
}



// �¸��� �� enum�� �� Ŭ���̾�Ʈ�� �����ش�. (UI or �ش� �� Ŭ�� ����ġ ��� ��ư �� �׵��� ����)
bool	CGameServer::WinGame( BYTE nCamp )
{
	PACKET_WIN_GAME stPacket;

	stPacket.nCamp = nCamp;

	// ��� Ŭ�󿡰� �¸� �� ������.
	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();	
	
	for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
	}

	CStateManager &rStateManager = CGameApp::GetStateManager();	

	if( nCamp == conPlayers[ 0 ].userInfo.camp )
	{
		((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->WinGame( nCamp, true );
	}
	else
	{
		((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->WinGame( nCamp, false );
	}

	NetworkManager::GetInstance()->UnLockPlayers();	

	return true;
}

void	CGameServer::SendStartGame()
{
	PACKET_GAME_JOIN stPacket;

	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	// Ŭ��鿡�� �ε�ȭ�鿡�� ����ȭ�� ��ȯ �� ���� ����
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );	
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

//���� �޽��� ó��
void	CGameServer::ProcessTerritory(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	// �Ȱ��� ������ ������ ȮȮ ���� �Ͼ�� �ʰ� ������ �� TERRITORY_TIME(3��)���� ���� �ٲ�� �� ����
//	if( NiGetCurrentTimeInSec() - m_fTerritoryTime < TERRITORY_TIME )
//		return;

	LPACKET_UPDATE_TERRITORY pPacket = (LPACKET_UPDATE_TERRITORY)pBuf;

	BYTE nTerritoryUser		= pPacket->nID;			// ������ ��
	BYTE nNewTerritory		= pPacket->nTerritory;	// ���� ������ ��
	BYTE nPreviousTerritory	= 0;					// ������ �����ߴ� ��
	BYTE nOldCamp			= 0;					// ������ �����ߴ� ��	
	BYTE nNewCamp			= pPacket->nCamp;		// ������ ���ο� ��
	bool bWin				= false;
	BYTE bTeam				= 0;

	PlayersMap::iterator	iterPlayer;
	
	// ���� ���� ���¸� ���´�.
	EnterCriticalSection(&m_csStateTerritory);
	nOldCamp = m_nStateTerritory[ nNewTerritory ];	

	// ���� ���� ���� ���� ������ ��� ó������ �ʴ´�.
	if( nOldCamp == nNewCamp )
	{
		LeaveCriticalSection(&m_csStateTerritory);
		return;
	}

	// �������� ���Ѵ� ���
	if( nOldCamp )
	{
		if( nOldCamp == nsPlayerInfo::USA  )
		{
			EnterCriticalSection(&m_csEU_Territory);
			EnterCriticalSection(&m_csUSA_Territory);			

			if( !m_listUSA_Territory.empty() )
			{				
				std::list<BYTE>::iterator iterTer;
				for( iterTer =  m_listUSA_Territory.begin(); iterTer != m_listUSA_Territory.end(); ++iterTer )
				{
					if( nNewTerritory == *iterTer )
					{
						m_listUSA_Territory.erase( iterTer );
						break;
					}
				}
				
				if( !m_listUSA_Territory.empty() )
					nPreviousTerritory = m_listUSA_Territory.back();
				else
					nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;

			}
			// �������� 1���� ������ ���� ��ġ�� ����
			else
			{
				nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;
			}
			
			// ���� ������ ���� �� �������� �ڷᱸ���� �߰� ������ ���µ� ����!!
			m_listEU_Territory.push_back( nNewTerritory );
			m_nStateTerritory[ nNewTerritory ] =  nsPlayerInfo::EU;

			// ���� �� ���� �ߴٸ� �¸� ó��
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);			
		}
		// EU ��� EU�� ���� �������� ����
		else
		{
			EnterCriticalSection(&m_csEU_Territory);
			EnterCriticalSection(&m_csUSA_Territory);

			if( !m_listEU_Territory.empty() )
			{
				std::list<BYTE>::iterator iterTer;
				for( iterTer =  m_listEU_Territory.begin(); iterTer != m_listEU_Territory.end(); ++iterTer )
				{
					if( nNewTerritory == *iterTer )
					{
						m_listEU_Territory.erase( iterTer );
						break;
					}
				}

				if( !m_listEU_Territory.empty() )
					nPreviousTerritory = m_listEU_Territory.back();
				else
					nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;

			}			
			else
			{
				nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;
			}
			
			m_listUSA_Territory.push_back( nNewTerritory );
			m_nStateTerritory[ nNewTerritory ] =  nsPlayerInfo::USA;

			// �� ���� �ߴٸ� �¸� ó��
			if( m_listUSA_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::USA;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);		
		}

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			// �Ʊ��� ���ο� �������� ����
			if( iterPlayer->second.userInfo.camp == nNewCamp )
			{
				pPacket->nCamp				= nNewCamp;
				pPacket->nTerritory			= nNewTerritory;

				pPacket->nRenderCamp		= nNewCamp;
				pPacket->nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
			}
			//������ ���� �������� ����
			else
			{
				pPacket->nCamp				= nOldCamp;
				pPacket->nTerritory			= nPreviousTerritory;
				
				pPacket->nRenderCamp		= nNewCamp;
				pPacket->nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );					
			}

		}
		
		// ����� ���� �Ʊ��̸� ���嵵 ��� ȭ�� ����
		if( conPlayers[0].userInfo.camp == nNewCamp )
		{
			conPlayers[0].userInfo.territory = nNewTerritory;						
		}
		else
		{
			conPlayers[0].userInfo.territory = nPreviousTerritory;
		}

		CTerritory::GetInstance()->UpdateTerritory( nNewCamp, nNewTerritory );

		//==============================================================================
		//				UI ����
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		  conPlayers[ nTerritoryUser ].userRoomInfo.m_strID, nNewCamp );
		
		NetworkManager::GetInstance()->UnLockPlayers();
	}
	// �������� ���� ������ �ȵǾ� �ְ�, ���� ���ɵǾ��ٸ�,
	else
	{
		m_nStateTerritory[ nNewTerritory ] =  nNewCamp;

		if( nNewCamp == nsPlayerInfo::USA )
		{
			EnterCriticalSection(&m_csUSA_Territory);
			m_listUSA_Territory.push_back( nNewTerritory );
			if( m_listUSA_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::USA;
			}
			LeaveCriticalSection(&m_csUSA_Territory);
		}
		else
		{
			EnterCriticalSection(&m_csEU_Territory);
			m_listEU_Territory.push_back( nNewTerritory );
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}
			LeaveCriticalSection(&m_csEU_Territory);
		}

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		if( conPlayers[0].userInfo.camp == nNewCamp )
		{
			conPlayers[0].userInfo.territory = nNewTerritory;
		}

		// ���� ��� �������� ����		
		CTerritory::GetInstance()->UpdateTerritory( nNewCamp, nNewTerritory );

		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
				pPacket->nCamp		= nNewCamp;
				pPacket->nTerritory = nNewTerritory;

				pPacket->nRenderCamp		= nNewCamp;
				pPacket->nRenderTerritory	= nNewTerritory;
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
		}

		//==============================================================================
		//				UI ����
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		  conPlayers[ nTerritoryUser ].userRoomInfo.m_strID, nNewCamp );

		NetworkManager::GetInstance()->UnLockPlayers();		
	}

	LeaveCriticalSection(&m_csStateTerritory);

	// �¸�
	if( bWin )
	{
		WinGame( bTeam );
	}

	// ������ �ð� ����
//	m_fTerritoryTime = NiGetCurrentTimeInSec();
}

void	CGameServer::SendUpdateTerritory(BYTE nTerritory)
{
	// �Ȱ��� ������ ������ ȮȮ ���� �Ͼ�� �ʰ� ������ �� TERRITORY_TIME(3��)���� ���� �ٲ�� �� ����
//	if( NiGetCurrentTimeInSec() - m_fTerritoryTime < TERRITORY_TIME )
//		return;

 	BYTE nNewTerritory		= nTerritory;				// ���� ������ ��
	BYTE nPreviousTerritory	= 0;						// ������ �����ߴ� ��
	BYTE nOldCamp			= 0;						// ������ �����ߴ� ��	
	BYTE nNewCamp			= m_stCaptain.userInfo.camp;// ������ ���ο� ��
	bool bWin				= false;
	BYTE bTeam				= 0;

	PlayersMap::iterator	iterPlayer;
	PACKET_UPDATE_TERRITORY stPacket;

	stPacket.nID	= m_stCaptain.userInfo.id;

	// ���� ���� ���¸� ���´�.
	EnterCriticalSection(&m_csStateTerritory);
	nOldCamp = m_nStateTerritory[ nNewTerritory ];	

	// �츮 �������� �˻�ó�� ���� �ʴ´�.
	if( nOldCamp == nNewCamp )
	{
		LeaveCriticalSection(&m_csStateTerritory);
		return;
	}

	// ������ ������ ���� �ִٸ�
	if( nOldCamp )
	{
		// USA ��� USA�� ���� �������� ����
		if( nOldCamp == nsPlayerInfo::USA  )
		{
			EnterCriticalSection(&m_csEU_Territory);
			EnterCriticalSection(&m_csUSA_Territory);			

			if( !m_listUSA_Territory.empty() )
			{
				std::list<BYTE>::iterator iterTer;
				for( iterTer =  m_listUSA_Territory.begin(); iterTer != m_listUSA_Territory.end(); ++iterTer )
				{
					if( nNewTerritory == *iterTer )
					{
						m_listUSA_Territory.erase( iterTer );
						break;
					}
				}
				
				if( !m_listUSA_Territory.empty() )
					nPreviousTerritory = m_listUSA_Territory.back();
				else
					nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;

			}
			
			m_listEU_Territory.push_back( nNewTerritory );
			m_nStateTerritory[ nNewTerritory ] =  nsPlayerInfo::EU;

			// �� ���� �ߴٸ� �¸� ó��
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);		

		}
		// EU ��� EU�� ���� �������� ����
		else
		{
			EnterCriticalSection(&m_csEU_Territory);
			EnterCriticalSection(&m_csUSA_Territory);

			if( !m_listEU_Territory.empty() )
			{
				std::list<BYTE>::iterator iterTer;
				for( iterTer =  m_listEU_Territory.begin(); iterTer != m_listEU_Territory.end(); ++iterTer )
				{
					if( nNewTerritory == *iterTer )
					{
						m_listEU_Territory.erase( iterTer );
						break;
					}
				}

				if( !m_listEU_Territory.empty() )
					nPreviousTerritory = m_listEU_Territory.back();
				else
					nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;
			}
			
			m_listUSA_Territory.push_back( nNewTerritory );
			m_nStateTerritory[ nNewTerritory ] =  nsPlayerInfo::USA;

			// �� ���� �ߴٸ� �¸� ó��
			if( m_listUSA_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::USA;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);	
		}

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
		
		conPlayers.begin()->second.userInfo.territory = nNewTerritory;
		
		// ���� ��� �������� ����
		CTerritory::GetInstance()->UpdateTerritory( nNewCamp, nNewTerritory );

		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			// �Ʊ��� ���ο� �������� ����
			if( iterPlayer->second.userInfo.camp == nNewCamp )
			{
				stPacket.nCamp				= nNewCamp;
				stPacket.nTerritory			= nNewTerritory;

				stPacket.nRenderCamp		= nNewCamp;
				stPacket.nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
			}
			//������ ���� �������� ����
			else
			{
				stPacket.nCamp			= nOldCamp;
				stPacket.nTerritory		= nPreviousTerritory;

				stPacket.nRenderCamp	= nNewCamp;
				stPacket.nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );					
			}
		}

		//==============================================================================
		//				UI ����
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		 conPlayers[ 0 ].userRoomInfo.m_strID, nNewCamp );

		NetworkManager::GetInstance()->UnLockPlayers();
	}
	// �������� ���� ������ �ȵǾ� �ְ�, ���� ���ɵǾ��ٸ�,
	else
	{
		m_nStateTerritory[ nNewTerritory ] =  nNewCamp;

		if( nNewCamp == nsPlayerInfo::USA )
		{
			EnterCriticalSection(&m_csUSA_Territory);
			m_listUSA_Territory.push_back( nNewTerritory );
			if( m_listUSA_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::USA;
			}
			LeaveCriticalSection(&m_csUSA_Territory);
		}
		else
		{
			EnterCriticalSection(&m_csEU_Territory);
			m_listEU_Territory.push_back( nNewTerritory );
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}
			LeaveCriticalSection(&m_csEU_Territory);
		}

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		conPlayers.begin()->second.userInfo.territory = nNewTerritory;		

		// ���� ��� �������� ����
		CTerritory::GetInstance()->UpdateTerritory( nNewCamp, nNewTerritory );

		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			stPacket.nCamp				= nNewCamp;
			stPacket.nTerritory			= nNewTerritory;

			stPacket.nRenderCamp		= nNewCamp;
			stPacket.nRenderTerritory	= nNewTerritory;

			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
		}

		//==============================================================================
		//				UI ����
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		 conPlayers[ 0 ].userRoomInfo.m_strID, nNewCamp );

		NetworkManager::GetInstance()->UnLockPlayers();

	}

	LeaveCriticalSection(&m_csStateTerritory);

	if( bWin )
	{
		WinGame( bTeam );
	}

	// ������ �ð� ����
//	m_fTerritoryTime = NiGetCurrentTimeInSec();
}

// �ð� ����� �¸� ���� ����
void CGameServer::CheckWinTeam()
{
	unsigned int nUSA_Territory = 0, nEU_Territory = 0 ;

	EnterCriticalSection(&m_csEU_Territory);
	EnterCriticalSection(&m_csUSA_Territory);			

	nUSA_Territory	= m_listUSA_Territory.size();
	nEU_Territory	= m_listEU_Territory.size();

	// USA�� �������� �� ���ٸ�
	if( nUSA_Territory > nEU_Territory )
	{
		WinGame( nsPlayerInfo::USA );
	}
	// EU�� �������� �� ���ٸ�
	else
	{
		WinGame( nsPlayerInfo::EU );
	}

	LeaveCriticalSection(&m_csUSA_Territory);
	LeaveCriticalSection(&m_csEU_Territory);
}


BYTE CGameServer::GetPosAtRoom()
{
	return m_stCaptain.userRoomInfo.m_nPosAtRoom;
}


BYTE	CGameServer::SearchEmptyPos( BYTE nType, BYTE nOldPos, BYTE* nNewPos  )
{

	using namespace nsPlayerInfo;

	BOOL	bSearchPos = FALSE;
	BYTE	nUSA = 0;
	BYTE	nEU = 0;
	BYTE	nOldTeam = NetworkManager::GetInstance()->GetTeamType(nOldPos);	

	EnterCriticalSection(&m_csPosAtRoom);

	for( BYTE i = 0; i < MAX_TEAM_PLAYER; i++ )
	{
		if( !m_nPlayerPosAtRoom[ i ])
		{
			if( !bSearchPos && nType == USA )
			{
				*nNewPos = i;
				bSearchPos = TRUE;
			}
		}
		else
			nUSA++;
	}

	for( BYTE i = MAX_TEAM_PLAYER; i < MAX_TEAM_PLAYER*2; i++ )
	{
		if( !m_nPlayerPosAtRoom[ i ] )
		{
			if( !bSearchPos && nType == EU )
			{
				*nNewPos = i;
				bSearchPos = TRUE;
			}
		}
		else
			nEU++;
	}

	if( !bSearchPos )
	{
		if( !m_nPlayerPosAtRoom[ MAX_TEAM_PLAYER*2 ] && 
			nType == GEURRILLA0 )
		{
			*nNewPos = MAX_TEAM_PLAYER*2;
			bSearchPos = TRUE;
		}

		if( !m_nPlayerPosAtRoom[ MAX_TEAM_PLAYER*2+1 ] && 
			nType == GEURRILLA1 )
		{
			*nNewPos = MAX_TEAM_PLAYER*2+1;
			bSearchPos = TRUE;
		}
	}

	LeaveCriticalSection(&m_csPosAtRoom);


	// �ٲܷ��� ���� USA �ε�, ���� ���� �ִ� EU���� �ο����� ���ٸ�
	if( nType == USA && nOldTeam == EU )
	{
		if( nUSA > nEU )
			bSearchPos = FALSE;		
	}
	else if( nType == EU && nOldTeam == USA )
	{
		if( nEU > nUSA )
			bSearchPos = FALSE;		
	}
	else if( nOldTeam == GEURRILLA0 || nOldTeam == GEURRILLA1 )
	{
		if( nType == USA )
		{
			if( nUSA > nEU )
				bSearchPos = FALSE;	
		}
		else if( nType == EU )
		{
			if( nEU > nUSA )
			bSearchPos = FALSE;		
		}			
	}

	return bSearchPos;
}

void	CGameServer::SendChangeJob(BYTE nJob)
{
	PACKET_CHANGE_JOB stPacket;

	stPacket.nID	= m_stCaptain.userInfo.id;
	stPacket.nJob	= m_stCaptain.userRoomInfo.m_nJob = nJob;
	
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_nJob = nJob;

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
		}
	}

	//---------------------------------------------------------------------------------------	
	//						������ UI ����
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticText(
		m_stCaptain.userRoomInfo.m_nPosAtRoom, m_stCaptain.userRoomInfo.m_nJob, m_stCaptain.userRoomInfo.m_strID );

	NetworkManager::GetInstance()->UnLockPlayers();

}

bool	CGameServer::StartGame()
{
	// ���� ������ �̿��Ͽ� userinfo�� �������� ���� �ִϸ��̼��� �����Ѵ�.
	using namespace nsPlayerInfo;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		m_bCntInitPlyers = static_cast<BYTE>( conPlayers.size() );

		if( 1 == m_bCntInitPlyers )
			return false;

		//	��� ���� �������� üũ!!
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{	
			// �غ�Ϸ� �Ǵ� ���� ���°� �ƴ� ����Ʈ �����̸�
			if(  DEFAULT == iterPlayer->second.userRoomInfo.m_nStateAtRoom )
			{			
				NetworkManager::GetInstance()->UnLockPlayers();
				return false;
			}
		}

		PACKET_CHANGE_GAMESTATE stPacket;	
		
		//	���� Data���� �濡 ���� ��ġ�� �̿��Ͽ� ���� �� ����
		for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
		{	
			// ������ġ �� ķ�� ����
			iterPlayer->second.userInfo.camp				= NetworkManager::GetInstance()->GetTeamType( iterPlayer->second.userRoomInfo.m_nPosAtRoom,false );

			// ������ ���� ���� ĳ���� Ÿ�� ����
			iterPlayer->second.userInfo.BranchOfMilitary	= NetworkManager::GetInstance()->GetCharacterIndex( 
				iterPlayer->second.userInfo.camp, iterPlayer->second.userRoomInfo.m_nJob );
		}

		// �ڽ��� ���� ����
		NetworkManager::GetInstance()->SetSelfData(
		conPlayers[ NetworkManager::GetInstance()->GetID() ] );

		m_stCaptain = conPlayers[ 0 ];

		// ȭ�� ��ȯ
		CStateManager &rStateManager = CGameApp::GetStateManager();
		rStateManager.ChangeState("GamePlay");

//		// �κ� ���¿��� ���� ���·� State ��ȯ
//		m_nCurNetState = NET_GAME_STATE;

		// ��Ŷ ����
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );
				
	}

	NetworkManager::GetInstance()->UnLockPlayers();	

	return true;	
}


void	CGameServer::SendChatMsg( const CEGUI::String& strChatMsg, BYTE stateType )
{

	CEGUI::String strMsg = "[";
	strMsg += m_stCaptain.userRoomInfo.m_strID;
	strMsg += "] ";
	strMsg += strChatMsg;

	// �κ񿡼� ä�� ó��
	if( nsPlayerInfo::NET_ROOM_STATE == stateType )
	{
		PACKET_CHATMSG_AT_ROOM stPacket;
		strcpy_s( stPacket.strMsg, MAX_CHAT_LEN, CONV_UTF8_MBCS(strMsg.c_str()) );

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		if( !conPlayers.empty() )
		{
			PlayersMap::iterator	iterPlayer;
			for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			{						
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
			}
		}
		
		//---------------------------------------------------------------------------------------	
		//						������ UI ����
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->AddChatText( strMsg );

		NetworkManager::GetInstance()->UnLockPlayers();


	}
	//	���� �߿��� ä�� ó��
	else if( nsPlayerInfo::NET_GAME_STATE == stateType )
	{
		PACKET_CHATMSG_AT_GAME stPacket;
		strcpy_s( stPacket.strMsg, MAX_CHAT_LEN, CONV_UTF8_MBCS(strMsg.c_str()) );

		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		if( !conPlayers.empty() )
		{
			PlayersMap::iterator	iterPlayer;
			for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
			{						
				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
			}
		}
		
		//---------------------------------------------------------------------------------------	
		//						������ UI ����
		//---------------------------------------------------------------------------------------
		
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->AddChatText( strMsg );

		NetworkManager::GetInstance()->UnLockPlayers();
		
	}
}



void	CGameServer::SendExitAtRoom()
{
	PACKET_EXIT_GAMELOBBY stPacket;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{						
			RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
		}
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

BYTE	CGameServer::GetEmptyPosAtRoom()
{
	EnterCriticalSection(&m_csPosAtRoom);

	BYTE nCntUSA = 0, nCntEU = 0, nPos = 0;
	BOOL bGeullirra0 = FALSE, bGeullirra1 = FALSE;

	for( BYTE i = 0; i < (MAX_TEAM_PLAYER+1)*2; i++ )
	{
		if( m_nPlayerPosAtRoom[ i ] )
		{
			if( i < MAX_TEAM_PLAYER )
			{
				nCntUSA++;
			}
			else if( MAX_TEAM_PLAYER <= i && i < MAX_TEAM_PLAYER * 2)
			{
				nCntEU++;
			}
			else if( i == MAX_TEAM_PLAYER * 2 )
			{
				bGeullirra0 = TRUE;
			}
			else if( i ==  MAX_TEAM_PLAYER * 2 + 1 )
			{
				bGeullirra1 = TRUE;
			}
		}
	}

	if( nCntUSA < MAX_TEAM_PLAYER && nCntUSA <= nCntEU )
	{
		for( BYTE i = 0; i < MAX_TEAM_PLAYER; i++ )
			if( !m_nPlayerPosAtRoom[ i ] )
			{
				nPos = i;
				break;
			}
	}
	else if( nCntEU < MAX_TEAM_PLAYER && nCntEU < nCntUSA )
	{
		for( BYTE i = MAX_TEAM_PLAYER; i < MAX_TEAM_PLAYER*2; i++ )
			if( !m_nPlayerPosAtRoom[ i ] )
			{
				nPos = i;
				break;
			}
	}
	else if( !bGeullirra0 )
	{
		nPos = MAX_TEAM_PLAYER*2;
	}
	else if( !bGeullirra1 )
	{
		nPos = MAX_TEAM_PLAYER*2+1;
	}

	m_nPlayerPosAtRoom[ nPos ] = TRUE;

	LeaveCriticalSection(&m_csPosAtRoom);

	return nPos;
}

//�������� �߻��� fire �����͸� ó��
void CGameServer::SendSelfGunfireData( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessGunFire(pBuf,NULL);
}
//�������� �߻��� fire result ó��
void CGameServer::SendSelfFireResult( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessFireResult(pBuf,NULL);
}

//�������� �߻��� ���� ������ ó��
void CGameServer::SelfSoundData( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessSoundData(pBuf,NULL);
}

//�������� �߻��� ���º��� ������ ó��
void CGameServer::SelfChangeStatus( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessChangeStatus(pBuf,NULL);
}



//�������� ���ӵ� ��� Ŭ���̾�Ʈ �鿡�� �ð� ����
void CGameServer::SendTimeToClient(float fTime)
{
	struct _PACKET_PLAYTIME		packet;

	packet.time = fTime;

	//��� �����ڿ��� ����
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;

	for(iterPlayer = conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)&packet,packet.size);
	}
	NetworkManager::GetInstance()->UnLockPlayers();	
}


bool CGameServer::PacketProcessAtGame(const UCHAR* pBuf,CLIENTINFO* pClientInfo)
{
		DWORD	dwType = 0;

	//��Ŷ�� Ÿ���� �о�´�.
	dwType = (int)pBuf[1];	
		
	switch(dwType)
	{

	case UNIT_INFO:		
		ProcessUpdateUser(pBuf,pClientInfo);
		break;

	case GUN_FIRE:
		ProcessGunFire(pBuf,pClientInfo);
		break;

	case FIRE_RESULT:
		ProcessFireResult(pBuf,pClientInfo);
		break;

	case CTOS_CHAT_IN_GAME:			
		ProcessChatMsgInGame(pBuf,pClientInfo);
		break;

	case UPDATE_TERRITORY:
		ProcessTerritory(pBuf,pClientInfo);
		break;
	
	case SOUND_DATA:		//Ŭ���̾�Ʈ���� ���۵� ���� ������ ó��
		ProcessSoundData(pBuf,pClientInfo);
		break;

	case CHANGE_STATUS:
		ProcessChangeStatus(pBuf,pClientInfo);
		break;

	default:
		return false;
	}

	return true;
}