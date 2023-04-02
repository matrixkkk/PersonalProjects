#include "GameServer.h"
#include "CWorkerThread.h"
//#include "ZFLog.h"

CWorkerThread::CWorkerThread()
{

}

CWorkerThread::~CWorkerThread()
{

}

//스레드 생성
bool CWorkerThread::Create(CNetSystemServer *pNetSystem)
{
	m_pNetSystem = pNetSystem;

	m_pkThread = NiThread::Create(this);
	assert(m_pkThread);
	
	m_pkThread->SetPriority(NiThread::NORMAL);
	m_pkThread->Resume();

	return true;
}
int CWorkerThread::Suspend()
{
	return m_pkThread->Suspend();
}

int CWorkerThread::Resume()
{
	return m_pkThread->Resume();
}

void CWorkerThread::EndThread()
{
	m_bRunnging = false;

	//스레드가 끝나길 기다린다.
	if(m_pkThread)
	{
		m_pkThread->WaitForCompletion();
		NiDelete m_pkThread;
		m_pkThread = NULL;
	}
}

unsigned int CWorkerThread::ThreadProcedure(void *pvArg)
{
	NiThread* thread = (NiThread*)pvArg;
	
	CLIENTINFO*		pClientInfo = NULL;
	BOOL bSuccess = TRUE;
	//Overlapped I/O작업에서 전송된 데이터 크기
	DWORD dwSize = 0;

	LPOVERLAPPED lpOverlapped = NULL;

	while(m_bRunnging)
	{
		bSuccess = GetQueuedCompletionStatus(m_pNetSystem->GetIOCPHandle(),
			&dwSize,(LPDWORD)&pClientInfo,&lpOverlapped,INFINITE);
		
		//
		if( pClientInfo == 0 )//lpOverlapped == NULL)
		{
			m_bRunnging = false;
			break;
		}


		//Client가 접속을 끊었을 경우
		if(bSuccess == 0 && dwSize == 0 && pClientInfo != NULL)
		{
			m_pNetSystem->DisConnectProcess(pClientInfo);
			m_pNetSystem->CloseSocket(pClientInfo);
			//클라이언트 개수 감소
			int& nClientCnt = m_pNetSystem->GetClientCnt();
			nClientCnt--;
			continue;
		}		
		
		if( lpOverlapped == NULL )
			continue;

		SOCKETINFO* pOverlappedEx = (SOCKETINFO*)lpOverlapped;

		//Overlapped I/O Recv작업 결과 뒤 처리
		//데이터를 수신한 경0우
		if(pOverlappedEx->eOperation == OP_RECV)
		{			
			UCHAR* bufPtr = pOverlappedEx->m_IOCPbuf;
			int restDataSize = dwSize;

			while(restDataSize)
			{
				if(pOverlappedEx->nPackagingSize == 0)	//미완성 패킷이 존재하지 않는다 == 패킷이 처음부터 전송되었다.
					pOverlappedEx->nPackagingSize = (int) bufPtr[0];		//첫번째 바이트가 패킷 크기
				
				int required = pOverlappedEx->nPackagingSize - pOverlappedEx->nReceived;

				if(restDataSize < required)	//더 이상 패킷을 만들 수 없다. 루프를 중지한다.
				{					
					memcpy(pOverlappedEx->ucPacketBuf + pOverlappedEx->nReceived,bufPtr,restDataSize);
					pOverlappedEx->nReceived += restDataSize;
					//m_pNetSystem->BindRecv(pClientInfo);
					break;
				}
				/*if(restDataSize < required)	//더 이상 패킷을 만들 수 없다. 루프를 중지한다.
				{					
					//처리하지 못하고 남은 패킷을 맨 앞으로
					memcpy(pOverlappedEx->ucPacketBuf,bufPtr,restDataSize);
					pOverlappedEx->nReceived = restDataSize;
					m_pNetSystem->BindRecv(pClientInfo);
					break;
				}*/
				else						//패킷을 완성할 수 있다.	
				{
					memcpy(pOverlappedEx->ucPacketBuf + pOverlappedEx->nReceived,
						bufPtr,required);
					bool retval = m_pNetSystem->PacketProcess(pOverlappedEx->ucPacketBuf,pClientInfo);
					pOverlappedEx->nReceived = 0;
					restDataSize -= required;
					bufPtr += required;
					pOverlappedEx->nPackagingSize = 0;
				}
			}
			m_pNetSystem->BindRecv(pClientInfo);
		}
		else if(pOverlappedEx->eOperation == OP_SEND)
		{
			pOverlappedEx->nRemainLen -= dwSize;
			if(pOverlappedEx->nRemainLen > 0)			//패킷을 다 보내지 못했다.
			{
				pOverlappedEx->wsaBuf.buf += dwSize;
				pOverlappedEx->wsaBuf.len = pOverlappedEx->nRemainLen;
				m_pNetSystem->SendMsg(pClientInfo);
			}
			else		//한 패킷을 다 보냄, 다음 보낼 패킷 검사
			{
				EnterCriticalSection(&pClientInfo->sendSockInfo.send_queue_lock);
				if(!pOverlappedEx->m_SendPacketQueue.empty())
				{
					UCHAR* packetPtr = pOverlappedEx->m_SendPacketQueue.front();
					pOverlappedEx->m_SendPacketQueue.pop();
					unsigned packet_size = packetPtr[0];
					memcpy(pOverlappedEx->m_IOCPbuf,packetPtr,packet_size);
					free(packetPtr);
					pOverlappedEx->nRemainLen = packet_size;
					pOverlappedEx->wsaBuf.buf = (CHAR*)pOverlappedEx->m_IOCPbuf;
					pOverlappedEx->wsaBuf.len = pOverlappedEx->nRemainLen;
					m_pNetSystem->SendMsg(pClientInfo);
				}
				else		//더 이상 보낼것이 없음
				{
					pOverlappedEx->bBusySending = false;
				}
				LeaveCriticalSection(&pClientInfo->sendSockInfo.send_queue_lock);
			}
		}
	}
	
	return 0;
}