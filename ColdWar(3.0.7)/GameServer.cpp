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

// 서버시작
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

	// 초기 시작은 로비 방 상태
	m_nCurNetState	= NET_ROOM_STATE;

	InitializeCriticalSection(&m_csStateTerritory);
	InitializeCriticalSection(&m_csUSA_Territory);
	InitializeCriticalSection(&m_csEU_Territory);
	InitializeCriticalSection(&m_csPosAtRoom);

	m_fTerritoryTime = NiGetCurrentTimeInSec();

	CNetSystemServer::Begin();

	return true;
}

// 서버시작
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

	//접속자에게 ID 전송
	struct give_id packet1;
	packet1.size = sizeof(packet1);
	packet1.type = GIVE_ID;
	packet1.id = info.userInfo.id;

	RegisterSendMsg(pClientInfo,(unsigned char*)&packet1,packet1.size );
	
#ifdef NET_TEST
	char temp[128];
	sprintf_s(temp,sizeof(temp),"패킷 타입 = GIVE_ID 사이즈 = %d",packet1.size);
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
		//						나가는 유저의 고유 ID와 자료구조에서 제거
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
		//						방에 남은 유저들에게 제거 메시지 전송
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
		//						방장의 UI 갱신
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
	//	게임중에 나갔을 때 처리
	else
	{


	}
		/*
		GAME_USER_INFO userInfo;

		int id;

		CGameApp::mp_Appication->EnterCS();
		
		vector<GAME_USER_INFO>& vecUsers = NetworkManager::GetInstance()->GetVecUserInfo();
		
		//접속이 끊긴 유저의 ID와 캐릭터 정보를 공유데이터에서 제거
		for(unsigned int i=1;i<vecUsers.size();i++)
		{
			if(vecUsers[i].pClientInfo->socketClient == pClientInfo->socketClient)
			{
				id = vecUsers[i].userInfo.id;
				NetworkManager::GetInstance()->DeleteUser(id);
				
				char temp[128];
				sprintf_s(temp,sizeof(temp),"ID : %d 번 접속종료",id);
//				ZFLog::g_sLog->Log(temp);
				break;
			}
		}
		CGameApp::mp_Appication->LeaveCS();
		
		//다른 접속자들에게 접속 해제를 알리는 패킷 패킹
		struct another_disconnect packet1;
		packet1.size = sizeof(packet1);
		packet1.type = ANOTHER_DISCONNECT;
		packet1.id = id;

		CGameApp::mp_Appication->EnterCS();
		
		//다른 접속자들에게 접속해제를 알림
		for(unsigned int i=0;i<vecUsers.size();i++)
		{
			//접속해제 정보 전송
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
	
	//받은 유저 정보를 접속된 모든 클라이언트에게 전송
	vector<GAME_USER_INFO>& vecUsers = NetworkManager::GetInstance()->GetVecUserInfo();
	
	for(unsigned int i=1;i<vecUsers.size();i++)
	{
		struct user_info* pPacket1 = (struct user_info*)pBuf;
		RegisterSendMsg( vecUsers[i].pClientInfo, (unsigned char*)&pPacket, pPacket->size );
	}

	//현재 접속자에게 기존접속자의 정보를 전송
	for(unsigned int i=0;i<vecUsers.size();i++)
	{
		struct user_info packet;
			
		GAME_USER_INFO gameUser = vecUsers[i];
		
		packet.info = gameUser.userInfo;
		packet.type = ADD_USER;
		packet.size = sizeof(packet);

		RegisterSendMsg(pClientInfo,(unsigned char*)&packet,packet.size);

	}	

	//네트워크 매니져에 유저 목록 추가
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
		
	//서버 유저의 정보를 업데이트 한다.
	NetworkManager::GetInstance()->UpdateUserData(info->info);
	
	//모든 플레이어들에게 수정된 정보를 전송	
	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayer = NetworkManager::GetInstance()->LockPlayers();

	for( iterPlayer = ++(conPlayer.begin()); iterPlayer != conPlayer.end(); ++iterPlayer )
	{
		RegisterSendMsg( iterPlayer->second.pClientInfo, (unsigned char*)&sendInfo, sendInfo.size);
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

//발사 광선 처리
void CGameServer::ProcessGunFire(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	struct _PACKET_GUN_FIRE* pPacket = (_PACKET_GUN_FIRE*)pBuf;

	BYTE id = (BYTE)pPacket->fireInfo.damageUserID;
	
	//총에 맞은 사람에게 광선 정보 전송
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;
	
	//서버에서도 처리
	NetworkManager::GetInstance()->RegisterFireInfo(pPacket->fireInfo);
	
	//다른 클라이언트라면 모든 유저에게 전송

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}	

	NetworkManager::GetInstance()->UnLockPlayers();	
}
//발사 결과 처리
void CGameServer::ProcessFireResult(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	struct _PACKET_FIRE_RESULT* pPacket = (_PACKET_FIRE_RESULT*)pBuf;

	//모든 접속자에게 전송
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}
	NetworkManager::GetInstance()->UnLockPlayers();
	
	//서버 자체 처리
	NetworkManager::GetInstance()->RegisterFireResult(pPacket->fireResult);
}

//사운드 데이터 처리
void CGameServer::ProcessSoundData(const unsigned char* pBuf,CLIENTINFO *pClientInfo)
{
	struct _PACKET_SOUND_DATA* pPacket = (_PACKET_SOUND_DATA*)pBuf;

	//모든 접속자에게 전송
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	PlayersMap::iterator iterPlayer;

	for(iterPlayer = ++conPlayers.begin();iterPlayer != conPlayers.end();++iterPlayer)
	{
		RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)pPacket,pPacket->size);
	}
	NetworkManager::GetInstance()->UnLockPlayers();

	//서버 자체 처리
	NiTransform		kTrans	  = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter()->GetCharRoot()->GetWorldTransform(); 
		
	CSoundManager::GetInstance()->RegisterSound(pPacket->eSoundCode,pPacket->kSoundSpot,kTrans,pPacket->b2DSound);
}

//상태 변경 메시지 처리
void CGameServer::ProcessChangeStatus(const unsigned char* pBuf,CLIENTINFO* pClientInfo)	//상태 변경 메시지 처리
{
	//이 메시지는 플레이어의 상태만 변경 하는 것이므로 모든 클라이언트에게 보낼 필요가 없다
	//이유는 어차피 매 프레임마다 플레이어의 상태를 전송해주기 때문이다(쓸데없는 오버헤드를 피함)
	struct _PACKET_CHANGE_STATUS* pPacket = (_PACKET_CHANGE_STATUS*)pBuf;

	BYTE desID = pPacket->id;
	CLIENTINFO* pDestClientInfo;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	pDestClientInfo = conPlayers[desID].pClientInfo;				//바꿀 플레이어의 ID의 Client정보를 가져옴
	RegisterSendMsg(pDestClientInfo,(UCHAR*)pPacket,pPacket->size);	//그 플레이어에게만 전송	

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

	//패킷의 타입을 읽어온다.
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
	// 패킷
	LPACKET_CONNECT_ROOM pPacket	= (LPACKET_CONNECT_ROOM)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 플레이어 생성
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
	//						기존 유저들에게 신규 유저 정보 송신
	//---------------------------------------------------------------------------------------		
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{						
		RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size);		
	}
	//---------------------------------------------------------------------------------------	
	//						신규 유저에게 기존 유저들 정보 송신
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
	//						신규 접속자를 자료구조에 추가
	//---------------------------------------------------------------------------------------
	conPlayers.insert( std::make_pair( pPacket->nID, stPlayer ) );

	//---------------------------------------------------------------------------------------	
	//						신규 접속자의 방에서의 위치, 상태, 직업을 전송
	//---------------------------------------------------------------------------------------

	PACKET_INIT_INFO stNewPlayer;
	stNewPlayer.nID				= pPacket->nID;
	stNewPlayer.nJob			= SOLIDIER;
	stNewPlayer.nStateAtRoom	= DEFAULT;
	stNewPlayer.nPosAtRoom		= stPlayer.userRoomInfo.m_nPosAtRoom;

	RegisterSendMsg( pClientInfo, reinterpret_cast<UCHAR*>(&stNewPlayer), stNewPlayer.size);		

	//---------------------------------------------------------------------------------------	
	//						방장의 UI 갱신
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
	// 패킷
	LPACKET_CHATMSG_AT_ROOM pPacket	= (LPACKET_CHATMSG_AT_ROOM)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 접속한 플레이어들에게 채팅 메시지 전송
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
	//						방장의 UI 갱신
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->AddChatText( CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8(pPacket->strMsg) ) );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

bool	CGameServer::ProcessReady( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// 패킷
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
	//						방장의 UI 갱신
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticTextAlpha( nPos, pPacket->nState );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

//	일반 유저의 팀 교체 메세지 처리
bool	CGameServer::ProcessChangeTeam( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// 패킷
	LPACKET_CHANGE_TEAM pPacket	= (LPACKET_CHANGE_TEAM)pBuf;
	BYTE nNewPos = 0;
	BYTE nOldPos = 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	// 기존 위치
	nOldPos = conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom;

	// 옮기고자 하는 팀으로 옮길 수 있다면
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
		//						방장의 UI 갱신
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->ChangePos( nOldPos, nNewPos );
	}

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}


// 방장이 자신이 자리를 바꿀때
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
		//						방장의 UI 갱신
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->ChangePos( nOldPos, nNewPos );

		NetworkManager::GetInstance()->UnLockPlayers();
	}	
}

bool CGameServer::ProcessChangeJob( const UCHAR* pBuf,CLIENTINFO* pClientInfo )
{
	// 패킷
	LPACKET_CHANGE_JOB pPacket	= (LPACKET_CHANGE_JOB)pBuf;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	// 기존 위치
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
	//						방장의 UI 갱신
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticText(
		conPlayers[ pPacket->nID ].userRoomInfo.m_nPosAtRoom, pPacket->nJob, conPlayers[ pPacket->nID ].userRoomInfo.m_strID );

	NetworkManager::GetInstance()->UnLockPlayers();

	return true;
}

//유저의 데이터를 업데이트
void	CGameServer::ProcessChatMsgInGame(const unsigned char* pBuf,CLIENTINFO* pClientInfo)	
{
	// 패킷
	LPACKET_CHATMSG_AT_GAME pPacket	= (LPACKET_CHATMSG_AT_GAME)pBuf;

	//---------------------------------------------------------------------------------------	
	//						 접속한 플레이어들에게 채팅 메시지 전송
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
	//						방장의 UI 갱신
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
	
	// 모두 조인 하였는지 체크
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{						
		if( iterPlayer->second.userRoomInfo.m_bInitialize )
			nCntPlayers++;
	}	

	// 모두 조인 하였다면
	if( nCntPlayers == static_cast<BYTE>(conPlayers.size()) )
	{		
		// 로비 상태에서 게임 상태로 State 전환
		m_nCurNetState = nsPlayerInfo::NET_GAME_STATE;

		// 방장 게임 화면으로 전환 및 렌더 시작		
		NetworkManager::GetInstance()->CallStartEvent();
	}

	NetworkManager::GetInstance()->UnLockPlayers();
	
	
	return true;
}



// 승리한 팀 enum을 각 클라이언트에 보내준다. (UI or 해당 팀 클라 경험치 등등 암튼 머 그딴거 위해)
bool	CGameServer::WinGame( BYTE nCamp )
{
	PACKET_WIN_GAME stPacket;

	stPacket.nCamp = nCamp;

	// 모든 클라에게 승리 팀 보낸다.
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
	
	// 클라들에게 로딩화면에서 게임화면 전환 및 렌더 시작
	for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );	
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

//점령 메시지 처리
void	CGameServer::ProcessTerritory(const unsigned char* pBuf,CLIENTINFO* pClientInfo)
{
	// 똑같은 곳에서 점령이 확확 자주 일어나지 않게 점령한 후 TERRITORY_TIME(3초)동안 점령 바뀌는 것 제한
//	if( NiGetCurrentTimeInSec() - m_fTerritoryTime < TERRITORY_TIME )
//		return;

	LPACKET_UPDATE_TERRITORY pPacket = (LPACKET_UPDATE_TERRITORY)pBuf;

	BYTE nTerritoryUser		= pPacket->nID;			// 점령한 놈
	BYTE nNewTerritory		= pPacket->nTerritory;	// 새로 점령한 곳
	BYTE nPreviousTerritory	= 0;					// 이전에 점령했던 곳
	BYTE nOldCamp			= 0;					// 기존에 점령했던 팀	
	BYTE nNewCamp			= pPacket->nCamp;		// 점령한 새로운 팀
	bool bWin				= false;
	BYTE bTeam				= 0;

	PlayersMap::iterator	iterPlayer;
	
	// 현재 점령 상태를 얻어온다.
	EnterCriticalSection(&m_csStateTerritory);
	nOldCamp = m_nStateTerritory[ nNewTerritory ];	

	// 같은 팀이 같은 곳을 점령한 경우 처리하지 않는다.
	if( nOldCamp == nNewCamp )
	{
		LeaveCriticalSection(&m_csStateTerritory);
		return;
	}

	// 점령지를 빼앗는 경우
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
			// 점령지가 1개도 없으면 시작 위치로 셋팅
			else
			{
				nPreviousTerritory = nsPlayerInfo::TERRITORY_NO;
			}
			
			// 새로 점령한 팀은 새 점령지를 자료구조에 추가 점령지 상태도 갱신!!
			m_listEU_Territory.push_back( nNewTerritory );
			m_nStateTerritory[ nNewTerritory ] =  nsPlayerInfo::EU;

			// 만약 다 점령 했다면 승리 처리
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);			
		}
		// EU 라면 EU의 이전 점령지로 변경
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

			// 다 점령 했다면 승리 처리
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
			// 아군은 새로운 점령지로 변경
			if( iterPlayer->second.userInfo.camp == nNewCamp )
			{
				pPacket->nCamp				= nNewCamp;
				pPacket->nTerritory			= nNewTerritory;

				pPacket->nRenderCamp		= nNewCamp;
				pPacket->nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );		
			}
			//적군은 이전 점령지로 변경
			else
			{
				pPacket->nCamp				= nOldCamp;
				pPacket->nTerritory			= nPreviousTerritory;
				
				pPacket->nRenderCamp		= nNewCamp;
				pPacket->nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(pPacket), pPacket->size );					
			}

		}
		
		// 방장과 같은 아군이면 방장도 깃발 화면 갱신
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
		//				UI 갱신
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		  conPlayers[ nTerritoryUser ].userRoomInfo.m_strID, nNewCamp );
		
		NetworkManager::GetInstance()->UnLockPlayers();
	}
	// 점령지가 아직 점령이 안되어 있고, 새로 점령되었다면,
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

		// 방장 깃발 렌더정보 갱신		
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
		//				UI 갱신
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		  conPlayers[ nTerritoryUser ].userRoomInfo.m_strID, nNewCamp );

		NetworkManager::GetInstance()->UnLockPlayers();		
	}

	LeaveCriticalSection(&m_csStateTerritory);

	// 승리
	if( bWin )
	{
		WinGame( bTeam );
	}

	// 점령한 시간 갱신
//	m_fTerritoryTime = NiGetCurrentTimeInSec();
}

void	CGameServer::SendUpdateTerritory(BYTE nTerritory)
{
	// 똑같은 곳에서 점령이 확확 자주 일어나지 않게 점령한 후 TERRITORY_TIME(3초)동안 점령 바뀌는 것 제한
//	if( NiGetCurrentTimeInSec() - m_fTerritoryTime < TERRITORY_TIME )
//		return;

 	BYTE nNewTerritory		= nTerritory;				// 새로 점령한 곳
	BYTE nPreviousTerritory	= 0;						// 이전에 점령했던 곳
	BYTE nOldCamp			= 0;						// 기존에 점령했던 팀	
	BYTE nNewCamp			= m_stCaptain.userInfo.camp;// 점령한 새로운 팀
	bool bWin				= false;
	BYTE bTeam				= 0;

	PlayersMap::iterator	iterPlayer;
	PACKET_UPDATE_TERRITORY stPacket;

	stPacket.nID	= m_stCaptain.userInfo.id;

	// 현재 점령 상태를 얻어온다.
	EnterCriticalSection(&m_csStateTerritory);
	nOldCamp = m_nStateTerritory[ nNewTerritory ];	

	// 우리 점령지면 검사처리 하지 않는다.
	if( nOldCamp == nNewCamp )
	{
		LeaveCriticalSection(&m_csStateTerritory);
		return;
	}

	// 기존에 점령한 팀이 있다면
	if( nOldCamp )
	{
		// USA 라면 USA의 이전 점령지로 변경
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

			// 다 점령 했다면 승리 처리
			if( m_listEU_Territory.size() == nsPlayerInfo::TERRITORY_MAX )
			{
				bWin = true;
				bTeam = nsPlayerInfo::EU;
			}

			LeaveCriticalSection(&m_csUSA_Territory);
			LeaveCriticalSection(&m_csEU_Territory);		

		}
		// EU 라면 EU의 이전 점령지로 변경
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

			// 다 점령 했다면 승리 처리
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
		
		// 방장 깃발 렌더정보 갱신
		CTerritory::GetInstance()->UpdateTerritory( nNewCamp, nNewTerritory );

		for( iterPlayer = (++conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			// 아군은 새로운 점령지로 변경
			if( iterPlayer->second.userInfo.camp == nNewCamp )
			{
				stPacket.nCamp				= nNewCamp;
				stPacket.nTerritory			= nNewTerritory;

				stPacket.nRenderCamp		= nNewCamp;
				stPacket.nRenderTerritory	= nNewTerritory;

				RegisterSendMsg( iterPlayer->second.pClientInfo, reinterpret_cast<UCHAR*>(&stPacket), stPacket.size );		
			}
			//적군은 이전 점령지로 변경
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
		//				UI 갱신
		//==============================================================================

		CStateManager &rStateManager = CGameApp::GetStateManager(); 
		 ((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		 conPlayers[ 0 ].userRoomInfo.m_strID, nNewCamp );

		NetworkManager::GetInstance()->UnLockPlayers();
	}
	// 점령지가 아직 점령이 안되어 있고, 새로 점령되었다면,
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

		// 방장 깃발 렌더정보 갱신
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
		//				UI 갱신
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

	// 점령한 시간 갱신
//	m_fTerritoryTime = NiGetCurrentTimeInSec();
}

// 시간 종료시 승리 여부 결정
void CGameServer::CheckWinTeam()
{
	unsigned int nUSA_Territory = 0, nEU_Territory = 0 ;

	EnterCriticalSection(&m_csEU_Territory);
	EnterCriticalSection(&m_csUSA_Territory);			

	nUSA_Territory	= m_listUSA_Territory.size();
	nEU_Territory	= m_listEU_Territory.size();

	// USA가 정령지가 더 많다면
	if( nUSA_Territory > nEU_Territory )
	{
		WinGame( nsPlayerInfo::USA );
	}
	// EU가 정령지가 더 많다면
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


	// 바꿀려는 팀이 USA 인데, 현재 내가 있는 EU보다 인원수가 많다면
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
	//						방장의 UI 갱신
	//---------------------------------------------------------------------------------------
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticText(
		m_stCaptain.userRoomInfo.m_nPosAtRoom, m_stCaptain.userRoomInfo.m_nJob, m_stCaptain.userRoomInfo.m_strID );

	NetworkManager::GetInstance()->UnLockPlayers();

}

bool	CGameServer::StartGame()
{
	// 현재 정보를 이용하여 userinfo의 최종적인 팀과 애니메이션을 결정한다.
	using namespace nsPlayerInfo;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		m_bCntInitPlyers = static_cast<BYTE>( conPlayers.size() );

		if( 1 == m_bCntInitPlyers )
			return false;

		//	모두 레뒤 상태인지 체크!!
		PlayersMap::iterator	iterPlayer;
		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{	
			// 준비완료 또는 상점 상태가 아닌 디폴트 상태이면
			if(  DEFAULT == iterPlayer->second.userRoomInfo.m_nStateAtRoom )
			{			
				NetworkManager::GetInstance()->UnLockPlayers();
				return false;
			}
		}

		PACKET_CHANGE_GAMESTATE stPacket;	
		
		//	서버 Data정리 방에 현재 위치를 이용하여 최종 팀 결정
		for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
		{	
			// 시작위치 팀 캠프 결정
			iterPlayer->second.userInfo.camp				= NetworkManager::GetInstance()->GetTeamType( iterPlayer->second.userRoomInfo.m_nPosAtRoom,false );

			// 직업과 팀에 따른 캐릭터 타입 결정
			iterPlayer->second.userInfo.BranchOfMilitary	= NetworkManager::GetInstance()->GetCharacterIndex( 
				iterPlayer->second.userInfo.camp, iterPlayer->second.userRoomInfo.m_nJob );
		}

		// 자신의 정보 셋팅
		NetworkManager::GetInstance()->SetSelfData(
		conPlayers[ NetworkManager::GetInstance()->GetID() ] );

		m_stCaptain = conPlayers[ 0 ];

		// 화면 전환
		CStateManager &rStateManager = CGameApp::GetStateManager();
		rStateManager.ChangeState("GamePlay");

//		// 로비 상태에서 게임 상태로 State 전환
//		m_nCurNetState = NET_GAME_STATE;

		// 패킷 전송
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

	// 로비에서 채팅 처리
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
		//						방장의 UI 갱신
		//---------------------------------------------------------------------------------------
		CStateManager &rStateManager = CGameApp::GetStateManager();	
		((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->AddChatText( strMsg );

		NetworkManager::GetInstance()->UnLockPlayers();


	}
	//	게임 중에서 채팅 처리
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
		//						방장의 UI 갱신
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

//서버에서 발생된 fire 데이터를 처리
void CGameServer::SendSelfGunfireData( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessGunFire(pBuf,NULL);
}
//서버에서 발생된 fire result 처리
void CGameServer::SendSelfFireResult( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessFireResult(pBuf,NULL);
}

//서버에서 발생된 사운드 데이터 처리
void CGameServer::SelfSoundData( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessSoundData(pBuf,NULL);
}

//서버에서 발생된 상태변경 데이터 처리
void CGameServer::SelfChangeStatus( const unsigned char* pBuf, DWORD byteSize)
{
	ProcessChangeStatus(pBuf,NULL);
}



//서버에서 접속된 모든 클라이언트 들에게 시간 전송
void CGameServer::SendTimeToClient(float fTime)
{
	struct _PACKET_PLAYTIME		packet;

	packet.time = fTime;

	//모든 접속자에게 전송
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

	//패킷의 타입을 읽어온다.
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
	
	case SOUND_DATA:		//클라이언트에서 전송된 사운드 데이터 처리
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