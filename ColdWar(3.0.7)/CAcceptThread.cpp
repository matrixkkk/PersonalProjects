#include "GameServer.h"
#include "CAcceptThread.h"

//#ifdef ZFLOG
//#include "ZFLog.h"
//#endif

CAcceptThread::CAcceptThread()
{
	m_bRunning = true;
}

CAcceptThread::~CAcceptThread()
{

}

//스레드 생성
bool CAcceptThread::Create(CNetSystemServer *pNetSystem)
{
	m_pNetSystem = pNetSystem;

	m_pkThread = NiThread::Create(this);
	assert(m_pkThread);
	
	m_pkThread->SetPriority(NiThread::BELOW_NORMAL);
	m_pkThread->Resume();
	
	return true;
}

int CAcceptThread::Suspend()
{
	return m_pkThread->Suspend();
}

int CAcceptThread::Resume()
{
	return m_pkThread->Resume();
}

void CAcceptThread::EndThread()
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


unsigned int CAcceptThread::ThreadProcedure(void *pvArg)
{
	SOCKADDR_IN	clientAddr;
	int addrlen = sizeof(clientAddr);
	int& nClientCnt = m_pNetSystem->GetClientCnt();

	while(m_bRunning)
	{
		//접속을 받을 구조체의 인덱스를 얻어온다.
		CLIENTINFO*		pClientInfo = m_pNetSystem->GetEmptyClientInfo();
		if(pClientInfo == NULL)
			return -1;

		//클라이언트 접속 요청이 올 때까지 기다린다.
		pClientInfo->socketClient = accept(m_pNetSystem->GetListenSocket(),(SOCKADDR*)&clientAddr,&addrlen);
		if(pClientInfo->socketClient == INVALID_SOCKET)
			continue;

		if(nClientCnt >= MAX_CLIENT - 1)
		{
			NiMessageBox("[TCP서버] 인원이 초과","접속해제");
			m_pNetSystem->CloseSocket(pClientInfo);
			continue;
		}
//#ifdef NET_TEST
		char temp[255];
		getpeername(m_pNetSystem->GetListenSocket(),(SOCKADDR*)&clientAddr,&addrlen);
		sprintf_s(temp,sizeof(temp),"[서버] 클라이언트 접속 IP = %s",inet_ntoa(clientAddr.sin_addr));
//		ZFLog::g_sLog->Log(temp);
//#endif

		// I/O CompletionPort 객체와 소켓을 연결시킨다.
		bool bRet = m_pNetSystem-> BindIOCP(pClientInfo);
		if(!bRet)
			return -1;

		//recv
		bRet = m_pNetSystem->BindRecv(pClientInfo);
		if(!bRet)
			return -1;

		//클라이언트 갯수 증가
		nClientCnt++;

		m_pNetSystem->ConnectProcess(pClientInfo);
	}
	return 0;
}