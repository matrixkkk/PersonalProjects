/*
	네트워크를 위해 공통적으로 사용이 되는 구조체나 상수 정의

*/

#ifndef NETCOMMON_H_
#define NETCOMMON_H_

#include <winsock2.h>
#include <queue>
#include "protocol.h"

using namespace std;

enum enumOperation
{
	OP_RECV,OP_SEND
};

#define MAX_SOCKBUF			20	//recv를 위한 버퍼 크기
#define MAX_PACKET_SIZE		255
#define MAX_WORKERTHREAD	4
#define USEPORT				9000	//사용할 포트 번호
#define MAX_CLIENT			20		//최대 클라이언트 개수
#define NET_DELAY			80		//네트워크 딜레이

//소켓 정보 구조체
struct SOCKETINFO
{
	WSAOVERLAPPED		wsaOverlapped;
	WSABUF				wsaBuf;
	UCHAR				m_IOCPbuf[MAX_PACKET_SIZE];				//IOCP send/recv 버퍼
	UCHAR				ucPacketBuf[MAX_PACKET_SIZE];			//recv되는 패킷이 조립되는 버퍼
	queue<UCHAR*>		m_SendPacketQueue;						//보낼 패킷들을 저장할 컨테이너
	int					nRemainLen;								//미완료된 패킷 크기를 저장
	int					nPackagingSize;							//지금 조립중인 패킷의 크기
	int					nReceived;								//받은 양 저장
	enumOperation		eOperation;			
	bool				bBusySending;							//현재 IOCP에서 보내는 중인지를 표시
	CRITICAL_SECTION	send_queue_lock;
};

//클라이언트 정보를 담기 위한 구조체
struct CLIENTINFO
{
	//int				id;				//소켓 사용자 ID
	SOCKET			socketClient;
	SOCKETINFO		recvSockInfo;	
	SOCKETINFO		sendSockInfo;

	CLIENTINFO()
	{
		ZeroMemory(this,sizeof(*this));
		recvSockInfo.eOperation = OP_RECV;
		sendSockInfo.eOperation = OP_SEND;
		sendSockInfo.bBusySending = false;
		socketClient = INVALID_SOCKET;
		InitializeCriticalSection(&sendSockInfo.send_queue_lock);		//임계영역 초기화
	}
};

//유저들에 대한 정보
struct GAME_USER_INFO
{
	CLIENTINFO*		pClientInfo;	//클라이언트 소켓 정보 구조체
	/*
		.... 추가적인 정보 기술
	*/
	//유저 정보
	UserInfo		userInfo;

	UserRoomInfo	userRoomInfo;

	GAME_USER_INFO()
	{
		ZeroMemory(this,sizeof(GAME_USER_INFO));
		userInfo.territory = nsPlayerInfo::TERRITORY_NO;
	}
};


#endif
