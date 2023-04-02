/*
	��Ʈ��ũ�� ���� ���������� ����� �Ǵ� ����ü�� ��� ����

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

#define MAX_SOCKBUF			20	//recv�� ���� ���� ũ��
#define MAX_PACKET_SIZE		255
#define MAX_WORKERTHREAD	4
#define USEPORT				9000	//����� ��Ʈ ��ȣ
#define MAX_CLIENT			20		//�ִ� Ŭ���̾�Ʈ ����
#define NET_DELAY			80		//��Ʈ��ũ ������

//���� ���� ����ü
struct SOCKETINFO
{
	WSAOVERLAPPED		wsaOverlapped;
	WSABUF				wsaBuf;
	UCHAR				m_IOCPbuf[MAX_PACKET_SIZE];				//IOCP send/recv ����
	UCHAR				ucPacketBuf[MAX_PACKET_SIZE];			//recv�Ǵ� ��Ŷ�� �����Ǵ� ����
	queue<UCHAR*>		m_SendPacketQueue;						//���� ��Ŷ���� ������ �����̳�
	int					nRemainLen;								//�̿Ϸ�� ��Ŷ ũ�⸦ ����
	int					nPackagingSize;							//���� �������� ��Ŷ�� ũ��
	int					nReceived;								//���� �� ����
	enumOperation		eOperation;			
	bool				bBusySending;							//���� IOCP���� ������ �������� ǥ��
	CRITICAL_SECTION	send_queue_lock;
};

//Ŭ���̾�Ʈ ������ ��� ���� ����ü
struct CLIENTINFO
{
	//int				id;				//���� ����� ID
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
		InitializeCriticalSection(&sendSockInfo.send_queue_lock);		//�Ӱ迵�� �ʱ�ȭ
	}
};

//�����鿡 ���� ����
struct GAME_USER_INFO
{
	CLIENTINFO*		pClientInfo;	//Ŭ���̾�Ʈ ���� ���� ����ü
	/*
		.... �߰����� ���� ���
	*/
	//���� ����
	UserInfo		userInfo;

	UserRoomInfo	userRoomInfo;

	GAME_USER_INFO()
	{
		ZeroMemory(this,sizeof(GAME_USER_INFO));
		userInfo.territory = nsPlayerInfo::TERRITORY_NO;
	}
};


#endif
