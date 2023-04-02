#include "stdafx.h"
#include "NetLobbyServer.h"


NetLobbyServer::NetLobbyServer()
{
	InitializeCriticalSection(&m_csListRooms);
	InitializeCriticalSection(&m_csListPlayersInLobby);
	InitializeCriticalSection(&m_csListPlayersInRoom);

	m_listRooms.clear();
	m_listPlayersInLobby.clear();
	m_listPlayersInRoom.clear();

	::ZeroMemory( &m_dbMysql, sizeof(m_dbMysql) );
}

NetLobbyServer::~NetLobbyServer()
{
	End();
}


// ���� ����
BOOL NetLobbyServer::Begin()
{
	if( !InitSocket() )
	{
		MessageBox( NULL, L"NetLobbyServer::Begin() - InitSocket()", L"Failed", MB_OK );
		return FALSE;
	}

	if( !Bind() )
	{
		MessageBox( NULL, L"NetLobbyServer::Begin() - Bind()", L"Failed", MB_OK );
		return FALSE;
	}

	
	// MYSQL �ʱ�ȭ
	mysql_init(&m_dbMysql);
	
	// DB ����
	if ( !mysql_real_connect(&m_dbMysql, MYSQL_HOST, MYSQL_USER, MYSQL_PWD, MYSQL_DB,3306, 0, 0 ) )
	{
		MessageBox( NULL, L"NetLobbyServer::Begin() - mysql_real_connect()", L"MySql DB Connect Failed", MB_OK );
		return FALSE;
	}

	//	����Ÿ ���̽� ����
  	if (mysql_select_db( &m_dbMysql, MYSQL_DB) < 0)
  	{
  		MessageBox( NULL, L"NetLobbyServer::Begin() - mysql_select_db()", L"MySql DB Connect Failed", MB_OK );
  		mysql_close( &m_dbMysql );
		return FALSE;
  	}

	
	if( CreateWorkerThread() )
		m_bRunWorkerThread = TRUE;

	InitializeCriticalSection(&m_csListRooms);
	InitializeCriticalSection(&m_csListPlayersInLobby);
	InitializeCriticalSection(&m_csListPlayersInRoom);
	
	return TRUE;
}


// ���� ����
BOOL NetLobbyServer::End()
{

	DeleteCriticalSection(&m_csListRooms);
	DeleteCriticalSection(&m_csListPlayersInLobby);
	DeleteCriticalSection(&m_csListPlayersInRoom);

	if( !m_listRooms.empty() )
	{
	//	std::list< RoomInfo >::iterator iterRoom;
	//	for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
	//		delete[] iterRoom->m_pJoiner;			

		m_listRooms.clear();
	}

	if( !m_listPlayersInLobby.empty() )
	{
//		std::list< PlayerInfo >::iterator iterPlayer;
//		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
//			delete iterPlayer;
//			delete iterPlayer->m_pSocketInfo;

		m_listPlayersInLobby.clear();
	}

	if( !m_listPlayersInRoom.empty() )
	{
//		std::list< PlayerInfo* >::iterator iterPlayer;
//		for( iterPlayer = m_listPlayersInRoom.begin(); iterPlayer != m_listPlayersInRoom.end(); ++iterPlayer )
//			delete *iterPlayer;
			//delete iterPlayer->m_pSocketInfo;

		m_listPlayersInRoom.clear();
	}
	
	// db
	mysql_close( &m_dbMysql );

	m_bRunWorkerThread = FALSE;
	DestroyWorKerThread();
	
	if( !CloseSocket() )
		return FALSE;


	return TRUE;
}


//--------- Packet Analysis & Handling -----------

//	Recv �Ǿ�� ��Ŷ�� �м� �׿� �°� ó���ϴ� �Լ�
void	NetLobbyServer::ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type = 0;
	memcpy( &type, pBuf, sizeof( BYTE ) );

	switch( type )
	{

	case CTOS_ROOM_LIST:
		// �� ����Ʈ ���� �۽�
		RoomList( pSocketInfo, pBuf, nLen );

		break;

	case CTOS_PLAYER_LIST:
		// ������ ����Ʈ ���� �۽�
		PlayList( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_LOGIN:
		// ���� ���� �۽� ó��
		Login( pSocketInfo, pBuf, nLen );
		break;
	
	case CTOS_OUT_ROOM:
		// �ش� Ŭ���̾�Ʈ �� ������ ��û�� �����Ͽ� �� ������ ó��
		OutRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_DISCONNECT:
		// �ش� Ŭ���̾�Ʈ�� ���� �����û�� ���� ó��
		Disconnect( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_JOIN_MEMBERSHIP:
		// ȸ������ ��û
		JoinMember( pSocketInfo, pBuf, nLen ); 
		break;

	case CTOS_CHAT_IN_LOBBY:
		// �κ񿡼� �Ѿ�� ä�� �޽���
		ChatInLobby( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_CHAT_IN_ROOM:
		// �� �Ѿ�� ä�� �޽���
		ChatInRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_CREATE_ROOM:
		// �� ���� �޽���
		CreateRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_ENTER_ROOM:
		// �� ���� �޽���
		JoinRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_READY:
		// ��ȿ� �ִ� ������ ���� ���� ��û ���¸� ���� ó��
		GameReady( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_MAP_CHANGE:
		// ��ȿ� �ִ� ������ �� ������ ���� ó��
		MapChange( pSocketInfo, pBuf, nLen );
		break;	

	case CTOS_GAME_START:
		// �濡�� ���� ���� ��û ���� ó��	
		GameStart( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_INVITATION:
		// �κ� �ִ� �����鿡�� �ʴ� �޽��� �۽� ó��
		Invitation	( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_FORCE_OUT_ROOM:
		// ���� 1�� �����Ű�� ��û ���� ó��
		OutRoomByCaptain( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_GIVE_CAPTAIN:
		// ���� ���� ��û�� ���� ó��
		GiveCaptain( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_JOINER_INFO:
		// ���� 1�� ���� ��ģ ó��
		JoinerInfoAtRoom( pSocketInfo, pBuf, nLen );
		break;
	}
}
	
//	������ ���������� �̿��Ͽ� �ڷᱸ���� �Ҵ�
void	NetLobbyServer::ProcessConnect( SocketInfo *pSocketInfo )
{


}
	
//	���� ������ ���������� �ڷᱸ���κ��� ����
void	NetLobbyServer::ProcessDisConnect( SocketInfo *pSocketInfo )
{


}



//-------------------------------------------------------------------------
//
//								Recv Functions
//
//	Desc : Client�� ���� ��û �޽����� �޾Ƽ� ó���ϴ� �Լ� ����
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
//
//	Function	: Ŭ���̾�Ʈ�κ��� ȸ������ �޽��� ���� ó��
//	Packet		: Size | type | ID | PW | GAME_ID
//	Size		: DWORD + BYTE + (char) * USER_ID_LEN(25) * 3  = 75BYTE
//-------------------------------------------------------------------------------------

void	NetLobbyServer::JoinMember( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{

	if( &m_dbMysql == NULL )
		return;

	char pTempBuf[ BUFSIZE ];	
	char szID[ USER_ID_LEN ];
	char szPW[ USER_ID_LEN ];
	char szGameID[ USER_ID_LEN ];
	char query[255];
	::ZeroMemory( query, sizeof(query) );
	::ZeroMemory( pTempBuf, BUFSIZE );

	MYSQL_RES   *sql_result = NULL;
	
	BYTE type;
	DWORD dwSize;

	::CopyMemory( szID		,	pBuf + sizeof(BYTE)									,	sizeof(char) * USER_ID_LEN );
	::CopyMemory( szPW		,	pBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN	,	sizeof(char) * USER_ID_LEN );
	::CopyMemory( szGameID	,	pBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN*2	,	sizeof(char) * USER_ID_LEN );

	//------- ���� DB���� �ش� IDȭ GamedID ���� ���� �ľ�--------

	// ������ �ۼ�
	sprintf_s( query, "select * from member where ID = \"%s\" AND Name = \"%s\"", szID, szGameID);

	// ���� ����
	if( 0 != mysql_query( &m_dbMysql, query ) )
	{
		MessageBox( NULL, L"NetLobbyServer::JoinMember - mysql_query() Failed", L"Faield", MB_OK );        
		return;		
	}

	// ����� ��´�.
	sql_result = mysql_store_result( &m_dbMysql );

	//	��� �������� row���� ������.
	//  �̹� �����ϸ��ش� Client���� Fail �޽��� ����
	if( mysql_num_rows( sql_result ) >= 1 )
	{
		type = NetLobbyServer::STOC_JOIN_MEMBERSHIP_FAIL;
		dwSize = sizeof(DWORD) + sizeof(BYTE);
		
		::CopyMemory( pTempBuf					,	&dwSize	,	sizeof(DWORD)	);
		::CopyMemory( pTempBuf + sizeof(DWORD)	,	&type	,	sizeof(BYTE)	);

		Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );

		return;
	}
		

	::ZeroMemory( query, sizeof(query) );
	sprintf_s(query, "insert into member values "
                   "('%s', '%s', '%s')",
                   szID, szPW, szGameID );

    if ( 0 != mysql_query( &m_dbMysql, query ) )
    {
		MessageBox( NULL, L"NetLobbyServer::JoinMember - mysql_query() Failed", L"Faield", MB_OK );        
        return;
	}

	type = NetLobbyServer::STOC_JOIN_MEMBERSHIP_OK;
	dwSize = sizeof(BYTE);
		
	::CopyMemory( pTempBuf,	&type	,	sizeof(BYTE)	);

	Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );

}


//-------------------------------------------------------------------------------------
//
//	Function	: Ŭ���̾�Ʈ�κ��� ���� �޽��� ���� ó��
//	Packet		: Size | type | m_nPlayerNo | m_wstrID(Client ID) | 
//	Size		: DWORD + BYTE + BYTE  + (char) * USER_ID_LEN(25) = 50BYTE
//-------------------------------------------------------------------------------------

void	NetLobbyServer::Login( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	// ���� �Ǵ� DB �� �ִ� ȸ�������� �̿��Ͽ� ���� ���� Ȯ��
	
	// DB�� ���� ���� �ľ�
	char pTempBuf[ BUFSIZE ];
	char szID[ USER_ID_LEN ];
	char szPW[ USER_ID_LEN ];
	char query[255];
	::ZeroMemory( pTempBuf, BUFSIZE );
	::ZeroMemory( szID, sizeof(szID) );
	::ZeroMemory( szPW, sizeof(szPW) );
	::ZeroMemory( query, sizeof(query) );
	
	MYSQL_RES   *sql_result = NULL;
			
	BYTE type;
	DWORD dwSize;

	::CopyMemory( szID,	pBuf + sizeof(BYTE)									,	sizeof(char) * USER_ID_LEN );
	::CopyMemory( szPW,	pBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN	,	sizeof(char) * USER_ID_LEN );

	// ������ �ۼ�
	sprintf_s( query, "select * from %s where ID = \"%s\" AND PW = \"%s\"; ", MEMBER_TABLE, szID, szPW);

	// ���� ����
	if( 0 != mysql_query( &m_dbMysql, query ) )
	{
		MessageBox( NULL, L"NetLobbyServer::Login - mysql_query() Failed", L"Faield", MB_OK );        
		return;		
	}

	// ����� ��´�.
	sql_result = mysql_store_result( &m_dbMysql );
	
	// �����ϴ� ���̵��� �ڷᱸ���� �߰� �� ó�� ����
	if( mysql_num_rows( sql_result ) >= 1 )
	{
		char	pBuf[USER_ID_LEN];
		wchar_t pwBuf[USER_ID_LEN];
		::ZeroMemory( pBuf, sizeof(pBuf) );

		MYSQL_ROW sql_row = mysql_fetch_row(sql_result);
		strcpy_s( pBuf, sql_row[2] );
		MultiByteToWideChar(CP_ACP, 0, pBuf, -1, pwBuf, sizeof(pwBuf) ); 

		//PlayerInfo *pConnectPlayer	= new PlayerInfo;
		PlayerInfo pConnectPlayer;
		pConnectPlayer.m_Sock		= pSocketInfo->m_Sock;
		pConnectPlayer.m_SockAddr	= pSocketInfo->m_SockAddr;
//		pConnectPlayer->m_nPlayerNo = 1;
		
		::CopyMemory( pConnectPlayer.m_wstrID, pwBuf, sizeof(wchar_t) * USER_ID_LEN );

		// �ű� ������ m_nPlayerNo ���� ��ȣ ����
		EnterCriticalSection(&m_csListPlayersInLobby );

		std::list< PlayerInfo >::iterator iterPlayer;
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			if( iterPlayer->m_nPlayerNo != pConnectPlayer.m_nPlayerNo )
				break;
			else
				++pConnectPlayer.m_nPlayerNo;
		}		

		//-------------------------------------------------------------------------------------
		//
		//	Function	: �ű� ������ ���� �� �κ� �ִ� Client���� ����
		//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) | m_wstrID( �÷��̾� ���� ���̵� )
		//	Size		: BYTE + wchar_t * USER_ID_LEN = 51byte
		//-------------------------------------------------------------------------------------
		
		if( !m_listPlayersInLobby.empty() )
		{
			::ZeroMemory( pTempBuf, BUFSIZE );
			type = NetLobbyServer::STOC_ADD_PLAYER;
			dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

			::CopyMemory( pTempBuf					,	&type							,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + sizeof(BYTE)	,	&pConnectPlayer.m_nPlayerNo	,	sizeof(BYTE)					);	
			::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	pConnectPlayer.m_wstrID		,	sizeof(wchar_t) * USER_ID_LEN	);	
			
			// �ű� �����ڿ� ���� ������ ���� Lobby�� �ִ� Client�鿡�� ����
			for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
			{
				Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );
			}
		}
		
		// �ű� �����ڸ� �ڷᱸ���� �߰�
		m_listPlayersInLobby.push_back( pConnectPlayer );
		m_listPlayersInLobby.sort();

		LeaveCriticalSection(&m_csListPlayersInLobby );
		
		//-------------------------------------------------------------------------------------
		//
		//	Function	: ������ Ŭ���̾�Ʈ���� ���ο� Client�� ���� ���� ���߰� �α����� ���� �Ǿ����� ������ȣ�� �����ش�.
		//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) 
		//	Size		: BYTE + BYTE 1byte
		//-------------------------------------------------------------------------------------
		::ZeroMemory( pTempBuf, BUFSIZE );
		dwSize = sizeof(BYTE)*2 + sizeof(wchar_t)*USER_ID_LEN;
		type = NetLobbyServer::STOC_LOGIN_OK;
		
		::CopyMemory( pTempBuf 					,	&type						,	sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)	,	&pConnectPlayer.m_nPlayerNo,	sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&pConnectPlayer.m_wstrID	,	sizeof(wchar_t) * USER_ID_LEN	);

		Send( &pConnectPlayer.m_SockAddr, pTempBuf, (int)dwSize );
		
		::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// ���̾�α� ���� ������Ʈ
	}
}



//-------------------------------------------------------------------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ���� �κ񼭹� ������ �� ����Ʈ ���� �۽�
//	Packet		: | type | nRoomNo(���ȣ) | m_nJoiners(����������) | m_nMaxJoiners(�������ο�) |  m_bRoomState(�����) | m_nMapInfo(������) | m_wstrRoomName(���̸�)	
//	Size		: BYTE + DWORD + BYTE + BYTE + BYTE + BYTE + BYTE + (wchat_t) * ROOM_NAME_LEN(30) = 64BYTE		512 / 64
//-------------------------------------------------------------------------------------------------------------------------------------------------
void	NetLobbyServer::RoomList( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	if( m_listRooms.empty() )
		return;

	BYTE type = NetLobbyServer::STOC_ADD_ROOMLIST;
	DWORD dwSize = sizeof(DWORD) + sizeof(BYTE);
	DWORD dwRoomPacketSize = sizeof(BYTE) * 5 + sizeof(wchar_t) * ROOM_NAME_LEN;	//63Byte
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, BUFSIZE );

	int cntRoom = 0;
	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( pTempBuf, &type, sizeof(BYTE) );

	EnterCriticalSection(&m_csListRooms );
	
	for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
	{
		if( cntRoom < static_cast< int >( (BUFSIZE - (sizeof(BYTE) + sizeof(DWORD))) / dwRoomPacketSize ) ) 
		{
			::CopyMemory( pTempBuf + dwSize					,	&iterRoom->m_nRoomNo	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)	,	&iterRoom->m_nJoiners	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*2,	&iterRoom->m_nMaxJoiners,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*3,	&iterRoom->m_bRoomState	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*4,	&iterRoom->m_nMapInfo	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*5,	iterRoom->m_wstrRoomName,	sizeof(wchar_t) * ROOM_NAME_LEN );
		
			++cntRoom;
			dwSize += dwRoomPacketSize;
		}
		else
		{
			//dwSize -= dwRoomPacketSize;
			::CopyMemory( pTempBuf + sizeof(BYTE),	&dwSize,	sizeof(DWORD) );
			
			Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );
			
			ZeroMemory( pTempBuf, BUFSIZE );
			dwSize = sizeof(DWORD) + sizeof(BYTE);

			::CopyMemory( pTempBuf							,	&type					,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize					,	&iterRoom->m_nRoomNo	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)	,	&iterRoom->m_nJoiners	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*2,	&iterRoom->m_nMaxJoiners,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*3,	&iterRoom->m_bRoomState	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)*4,	iterRoom->m_wstrRoomName,	sizeof(wchar_t) * ROOM_NAME_LEN );

			cntRoom = 1;			
		}
	}
	
	LeaveCriticalSection(&m_csListRooms );

	// ���� Data ����
	if( cntRoom >= 1  && dwSize > (sizeof(DWORD) + sizeof(BYTE)) )
	{
		::CopyMemory( pTempBuf + sizeof(BYTE), &dwSize, sizeof(DWORD) );
		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
	}


	//-------------------------------------------------------------------------------------
	//
	//	Function	: �븮��Ʈ �Ϸ� Message �۽� Client�� �� ������ ������ ���� �ڷᱸ���� Data�� MFC �������̽��� ��� �۾� ����
	//	Packet		: | type | 
	//	Size		:   BYTE   0byte
	//-------------------------------------------------------------------------------------

	ZeroMemory( pTempBuf, BUFSIZE );
	dwSize = sizeof(BYTE);
	type = NetLobbyServer::STOC_ROOMLIST_SEND_OK;	
	::CopyMemory( pTempBuf,	&type, sizeof(BYTE) );

	Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
	
}

//-------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ���� �κ񼭹� ������ Client ����Ʈ ���� �۽�
//	Packet		:  type | Size | m_nPlayerNo | m_wstrID(Client ID) | 
//	Size		: DWORD + BYTE + BYTE + (wchat_t) * USER_ID_LEN(25) = 51BYTE		512 / 60
//-------------------------------------------------------------------------------------
void	NetLobbyServer::PlayList( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{

	if( m_listPlayersInLobby.empty() )
		return;

	BYTE type = NetLobbyServer::STOC_ADD_PlAYERLIST;
	DWORD dwSize = sizeof(DWORD) + sizeof(BYTE);
	DWORD dwPlayerPacketSize =  sizeof(BYTE) + sizeof(wchar_t) * USER_ID_LEN;	//63Byte
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, BUFSIZE );

	int cntPlayer = 0;
	std::list< PlayerInfo >::iterator iterPlayer;

	::CopyMemory( pTempBuf, &type, sizeof(BYTE) );

	EnterCriticalSection(&m_csListPlayersInLobby );
	
	for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
	{
		if( cntPlayer < static_cast< int >( (BUFSIZE - (sizeof(BYTE) + sizeof(DWORD))) / dwPlayerPacketSize ) )
		{
			::CopyMemory( pTempBuf + dwSize					, &iterPlayer->m_nPlayerNo,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)	, iterPlayer->m_wstrID	,	sizeof(wchar_t) * USER_ID_LEN	);

			++cntPlayer;
			dwSize += dwPlayerPacketSize;
		}
		else
		{
			::CopyMemory( pTempBuf + sizeof(BYTE), &dwSize,	sizeof(DWORD) );
			Send( &pSocketInfo->m_SockAddr, m_pBuf, dwSize );
			
			ZeroMemory( pTempBuf, BUFSIZE );
			dwSize = sizeof(DWORD) + sizeof(BYTE);
			
			::CopyMemory( pTempBuf + dwSize					, &iterPlayer->m_nPlayerNo	,	sizeof(BYTE)					);
			::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)	, iterPlayer->m_wstrID		,	sizeof(wchar_t) * USER_ID_LEN	);			

			cntPlayer = 1;			
		}
	}
	
	LeaveCriticalSection(&m_csListPlayersInLobby );

	// ���� Data ����
	if( cntPlayer >= 1  && dwSize > sizeof(DWORD) + sizeof(BYTE) )
	{
		::CopyMemory( pTempBuf + sizeof(BYTE), &dwSize, sizeof(DWORD) );
		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
	}

	//-------------------------------------------------------------------------------------
	//
	//	Function	: �÷��̾� ����Ʈ �Ϸ� Message �۽� Client�� �� ������ ������ ���� �ڷᱸ���� Data�� MFC �������̽��� ��� �۾� ����
	//	Packet		: Size | type | 
	//	Size		: DWORD + BYTE 0byte
	//-------------------------------------------------------------------------------------

	// ���� �������� ���ۿϷ� �޼��� ����
	ZeroMemory( pTempBuf, BUFSIZE );
	dwSize = sizeof(BYTE);
	type = NetLobbyServer::STOC_PLAYERLIST_SEND_OK;

	::CopyMemory( pTempBuf,	&type	,	sizeof(BYTE)	);

	Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );
}

//-------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ�� �� ���� ��û�� �����Ͽ� �� ���� ó��-	
//	Packet		: Size | type | nPlayerNo(��û�� ����� ��ȣ) | ���̸�
//	Size		: DWORD + BYTE  + BYTE + wchar_t * ROOM_NAME_LEN(30) 61Byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::CreateRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type;
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	DWORD dwSize;

	PlayerInfo	stTmpPlayer;
	RoomInfo	stTmpRoom;
	BYTE		nPlayerInRoomNo;		// �濡 ������ ������ȣ
	BYTE		nPlayerInLobbyNo;		// �κ� ������ ������ȣ

	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;
	std::list< PlayerInfo >::iterator iterPlayerInRoom;

	::CopyMemory( &nPlayerInLobbyNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE) );	
	stTmpPlayer.m_nPlayerNo = nPlayerInLobbyNo;

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom );
	EnterCriticalSection( &m_csListPlayersInLobby );

	// ��û�� Client�� �κ񿡼� � Client���� ã�´�.
	if( std::binary_search( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer ) )
	{
		iterPlayerInLobby = std::lower_bound( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer);	

		// ��ȿ� �ִ� ������ ��ȣ ���� ����
		nPlayerInRoomNo = 1;

		if( !m_listPlayersInRoom.empty() )
		{
			for( iterPlayerInRoom = m_listPlayersInRoom.begin(); iterPlayerInRoom != m_listPlayersInRoom.end(); ++iterPlayerInRoom )
			{
				if( nPlayerInRoomNo != iterPlayerInRoom->m_nPlayerNo )
					break;
				else
					++nPlayerInRoomNo;
			}
		}	

		// �� ���� ��ȣ �ο�
		stTmpRoom.m_nRoomNo = 1;

		if( !m_listRooms.empty() )
		{
			for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
			{
				if( stTmpRoom.m_nRoomNo != iterRoom->m_nRoomNo )
					break;
				else
					++stTmpRoom.m_nRoomNo;
			}
		}

		iterPlayerInLobby->m_nPosAtRoom = 0;
		iterPlayerInLobby->m_nPlayerNo	= nPlayerInRoomNo;
		iterPlayerInLobby->m_nRoomNo	= stTmpRoom.m_nRoomNo;

		// �� ������ ����Ʈ�� �߰� & ����
		m_listPlayersInRoom.push_back( *iterPlayerInLobby );
		m_listPlayersInRoom.sort();

		// �κ� ������ ����Ʈ�� ���� 
		m_listPlayersInLobby.erase( iterPlayerInLobby );
				
		::CopyMemory( &stTmpRoom.m_nMaxJoiners	, pBuf + sizeof(BYTE)*2	, sizeof(BYTE)						);
		::CopyMemory( stTmpRoom.m_wstrRoomName	, pBuf + sizeof(BYTE)*3	, sizeof(wchar_t) * ROOM_NAME_LEN	);		

		m_listRooms.push_back( stTmpRoom );
		m_listRooms.sort();

		stTmpPlayer.m_nPlayerNo	= nPlayerInRoomNo;
		iterPlayerInRoom = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer);
		iterPlayerInRoom->m_nPosAtRoom = 0;	// ������ �濡���� ��ġ
		iterPlayerInRoom->m_nRoomNo = stTmpRoom.m_nRoomNo;

		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );
		iterRoom->m_pJoiner[ 0 ] = &(*iterPlayerInRoom);		
		iterRoom->m_nMapInfo	= 0;

	
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		//	Function	: �� ������ �Ϸ��� �κ� �ִ� ������ Client���� �� ���� Message �۽�
		//	Packet		: | type | m_nRoomNo(�� ��ȣ) | m_bRoomState(�� ����) | m_nJoiners( ���� �����ο� ) | m_nMaxJoiners( ��ü �����ο� ) | m_wstrRoomName( �� �̸� )
		//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE + wchar_t * ROOM_NAME_LEN(30) 64Byte
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		if( !m_listPlayersInLobby.empty() )
		{
			::ZeroMemory( pTempBuf, BUFSIZE );
			type	= NetLobbyServer::STOC_ADD_ROOM;
			dwSize	= sizeof(BYTE)*5 + sizeof(wchar_t)*ROOM_NAME_LEN;		
			
			::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)						);
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInLobbyNo			, sizeof(BYTE)						);			
			::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &stTmpRoom.m_nRoomNo		, sizeof(BYTE)						);			
			::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &stTmpRoom.m_nJoiners		, sizeof(BYTE)						);
			::CopyMemory( pTempBuf + sizeof(BYTE)*4	, &stTmpRoom.m_nMaxJoiners	, sizeof(BYTE)						);
			::CopyMemory( pTempBuf + sizeof(BYTE)*5	, &stTmpRoom.m_bRoomState	, sizeof(BYTE)						);
			::CopyMemory( pTempBuf + sizeof(BYTE)*6	, &stTmpRoom.m_wstrRoomName	, sizeof(wchar_t) *  ROOM_NAME_LEN	);

			for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
				Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
		}

		LeaveCriticalSection( &m_csListRooms );
		LeaveCriticalSection( &m_csListPlayersInRoom );
		LeaveCriticalSection( &m_csListPlayersInLobby );

		//-------------------------------------------------------------------------------------
		//
		//	Function	: �� ������ �Ϸ��� �ش� Ŭ���̾�Ʈ���� �ڽ��� �濡���� ���¿� ��ȭ�鿡 ������ �ʿ��� ���� �۽�
		//	Packet		: Size | type | nPayerNo(������ ���� ����Ʈ ������ȣ) 
		//	Size		: DWORD + BYTE + BYTE 0Byte
		//-------------------------------------------------------------------------------------	

		::ZeroMemory( pTempBuf, BUFSIZE );
		type	= NetLobbyServer::STOC_ROOM_CREATE_OK;
		dwSize	= sizeof(BYTE)*3 + sizeof(wchar_t) * ROOM_NAME_LEN;
				
		::CopyMemory( pTempBuf					,	&type							, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)	,	&nPlayerInRoomNo				, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&stTmpRoom.m_nRoomNo			, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	&stTmpRoom.m_wstrRoomName		, sizeof(wchar_t)*ROOM_NAME_LEN	);

		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );

	}
	else
	{
		LeaveCriticalSection( &m_csListRooms );
		LeaveCriticalSection( &m_csListPlayersInRoom );
		LeaveCriticalSection( &m_csListPlayersInLobby );
	}

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// ���̾�α� ���� ������Ʈ
//	LeaveCriticalSection( &m_csListRooms );
//	LeaveCriticalSection( &m_csListPlayersInRoom );
//	LeaveCriticalSection( &m_csListPlayersInLobby );

}

//-------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ�� �� ���� ��û�� �����Ͽ� ���� ó��
//	Packet		: Size | type | nPlayerNo(�÷��̾��ȣ), nRoomNo(���ȣ) 
//	Size		: DWORD + BYTE + BYTE 1byte
//-------------------------------------------------------------------------------------

void	NetLobbyServer::JoinRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type;
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	DWORD dwSize;

	PlayerInfo	stTmpPlayer;
	RoomInfo	stTmpRoom;
	BYTE		nPlayerInRoomNo = 0;	// �濡 ������ ������ȣ
	BYTE		nPlayerInLobbyNo = 0;	// �κ� ������ ������ȣ
	BYTE		nPosInRoom = 0;			// �κ� ������ ������ȣ
	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayer;
	std::list< PlayerInfo >::iterator iterPlayerInRoom;

	
	::CopyMemory( &stTmpPlayer.m_nPlayerNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE) );	
	::CopyMemory( &stTmpRoom.m_nRoomNo		, pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );	

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom );
	EnterCriticalSection( &m_csListPlayersInLobby );

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );	

		switch( iterRoom->m_bRoomState )
		{
		case ROOM_STAND:
			//  ��� �����̹Ƿ� �������
			
			// 3. �ش� �濡 ����, PlaerInRoom ����Ʈ�� �߰�
			// 4. �ش� �濡 �ִ� ���� �����鿡�� ���ο� ���� ���� �۽�
			// 5. �ش� �濡 ���¸� �κ� �����鿡�� ����
			// 1. ���� �κ񿡼� ���� ��Ͽ��� ����
			// 2. �̸� �� �����鿡�� �˸���.

			// ��ȿ� �ִ� ������ ��ȣ ���� ����
			nPlayerInRoomNo = 1;		
			for( iterPlayer = m_listPlayersInRoom.begin(); iterPlayer != m_listPlayersInRoom.end(); ++iterPlayer )
			{
				if( nPlayerInRoomNo != iterPlayer->m_nPlayerNo )
					break;
				else
					++nPlayerInRoomNo;
			}
			
			// �κ񿡼� �ش� ������ ��ȣ�� ã�Ƽ� ����Ʈ���� ������ �κ� ������ Client���� ������ Client �ѹ� �۽�
			if( std::binary_search( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(),stTmpPlayer ) )
			{
				iterPlayer = std::lower_bound( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer );	
				
				// �κ񿡼��� ��ȣ ����
				nPlayerInLobbyNo = iterPlayer->m_nPlayerNo;
			
				// �� ����Ʈ������ ��ȣ�� ��ü
				iterPlayer->m_nPlayerNo = nPlayerInRoomNo;
				iterPlayer->m_nRoomNo	= stTmpRoom.m_nRoomNo;

				wcscpy_s( stTmpPlayer.m_wstrID, iterPlayer->m_wstrID );

				// �濡 �ִ� ���� ����Ʈ�� �߰�
				m_listPlayersInRoom.push_back( *iterPlayer );
				m_listPlayersInRoom.sort();
				stTmpPlayer.m_nPlayerNo = nPlayerInRoomNo;

				iterPlayerInRoom = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );

				// �κ� �ִ� ���� ����Ʈ���� ����
				m_listPlayersInLobby.erase( iterPlayer );
				
				// ������ ����
				if( ++iterRoom->m_nJoiners ==  iterRoom->m_nMaxJoiners )
					iterRoom->m_bRoomState = ROOM_FULL;
				
				// �濡���� ��ġ�� ����
				for( BYTE iPlayer = 0; iPlayer < iterRoom->m_nMaxJoiners; iPlayer++ )
				{
					if( NULL == iterRoom->m_pJoiner[ iPlayer ] )
						break;
					nPosInRoom++;
				}

				iterRoom->m_nJoinerState [ nPosInRoom ] = STATE_DEFAULT;
				iterPlayerInRoom->m_nPosAtRoom = nPosInRoom;
		
				if( !m_listPlayersInLobby.empty() )
				{
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �κ� �ִ� ���� �������� ������� �� ������ ���ؼ� ����Ʈ����  ���� �޼��� �۽�
				//				  �׸��� ����� �������� �ο����� ����						
				//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) |
				//				: | type | m_nRoomNo(���ȣ) | m_nJoiners(���������ο� | 
				//	Size		:   BYTE + BYTE + 1byte
				//				:   BYTE + BYTE + BYTE 2byte;
				//-------------------------------------------------------------------------------------
					::ZeroMemory( pTempBuf, BUFSIZE );
					dwSize	= sizeof(BYTE)*2;
					type	= NetLobbyServer::STOC_REMOVE_PLAYER_IN_LOBBY;

					char pTmpBuf[4]; 
					BYTE tmpType	= NetLobbyServer::STOC_UPDATE_ROOMSTATE_IN_LOBBY;
					DWORD dwTmpSize	= sizeof(BYTE)*4;
				
					::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInLobbyNo			, sizeof(BYTE)	);
					
					::CopyMemory( pTmpBuf 					, &tmpType					, sizeof(BYTE)	);
					::CopyMemory( pTmpBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo		, sizeof(BYTE)	);
					::CopyMemory( pTmpBuf + sizeof(BYTE)*2	, &iterRoom->m_nJoiners		, sizeof(BYTE)	);
					::CopyMemory( pTmpBuf + sizeof(BYTE)*3	, &iterRoom->m_bRoomState	, sizeof(BYTE)	);

					for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
					{
						Send( &iterPlayer->m_SockAddr, pTempBuf	, dwSize	);
						Send( &iterPlayer->m_SockAddr, pTmpBuf	, dwTmpSize	);
					}
				}

				//-------------------------------------------------------------------------------------
				//
				//	Function	: ��ȿ� �ִ� ���� ����鿡�� ���ο� ������ ���� �۽�
				//	Packet		: | type | nPosInRoom(��ȿ����� ��ġ) | newUserState(���� ����) | newUserState(�÷��̾� ����), m_wstrID(ID) 
				//	Size		:  BYTE + BYTE + BYTE + wchar_t * USER_ID_LEN  51byte
				//-------------------------------------------------------------------------------------

				::ZeroMemory( pTempBuf, BUFSIZE );
				dwSize = sizeof(BYTE)*3 + sizeof(wchar_t) * USER_ID_LEN;
				type = NetLobbyServer::STOC_JOINROOM_PLAYERINFO;
				UserInRoomState newUserState= STATE_DEFAULT;
				
				::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)					);				
				::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPosInRoom				, sizeof(BYTE)					);								
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &newUserState				, sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)*3	, stTmpPlayer.m_wstrID		, sizeof(wchar_t) * USER_ID_LEN	);

				for( int iPlayer = 0; iPlayer < iterRoom->m_nMaxJoiners; iPlayer++ )
				{
					if( NULL != iterRoom->m_pJoiner[ iPlayer ] )
						Send( &iterRoom->m_pJoiner[ iPlayer ]->m_SockAddr, pTempBuf, (int)dwSize );
				}

				//----------------------------------------------------------------------------------------------------------------------
				//
				//	Function	: ��ȿ� ���� �ű� �������� ���� �����鿡 ���� ���� ����
				//	Packet		:	| type | nPlayerInRoomNo(�濡 �ִ� ������ȣ) | stTmpRoom.m_nRoomNo(���ȣ) | nPosInRoom(�濡���� ��ġ) |
				//					| m_nJoiners | nState(���� ����) | m_wstrID(ID) | m_strConnRoomName( ���̸� ) |
				//	Size		: BYTE + BYTE + BYTE + BYTE + BYTE + wchar_t * USER_ID_LEN + wchar_t * ROOM_NAME_LEN = 114byte
				//----------------------------------------------------------------------------------------------------------------------
				::ZeroMemory( pTempBuf, BUFSIZE );
				DWORD dwPacketPalyerSize = sizeof(BYTE) + sizeof(wchar_t)*USER_ID_LEN;
				dwSize = sizeof(BYTE)*6 + sizeof(wchar_t) * ROOM_NAME_LEN;
				type = NetLobbyServer::STOC_ENTER_ROOM;			// �濡 �־��� ���� ������ ���� ���� �۽Ű� ������ ���;
				BYTE nJoiner = iterRoom->m_nJoiners - 1;
				
				::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInRoomNo			, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &stTmpRoom.m_nRoomNo		, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &nPosInRoom				, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)*4	, &nJoiner					, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)*5	, &iterRoom->m_nMapInfo		, sizeof(BYTE)						);
				::CopyMemory( pTempBuf + sizeof(BYTE)*6	, iterRoom->m_wstrRoomName	, sizeof(wchar_t) * ROOM_NAME_LEN	);

				for( int iPlayer = 0; iPlayer < iterRoom->m_nMaxJoiners; iPlayer++ )
				{				
					if( NULL != iterRoom->m_pJoiner[ iPlayer ] )
					{
						::CopyMemory( pTempBuf + dwSize					, &iterRoom->m_nJoinerState[ iPlayer ]			, sizeof(BYTE)					);
						::CopyMemory( pTempBuf + dwSize + sizeof(BYTE)	, iterRoom->m_pJoiner[ iPlayer ]->m_wstrID		, sizeof(wchar_t) * USER_ID_LEN	);
						dwSize += dwPacketPalyerSize;
					}
				}

				Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );

				// �ű����� RoomInfo�� �߰� ����				
				iterRoom->m_pJoiner[ nPosInRoom ] = &(*iterPlayerInRoom);				

			}

			break;

		case ROOM_FULL:
			//-------------------------------------------------------------------------------------
			//
			//	Function	: ����(6��)�� �� �� ����
			//	Packet		: Size | type 
			//	Size		: DWORD + BYTE 
			//-------------------------------------------------------------------------------------
			dwSize = sizeof(DWORD) + sizeof(BYTE);
			type = NetLobbyServer::STOC_NOT_ENTER_FOOL;
			::CopyMemory( pTempBuf,					&dwSize	, sizeof(DWORD)	);
			::CopyMemory( pTempBuf + sizeof(DWORD), &type	, sizeof(BYTE)	);

			Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );

			break;

		case ROOM_RUNNING_GAME:
			//-------------------------------------------------------------------------------------
			//
			//	Function	: ���� ���� ����
			//	Packet		: Size | type 
			//	Size		: DWORD + BYTE 
			//-------------------------------------------------------------------------------------
			dwSize = sizeof(DWORD) + sizeof(BYTE);
			type = NetLobbyServer::STOC_NOT_ENTER_GAMMING;
			::CopyMemory( pTempBuf,					&dwSize	, sizeof(DWORD)	);
			::CopyMemory( pTempBuf + sizeof(DWORD), &type	, sizeof(BYTE)	);

			Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );

			break;

		}

		LeaveCriticalSection( &m_csListPlayersInLobby );
		LeaveCriticalSection( &m_csListPlayersInRoom );
		LeaveCriticalSection( &m_csListRooms );

		return;
	}

	LeaveCriticalSection( &m_csListPlayersInLobby );
	LeaveCriticalSection( &m_csListPlayersInRoom );
	LeaveCriticalSection( &m_csListRooms );
	
}

//-------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ �� ������ ��û�� �����Ͽ� �� ������ ó��
//	Packet		: | type | m_PlayNo( ������ȣ ) | m_RoomNo( ���ȣ ) | m_nPosInRoom(�濡���� ��ġ) | nCaptain(���忩��) |
//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE =  4byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::OutRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{

	BYTE type;
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	DWORD dwSize;

	PlayerInfo	stTmpPlayer;
	RoomInfo	stTmpRoom;
	BYTE		m_bCaptain = 0;
	BYTE		nPosInRoom = 0;	

//	BYTE		nPlayerInRoomNo;		// �濡 ������ ������ȣ
	BYTE		nPlayerInLobbyNo;		// �κ� ������ ������ȣ
	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayer;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;

	
	::CopyMemory( &stTmpPlayer.m_nPlayerNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE) );	
	::CopyMemory( &stTmpRoom.m_nRoomNo		, pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );	
	::CopyMemory( &nPosInRoom				, pBuf + sizeof(BYTE)*3	, sizeof(BYTE) );
	::CopyMemory( &m_bCaptain				, pBuf + sizeof(BYTE)*4	, sizeof(BYTE) );

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom );
	EnterCriticalSection( &m_csListPlayersInLobby );


	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );			
		
		// �κ񿡼� ���� ���� ��ȣ ���ϱ�
		nPlayerInLobbyNo = 1;		
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			if( nPlayerInLobbyNo != iterPlayer->m_nPlayerNo )
				break;
			else
				++nPlayerInLobbyNo;
		}

//		stTmpPlayer.m_nPlayerNo = iterRoom->m_pJoiner[ nPosInRoom ]->m_nPlayerNo;

		// �뿡�� �ش� ������ ��ȣ�� ã�Ƽ� ����Ʈ���� ������ 
		if( std::binary_search( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );	
						
			// �� ����Ʈ������ ��ȣ�� ��ü
			iterPlayer->m_nPlayerNo = nPlayerInLobbyNo;
			iterPlayer->m_nRoomNo	= 0;

			//-------------------------------------------------------------------------------------
			//
			//	Function	: �ű� ������ ���� �� �κ� �ִ� Client���� ����
			//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) | m_wstrID( �÷��̾� ���� ���̵� )
			//	Size		: BYTE + wchar_t * USER_ID_LEN = 51byte
			//-------------------------------------------------------------------------------------			
			if( !m_listPlayersInLobby.empty() )
			{
				::ZeroMemory( pTempBuf, BUFSIZE );
				type = NetLobbyServer::STOC_ADD_PLAYER;
				dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

				::CopyMemory( pTempBuf					,	&type							,	sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)	,	&iterPlayer->m_nPlayerNo		,	sizeof(BYTE)					);	
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&iterPlayer->m_wstrID			,	sizeof(wchar_t) * USER_ID_LEN	);	
					
				// �ű� �����ڿ� ���� ������ ���� Lobby�� �ִ� Client�鿡�� ����
				for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
				{
					Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
				}
			}			
			
			// ������ ����
		//	nPosInRoom = iterPlayer->m_nPosAtRoom ;
			iterRoom->m_pJoiner[ nPosInRoom ] = NULL;
			for( int iJoiner = nPosInRoom + 1; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
			{
				iterRoom->m_pJoiner[ iJoiner - 1 ] = iterRoom->m_pJoiner[ iJoiner ];
			}			
			iterRoom->m_pJoiner[ --iterRoom->m_nJoiners ] = NULL;
			iterRoom->m_bRoomState = ROOM_STAND;		
			
			// �濡 ������ �ο��̿��ٸ�
			if( 0 >= iterRoom->m_nJoiners )
			{

				//-------------------------------------------------------------------------------------
				//
				//	Function	: �κ� �ִ� �����鿡�� �� ���� ���� �۽�
				//	Packet		: | type | nRoomNo( ���ȣ ) |
				//	Size		:  BYTE + BYTE 1Byte;
				//-------------------------------------------------------------------------------------
				if( !m_listPlayersInLobby.empty()  )
				{
					::ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_REMOVE_ROOM;
					dwSize = sizeof(BYTE)*2;

					::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo	, sizeof(BYTE) );

					for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
					{
						Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
					}
				}

				m_listRooms.erase( iterRoom );
				
			}
			// �濡 �����ִ� �����鿡�� ���� ������ ���� ���� �޽��� �۽�
			else
			{
			
				
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �濡 �����ִ� �����鿡�� ���� ������ ���� ���� �޽��� �۽�
				//	Packet		: | type | nPosInRoom( �÷��̾� �濡�� ��ġ�� ) | m_wstrID( �÷��̾� ID )
				//	Size		:  BYTE + BYTE + wchar_t * USER_ID_LEN = 51Byte;
				//-------------------------------------------------------------------------------------

				::ZeroMemory( pTempBuf, BUFSIZE );
				type = NetLobbyServer::STOC_REMOVE_PLAYER_IN_ROOM;
				dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

				::CopyMemory( pTempBuf					, &type						, sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPosInRoom				, sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	, iterPlayer->m_wstrID		, sizeof(wchar_t) * USER_ID_LEN	);

				for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
				{
					if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
					{
						Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
					}
				}
			
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �κ� �ִ� ���� �������� ������� �� ������ ���ؼ� ����Ʈ����  ���� �޼��� �۽�
				//				  �׸��� ����� �������� �ο����� ����						
				//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) | m_wstrID( �÷��̾� ID )
				//				: | type | m_nRoomNo(���ȣ) | m_nJoiners(���������ο� | m_bRoomState(�����) |
				//	Size		:   BYTE + BYTE + wchar_t * USER_ID_LEN = 51byte
				//				:   BYTE + BYTE + BYTE + BYTE +BYTE  = 3byte;
				//-------------------------------------------------------------------------------------

				if( !m_listPlayersInLobby.empty()  )
				{

					ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_UPDATE_ROOMSTATE_IN_LOBBY;
					dwSize =sizeof(BYTE)*4 ;

					::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo		, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &iterRoom->m_nJoiners		, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &iterRoom->m_bRoomState	, sizeof(BYTE)	);


					for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
					{
						Send( &iterPlayerInLobby->m_SockAddr, pTempBuf	, dwSize );
					}
				}


				// ���� ������ �����̶��
				if( m_bCaptain )
				{
					//-------------------------------------------------------------------------------------
					//
					//	Function	: �����̶�� ���� ���� ���� �޽��� �۽�
					//	Packet		: | type | iJoiner( �濡�� ��ġ�� ) |
					//	Size		:   BYTE + BYTE = 1BYTE				
					//-------------------------------------------------------------------------------------


					::ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_GAME_CAPTAIN_CHANGE;
					dwSize = sizeof(BYTE)*2;
					BYTE nCaptainPosInRoom = 0;

					for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
						{
							nCaptainPosInRoom		= iJoiner;
							iterRoom->m_nCaptain	= iJoiner;
							break;
						}
					}

					::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &nCaptainPosInRoom	, sizeof(BYTE) );

					for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
						{
							Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
						}							
					}
				}
			}
			

			//-------------------------------------------------------------------------------------

			m_listPlayersInLobby.push_back( *iterPlayer );
			m_listPlayersInLobby.sort();

			// �濡 �ִ� ���� ����Ʈ���� ����
			m_listPlayersInRoom.erase( iterPlayer );	

			// �� ������ ����
			::ZeroMemory( pTempBuf, BUFSIZE );
			type = NetLobbyServer::STOC_OUT_ROOM;
			dwSize = sizeof(BYTE)*2;

			::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInLobbyNo		, sizeof(BYTE) );			

			Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
		}
	}

	LeaveCriticalSection( &m_csListPlayersInLobby );
	LeaveCriticalSection( &m_csListPlayersInRoom );
	LeaveCriticalSection( &m_csListRooms );

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// ���̾�α� ���� ������Ʈ

}


//-------------------------------------------------------------------------------------
//
//	Function	: ������ ������ ���� 1�� �����Ű�� ó��
//	Packet		: | type | m_RoomNo( ���ȣ ) | m_nPosInRoom(�濡���� ��ġ) 
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::OutRoomByCaptain( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type;
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	DWORD dwSize;

	PlayerInfo	stTmpPlayer;
	RoomInfo	stTmpRoom;
	BYTE		m_bCaptain = 0;
	BYTE		nPosInRoom = 0;	

//	BYTE		nPlayerInRoomNo;		// �濡 ������ ������ȣ
	BYTE		nPlayerInLobbyNo;		// �κ� ������ ������ȣ
	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayer;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;
		
	::CopyMemory( &stTmpRoom.m_nRoomNo		, pBuf + sizeof(BYTE)	, sizeof(BYTE) );	
	::CopyMemory( &nPosInRoom				, pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );	

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom );
	EnterCriticalSection( &m_csListPlayersInLobby );

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );			
		
		// �κ񿡼� ���� ���� ��ȣ ���ϱ�
		nPlayerInLobbyNo = 1;		
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			if( nPlayerInLobbyNo != iterPlayer->m_nPlayerNo )
				break;
			else
				++nPlayerInLobbyNo;
		}

		stTmpPlayer.m_nPlayerNo = iterRoom->m_pJoiner[ nPosInRoom ]->m_nPlayerNo;

		// �뿡�� �ش� ������ ��ȣ�� ã�Ƽ� ����Ʈ���� ������ 
		if( std::binary_search( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );	
						
			// �� ����Ʈ������ ��ȣ�� ��ü
			iterPlayer->m_nPlayerNo = nPlayerInLobbyNo;
			iterPlayer->m_nRoomNo	= 0;

			//-------------------------------------------------------------------------------------
			//
			//	Function	: �ű� ������ ���� �� �κ� �ִ� Client���� ����
			//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) | m_wstrID( �÷��̾� ���� ���̵� )
			//	Size		: BYTE + wchar_t * USER_ID_LEN = 51byte
			//-------------------------------------------------------------------------------------			
			if( !m_listPlayersInLobby.empty() )
			{
				::ZeroMemory( pTempBuf, BUFSIZE );
				type = NetLobbyServer::STOC_ADD_PLAYER;
				dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

				::CopyMemory( pTempBuf					,	&type							,	sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)	,	&iterPlayer->m_nPlayerNo		,	sizeof(BYTE)					);	
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&iterPlayer->m_wstrID			,	sizeof(wchar_t) * USER_ID_LEN	);	
					
				// �ű� �����ڿ� ���� ������ ���� Lobby�� �ִ� Client�鿡�� ����
				for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
				{
					Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
				}
			}			
			
			// ������ ����
		//	nPosInRoom = iterPlayer->m_nPosAtRoom ;
			iterRoom->m_pJoiner[ nPosInRoom ] = NULL;
			for( int iJoiner = nPosInRoom + 1; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
			{
				iterRoom->m_pJoiner[ iJoiner - 1 ] = iterRoom->m_pJoiner[ iJoiner ];
			}			
			iterRoom->m_pJoiner[ --iterRoom->m_nJoiners ] = NULL;
			iterRoom->m_bRoomState = ROOM_STAND;		
			
			// �濡 ������ �ο��̿��ٸ�
			if( 0 >= iterRoom->m_nJoiners )
			{

				//-------------------------------------------------------------------------------------
				//
				//	Function	: �κ� �ִ� �����鿡�� �� ���� ���� �۽�
				//	Packet		: | type | nRoomNo( ���ȣ ) |
				//	Size		:  BYTE + BYTE 1Byte;
				//-------------------------------------------------------------------------------------
				if( !m_listPlayersInLobby.empty()  )
				{
					::ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_REMOVE_ROOM;
					dwSize = sizeof(BYTE)*2;

					::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo	, sizeof(BYTE) );

					for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
					{
						Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
					}
				}

				m_listRooms.erase( iterRoom );
				
			}
			// �濡 �����ִ� �����鿡�� ���� ������ ���� ���� �޽��� �۽�
			else
			{
			
				
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �濡 �����ִ� �����鿡�� ���� ������ ���� ���� �޽��� �۽�
				//	Packet		: | type | nPosInRoom( �÷��̾� �濡�� ��ġ�� ) | m_wstrID( �÷��̾� ID )
				//	Size		:  BYTE + BYTE + wchar_t * USER_ID_LEN = 51Byte;
				//-------------------------------------------------------------------------------------

				::ZeroMemory( pTempBuf, BUFSIZE );
				type = NetLobbyServer::STOC_REMOVE_PLAYER_IN_ROOM;
				dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

				::CopyMemory( pTempBuf					, &type						, sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPosInRoom				, sizeof(BYTE)					);
				::CopyMemory( pTempBuf + sizeof(BYTE)*2	, iterPlayer->m_wstrID		, sizeof(wchar_t) * USER_ID_LEN	);

				for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
				{
					if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
					{
						Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
					}
				}
			
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �κ� �ִ� ���� �������� ������� �� ������ ���ؼ� ����Ʈ����  ���� �޼��� �۽�
				//				  �׸��� ����� �������� �ο����� ����						
				//	Packet		: | type | nPlayerNo(�÷��̾��ȣ) | m_wstrID( �÷��̾� ID )
				//				: | type | m_nRoomNo(���ȣ) | m_nJoiners(���������ο� | m_bRoomState(�����) |
				//	Size		:   BYTE + BYTE + wchar_t * USER_ID_LEN = 51byte
				//				:   BYTE + BYTE + BYTE + BYTE +BYTE  = 3byte;
				//-------------------------------------------------------------------------------------

				if( !m_listPlayersInLobby.empty()  )
				{
					ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_UPDATE_ROOMSTATE_IN_LOBBY;
					dwSize =sizeof(BYTE)*4 ;

					::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo		, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &iterRoom->m_nJoiners		, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &iterRoom->m_bRoomState	, sizeof(BYTE)	);


					for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
					{
						Send( &iterPlayerInLobby->m_SockAddr, pTempBuf	, dwSize );
					}
				}


					// ���� ������ �����̶��
				if( m_bCaptain )
				{
				//-------------------------------------------------------------------------------------
				//
				//	Function	: �����̶�� ���� ���� ���� �޽��� �۽�
				//	Packet		: | type | iJoiner( �濡�� ��ġ�� ) |
				//	Size		:   BYTE + BYTE = 1BYTE				
				//-------------------------------------------------------------------------------------


					::ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_GAME_CAPTAIN_CHANGE;
					dwSize = sizeof(BYTE)*2;
					BYTE nCaptainPosInRoom = 0;

					for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
						{
							nCaptainPosInRoom = iJoiner;
							break;
						}
					}

//					if( nPosInRoom < nCaptainPosInRoom )
//						--nCaptainPosInRoom;
			
					::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &nCaptainPosInRoom	, sizeof(BYTE) );

					for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
						{
							Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
						}							
					}
				}

			}
			

			//-------------------------------------------------------------------------------------

		
		
			// �� ������ ����
			::ZeroMemory( pTempBuf, BUFSIZE );
			type = NetLobbyServer::STOC_OUT_ROOM;
			dwSize = sizeof(BYTE)*2;

			::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInLobbyNo		, sizeof(BYTE) );			

			Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );

			m_listPlayersInLobby.push_back( *iterPlayer );
			m_listPlayersInLobby.sort();

				// �濡 �ִ� ���� ����Ʈ���� ����
			m_listPlayersInRoom.erase( iterPlayer );	

		}
	}

	LeaveCriticalSection( &m_csListPlayersInLobby );
	LeaveCriticalSection( &m_csListPlayersInRoom );
	LeaveCriticalSection( &m_csListRooms );

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// ���̾�α� ���� ������Ʈ

}


//-------------------------------------------------------------------------------------
//
//	Function	: �ش� Ŭ���̾�Ʈ�� ���� �����û�� ���� ó��
//	Packet		: Size | type | nPlayerNo(�÷��̾��ȣ) | nRoomNo(���ȣ) 
//	Size		: DWORD + BYTE + BYTE 1byte
//-------------------------------------------------------------------------------------		
void	NetLobbyServer::Disconnect( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{

	char pTempBuf[ BUFSIZE ];
	PlayerInfo	stTmpPlayer;
	RoomInfo	stTmpRoom;
	BYTE type;
	BYTE bCaptain = 0;
	BYTE nPosInRoom = 0;
	DWORD dwSize;

	::CopyMemory( &stTmpPlayer.m_nRoomNo	,	pBuf + sizeof(BYTE)		, sizeof(BYTE) );
	::CopyMemory( &stTmpPlayer.m_nPlayerNo	,	pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );
	::CopyMemory( &bCaptain					,	pBuf + sizeof(BYTE)*3	, sizeof(BYTE) );
	::CopyMemory( &nPosInRoom				,	pBuf + sizeof(BYTE)*4	, sizeof(BYTE) );

	stTmpRoom.m_nRoomNo = stTmpPlayer.m_nRoomNo;

	std::list< PlayerInfo >::iterator	iterPlayer;
	std::list< RoomInfo >::iterator		iterRoom;

	// �κ񿡼� ������ ��������
	if( 0 == stTmpPlayer.m_nRoomNo )
	{		
		EnterCriticalSection(&m_csListPlayersInLobby);		

		if( std::binary_search( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer );

//			delete *iterPlayer;

			m_listPlayersInLobby.erase( iterPlayer );			

			if( !m_listPlayersInLobby.empty() )
			{
				::ZeroMemory( pTempBuf, BUFSIZE );
				dwSize = sizeof(BYTE)*2;
				type = NetLobbyServer::STOC_REMOVE_PLAYER_IN_LOBBY;			
			
				::CopyMemory( pTempBuf					, &type							, sizeof(BYTE)	);
				::CopyMemory( pTempBuf + sizeof(BYTE)	, &stTmpPlayer.m_nPlayerNo		, sizeof(BYTE)	);

				for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
				{
					Send( &iterPlayer->m_SockAddr, pTempBuf, (int)dwSize );
				}
			}
		}

		LeaveCriticalSection(&m_csListPlayersInLobby);
	}

	// �濡�� ������ ��������
	else
	{
		
		EnterCriticalSection(&m_csListPlayersInRoom);
		EnterCriticalSection(&m_csListPlayersInLobby);
		EnterCriticalSection(&m_csListRooms);

		if( std::binary_search( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );

//			delete *iterPlayer;
			stTmpPlayer.m_nPosAtRoom = iterPlayer->m_nPosAtRoom;			

			if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
			{
				iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );

				// �濡�� �ش� Disconnect�� Client�� ����
				iterRoom->m_pJoiner[ stTmpPlayer.m_nPosAtRoom ] = NULL;
				for( int iJoiner = stTmpPlayer.m_nPosAtRoom + 1; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
				{
					iterRoom->m_pJoiner[ iJoiner - 1 ] = iterRoom->m_pJoiner[ iJoiner ];
				}			
				iterRoom->m_pJoiner[ --iterRoom->m_nJoiners ] = NULL;
				iterRoom->m_bRoomState = ROOM_STAND;		
				
				if( 0 >= iterRoom->m_nJoiners )
				{
					//-------------------------------------------------------------------------------------
					//
					//	Function	: �����̿���, ������ �ο��� �������� �κ񿡼� ������ Client���� ������ Event �۽�.
					//	Packet		: | type | m_nRoomNo(���ȣ)
					//	Size		: DWORD + BYTE + BYTE 1byte
					//-------------------------------------------------------------------------------------

					if( !m_listPlayersInLobby.empty() )
					{
						::ZeroMemory( pTempBuf, BUFSIZE );
						dwSize = sizeof(BYTE)*2;
						type = NetLobbyServer::STOC_REMOVE_ROOM;

						::CopyMemory( pTempBuf					,	&type					, sizeof(BYTE) );
						::CopyMemory( pTempBuf + sizeof(BYTE)	,	&iterRoom->m_nRoomNo	, sizeof(BYTE) );

						for( iterPlayer =  m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
						{
							Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );
						}
					}
					
					m_listRooms.erase( iterRoom );

				}
				else
				{				
/*
					//-------------------------------------------------------------------------------------
					//
					//	Function	: �ش� Ŭ���̾�Ʈ�� ���� �����û�� ���� ó��
					//	Packet		: | type | m_nPosAtRoom(�濡�� ��ġ) 
					//	Size		: DWORD + BYTE + BYTE 1byte
					//-------------------------------------------------------------------------------------

					::ZeroMemory( pTempBuf, BUFSIZE );
					dwSize = sizeof(BYTE)*2;
					type = NetLobbyServer::STOC_REMOVE_PLAYER_IN_ROOM;

					::CopyMemory( pTempBuf					, &type							, sizeof(BYTE)	);
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &stTmpPlayer.m_nPosAtRoom		, sizeof(BYTE)	);

					for( int iPlayer = 0; iPlayer < iterRoom->m_nJoiners; iPlayer++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iPlayer ] )
							Send( &iterRoom->m_pJoiner[ iPlayer ]->m_SockAddr, pTempBuf, dwSize );
					}

					iterRoom->m_nJoiners--;
*/
					//-------------------------------------------------------------------------------------
					//
					//	Function	: �濡 �����ִ� �����鿡�� ���� ������ ���� ���� �޽��� �۽�
					//	Packet		: | type | nPosInRoom( �÷��̾� �濡�� ��ġ�� ) | m_wstrID( �÷��̾� ID )
					//	Size		:  BYTE + BYTE + wchar_t * USER_ID_LEN = 51Byte;
					//-------------------------------------------------------------------------------------

					::ZeroMemory( pTempBuf, BUFSIZE );
					type = NetLobbyServer::STOC_REMOVE_PLAYER_IN_ROOM;
					dwSize = sizeof(BYTE)*2 + sizeof(wchar_t) * USER_ID_LEN;

					::CopyMemory( pTempBuf					, &type						, sizeof(BYTE)					);
					::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPosInRoom				, sizeof(BYTE)					);
					::CopyMemory( pTempBuf + sizeof(BYTE)*2	, iterPlayer->m_wstrID		, sizeof(wchar_t) * USER_ID_LEN	);

					for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
					{
						if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
						{
							Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
						}
					}


					// ���� ������ �����̶��				
					if( bCaptain )
					{
						//-------------------------------------------------------------------------------------
						//
						//	Function	: �����̶�� ���� ���� ���� �޽��� �۽�
						//	Packet		: | type | iJoiner( �濡�� ��ġ�� ) |
						//	Size		:   BYTE + BYTE = 1BYTE				
						//-------------------------------------------------------------------------------------

						::ZeroMemory( pTempBuf, BUFSIZE );
						type = NetLobbyServer::STOC_GAME_CAPTAIN_CHANGE;
						dwSize = sizeof(BYTE)*2;
						BYTE nCaptainPosInRoom = 0;

						for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
						{
							if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
							{
								nCaptainPosInRoom		= iJoiner;
								iterRoom->m_nCaptain	= iJoiner;
								break;
							}
						}

						::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
						::CopyMemory( pTempBuf + sizeof(BYTE)	, &nCaptainPosInRoom	, sizeof(BYTE) );

						for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
						{
							if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
							{
								Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
							}							
						}
					}
				}									
			}
		}

		m_listPlayersInRoom.erase( iterPlayer );

		LeaveCriticalSection(&m_csListRooms);
		LeaveCriticalSection(&m_csListPlayersInLobby);
		LeaveCriticalSection(&m_csListPlayersInRoom);		
		
	}	

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// ���̾�α� ���� ������Ʈ
}

//		�ش� Ŭ���̾�Ʈ chat ������ Lobby�� �ִ� ������ Ŭ���̾�Ʈ�鿡�� ����
void	NetLobbyServer::ChatInLobby( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	BYTE type = NetLobbyServer::STOC_CHAT_IN_LOBBY;
	DWORD dwSize = 0;
	BYTE nLenMsg = 0;

	::CopyMemory( &nLenMsg,		pBuf + sizeof(BYTE)		, sizeof(BYTE) );

	::CopyMemory( pTempBuf					,	&type					, sizeof(BYTE)				);
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&nLenMsg				, sizeof(BYTE)				);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	pBuf + sizeof(BYTE)*2	, sizeof(wchar_t) * nLenMsg );

	dwSize = sizeof(BYTE)*2 + sizeof(wchar_t)*nLenMsg;

	std::list< PlayerInfo >::iterator iterPlayer;

	EnterCriticalSection( &m_csListPlayersInLobby );

	for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
	{
		Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );		
	}	

	LeaveCriticalSection( &m_csListPlayersInLobby );
}


//		�ش� Ŭ���̾�Ʈ�� chat ������ ���� Ŭ���̾�Ʈ�� ��ȿ� ������ ������ Ŭ���̾�Ʈ�鿡�� ����
void	NetLobbyServer::ChatInRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	BYTE type = NetLobbyServer::STOC_CHAT_IN_ROOM;
	DWORD dwSize = 0;
	BYTE nLenMsg = 0;
	RoomInfo tmpRoom;

	::CopyMemory( &tmpRoom.m_nRoomNo	,	pBuf + sizeof(BYTE)		, sizeof(BYTE) );
	::CopyMemory( &nLenMsg				,	pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );	

	::CopyMemory( pTempBuf					,	&type					, sizeof(BYTE)				);	
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&nLenMsg				, sizeof(BYTE)				);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	pBuf + sizeof(BYTE)*3	, sizeof(wchar_t) * nLenMsg );	

	dwSize = sizeof(BYTE)*2 + sizeof(wchar_t)*nLenMsg;

	std::list< RoomInfo >::iterator iterRoom;

	EnterCriticalSection( &m_csListRooms );

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );

		for( int iPlayer = 0; iPlayer < iterRoom->m_nJoiners; iPlayer++ )
		{
			Send( &iterRoom->m_pJoiner[ iPlayer ]->m_SockAddr, pTempBuf, dwSize );
		}
	}

	LeaveCriticalSection( &m_csListRooms );

}

//		
//-------------------------------------------------------------------------------------
//
//	Function	: ��ȿ� �ִ� ������ ���� ���� ��û ���¸� ���� ó��
//	Packet		: | type | m_PlayNo( ������ȣ ) | m_RoomNo( ���ȣ ) | m_nPosInRoom(�濡���� ��ġ) | nPushType(�غ�Ϸ� ����) |
//	Size		:  BYTE + BYTE + BYTE + BYTE =  3byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::GameReady( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
	BYTE type = 0;
	DWORD dwSize = 0;
	
	RoomInfo tmpRoom;
	PlayerInfo tmpPlayer;
	BYTE nPosInRoom = 0;
	BYTE nPushType	= 0;

	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( &tmpPlayer.m_nPlayerNo		,	pBuf + sizeof(BYTE)		,		sizeof(BYTE) );
	::CopyMemory( &tmpRoom.m_nRoomNo			,	pBuf + sizeof(BYTE)*2	,		sizeof(BYTE) );
	::CopyMemory( &nPosInRoom					,	pBuf + sizeof(BYTE)*3	,		sizeof(BYTE) );
	::CopyMemory( &nPushType					,	pBuf + sizeof(BYTE)*4	,		sizeof(BYTE) );


	EnterCriticalSection( &m_csListRooms );

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );

		if( nPushType )
			iterRoom->m_nJoinerState[ nPosInRoom ] = STATE_READY;
		else
			iterRoom->m_nJoinerState[ nPosInRoom ] = STATE_DEFAULT;


		//-------------------------------------------------------------------------------------
		//
		//	Function	: ��ȿ� �ִ� ������ �����鿡�� Ư�� ������ ���� ���� ������ �۽�
		//	Packet		: | type | nPosInRoom( �濡���� ��ġ ) | nPushType( ���� ���� ) |
		//	Size		:  BYTE + BYTE + BYTE + BYTE =  3byte
		//-------------------------------------------------------------------------------------
		type = NetLobbyServer::STOC_JOINER_READY_STATE;
		dwSize = sizeof(BYTE)*3;

		::CopyMemory( pTempBuf					,	&type			,	sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)	,	&nPosInRoom		,	sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nPushType		,	sizeof(BYTE) );

		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{
			if( NULL != iterRoom->m_pJoiner[ iJoiner ] && nPosInRoom != iJoiner )
			{
				Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
			}
		}
	}

	LeaveCriticalSection( &m_csListRooms );
}


//		��ȿ� �ִ� ������ �� ������ ���� ó��
void	NetLobbyServer::MapChange	( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
	BYTE type = 0;
	DWORD dwSize = 0;
	RoomInfo tmpRoom;
	BYTE	nMapIndex = 0;

	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;

	::CopyMemory( &tmpRoom.m_nRoomNo	,	pBuf + sizeof(BYTE)		,	sizeof(BYTE) );
	::CopyMemory( &tmpRoom.m_nMapInfo	,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE) );

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInLobby);
	
	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );
		iterRoom->m_nMapInfo = tmpRoom.m_nMapInfo;

		//-------------------------------------------------------------------------------------
		//
		//	Function	: ������ ���� ��ü ��û
		//	Packet		: | type | nMapIndex( �ʹ�ȣ ) |
		//	Size		:  BYTE + BYTE=  2byte
		//-------------------------------------------------------------------------------------
		type = NetLobbyServer::STOC_GAME_MAP_CHANGE;
		dwSize = sizeof(BYTE)*2;

		::CopyMemory( pTempBuf					,	&type					,	sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)	,	&tmpRoom.m_nMapInfo		,	sizeof(BYTE) );
		
		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{
			if( NULL != iterRoom->m_pJoiner[ iJoiner ] && STATE_CAPTAIN != iterRoom->m_nJoinerState[ iJoiner ] )
			{
				Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
			}
		}

		//-------------------------------------------------------------------------------------
		//
		//	Function	: �κ� �ִ� ������ �������� ���� ��ü ���� �۽�
		//	Packet		: | type | tmpRoom.m_nRoomNo(�� ��ȣ) | nMapIndex( �ʹ�ȣ ) |
		//	Size		:  BYTE + BYTE=  2byte
		//-------------------------------------------------------------------------------------

		if( !m_listPlayersInLobby.empty() )
		{
			::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
			type   = NetLobbyServer::STOC_GAME_MAP_CHANGE_IN_LOBBY;
			dwSize = sizeof(BYTE)*3;

			::CopyMemory( pTempBuf					,	&type					,	sizeof(BYTE) );
			::CopyMemory( pTempBuf + sizeof(BYTE)	,	&tmpRoom.m_nRoomNo		,	sizeof(BYTE) );
			::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&tmpRoom.m_nMapInfo		,	sizeof(BYTE) );		

			for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
			{
				Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
			}
		}
	}

	LeaveCriticalSection( &m_csListPlayersInLobby);
	LeaveCriticalSection( &m_csListRooms );

}

//-------------------------------------------------------------------------------------
//
//	Function	: �������� Game Process�� �����Ͽ� ������ Ŭ���̾�Ʈ ������ �ش� Ŭ���̾�Ʈ(����)���� ����
//	Packet		: | type | m_nPosInRoom( ���� ��ġ ) | m_RoomNo( ���ȣ ) | 
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::GameStart( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
	BYTE type = 0;
	DWORD dwSize = 0;
	
	RoomInfo tmpRoom;
	BYTE	nCaptainPosInRoom = 0;
	
	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;

	::CopyMemory( &nCaptainPosInRoom		,	pBuf + sizeof(BYTE)		,	sizeof(BYTE) );
	::CopyMemory( &tmpRoom.m_nRoomNo		,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE) );

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInLobby);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );

		iterRoom->m_bRoomState = ROOM_RUNNING_GAME;
	
		//	�κ� �ִ� �����鿡�� �� ���� ���� ���� �۽�
		if( !m_listPlayersInLobby.empty() )
		{	
			type	= NetLobbyServer::STOC_UPDATE_ROOMSTATE_IN_LOBBY;
			dwSize	= sizeof(BYTE)*4;					

			::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)	);
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo		, sizeof(BYTE)	);
			::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &iterRoom->m_nJoiners		, sizeof(BYTE)	);
			::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &iterRoom->m_bRoomState	, sizeof(BYTE)	);

			for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
			{
				Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
			}
		}
		
		//-------------------------------------------------------------------------------------
		//
		//	Function	: ���忡�� ���ӿ� ������ ������ ������ �ּ� ������ �۽�
		//	Packet		: | type | m_nJoiners(�ο� ��) | SOCKADDR_IN * n�� |
		//	Size		:  BYTE + BYTE + SOCKADDR_IN * N��=  1byte
		//-------------------------------------------------------------------------------------
		
		::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
		type	= NetLobbyServer::STOC_GAME_JOINER_ADDR_LIST;
		dwSize	= sizeof(BYTE)*3 + sizeof(SOCKADDR_IN);					
//		DWORD dwJoinerSize = sizeof(SOCKADDR_IN) + sizeof(wchar_t)*USER_ID_LEN;
		DWORD dwJoinerSize = sizeof(wchar_t)*USER_ID_LEN;
		BYTE nJoiner = iterRoom->m_nJoiners - 1;

		::CopyMemory( pTempBuf 					, &type													, sizeof(BYTE)			);
		::CopyMemory( pTempBuf + sizeof(BYTE)	, &nJoiner												, sizeof(BYTE)			);
		::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &iterRoom->m_nMapInfo									, sizeof(BYTE)			);
		::CopyMemory( pTempBuf + sizeof(BYTE)*3	, &iterRoom->m_pJoiner[ nCaptainPosInRoom ]->m_SockAddr	, sizeof(SOCKADDR_IN)	);

			
/*
		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{	
			if( NULL != iterRoom->m_pJoiner[ iJoiner ] && STATE_CAPTAIN != iterRoom->m_nJoinerState[ iJoiner ] )
			{
				::CopyMemory( pTempBuf + dwSize, &iterRoom->m_pJoiner[ iJoiner ]->m_wstrID, sizeof(wchar_t)*USER_ID_LEN	);

//				::CopyMemory( pTempBuf + dwSize							, &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr	, sizeof(SOCKADDR_IN)			);
//				::CopyMemory( pTempBuf + dwSize + sizeof(SOCKADDR_IN)	, &iterRoom->m_pJoiner[ iJoiner ]->m_wstrID		, sizeof(wchar_t)*USER_ID_LEN	);

				dwSize += dwJoinerSize;
			}
		}
*/
		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );


		//-------------------------------------------------------------------------------------
		//
		//	Function	: ���ӿ� ������ ������ �������� ���� �ּ� ���� �۽�
		//	Packet		: | type | SOCKADDR_IN  |
		//	Size		:  BYTE + BYTE + SOCKADDR_IN * N��=  1byte
		//-------------------------------------------------------------------------------------
		
		::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
		type	= NetLobbyServer::STOC_GAME_CAPTAIN_ADDR;
		dwSize	= sizeof(BYTE)*4 + sizeof(SOCKADDR_IN) + sizeof(wchar_t)*USER_ID_LEN;					
		
		::CopyMemory( pTempBuf 											, &type														, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)							, &nJoiner													, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*2							, &iterRoom->m_nMapInfo										, sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*3							, &iterRoom->m_nCaptain										, sizeof(BYTE)					);		
		::CopyMemory( pTempBuf + sizeof(BYTE)*4							, &iterRoom->m_pJoiner[ nCaptainPosInRoom ]->m_SockAddr		, sizeof(SOCKADDR_IN)			);

//		::CopyMemory( pTempBuf + sizeof(BYTE)*2 + sizeof(SOCKADDR_IN)	, &iterRoom->m_pJoiner[ nCaptainPosInRoom ]->m_wstrID		, sizeof(wchar_t)*USER_ID_LEN	);

		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{
			if( NULL!= iterRoom->m_pJoiner[ iJoiner ] && STATE_CAPTAIN != iterRoom->m_nJoinerState[ iJoiner ] ) 
			{			
				Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
			}
		}

	}

	LeaveCriticalSection( &m_csListPlayersInLobby);
	LeaveCriticalSection( &m_csListRooms );

}


//-------------------------------------------------------------------------------------
//
//	Function	: �κ� �ִ� �����鿡�� �ʴ� �޽��� �۽� ó��
//	Packet		: | type | m_RoomNo( ���ȣ ) | 
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
void NetLobbyServer::Invitation	( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
	BYTE type = 0;
	DWORD dwSize = 0;
	BYTE nPlayers = 0;
	
	RoomInfo tmpRoom;	
	
	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;
	
	::CopyMemory( &tmpRoom.m_nRoomNo		,	pBuf + sizeof(BYTE)	,	sizeof(BYTE) );

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInLobby);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );


		if( !m_listPlayersInLobby.empty() )
		{	
			type			= NetLobbyServer::STOC_GAME_INVITATION;
			dwSize			= sizeof(BYTE)*2;
			BYTE nPlayers	= static_cast< BYTE >( m_listPlayersInLobby.size() );

			::CopyMemory( pTempBuf 					, &type						, sizeof(BYTE)	);
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &iterRoom->m_nRoomNo		, sizeof(BYTE)	);			

			for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
			{
				Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
			}
			
			::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
			type			= NetLobbyServer::STOC_GAME_INVITATION_OK;
			dwSize			= sizeof(BYTE)*2;
			
			::CopyMemory( pTempBuf 					, &type			, sizeof(BYTE)	);
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayers		, sizeof(BYTE)	);

			Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
		}
	}
	
	LeaveCriticalSection( &m_csListPlayersInLobby);
	LeaveCriticalSection( &m_csListRooms );	

}

//-------------------------------------------------------------------------------------
//
//	Function	: �κ񿡼� Chat Message �۽�
//	Packet		: | type | nSendMsg( ä�� �޽��� ����) | strSendMsg
//	Size		:   BYTE + BYTE + sizeof(wchar_t) * nLen = ?byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::SendMsgOfManager( CString &msg )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
	BYTE	type	= NetLobbyServer::STOC_MANAGER_MESSAGE;	
	BYTE	nMsgLen = static_cast< BYTE >( msg.GetLength() );
	DWORD	dwSize	= sizeof(BYTE)*2 + sizeof(wchar_t)*nMsgLen;

	std::list< PlayerInfo >::iterator iterPlayer;

	::CopyMemory( pTempBuf,						&type						,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE),		&nMsgLen					,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2,	msg.GetBuffer()				,	sizeof(wchar_t)*nMsgLen	);

	EnterCriticalSection(&m_csListPlayersInLobby);
	EnterCriticalSection(&m_csListPlayersInRoom);

	if( !m_listPlayersInLobby.empty() )
	{
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );
		}
	}

	if( !m_listPlayersInRoom.empty() )
	{
		for( iterPlayer = m_listPlayersInRoom.begin(); iterPlayer != m_listPlayersInRoom.end(); ++iterPlayer )
		{
			Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );
		}
	}
	
	LeaveCriticalSection(&m_csListPlayersInRoom);
	LeaveCriticalSection(&m_csListPlayersInLobby);
	
}

//-------------------------------------------------------------------------------------
//
//	Function	: ���� ���� �޽��� �۽� ��û
//	Packet		: | type | m_nPosInRoom( ���� ���� ) | nPosInRoom( ���ο� ���� ) |
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::GiveCaptain( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	
//	BYTE	nOldCaptain = 0;
	BYTE	nNewCaptain = 0;
//	BYTE	tempState	= 0;
	BYTE	type		= 0;
	DWORD	dwSize		= 0;

	RoomInfo stTmpRoom;

	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInRoom;	

	::CopyMemory( &stTmpRoom.m_nRoomNo	,	pBuf + sizeof(BYTE)		,	sizeof(BYTE)	);
	::CopyMemory( &nNewCaptain			,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE)	);

	
	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );

		iterRoom->m_nCaptain = nNewCaptain;

//		tempState								= iterRoom->m_nJoinerState[ nNewCaptain ];		
//		iterRoom->m_nJoinerState[ nNewCaptain ] = STATE_CAPTAIN;
//		iterRoom->m_nJoinerState[ nOldCaptain ] = tempState;		

		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{
			if( NULL != iterRoom->m_pJoiner[ iJoiner ] )
			{
				iterRoom->m_nJoinerState[ iJoiner ] = STATE_DEFAULT;
			}
		}

		iterRoom->m_nJoinerState[ nNewCaptain ] = STATE_CAPTAIN;				

		//-------------------------------------------------------------------------------------
		//
		//	Function	: ���� ���� �޽��� �۽� ��û
		//	Packet		: | type | nOldCaptain( ���� ���� ) | nNewCaptain( ���ο� ���� ) |
		//	Size		:  BYTE + BYTE + BYTE =  2byte
		//-------------------------------------------------------------------------------------
		type	= NetLobbyServer::STOC_GAME_GIVE_CAPTAIN;
		dwSize	= sizeof(BYTE)*2;							

		::CopyMemory( pTempBuf 					, &type				, sizeof(BYTE)	);
		::CopyMemory( pTempBuf + sizeof(BYTE)	, &nNewCaptain		, sizeof(BYTE)	);
		

		for( BYTE iJoiner = 0; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
		{	
			if( NULL!= iterRoom->m_pJoiner[ iJoiner ] )
			{
				Send( &iterRoom->m_pJoiner[ iJoiner ]->m_SockAddr, pTempBuf, dwSize );
			}
		}		
	}		

	LeaveCriticalSection( &m_csListPlayersInRoom);
	LeaveCriticalSection( &m_csListRooms );	
}


//-------------------------------------------------------------------------------------
//
//	Function	: �濡 �ִ� ���� 1���� ���� ���� ó��
//	Packet		: | type | m_RoomNo( �� ��ȣ ) | nPosInRoom( ���� ��ġ ) |
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
void	NetLobbyServer::JoinerInfoAtRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	char pTempBuf[ BUFSIZE ];
	char query[255];
	char win[5], fail[5], level[3], exp[10];
	
	BYTE	nWin = 0, nFail = 0, nLevel = 0;
	unsigned short nExp = 0;			

	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	::ZeroMemory( query, sizeof(query) );
	
	BYTE	nPosInRoom	= 0;
	BYTE	type		= 0;
	DWORD	dwSize		= 0;

	MYSQL_RES   *sql_result = NULL;

	RoomInfo stTmpRoom;

	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInRoom;	

	::CopyMemory( &stTmpRoom.m_nRoomNo	,	pBuf + sizeof(BYTE)		,	sizeof(BYTE)	);
	::CopyMemory( &nPosInRoom			,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE)	);	

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );

		if( NULL != iterRoom->m_pJoiner[ nPosInRoom ] )
			goto End;

		sprintf_s( query, "select * from member where NAME = \"%s\"",  iterRoom->m_pJoiner[ nPosInRoom ]->m_wstrID);

		// ���� ����
		if( 0 != mysql_query( &m_dbMysql, query ) )
		{
			MessageBox( NULL, L"NetLobbyServer::JoinMember - mysql_query() Failed", L"Faield", MB_OK );        
			goto End;
		}

		// ����� ��´�.
		sql_result = mysql_store_result( &m_dbMysql );

		if( mysql_num_rows( sql_result ) >= 1 )
		{
			MYSQL_ROW sql_row = mysql_fetch_row(sql_result);
			
			strcpy_s( win	, sql_row[3] );
			strcpy_s( fail	, sql_row[4] );
			strcpy_s( level	, sql_row[5] );
			strcpy_s( exp	, sql_row[6] );
			
			nWin	= static_cast< BYTE >( atoi(win) );
			nFail	= static_cast< BYTE >( atoi(fail) );
			nLevel	= static_cast< BYTE >( atoi(level) );
			nExp	= static_cast< unsigned short >( atoi(exp) );

			//------------------------------------------------------------------------------------
			//	Function	: ���� 1�� ���� �۽�
			//	Packet		: | type | nPosInRoom( �� ��ȣ ) | nWin( �� ) | nFail( �� ) | nLevel( ���� ) | nExp( ����ġ ) |
			//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE + USHORT=  6byte
			//------------------------------------------------------------------------------------

			type = NetLobbyServer::STOC_GAME_JOINER_INFO;
			dwSize = sizeof(BYTE)*4 + sizeof( unsigned short );
			
			::CopyMemory( pTempBuf					,	&type		,	sizeof(BYTE)			);			
			::CopyMemory( pTempBuf + sizeof(BYTE)	,	&nPosInRoom	,	sizeof(BYTE)			);
			::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nWin		,	sizeof(BYTE)			);
			::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	&nFail		,	sizeof(BYTE)			);
			::CopyMemory( pTempBuf + sizeof(BYTE)*4	,	&nLevel		,	sizeof(BYTE)			);
			::CopyMemory( pTempBuf + sizeof(BYTE)*5	,	&nExp		,	sizeof(unsigned short)	);

			Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );			
		}
		else
			goto End;

	}

End:
	LeaveCriticalSection( &m_csListPlayersInRoom);
	LeaveCriticalSection( &m_csListRooms );	
}