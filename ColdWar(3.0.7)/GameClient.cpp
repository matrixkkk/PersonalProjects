#include "GameClient.h"
//#include "ZFLog.h"
#include "GameApp.h"
#include "CStateManager.h"
#include "CGameRoomState.h"
#include "CGamePlayState.h"
#include "NetWorkManager.h"
#include "CharacterManager.h"
#include "CMenuState.h"
#include "strconv.h"
#include "WorldManager.h"
#include "CTerritory.h"

SOCKET				CGameClient::m_socket = NULL;
CGameClient*	CGameClient::m_gClient = NULL;

CGameClient::CGameClient()
{
	//윈속 초기화
	if(WSAStartup(MAKEWORD(2,2),&m_wsa) != 0)
	{
		Err_Display("WSAStartup()");
		exit(1);
	}
	
	m_pClientThread = NULL;

	m_bConnect = false;

	m_fLastSendData = 0;
}

CGameClient::~CGameClient()
{
	//윈속 종료
	WSACleanup();
}

//클라이언트 네트워크 시작
bool CGameClient::Start()
{
	if(!Init())
		return false;

	if(!Connect())
		return false;

	return true;
}

//네트워크 초기화
bool CGameClient::Init()
{
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_socket == INVALID_SOCKET)
	{
		Err_Display("Socket()");
		return false;
	}


	return true;
}

//서버와 연결 -  통신을 위한 스레드 생성
bool CGameClient::Connect()
{
	int retval;

	DWORD	dwServerIP;
	//서버 주소를 text에서 읽어 온다.
//	FILE *pFile = NULL;
//	fopen_s(&pFile,"server.txt","r");

//	if(pFile == NULL)
//		return false;

//	char address[30];

//	fgets(address,sizeof(address),pFile);
//	fclose(pFile);

//	dwServerIP = inet_addr(address);
	dwServerIP = inet_addr(NetworkManager::GetInstance()->GetIP());


	
	SOCKADDR_IN	serveraddr;
	ZeroMemory(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = dwServerIP;//inet_addr("127.0.0.1");

	//소켓 옵션 셋
	BOOL bNoDelay = TRUE;
	retval = setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&bNoDelay,sizeof(bNoDelay));
	if(retval == SOCKET_ERROR)
	{
		Err_Display("setsockopt()");
		return false;
	}
	retval = WSAConnect(m_socket,(SOCKADDR*)&serveraddr,sizeof(serveraddr),NULL,NULL,NULL,NULL);
	if(retval == SOCKET_ERROR)
	{
		Err_Display("connect()");
		return false;
	}

	//데이터 전송용 스레드 생성
	m_pClientThread = NiNew CClientThread;
	if(m_pClientThread == NULL)
		return false;

	if(!m_pClientThread->Create(this))
		return false;

	
	m_bConnect = true;
	
	return true;
}

//클라이언트 종료
bool CGameClient::End()
{
//	if( !m_mapPlayers.empty() )
//		m_mapPlayers.clear();

//	DeleteCriticalSection(&m_csPlayers);

	DisConnect();

	return true;
}


//----------------------------
//연결을 해제
bool CGameClient::DisConnect()
{
	m_bConnect = false;

	if(m_socket)
		closesocket(m_socket);

	WSACleanup();

	return true;
}

//데이타를 보낸다.
bool CGameClient::SendData(void* pData,unsigned char pSendbytes)
{
	WSABUF buf;
	buf.buf = (char*)pData;
	buf.len = (LONG)pSendbytes;
	DWORD dataSize = (DWORD) pSendbytes;

	if(WSASend(m_socket,&buf,1,&dataSize,0,NULL,NULL) == SOCKET_ERROR)
	{
//		Err_Display("send()");
		return false;
	}

	return true;
}
bool CGameClient::SendDelayData(void* pData,unsigned char pSendbytes)
{
	//네트워크 딜레이 적용
	float fDelta = NiGetCurrentTimeInSec() - m_fLastSendData;
	if(fDelta < NET_DELAY/1000)
	{
		return true;
	}
	WSABUF	buf;
	buf.buf = (char*)pData;
	buf.len = (LONG)pSendbytes;
	DWORD dataSize = (DWORD) pSendbytes;
	
	if(WSASend(m_socket,&buf,1,&dataSize,0,NULL,NULL) == SOCKET_ERROR)
	{
//		Err_Display("send()");
		return false;
	}
	//마지막으로 데이터를 보낸 시간 저장
	m_fLastSendData = NiGetCurrentTimeInSec();

	return true;
}

SOCKET	CGameClient::GetSocket()
{
	return m_socket;
}
CGameClient* CGameClient::GetInstance()
{
	return m_gClient;
}

bool CGameClient::GetConnecting()
{
	return m_bConnect;
}


//에러 메세지 출력 하는 함수
void CGameClient::Err_Display(char* msg)
{
	LPVOID lpMsgBuf;
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL,WSAGetLastError(),
				  MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
				  (LPTSTR)&lpMsgBuf,0,NULL);
	MessageBox(NULL,(LPCTSTR)lpMsgBuf,msg,MB_ICONERROR);
	LocalFree(lpMsgBuf);
}



//------------------------------------------------------------
//
//				Packet Process Function 
//
//------------------------------------------------------------	

void	CGameClient::MySelfInfoAtRoom( const char *pBuf )
{
	LPACKET_INIT_INFO stPacket = (LPACKET_INIT_INFO)pBuf;
//	PlayersMap				conPlayers;
	PlayersMap::iterator	iterPlayer;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	iterPlayer = conPlayers.find(NetworkManager::GetInstance()->GetID());

	if( iterPlayer != conPlayers.end() )
	{
		iterPlayer->second.userRoomInfo.m_nPosAtRoom	= stPacket->nPosAtRoom;
		iterPlayer->second.userRoomInfo.m_nJob			= stPacket->nJob;
		iterPlayer->second.userRoomInfo.m_nStateAtRoom	= stPacket->nStateAtRoom;	

		CStateManager &rStateManager = CGameApp::GetStateManager();	

		CGameRoomState* pRoomState = (CGameRoomState*)rStateManager.GetStateClass("GameRoom");

		PACKET_CONNECT_ROOM pp;
		strcpy_s( pp.strGameID,  USER_ID_LEN, CONV_UTF8_MBCS(iterPlayer->second.userRoomInfo.m_strID.c_str()) );

		if( pRoomState )
		{
			pRoomState->SetStaticText( iterPlayer->second.userRoomInfo.m_nPosAtRoom, iterPlayer->second.userRoomInfo.m_nJob, iterPlayer->second.userRoomInfo.m_strID );
			pRoomState->InsertPlayerList( iterPlayer->second.userRoomInfo.m_strID, iterPlayer->second.userInfo.id );
			pRoomState->SetStaticTextAlpha( iterPlayer->second.userRoomInfo.m_nPosAtRoom, iterPlayer->second.userRoomInfo.m_nStateAtRoom );
		}

	}
	NetworkManager::GetInstance()->UnLockPlayers();
}

void	CGameClient::AddPlayer( const char *pBuf )
{
	GAME_USER_INFO gameUserInfo;
	LPACKET_ADD_PLAYER stPacket = (LPACKET_ADD_PLAYER)pBuf;
						
	gameUserInfo.pClientInfo			= NULL;
	
	gameUserInfo.userInfo.id				= stPacket->nID;
	gameUserInfo.userRoomInfo.m_nPosAtRoom	= stPacket->nPosAtRoom;
	gameUserInfo.userRoomInfo.m_nStateAtRoom= stPacket->nStateAtRoom;
	gameUserInfo.userRoomInfo.m_nJob		= stPacket->nJob;
	gameUserInfo.userRoomInfo.m_strID		= (CEGUI::utf8*)CONV_MBCS_UTF8(stPacket->strGameID);

	//PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	conPlayers.insert( std::make_pair(stPacket->nID, gameUserInfo) );
	
	CStateManager &rStateManager = CGameApp::GetStateManager();	

	CGameRoomState* pRoomState = (CGameRoomState*)rStateManager.GetStateClass("GameRoom");

	if( pRoomState )
	{
		pRoomState->SetStaticText( gameUserInfo.userRoomInfo.m_nPosAtRoom, gameUserInfo.userRoomInfo.m_nJob, gameUserInfo.userRoomInfo.m_strID );
		pRoomState->InsertPlayerList( gameUserInfo.userRoomInfo.m_strID, gameUserInfo.userInfo.id );
		pRoomState->SetStaticTextAlpha( gameUserInfo.userRoomInfo.m_nPosAtRoom, gameUserInfo.userRoomInfo.m_nStateAtRoom );
	}

	NetworkManager::GetInstance()->UnLockPlayers();

}

void	CGameClient::DisconnectPlayer( const char *pBuf )
{
	LPACKET_DISCONNECT_PLAYER_AT_ROOM stPacket = (LPACKET_DISCONNECT_PLAYER_AT_ROOM)pBuf;	

	//PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	CStateManager &rStateManager = CGameApp::GetStateManager();	

	CGameRoomState* pRoomState = (CGameRoomState*)rStateManager.GetStateClass("GameRoom");

	if( pRoomState )
	{
		pRoomState->SetEmptyStaticText( conPlayers[ stPacket->nID ].userRoomInfo.m_nPosAtRoom );
		pRoomState->RemovePlayerList( conPlayers[ stPacket->nID ].userRoomInfo.m_strID, stPacket->nID );
	}
	
	conPlayers.erase( stPacket->nID );

	NetworkManager::GetInstance()->UnLockPlayers();

}
void	CGameClient::AddChatMsg( const char *pBuf )
{
	LPACKET_CHATMSG_AT_ROOM stPacket = (LPACKET_CHATMSG_AT_ROOM)pBuf;	

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->AddChatText( 
		CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8(stPacket->strMsg) ) );
}

void	CGameClient::ReadyPlayer( const char *pBuf )
{
	LPACKET_GAME_READY stPacket = (LPACKET_GAME_READY)pBuf;	

	BYTE nPos;

//	PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	nPos = conPlayers[ stPacket->nID ].userRoomInfo.m_nPosAtRoom;
	conPlayers[ stPacket->nID ].userRoomInfo.m_nStateAtRoom = stPacket->nState;
	
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticTextAlpha( nPos, stPacket->nState );		

	NetworkManager::GetInstance()->UnLockPlayers();
}

void CGameClient::ChangeTeam( const char *pBuf )
{
	LPACKET_CHANGE_TEAM stPacket = (LPACKET_CHANGE_TEAM)pBuf;
	BYTE nOldPos = 0;
	BYTE nNewPos = stPacket->nTeam;

//	PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	nOldPos = conPlayers[ stPacket->nID ].userRoomInfo.m_nPosAtRoom;	
	conPlayers[ stPacket->nID ].userRoomInfo.m_nPosAtRoom = stPacket->nTeam;

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->ChangePos( nOldPos, nNewPos );

	NetworkManager::GetInstance()->UnLockPlayers();
}

void CGameClient::ChangeJob( const char *pBuf )
{
	LPACKET_CHANGE_JOB stPacket = (LPACKET_CHANGE_JOB)pBuf;

//	PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	conPlayers[ stPacket->nID ].userRoomInfo.m_nJob = stPacket->nJob;

	CStateManager &rStateManager = CGameApp::GetStateManager();	

	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetStaticText( 
		conPlayers[ stPacket->nID ].userRoomInfo.m_nPosAtRoom, stPacket->nJob, 
		conPlayers[ stPacket->nID ].userRoomInfo.m_strID );

	NetworkManager::GetInstance()->UnLockPlayers();
}

void CGameClient::ChangeGameState( const char *pBuf )
{	
//	PlayersMap		conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	if( !conPlayers.empty() )
	{
		
		PACKET_CHANGE_GAMESTATE stPacket;

		PlayersMap::iterator iterPlayer;
		//	방에 현재 위치를 이용하여 최종 팀 결정
		for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
		{	
			// 시작위치 팀 캠프 결정
			iterPlayer->second.userInfo.camp				= NetworkManager::GetInstance()->GetTeamType( iterPlayer->second.userRoomInfo.m_nPosAtRoom,false );

			// 직업과 팀에 따른 캐릭터 타입 결정
			iterPlayer->second.userInfo.BranchOfMilitary	= NetworkManager::GetInstance()->GetCharacterIndex( iterPlayer->second.userInfo.camp,
																				iterPlayer->second.userRoomInfo.m_nJob );
		}		
	}

	// 자신의 정보 셋팅
	NetworkManager::GetInstance()->SetSelfData(
		conPlayers[ NetworkManager::GetInstance()->GetID() ] );

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	rStateManager.ChangeState("GamePlay");

	NetworkManager::GetInstance()->UnLockPlayers();
}

void	CGameClient::StartGame( const char *pBuf )
{
	Sleep(100);
	NetworkManager::GetInstance()->CallStartEvent();
}


void CGameClient::ExitLobby( const char *pBuf )
{
	NetworkManager::GetInstance()->End();

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	rStateManager.ChangeState("MainMenu");
}

void CGameClient::AddChatMsgInGame( const char *pBuf )
{
	LPACKET_CHATMSG_AT_GAME stPacket = (LPACKET_CHATMSG_AT_GAME)pBuf;	

	// UI 갱신

	CStateManager &rStateManager = CGameApp::GetStateManager();	

	((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->AddChatText( 
		CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8( stPacket->strMsg) ) );
}

void CGameClient::UpdateKill( const char *pBuf )
{
	LPACKET_UPDATE_KILL stPacket = (LPACKET_UPDATE_KILL)pBuf;
	BYTE nTeam = 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	conPlayers[ stPacket->nID ].userInfo.kill = stPacket->nKill;
	nTeam = conPlayers[ stPacket->nID ].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();

	CStateManager &rStateManager = CGameApp::GetStateManager();	

//	((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->UpdateKill( stPacket->nID, nTeam, stPacket->nKill );
}

void CGameClient::UpdateDeath( const char *pBuf )
{
	LPACKET_UPDATE_DEATH stPacket = (LPACKET_UPDATE_DEATH)pBuf;
	BYTE nTeam = 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	
	conPlayers[ stPacket->nID ].userInfo.kill = stPacket->nDeath;
	nTeam = conPlayers[ stPacket->nID ].userInfo.camp;

	NetworkManager::GetInstance()->UnLockPlayers();

	CStateManager &rStateManager = CGameApp::GetStateManager();	

//	((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->UpdateDeath( stPacket->nID, nTeam, stPacket->nDeath );

}

void CGameClient::UpdateTerritory( const char *pBuf )
{

	LPACKET_UPDATE_TERRITORY stPacket = (LPACKET_UPDATE_TERRITORY)pBuf;
	BYTE nTeam = 0;

	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		if( iterPlayer->second.userInfo.camp == stPacket->nCamp )
		{
			iterPlayer->second.userInfo.territory = stPacket->nTerritory;
		}
	}
	
	//========================================================================
	//    "아무개가 점령했습니다." 표시 or 점령지 깃발 렌더 변경
	//========================================================================

	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	((CGamePlayState*)rStateManager.GetStateClass("GamePlay"))->ActiveTerritory(
		conPlayers[ stPacket->nID ].userRoomInfo.m_strID, stPacket->nRenderTerritory + 1);

	CTerritory::GetInstance()->UpdateTerritory( stPacket->nRenderCamp, stPacket->nRenderTerritory);

	NetworkManager::GetInstance()->UnLockPlayers();
}

void CGameClient::WinGame( const char *pBuf )
{
	LPACKET_WIN_GAME stPacket = (LPACKET_WIN_GAME)pBuf;

	BYTE nID = NetworkManager::GetInstance()->GetID();
	CStateManager &rStateManager = CGameApp::GetStateManager(); 
	CGamePlayState *pGamePlay = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	// 승리
	if( conPlayers[ nID ].userInfo.camp == stPacket->nCamp )
	{
		pGamePlay->WinGame( stPacket->nCamp, true );
	}
	// 패
	else
	{
		pGamePlay->WinGame( stPacket->nCamp, false );
	}

	NetworkManager::GetInstance()->UnLockPlayers();
}

void CGameClient::ExitGame( const char *pBuf )
{
	//네트워크 종료
//	NetworkManager::Destroy();
	//월드 렌더링 Data Destroy
//	WorldManager::Destroy();

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	rStateManager.ChangeState("MainMenu");
}

/*
//클라이언트에서 사격 광선으로 충돌 계산
void CGameClient::FirePerform(const char *pBuf)
{
	struct _PACKET_GUN_FIRE* packet1 = (_PACKET_GUN_FIRE*)pPacket;

	WorldManager::GetWorldManager()->PerformPick(packet1->kOrigin,packet1->kDirection);	
}
*/

//------------------------------------------------------------
//
//				Request Command Function 
//
//------------------------------------------------------------	

BYTE CGameClient::GetPosAtRoom()
{
//	PlayersMap	conPlayers;
	BYTE nPosAtRoom	= 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	nPosAtRoom = conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_nPosAtRoom;
	NetworkManager::GetInstance()->UnLockPlayers();

	return nPosAtRoom;
}

BYTE	CGameClient::GetCurrentState()
{
//	PlayersMap	conPlayers;
	BYTE nStateAtRoom = 0;

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	nStateAtRoom = conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_nStateAtRoom;
	NetworkManager::GetInstance()->UnLockPlayers();

	return nStateAtRoom;
}

void	CGameClient::RequestSendChat( const CEGUI::String& strChatMsg, BYTE stateType )
{
	if( strChatMsg.size() )
	{

		if( nsPlayerInfo::NET_ROOM_STATE == stateType )
		{

			PACKET_CHATMSG_AT_ROOM stPacket;
			
			PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

			CEGUI::String strMsg = "[";
			strMsg += conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_strID;
			strMsg += "] ";
			strMsg += strChatMsg;

			NetworkManager::GetInstance()->UnLockPlayers();

			strcpy_s( stPacket.strMsg, MAX_CHAT_LEN - 1, CONV_UTF8_MBCS(strMsg.c_str()) );
		
			SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 

		}
		else if( nsPlayerInfo::NET_GAME_STATE == stateType )
		{

			PACKET_CHATMSG_AT_GAME stPacket;
			
			PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

			CEGUI::String strMsg = "[";
			strMsg += conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_strID;
			strMsg += "] ";
			strMsg += strChatMsg;

			NetworkManager::GetInstance()->UnLockPlayers();

			strcpy_s( stPacket.strMsg, MAX_CHAT_LEN, CONV_UTF8_MBCS(strMsg.c_str()) );
		
			SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 


		}
	}
}


void	CGameClient::RequestReady()
{
	BYTE nState;
	PACKET_GAME_READY stPacket;

//	PlayersMap	conPlayers;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	nState	= conPlayers[ NetworkManager::GetInstance()->GetID() ].userRoomInfo.m_nStateAtRoom;
	
	if( nState == nsPlayerInfo::READY )
		nState = nsPlayerInfo::DEFAULT;
	else
		nState = nsPlayerInfo::READY;

	stPacket.nID	=  NetworkManager::GetInstance()->GetID();
	stPacket.nState = nState;

	NetworkManager::GetInstance()->UnLockPlayers();

	SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 
}

void	CGameClient::RequestChangeTeam( BYTE nTeam )
{
	PACKET_CHANGE_TEAM stPacket;

	stPacket.nID	=  NetworkManager::GetInstance()->GetID();
	stPacket.nTeam	= nTeam;

	SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 
}

void	CGameClient::RequestChangeJob( BYTE nJob )
{
	PACKET_CHANGE_JOB stPacket;

	stPacket.nID	=  NetworkManager::GetInstance()->GetID();
	stPacket.nJob	= nJob;

	SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 
}

void	CGameClient::RequestGameJoin()
{
	PACKET_GAME_JOIN stPacket;

	stPacket.nID =  NetworkManager::GetInstance()->GetID();

	SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 
}


void CGameClient::RequestTerritory( BYTE nCamp, BYTE nTerritory )
{
	PACKET_UPDATE_TERRITORY stPacket;

	stPacket.nID		= NetworkManager::GetInstance()->GetID();
	stPacket.nTerritory = nTerritory;
	stPacket.nCamp		= nCamp;

	SendData( reinterpret_cast< void* >( &stPacket ), stPacket.size ); 

}