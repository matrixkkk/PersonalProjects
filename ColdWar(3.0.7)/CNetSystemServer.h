/*
	Ŭ������  : CNetSystemServer
	�ۼ���	  : ������
	Ŭ��������: �� Ŭ������ CS�𵨿��� ������ ������ �ϱ����� Ŭ���� �Դϴ�.
	            
				IOCP �������̽��� ����մϴ�.
*/

#ifndef CNETSYSTEMSERVER_H_
#define CNETSYSTEMSERVER_H_

#include "NetCommon.h"

class CAcceptThread;
class CWorkerThread;

class CNetSystemServer : public NiMemObject
{
	CLIENTINFO*		m_pClientInfo;			//Ŭ���̾�Ʈ ���� ���� ����ü
	SOCKET			m_SockListen;			//Listen����
	int				m_nClientCnt;			//Ŭ���̾�Ʈ���� ��

	// --------���� ������---------- 
	CWorkerThread	*m_pWorkerThread[MAX_WORKERTHREAD];
	CAcceptThread	*m_pAcceptThread;
	
	HANDLE			m_hIOCP;								//IOCP�ڵ�
	char			m_szSocketBuffer[1024];	//���� ����


public:
	CNetSystemServer();
	~CNetSystemServer();

	void			Err_Display(char* msg);		//���� ���

	//----------------------------------------------------------
	bool	Begin();		// ��������
	bool	End();			// ���� ����
	
	//Get �Լ� -------------------------------------------------
	int&			GetClientCnt();
	
	void			DestroyThread();						//������ �ı�

	CLIENTINFO*		GetEmptyClientInfo();	//������� �ʴ� Ŭ���̾�Ʈ ���� ����ü�� ��ȯ
	SOCKET			GetListenSocket();
	HANDLE			GetIOCPHandle();		//IOCP �ڵ� ����
	
	bool			InitSocket();		//���� �ʱ�ȭ
	void			CloseSocket(CLIENTINFO* pClientInfo,bool bIsForce = false);		//������ ������ ����
	
	bool			BindListen();			//bind�� listenó��
	bool			StartServer();			//���ӿ�û�� �����ϰ� �޽����� �޾Ƽ� ó��
	bool			CreateWorkerThread();	//Work������ ����
	bool			CreateAccpeptThread();	//accept������ ����

	bool			BindIOCP(CLIENTINFO* pClientInfo);		//completionPort��ü�� ���ϰ� key�� ����
	bool			BindRecv(CLIENTINFO* pClientInfo);		//WSARecv
	bool			RegisterSendMsg(CLIENTINFO* pClientInfo,UCHAR* pMsg,int len);
	bool			SendMsg(CLIENTINFO* pClientInfo);

	virtual bool	ConnectProcess(CLIENTINFO* pClientInfo)	= 0;
	virtual bool	DisConnectProcess(CLIENTINFO* pClientInfo) = 0;
	virtual bool	PacketProcess(const UCHAR* pBuf,CLIENTINFO* pClientInfo) = 0;		
};

#include "CNetSystemServer.inl"

#endif

