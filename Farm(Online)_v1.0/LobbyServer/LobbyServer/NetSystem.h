#ifndef NETSYSTEM_H
#define NETSYSTEM_H

#include <winsock2.h>
#include <iostream>

#define BUFSIZE	512

// 클라이언트 1명의 대한 소켓과 주소 정보
// TCP 혹은 UDP 용으로 사용
struct SocketInfo
{
	SOCKET			m_Sock;
	SOCKADDR_IN		m_SockAddr;

	SocketInfo(){
		::ZeroMemory( &m_Sock,		sizeof(m_Sock)		);
		::ZeroMemory( &m_SockAddr,	sizeof(m_SockAddr)	);
	}
};


class NetSystem
{


protected:

	SOCKET			m_Sock;
	SOCKADDR_IN		m_SockAddr;
	//stSockInfo	*m_pStSockInfo;

	HANDLE			m_hWorkerThread;
	BOOL			m_bRunWorkerThread;
	BOOL			m_bRunNetSystem;

	char			m_pBuf[ BUFSIZE ];

public:

	NetSystem();

	//--------- MFC Control Interface ---------
	//
	//	MFC Controller에 의해 사용되는 함수들
	//
	//-----------------------------------------
	virtual BOOL Begin();		// 서버 시작
	virtual BOOL End();			// 서버 종료

public:

	//--------- NetWork Functions -------------
	//
	//	소켓 생성,삭제, 패킷, 오류 처리
	//
	//-----------------------------------------

	BOOL	InitSocket();
	BOOL	CloseSocket();
	BOOL	ListenAndBind();
	BOOL	Bind();
	

	void	Err_Quit	( wchar_t *pMsg );
	void	Err_Display	( wchar_t *pMsg );
	
	void	Send		( SOCKADDR_IN *pSockAddr, const char* pBuf, DWORD nLen );
	void	Recv		( const char* pBuf, int nLen );

	BOOL	CreateWorkerThread();
	void	BeginWorKerThread();
	void	DestroyWorKerThread();


public:

	//--------- Packet Analysis & Handling -----------

	//	Recv 되어온 패킷을 분석 그에 맞게 처리하는 함수
	virtual void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//	접속한 소켓정보를 이용하여 자료구조에 할당
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//	접속 종료한 소켓정보를 자료구조로부터 제거
	virtual void ProcessDisConnect( SocketInfo *pSocketInfo );

};

#endif
