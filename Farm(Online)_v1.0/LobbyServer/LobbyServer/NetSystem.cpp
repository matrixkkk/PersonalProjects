#include "stdafx.h"
#include "NetSystem.h"


//---------���� �۾��� ������ ����---------
DWORD WINAPI WorkerThread( LPVOID arg )
//-----------------------------------------
{
	NetSystem *pNetSystem = reinterpret_cast< NetSystem* >( arg );
	
	if( NULL != pNetSystem )
		pNetSystem->BeginWorKerThread();

	return 0;
}


//-----------------------------------------

NetSystem::NetSystem()
{
	::ZeroMemory( &m_Sock,		sizeof(m_Sock)		);
	::ZeroMemory( &m_SockAddr,	sizeof(m_SockAddr)	);
	
	m_hWorkerThread		= NULL;
	m_bRunWorkerThread	= FALSE;
	m_bRunNetSystem		= FALSE;

	::ZeroMemory( m_pBuf, sizeof(m_pBuf) );
}

//-----------------------------------------



//--------- MFC Control Interface ---------
//
//	MFC Controller�� ���� ���Ǵ� �Լ���
//
//-----------------------------------------

//------- ���� ����--------
BOOL	NetSystem::Begin()
//------------------------- 
{
	return TRUE;
}

//------- ���� ���� -------
BOOL	NetSystem::End()
//-------------------------
{
	return TRUE;
}

BOOL	NetSystem::CreateWorkerThread()
{
	m_hWorkerThread = CreateThread( NULL, 0, WorkerThread, (LPVOID)this, 0, NULL );

	if( NULL == m_hWorkerThread )
	{
		MessageBox( NULL, L"NetSystem::CreateWorkerThread() - CreateThread() Failed", L"Failed", MB_OK );
		return FALSE;
	}
	
	CloseHandle(m_hWorkerThread);

	return TRUE;
}

void	NetSystem::DestroyWorKerThread()
{
	if( NULL != m_hWorkerThread )
	{
		WaitForSingleObject( m_hWorkerThread, 2000 );
		CloseHandle(m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
}
//--------- NetWork Functions -------------
//
//	���� ����,����, ��Ŷ, ���� ó��
//
//-----------------------------------------

//---------- ���� ����------------
BOOL	NetSystem::InitSocket()
//--------------------------------
{
	WSADATA wsa;
	
	if( 0 != WSAStartup( MAKEWORD(2,2), &wsa ) )
		return FALSE;

	// UDP����
	m_Sock = socket( AF_INET, SOCK_DGRAM, 0 );

	if( m_Sock == INVALID_SOCKET)
	{
		Err_Quit(L"NetSystem::InitSocket - socket() Failed");
		return FALSE;
	}

	int optval;
	int optlen = sizeof(optval);

	if( SOCKET_ERROR == getsockopt( m_Sock, SOL_SOCKET, SO_RCVBUF, (char *)&optval, &optlen ) )
	{
		Err_Display(L"NetSystem::InitSocket() - getsockopt() Failed");
		return FALSE;
	}
	else
	{
		optval *= 2;

		if( SOCKET_ERROR == setsockopt( m_Sock, SOL_SOCKET, SO_RCVBUF, (char *)&optval, sizeof(optval) ) )
		{
			Err_Display(L"NetSystem::InitSocket() - setsockopt() Failed");
			return FALSE;
		}
	}	

	BOOL bOptval =TRUE;
	if( SOCKET_ERROR == setsockopt( m_Sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(bOptval) ) )
	{
		Err_Display(L"NetSystem::InitSocket() - setsockopt() Failed");
		return FALSE;
	}
	
	return TRUE;
}

//---------- ���� ����------------
BOOL	NetSystem::CloseSocket()
//--------------------------------
{
	/*		TCP ������ ��� ���
	LINGER	optval;
	optval.l_onoff	= 1;
	optval.l_linger = 10;
	
	if( SOCKET_ERROR == setsockopt( m_Sock, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(optval) ) )
	{
		Err_Display(L"NetSystem::CloseSocket() - setsockopt() Failed");
	}	
	*/

	closesocket(m_Sock);

	::ZeroMemory( &m_Sock,		sizeof(m_Sock)		);
	::ZeroMemory( &m_SockAddr,	sizeof(m_SockAddr)	);

	WSACleanup();

	return TRUE;
}

//---------- TCP ���� ������ �������� ��� Bind, Listen ����------------
BOOL	NetSystem::ListenAndBind()
//----------------------------------------------------------------------
{		

	m_SockAddr.sin_family		= AF_INET;
	m_SockAddr.sin_port			= htons(9900);
	m_SockAddr.sin_addr.s_addr	= htonl( INADDR_ANY );

	int nRet = bind( m_Sock, (SOCKADDR *)&m_SockAddr, sizeof(m_SockAddr) );

	if( 0 != nRet  )
	{
		MessageBox( NULL, L"NetSystem::ListenAndBind() - bind() Failed", L"Failed", MB_OK );
		return FALSE;
	}

	nRet = listen( m_Sock, 5 );

	if( 0 != nRet  )
	{
		MessageBox( NULL, L"NetSystem::ListenAndBind() - listen() Failed", L"Failed", MB_OK );
		return FALSE;
	}

	return TRUE;	
}

//---------- UDP ���� ������ �������� ��� Bind ����------------
BOOL	NetSystem::Bind()
//--------------------------------------------------------------
{
	m_SockAddr.sin_family		= AF_INET;
	m_SockAddr.sin_port			= htons(9900);
	m_SockAddr.sin_addr.s_addr	= htonl( INADDR_ANY );

	int ret = bind( m_Sock, (SOCKADDR *)&m_SockAddr, sizeof(m_SockAddr) );

	if( SOCKET_ERROR  == ret )
	{
		Err_Quit(L"NetSystem::Bind() - bind()");
		return FALSE;
	}

	return TRUE;	
}

//---------- ���� �ڵ� ����� ����------------
void	NetSystem::Err_Quit	( wchar_t *pMsg )
//--------------------------------------------
{
	LPVOID lpMsgBuf;
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );

	MessageBox( NULL, (LPCTSTR)lpMsgBuf, pMsg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
	exit(-1);
}

//---------------- ���� �ڵ� ���---------------
void	NetSystem::Err_Display( wchar_t *pMsg )
//----------------------------------------------
{
	LPVOID lpMsgBuf;
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, pMsg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
}
	
void	NetSystem::Send		( SOCKADDR_IN *pSockAddr, const char* pBuf, DWORD nLen )
{
	int retval = 0;
	int addrlen = sizeof(*pSockAddr);

	sendto( m_Sock, pBuf, nLen, 0, reinterpret_cast<SOCKADDR*>(pSockAddr), addrlen  );

	if( SOCKET_ERROR ==  retval ) 
		Err_Display(L"NetSystem::Send() - sendto()" );

}

void	NetSystem::Recv		( const char* pBuf, int nLen )
{

}


void	NetSystem::BeginWorKerThread()
{

	SOCKADDR_IN peeraddr;
	::ZeroMemory( &peeraddr, sizeof(peeraddr) );

	int addrlen = sizeof(peeraddr);
	int retval	= 0;
	SocketInfo pSocketInfo;
	::ZeroMemory(&pSocketInfo, sizeof(pSocketInfo) );

	while( m_bRunWorkerThread )
	{
		retval = recvfrom( m_Sock, m_pBuf, BUFSIZE, 0, (SOCKADDR *)&pSocketInfo.m_SockAddr, &addrlen );

		if( !m_bRunWorkerThread )
			break;
		
		if( retval == SOCKET_ERROR ) {
		//	Err_Display(L"NetSystem::BeginWorKerThread() - Recvfrom Failed");
			continue;
		}

		ProcessPacket( &pSocketInfo, m_pBuf, retval );
	}
}

//--------- Packet Analysis & Handling -----------

//	Recv �Ǿ�� ��Ŷ�� �м� �׿� �°� ó���ϴ� �Լ�
void NetSystem::ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
}
	
//	������ ���������� �̿��Ͽ� �ڷᱸ���� �Ҵ�
void NetSystem::ProcessConnect( SocketInfo *pSocketInfo )
{
}

//	���� ������ ���������� �ڷᱸ���κ��� ����
void NetSystem::ProcessDisConnect( SocketInfo *pSocketInfo )
{
}
