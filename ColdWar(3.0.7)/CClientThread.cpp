#include "CClientThread.h"
#include "GameClient.h"
#include "NetworkManager.h"
#include "GameApp.h"
#include "CStateManager.h"
#include "CGameRoomState.h"
#include "WorldManager.h"
//#include "ZFLog.h"
#include "strconv.h"


CClientThread::CClientThread()
{

}

CClientThread::~CClientThread()
{

}

//스레드 생성
bool CClientThread::Create(CGameClient *pNetSystem)
{
	m_pNetSystem = pNetSystem;

	m_pkThread = NiThread::Create(this);
	assert(m_pkThread);
	
	m_pkThread->SetPriority(NiThread::BELOW_NORMAL);
	m_pkThread->Resume();
	
	return true;
}

int CClientThread::Suspend()
{
	return m_pkThread->Suspend();
}

int CClientThread::Resume()
{
	return m_pkThread->Resume();
}

void CClientThread::EndThread()
{
	m_bRunning = false;
	
	//스레드가 끝나길 기다린다.
	if(m_pkThread)
		m_pkThread->WaitForCompletion();

	//끝났으면 스레드 객체 제거
	if(m_pkThread)
	{
		NiDelete m_pkThread;
		m_pkThread = NULL;
	}
}


unsigned int CClientThread::ThreadProcedure(void *pvArg)
{
	char buf[512];
	char remainBuf[512];

	DWORD	dwRemainSize = 0;
	DWORD	dwPacketSize = 0;
	DWORD	dwType = 0;
	DWORD	id = 0;

	ZeroMemory(remainBuf,sizeof(remainBuf));
	
	char*	pPacket = NULL;

	pPacket = buf;
	//서버와의 통신
	while(m_pNetSystem->GetConnecting())
	{		
		DWORD	dwRecvbytes = 0;
		dwRecvbytes = recv(m_pNetSystem->GetSocket(),pPacket,CLIENT_BUFSIZE,0);

		if(dwRecvbytes == SOCKET_ERROR)
			break;
		else if(dwRecvbytes == 0)
			break;

		//이전에 처리 못한 데이터가 있으면 받은데이터랑 통합
		if(dwRemainSize > 0)
		{
			memcpy(remainBuf + dwRemainSize,pPacket,CLIENT_BUFSIZE);
			memcpy(buf,remainBuf,512);
		}

		dwRemainSize += dwRecvbytes;
	
		while(1)
		{
			//패킷의 크기를 받는다.
			dwPacketSize = (int)pPacket[0];
			if(dwRemainSize < dwPacketSize)
			{
				ZeroMemory(remainBuf,sizeof(remainBuf));
				//남은 데이터를 버퍼에 저장
				memcpy(remainBuf,buf,dwRemainSize);
				break;
			}
			dwType = (int)pPacket[1];
		
			switch(dwType)
			{
				//------------------------------------------------------------------------------------------------------
				//
				//						방 관련 처리
				//
				//------------------------------------------------------------------------------------------------------

			case STOC_INIT_INFO:

				m_pNetSystem->MySelfInfoAtRoom( pPacket );
				break;

				// 새로운 플레이어 추가
			case STOC_ADD_PLAYER:

				m_pNetSystem->AddPlayer( pPacket );
				break;

				//	플레이어 1명 제거
			case CTOS_DISCONNECT_ROOM:

				m_pNetSystem->DisconnectPlayer( pPacket );
				break;

			case CTOS_CHAT_IN_ROOM:

				m_pNetSystem->AddChatMsg( pPacket );
				break;

			case CTOS_GAME_READY:

				m_pNetSystem->ReadyPlayer( pPacket );
				break;

			case CTOS_CHANGE_TEAM:

				m_pNetSystem->ChangeTeam( pPacket );
				break;

			case CTOS_CHANGE_JOB:
				
				m_pNetSystem->ChangeJob( pPacket );
				break;

			case STOC_EXIT_GAMELOBBY:

				m_pNetSystem->ExitLobby( pPacket );
				break;

			case STOC_CHANGE_GAMESTATE:

				m_pNetSystem->ChangeGameState( pPacket );
				break;

			case CTOS_GAME_JOIN:

				m_pNetSystem->StartGame( pPacket );
				break;
				
				
				//------------------------------------------------------------------------------------------------------
				//
				//						게임 관련 처리
				//
				//------------------------------------------------------------------------------------------------------
				
			case GIVE_ID:	//초기 connect 후에 id를 부여 받음
				{
					struct give_id	*packet1 = (struct give_id*)pPacket;
					
					CStateManager &rStateManager = CGameApp::GetStateManager();

					// 자료구조 추가
					//PlayersMap conPlayers;
					GAME_USER_INFO	stMySelfInfo;
					stMySelfInfo.userInfo.id = (BYTE) packet1->id;
					stMySelfInfo.userRoomInfo.m_strID = (CEGUI::utf8*)((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID();

					PACKET_CONNECT_ROOM stPacket1;
					strcpy_s( stPacket1.strGameID, USER_ID_LEN, ((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID() );
					
					PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
					conPlayers.insert( std::make_pair(  packet1->id, stMySelfInfo ) );
					NetworkManager::GetInstance()->UnLockPlayers();

					// 방 접속 요청
						
					PACKET_CONNECT_ROOM stPacket;
					stPacket.nID = stMySelfInfo.userInfo.id;
					strcpy_s( stPacket.strGameID, USER_ID_LEN, CONV_UTF8_MBCS(((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID()) );
					
					m_pNetSystem->SendData( reinterpret_cast<void*>( &stPacket), stPacket.size );

					NetworkManager::GetInstance()->SetID(stMySelfInfo.userInfo.id);
//					m_pNetSystem->SetMyID( id );

				}
				break;
			case ADD_USER:	//유저 정보 추가
				{
					GAME_USER_INFO gameUserInfo;
					struct user_info *packet1 = (struct user_info*)pPacket;
										
					gameUserInfo.pClientInfo = NULL;
					gameUserInfo.userInfo = packet1->info;

					NetworkManager::GetInstance()->AddUserData(gameUserInfo);

//					ZFLog::g_sLog->Log("TYPE : ADD_USER");
				}
				break;
			case UNIT_INFO:	//유저의 정보 업데이트
				{
					struct user_info* packet1 = (struct user_info*)pPacket;
					UserInfo info(packet1->info);
					NetworkManager::GetInstance()->UpdateUserData(info);
				}
				break;

			case GUN_FIRE:	//총 발사 
				{
					struct _PACKET_GUN_FIRE* packet = (struct _PACKET_GUN_FIRE*)pPacket;
					
					NetworkManager::GetInstance()->RegisterFireInfo(packet->fireInfo);
				}
				break;

			case FIRE_RESULT:
				{
					struct _PACKET_FIRE_RESULT* packet = (struct _PACKET_FIRE_RESULT*)pPacket;

					NetworkManager::GetInstance()->RegisterFireResult(packet->fireResult);
				}
				break;

			case PLAY_TIME:
				{
					struct _PACKET_PLAYTIME* packet = (_PACKET_PLAYTIME*)pPacket;
					WorldManager::GetWorldManager()->GetGameSystemManager()->SetGameTime(packet->time);
				}
				break;

			case SOUND_DATA:	//서버에서 전송받은 사운드 데이터 처리
				{
					struct _PACKET_SOUND_DATA* packet = (_PACKET_SOUND_DATA*)pPacket;

					CUserCharacter* pkCharacter = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();

					NiTransform kTrans = pkCharacter->GetCharRoot()->GetWorldTransform();
					
					CSoundManager::GetInstance()->RegisterSound(packet->eSoundCode,packet->kSoundSpot,kTrans,packet->b2DSound);
				}
				break;

			case CHANGE_STATUS:	//서버에서 전송받은 상태 변경 메시지 처리
				{
					struct _PACKET_CHANGE_STATUS* packet = (_PACKET_CHANGE_STATUS*)pPacket;

					CUserCharacter* pkCharacter = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();
					pkCharacter->SetStatus(packet->status);
				}
				break;
			case CTOS_CHAT_IN_GAME:

				m_pNetSystem->AddChatMsgInGame( pPacket );
				break;

			case STOC_EXIT_GAMEPLAY:

				m_pNetSystem->ExitGame( pPacket );
				break;


			case UPDATE_TERRITORY:
				{
					struct _PACKET_UPDATE_TERRITORY* packet = (_PACKET_UPDATE_TERRITORY*)pPacket;
					m_pNetSystem->UpdateTerritory( (const char*)packet );
//					ZFLog::g_sLog->Log("점령지 업데이트");
					break;
				}

			case WIN_GAME:

				m_pNetSystem->WinGame( pPacket );
				break;

			}
			//패킷 크기보다 크게 온 경우
			if(dwRemainSize > dwPacketSize)
			{
				//다음 패킷을 앞으로 복사
				memcpy(buf,buf + dwPacketSize,dwRemainSize - dwPacketSize);
				//뒷 내용은 제거
				memset(buf + dwRemainSize - dwPacketSize,0,CLIENT_BUFSIZE - (dwRemainSize - dwPacketSize));

				pPacket = buf;
				dwRemainSize -= dwPacketSize;

				continue;
			}
			else
			{
				memset(buf,0,CLIENT_BUFSIZE);
				pPacket = buf;
				dwRemainSize = 0;
				break;
			}
		}	
	}	
	
	return 0;
}