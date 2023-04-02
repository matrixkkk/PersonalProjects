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

//������ ����
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
	
	//�����尡 ������ ��ٸ���.
	if(m_pkThread)
		m_pkThread->WaitForCompletion();

	//�������� ������ ��ü ����
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
	//�������� ���
	while(m_pNetSystem->GetConnecting())
	{		
		DWORD	dwRecvbytes = 0;
		dwRecvbytes = recv(m_pNetSystem->GetSocket(),pPacket,CLIENT_BUFSIZE,0);

		if(dwRecvbytes == SOCKET_ERROR)
			break;
		else if(dwRecvbytes == 0)
			break;

		//������ ó�� ���� �����Ͱ� ������ ���������Ͷ� ����
		if(dwRemainSize > 0)
		{
			memcpy(remainBuf + dwRemainSize,pPacket,CLIENT_BUFSIZE);
			memcpy(buf,remainBuf,512);
		}

		dwRemainSize += dwRecvbytes;
	
		while(1)
		{
			//��Ŷ�� ũ�⸦ �޴´�.
			dwPacketSize = (int)pPacket[0];
			if(dwRemainSize < dwPacketSize)
			{
				ZeroMemory(remainBuf,sizeof(remainBuf));
				//���� �����͸� ���ۿ� ����
				memcpy(remainBuf,buf,dwRemainSize);
				break;
			}
			dwType = (int)pPacket[1];
		
			switch(dwType)
			{
				//------------------------------------------------------------------------------------------------------
				//
				//						�� ���� ó��
				//
				//------------------------------------------------------------------------------------------------------

			case STOC_INIT_INFO:

				m_pNetSystem->MySelfInfoAtRoom( pPacket );
				break;

				// ���ο� �÷��̾� �߰�
			case STOC_ADD_PLAYER:

				m_pNetSystem->AddPlayer( pPacket );
				break;

				//	�÷��̾� 1�� ����
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
				//						���� ���� ó��
				//
				//------------------------------------------------------------------------------------------------------
				
			case GIVE_ID:	//�ʱ� connect �Ŀ� id�� �ο� ����
				{
					struct give_id	*packet1 = (struct give_id*)pPacket;
					
					CStateManager &rStateManager = CGameApp::GetStateManager();

					// �ڷᱸ�� �߰�
					//PlayersMap conPlayers;
					GAME_USER_INFO	stMySelfInfo;
					stMySelfInfo.userInfo.id = (BYTE) packet1->id;
					stMySelfInfo.userRoomInfo.m_strID = (CEGUI::utf8*)((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID();

					PACKET_CONNECT_ROOM stPacket1;
					strcpy_s( stPacket1.strGameID, USER_ID_LEN, ((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID() );
					
					PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
					conPlayers.insert( std::make_pair(  packet1->id, stMySelfInfo ) );
					NetworkManager::GetInstance()->UnLockPlayers();

					// �� ���� ��û
						
					PACKET_CONNECT_ROOM stPacket;
					stPacket.nID = stMySelfInfo.userInfo.id;
					strcpy_s( stPacket.strGameID, USER_ID_LEN, CONV_UTF8_MBCS(((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->GetID()) );
					
					m_pNetSystem->SendData( reinterpret_cast<void*>( &stPacket), stPacket.size );

					NetworkManager::GetInstance()->SetID(stMySelfInfo.userInfo.id);
//					m_pNetSystem->SetMyID( id );

				}
				break;
			case ADD_USER:	//���� ���� �߰�
				{
					GAME_USER_INFO gameUserInfo;
					struct user_info *packet1 = (struct user_info*)pPacket;
										
					gameUserInfo.pClientInfo = NULL;
					gameUserInfo.userInfo = packet1->info;

					NetworkManager::GetInstance()->AddUserData(gameUserInfo);

//					ZFLog::g_sLog->Log("TYPE : ADD_USER");
				}
				break;
			case UNIT_INFO:	//������ ���� ������Ʈ
				{
					struct user_info* packet1 = (struct user_info*)pPacket;
					UserInfo info(packet1->info);
					NetworkManager::GetInstance()->UpdateUserData(info);
				}
				break;

			case GUN_FIRE:	//�� �߻� 
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

			case SOUND_DATA:	//�������� ���۹��� ���� ������ ó��
				{
					struct _PACKET_SOUND_DATA* packet = (_PACKET_SOUND_DATA*)pPacket;

					CUserCharacter* pkCharacter = WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter();

					NiTransform kTrans = pkCharacter->GetCharRoot()->GetWorldTransform();
					
					CSoundManager::GetInstance()->RegisterSound(packet->eSoundCode,packet->kSoundSpot,kTrans,packet->b2DSound);
				}
				break;

			case CHANGE_STATUS:	//�������� ���۹��� ���� ���� �޽��� ó��
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
//					ZFLog::g_sLog->Log("������ ������Ʈ");
					break;
				}

			case WIN_GAME:

				m_pNetSystem->WinGame( pPacket );
				break;

			}
			//��Ŷ ũ�⺸�� ũ�� �� ���
			if(dwRemainSize > dwPacketSize)
			{
				//���� ��Ŷ�� ������ ����
				memcpy(buf,buf + dwPacketSize,dwRemainSize - dwPacketSize);
				//�� ������ ����
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