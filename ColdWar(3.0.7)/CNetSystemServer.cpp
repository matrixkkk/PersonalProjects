#include "CNetSystemServer.h"
#include "CAcceptThread.h"
#include "CWorkerThread.h"


CNetSystemServer::CNetSystemServer()
{	
	m_nClientCnt = 0;
	m_hIOCP = NULL;
	m_SockListen = INVALID_SOCKET;
	ZeroMemory(m_szSocketBuffer,sizeof(m_szSocketBuffer));
	
	//Worker ������ �ʱ�ȭ
	for(int i=0;i<MAX_WORKERTHREAD ;i++)
		m_pWorkerThread[i] =NULL;

	m_pAcceptThread = NULL;

	//Client������ ������ ����ü�� ����
	m_pClientInfo = new CLIENTINFO[MAX_CLIENT];
}
CNetSystemServer::~CNetSystemServer()
{
	for(int i=0;i<MAX_WORKERTHREAD ;i++)
	{
		if(m_pWorkerThread[i])
		{
			NiDelete m_pWorkerThread[i];
			m_pWorkerThread[i] = NULL;
		}	
	}

	if(m_pAcceptThread)
	{
		NiDelete m_pAcceptThread;
		m_pAcceptThread = NULL;
	}
	
	if(m_pClientInfo)
	{
		NiDelete[] m_pClientInfo;
		m_pClientInfo = NULL;
	}
	WSACleanup();
}

//��Ʈ��ũ ����
bool CNetSystemServer::Begin()
{
	//���� ���̺귯�� ����, Listen���� ����
	if(!InitSocket())
		return false;

	if(!BindListen())
		return false;

	if(!StartServer())
		return false;

	return true;
}

bool CNetSystemServer::End()
{
	//������ ����������
	DestroyThread();
	
	//������ ��ü�� ����
	for(int i=0;i<MAX_CLIENT;i++)
	{
		if(m_pClientInfo[i].socketClient != INVALID_SOCKET)
		{
			CloseSocket(&m_pClientInfo[i]);
		}
	}
	return true;
}

bool CNetSystemServer::InitSocket()
{
	WSADATA	wsaData;

	int retval = WSAStartup(MAKEWORD(2,2),&wsaData);
	if(retval != 0)
		return false;

	//TCP Listen���� ����
	m_SockListen = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,NULL,WSA_FLAG_OVERLAPPED);
	if(m_SockListen == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}

bool CNetSystemServer::BindListen()
{
	SOCKADDR_IN		serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(USEPORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind
	int retval = bind(m_SockListen,(SOCKADDR*)&serverAddr,sizeof(SOCKADDR_IN));
	if(retval != 0)
		return false;

	//Listen
	retval = listen(m_SockListen,5);
	if(retval == SOCKET_ERROR)
		return false;

	return true;
}

bool CNetSystemServer::StartServer()
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if(m_hIOCP == NULL)
		return false;

	//���ӵ� Ŭ���̾�Ʈ �ּ� ������ ������ ����ü
	if(!CreateWorkerThread())
		return false;

	if(!CreateAccpeptThread())
		return false;

	return true;
}

void CNetSystemServer::CloseSocket(CLIENTINFO *pClientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0};	//SO_DONTLINGER�� ����

	//bISForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� 
	//���� ���� ��Ų��.
	if(bIsForce)
		stLinger.l_linger = 1;

	shutdown(pClientInfo->socketClient,SD_BOTH);
	//���� �ɼ��� �����Ѵ�.
	setsockopt(pClientInfo->socketClient,SOL_SOCKET,SO_LINGER,(char*)&stLinger,sizeof(stLinger));
	//���� ������ ���� ��Ų��.
	pClientInfo->socketClient = INVALID_SOCKET;
}

bool CNetSystemServer::CreateWorkerThread()
{
	//��ũ ������ ����
	for(int i=0;i<MAX_WORKERTHREAD ;i++)
	{
		m_pWorkerThread[i] = NiNew CWorkerThread;
		if(m_pWorkerThread == NULL)
			return false;
			
		//������ ����
		if(!m_pWorkerThread[i]->Create(this))
			return false;
	}
	return true;
}

//accept ������ ����
bool CNetSystemServer::CreateAccpeptThread()
{
	//accept������ ����
	m_pAcceptThread = NiNew CAcceptThread;
	if(m_pAcceptThread == NULL)
		return false;

	if(!m_pAcceptThread->Create(this))
		return false;

	return true;
}

bool CNetSystemServer::BindIOCP(CLIENTINFO *pClientInfo)
{
	HANDLE hIOCP;

	hIOCP = CreateIoCompletionPort( (HANDLE)pClientInfo->socketClient, m_hIOCP, reinterpret_cast<ULONG_PTR>(pClientInfo), 0 );
	if( hIOCP == NULL || m_hIOCP != hIOCP )
	{
		return false;
	}
	return true;
}

bool CNetSystemServer::BindRecv(CLIENTINFO *pClientInfo)
{
	DWORD dwFlag = 0;

	char *pBuf = NULL;

	if(pClientInfo == NULL || pClientInfo->socketClient == INVALID_SOCKET)
		return false;

	//Overlapped I/O�� ���� �� ������ �������ش�.
	pClientInfo->recvSockInfo.wsaBuf.len = MAX_PACKET_SIZE;
	pClientInfo->recvSockInfo.wsaBuf.buf = (CHAR*) pClientInfo->recvSockInfo.m_IOCPbuf;

	ZeroMemory(&pClientInfo->recvSockInfo.wsaOverlapped,sizeof(OVERLAPPED));

	int retval = WSARecv(pClientInfo->socketClient,
		&(pClientInfo->recvSockInfo.wsaBuf),1,NULL,&dwFlag,(LPWSAOVERLAPPED)&(pClientInfo->recvSockInfo),NULL);

	//socket_error�̸� Client socket�� �������ɷ� ó�� �Ѵ�.
	if(retval == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING )) return false;

	return true;
}

bool CNetSystemServer::RegisterSendMsg(CLIENTINFO* pClientInfo,UCHAR* pMsg,int nLen)
{
	if(pClientInfo == NULL)
		return false;

	EnterCriticalSection(&pClientInfo->sendSockInfo.send_queue_lock);
	if(pClientInfo->sendSockInfo.bBusySending)
	{
		UCHAR* newBuf = (UCHAR*) malloc(nLen);
		memcpy(newBuf,pMsg,nLen);
		pClientInfo->sendSockInfo.m_SendPacketQueue.push(newBuf);
	}
	else
	{
		pClientInfo->sendSockInfo.bBusySending = true;

		//unsigned packet_size = pMsg[0];
		unsigned char packet_size = pMsg[0];
		memcpy(pClientInfo->sendSockInfo.m_IOCPbuf,pMsg,packet_size);
		pClientInfo->sendSockInfo.nRemainLen = packet_size;
		pClientInfo->sendSockInfo.wsaBuf.buf = (CHAR*)pClientInfo->sendSockInfo.m_IOCPbuf;
		pClientInfo->sendSockInfo.wsaBuf.len = packet_size;
		SendMsg(pClientInfo);
	}
	LeaveCriticalSection(&pClientInfo->sendSockInfo.send_queue_lock);

	return true;
}

bool CNetSystemServer::SendMsg(CLIENTINFO* pClientInfo)
{
	if(pClientInfo == NULL ||
		pClientInfo->sendSockInfo.wsaBuf.buf == NULL ||
		pClientInfo->sendSockInfo.wsaBuf.len <= 0	 ||
		pClientInfo->socketClient == INVALID_SOCKET )
		return false;
	
	ZeroMemory(&pClientInfo->sendSockInfo.wsaOverlapped,sizeof(OVERLAPPED));

	int retval = WSASend(pClientInfo->socketClient,
		&(pClientInfo->sendSockInfo.wsaBuf),
		1,NULL,0,(LPWSAOVERLAPPED)&(pClientInfo->sendSockInfo),NULL);
	
	if(retval == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) ) return false;

	return true;
}	

CLIENTINFO* CNetSystemServer::GetEmptyClientInfo()
{
	for(int i=0;i<MAX_CLIENT;i++)
	{
		if(m_pClientInfo[i].socketClient == INVALID_SOCKET )
			return &m_pClientInfo[i];
	}
	return NULL;
}

void CNetSystemServer::DestroyThread()
{
	if( m_hIOCP != NULL )
	{
		for( int i=0 ; i<MAX_WORKERTHREAD ; i++ )
		{
			// WaitingThread Queue���� ������� �����忡 ����� ���� �޼����� ������
			PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
		}

		for( int i=0 ; i<MAX_WORKERTHREAD ; i++ )
		{			
			m_pWorkerThread[i]->EndThread();
		}
	}

	closesocket( m_SockListen );
	
	//AcceptThread�� ���Ḧ ��ٸ���.
	if(m_pAcceptThread)
		m_pAcceptThread->EndThread();

}

HANDLE	CNetSystemServer::GetIOCPHandle()
{
	return m_hIOCP;
}


//���� �޼��� ��� �ϴ� �Լ�
void CNetSystemServer::Err_Display(char* msg)
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