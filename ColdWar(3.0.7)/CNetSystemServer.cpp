#include "CNetSystemServer.h"
#include "CAcceptThread.h"
#include "CWorkerThread.h"


CNetSystemServer::CNetSystemServer()
{	
	m_nClientCnt = 0;
	m_hIOCP = NULL;
	m_SockListen = INVALID_SOCKET;
	ZeroMemory(m_szSocketBuffer,sizeof(m_szSocketBuffer));
	
	//Worker 스레드 초기화
	for(int i=0;i<MAX_WORKERTHREAD ;i++)
		m_pWorkerThread[i] =NULL;

	m_pAcceptThread = NULL;

	//Client정보를 저장할 구조체를 생성
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

//네트워크 시작
bool CNetSystemServer::Begin()
{
	//윈속 라이브러리 시작, Listen소켓 생성
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
	//생성된 스레드종료
	DestroyThread();
	
	//생성된 객체를 제거
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

	//TCP Listen소켓 생성
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

	//접속된 클라이언트 주소 정보를 저장할 구조체
	if(!CreateWorkerThread())
		return false;

	if(!CreateAccpeptThread())
		return false;

	return true;
}

void CNetSystemServer::CloseSocket(CLIENTINFO *pClientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0};	//SO_DONTLINGER로 설정

	//bISForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 
	//강제 종료 시킨다.
	if(bIsForce)
		stLinger.l_linger = 1;

	shutdown(pClientInfo->socketClient,SD_BOTH);
	//소켓 옵션을 설정한다.
	setsockopt(pClientInfo->socketClient,SOL_SOCKET,SO_LINGER,(char*)&stLinger,sizeof(stLinger));
	//소켓 연결을 종료 시킨다.
	pClientInfo->socketClient = INVALID_SOCKET;
}

bool CNetSystemServer::CreateWorkerThread()
{
	//워크 스레드 생성
	for(int i=0;i<MAX_WORKERTHREAD ;i++)
	{
		m_pWorkerThread[i] = NiNew CWorkerThread;
		if(m_pWorkerThread == NULL)
			return false;
			
		//스레드 생성
		if(!m_pWorkerThread[i]->Create(this))
			return false;
	}
	return true;
}

//accept 스레드 생성
bool CNetSystemServer::CreateAccpeptThread()
{
	//accept스레드 생성
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

	//Overlapped I/O를 위해 각 정보를 셋팅해준다.
	pClientInfo->recvSockInfo.wsaBuf.len = MAX_PACKET_SIZE;
	pClientInfo->recvSockInfo.wsaBuf.buf = (CHAR*) pClientInfo->recvSockInfo.m_IOCPbuf;

	ZeroMemory(&pClientInfo->recvSockInfo.wsaOverlapped,sizeof(OVERLAPPED));

	int retval = WSARecv(pClientInfo->socketClient,
		&(pClientInfo->recvSockInfo.wsaBuf),1,NULL,&dwFlag,(LPWSAOVERLAPPED)&(pClientInfo->recvSockInfo),NULL);

	//socket_error이면 Client socket이 끊어진걸로 처리 한다.
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
			// WaitingThread Queue에서 대기중인 쓰레드에 사용자 종료 메세지를 보낸다
			PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
		}

		for( int i=0 ; i<MAX_WORKERTHREAD ; i++ )
		{			
			m_pWorkerThread[i]->EndThread();
		}
	}

	closesocket( m_SockListen );
	
	//AcceptThread의 종료를 기다린다.
	if(m_pAcceptThread)
		m_pAcceptThread->EndThread();

}

HANDLE	CNetSystemServer::GetIOCPHandle()
{
	return m_hIOCP;
}


//에러 메세지 출력 하는 함수
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