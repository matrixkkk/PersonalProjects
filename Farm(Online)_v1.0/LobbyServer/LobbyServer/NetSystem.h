#ifndef NETSYSTEM_H
#define NETSYSTEM_H

#include <winsock2.h>
#include <iostream>

#define BUFSIZE	512

// Ŭ���̾�Ʈ 1���� ���� ���ϰ� �ּ� ����
// TCP Ȥ�� UDP ������ ���
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
	//	MFC Controller�� ���� ���Ǵ� �Լ���
	//
	//-----------------------------------------
	virtual BOOL Begin();		// ���� ����
	virtual BOOL End();			// ���� ����

public:

	//--------- NetWork Functions -------------
	//
	//	���� ����,����, ��Ŷ, ���� ó��
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

	//	Recv �Ǿ�� ��Ŷ�� �м� �׿� �°� ó���ϴ� �Լ�
	virtual void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//	������ ���������� �̿��Ͽ� �ڷᱸ���� �Ҵ�
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//	���� ������ ���������� �ڷᱸ���κ��� ����
	virtual void ProcessDisConnect( SocketInfo *pSocketInfo );

};

#endif
