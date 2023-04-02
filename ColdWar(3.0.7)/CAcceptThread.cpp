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

//������ ����
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


unsigned int CAcceptThread::ThreadProcedure(void *pvArg)
{
	SOCKADDR_IN	clientAddr;
	int addrlen = sizeof(clientAddr);
	int& nClientCnt = m_pNetSystem->GetClientCnt();

	while(m_bRunning)
	{
		//������ ���� ����ü�� �ε����� ���´�.
		CLIENTINFO*		pClientInfo = m_pNetSystem->GetEmptyClientInfo();
		if(pClientInfo == NULL)
			return -1;

		//Ŭ���̾�Ʈ ���� ��û�� �� ������ ��ٸ���.
		pClientInfo->socketClient = accept(m_pNetSystem->GetListenSocket(),(SOCKADDR*)&clientAddr,&addrlen);
		if(pClientInfo->socketClient == INVALID_SOCKET)
			continue;

		if(nClientCnt >= MAX_CLIENT - 1)
		{
			NiMessageBox("[TCP����] �ο��� �ʰ�","��������");
			m_pNetSystem->CloseSocket(pClientInfo);
			continue;
		}
//#ifdef NET_TEST
		char temp[255];
		getpeername(m_pNetSystem->GetListenSocket(),(SOCKADDR*)&clientAddr,&addrlen);
		sprintf_s(temp,sizeof(temp),"[����] Ŭ���̾�Ʈ ���� IP = %s",inet_ntoa(clientAddr.sin_addr));
//		ZFLog::g_sLog->Log(temp);
//#endif

		// I/O CompletionPort ��ü�� ������ �����Ų��.
		bool bRet = m_pNetSystem-> BindIOCP(pClientInfo);
		if(!bRet)
			return -1;

		//recv
		bRet = m_pNetSystem->BindRecv(pClientInfo);
		if(!bRet)
			return -1;

		//Ŭ���̾�Ʈ ���� ����
		nClientCnt++;

		m_pNetSystem->ConnectProcess(pClientInfo);
	}
	return 0;
}