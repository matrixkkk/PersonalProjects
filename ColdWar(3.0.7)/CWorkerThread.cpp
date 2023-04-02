#include "GameServer.h"
#include "CWorkerThread.h"
//#include "ZFLog.h"

CWorkerThread::CWorkerThread()
{

}

CWorkerThread::~CWorkerThread()
{

}

//������ ����
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

	//�����尡 ������ ��ٸ���.
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
	//Overlapped I/O�۾����� ���۵� ������ ũ��
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


		//Client�� ������ ������ ���
		if(bSuccess == 0 && dwSize == 0 && pClientInfo != NULL)
		{
			m_pNetSystem->DisConnectProcess(pClientInfo);
			m_pNetSystem->CloseSocket(pClientInfo);
			//Ŭ���̾�Ʈ ���� ����
			int& nClientCnt = m_pNetSystem->GetClientCnt();
			nClientCnt--;
			continue;
		}		
		
		if( lpOverlapped == NULL )
			continue;

		SOCKETINFO* pOverlappedEx = (SOCKETINFO*)lpOverlapped;

		//Overlapped I/O Recv�۾� ��� �� ó��
		//�����͸� ������ ��0��
		if(pOverlappedEx->eOperation == OP_RECV)
		{			
			UCHAR* bufPtr = pOverlappedEx->m_IOCPbuf;
			int restDataSize = dwSize;

			while(restDataSize)
			{
				if(pOverlappedEx->nPackagingSize == 0)	//�̿ϼ� ��Ŷ�� �������� �ʴ´� == ��Ŷ�� ó������ ���۵Ǿ���.
					pOverlappedEx->nPackagingSize = (int) bufPtr[0];		//ù��° ����Ʈ�� ��Ŷ ũ��
				
				int required = pOverlappedEx->nPackagingSize - pOverlappedEx->nReceived;

				if(restDataSize < required)	//�� �̻� ��Ŷ�� ���� �� ����. ������ �����Ѵ�.
				{					
					memcpy(pOverlappedEx->ucPacketBuf + pOverlappedEx->nReceived,bufPtr,restDataSize);
					pOverlappedEx->nReceived += restDataSize;
					//m_pNetSystem->BindRecv(pClientInfo);
					break;
				}
				/*if(restDataSize < required)	//�� �̻� ��Ŷ�� ���� �� ����. ������ �����Ѵ�.
				{					
					//ó������ ���ϰ� ���� ��Ŷ�� �� ������
					memcpy(pOverlappedEx->ucPacketBuf,bufPtr,restDataSize);
					pOverlappedEx->nReceived = restDataSize;
					m_pNetSystem->BindRecv(pClientInfo);
					break;
				}*/
				else						//��Ŷ�� �ϼ��� �� �ִ�.	
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
			if(pOverlappedEx->nRemainLen > 0)			//��Ŷ�� �� ������ ���ߴ�.
			{
				pOverlappedEx->wsaBuf.buf += dwSize;
				pOverlappedEx->wsaBuf.len = pOverlappedEx->nRemainLen;
				m_pNetSystem->SendMsg(pClientInfo);
			}
			else		//�� ��Ŷ�� �� ����, ���� ���� ��Ŷ �˻�
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
				else		//�� �̻� �������� ����
				{
					pOverlappedEx->bBusySending = false;
				}
				LeaveCriticalSection(&pClientInfo->sendSockInfo.send_queue_lock);
			}
		}
	}
	
	return 0;
}