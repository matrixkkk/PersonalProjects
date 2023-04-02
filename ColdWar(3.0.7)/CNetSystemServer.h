/*
	클래스명  : CNetSystemServer
	작성자	  : 김현구
	클래스설명: 본 클래스는 CS모델에서 서버의 역할을 하기위한 클래스 입니다.
	            
				IOCP 인터페이스를 사용합니다.
*/

#ifndef CNETSYSTEMSERVER_H_
#define CNETSYSTEMSERVER_H_

#include "NetCommon.h"

class CAcceptThread;
class CWorkerThread;

class CNetSystemServer : public NiMemObject
{
	CLIENTINFO*		m_pClientInfo;			//클라이언트 정보 저장 구조체
	SOCKET			m_SockListen;			//Listen소켓
	int				m_nClientCnt;			//클라이언트들의 수

	// --------서브 스레드---------- 
	CWorkerThread	*m_pWorkerThread[MAX_WORKERTHREAD];
	CAcceptThread	*m_pAcceptThread;
	
	HANDLE			m_hIOCP;								//IOCP핸들
	char			m_szSocketBuffer[1024];	//소켓 버퍼


public:
	CNetSystemServer();
	~CNetSystemServer();

	void			Err_Display(char* msg);		//에러 출력

	//----------------------------------------------------------
	bool	Begin();		// 서버시작
	bool	End();			// 서버 종료
	
	//Get 함수 -------------------------------------------------
	int&			GetClientCnt();
	
	void			DestroyThread();						//스레드 파괴

	CLIENTINFO*		GetEmptyClientInfo();	//사용하지 않는 클라이언트 정보 구조체를 반환
	SOCKET			GetListenSocket();
	HANDLE			GetIOCPHandle();		//IOCP 핸들 리턴
	
	bool			InitSocket();		//소켓 초기화
	void			CloseSocket(CLIENTINFO* pClientInfo,bool bIsForce = false);		//소켓의 연결을 종료
	
	bool			BindListen();			//bind와 listen처리
	bool			StartServer();			//접속요청을 수락하고 메시지를 받아서 처리
	bool			CreateWorkerThread();	//Work스레드 생성
	bool			CreateAccpeptThread();	//accept스레드 생성

	bool			BindIOCP(CLIENTINFO* pClientInfo);		//completionPort객체와 소켓과 key를 연결
	bool			BindRecv(CLIENTINFO* pClientInfo);		//WSARecv
	bool			RegisterSendMsg(CLIENTINFO* pClientInfo,UCHAR* pMsg,int len);
	bool			SendMsg(CLIENTINFO* pClientInfo);

	virtual bool	ConnectProcess(CLIENTINFO* pClientInfo)	= 0;
	virtual bool	DisConnectProcess(CLIENTINFO* pClientInfo) = 0;
	virtual bool	PacketProcess(const UCHAR* pBuf,CLIENTINFO* pClientInfo) = 0;		
};

#include "CNetSystemServer.inl"

#endif

