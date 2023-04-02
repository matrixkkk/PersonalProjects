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


// 서버 시작
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

	
	// MYSQL 초기화
	mysql_init(&m_dbMysql);
	
	// DB 접속
	if ( !mysql_real_connect(&m_dbMysql, MYSQL_HOST, MYSQL_USER, MYSQL_PWD, MYSQL_DB,3306, 0, 0 ) )
	{
		MessageBox( NULL, L"NetLobbyServer::Begin() - mysql_real_connect()", L"MySql DB Connect Failed", MB_OK );
		return FALSE;
	}

	//	데이타 베이스 고르기
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


// 서버 종료
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

//	Recv 되어온 패킷을 분석 그에 맞게 처리하는 함수
void	NetLobbyServer::ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type = 0;
	memcpy( &type, pBuf, sizeof( BYTE ) );

	switch( type )
	{

	case CTOS_ROOM_LIST:
		// 방 리스트 정보 송신
		RoomList( pSocketInfo, pBuf, nLen );

		break;

	case CTOS_PLAYER_LIST:
		// 접속자 리스트 정보 송신
		PlayList( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_LOGIN:
		// 접소 정보 송신 처리
		Login( pSocketInfo, pBuf, nLen );
		break;
	
	case CTOS_OUT_ROOM:
		// 해당 클라이언트 방 나가기 요청을 수신하여 방 나가기 처리
		OutRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_DISCONNECT:
		// 해당 클라이언트의 접속 종료요청을 수신 처리
		Disconnect( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_JOIN_MEMBERSHIP:
		// 회원가입 요청
		JoinMember( pSocketInfo, pBuf, nLen ); 
		break;

	case CTOS_CHAT_IN_LOBBY:
		// 로비에서 넘어온 채팅 메시지
		ChatInLobby( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_CHAT_IN_ROOM:
		// 방 넘어온 채팅 메시지
		ChatInRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_CREATE_ROOM:
		// 방 생성 메시지
		CreateRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_ENTER_ROOM:
		// 방 입장 메시지
		JoinRoom( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_READY:
		// 방안에 있는 유저의 게임 레디 요청 상태를 수신 처리
		GameReady( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_MAP_CHANGE:
		// 방안에 있는 유저의 맵 정보를 갱신 처리
		MapChange( pSocketInfo, pBuf, nLen );
		break;	

	case CTOS_GAME_START:
		// 방에서 게임 시작 요청 수신 처리	
		GameStart( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_INVITATION:
		// 로비에 있는 유저들에게 초대 메시지 송신 처리
		Invitation	( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_FORCE_OUT_ROOM:
		// 유저 1명 강퇴시키기 요청 수신 처리
		OutRoomByCaptain( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_GIVE_CAPTAIN:
		// 방장 위임 요청을 수신 처리
		GiveCaptain( pSocketInfo, pBuf, nLen );
		break;

	case CTOS_GAME_JOINER_INFO:
		// 유저 1명 정보 수친 처리
		JoinerInfoAtRoom( pSocketInfo, pBuf, nLen );
		break;
	}
}
	
//	접속한 소켓정보를 이용하여 자료구조에 할당
void	NetLobbyServer::ProcessConnect( SocketInfo *pSocketInfo )
{


}
	
//	접속 종료한 소켓정보를 자료구조로부터 제거
void	NetLobbyServer::ProcessDisConnect( SocketInfo *pSocketInfo )
{


}



//-------------------------------------------------------------------------
//
//								Recv Functions
//
//	Desc : Client로 부터 요청 메시지를 받아서 처리하는 함수 모음
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
//
//	Function	: 클라이언트로부터 회원가입 메시지 수신 처리
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

	//------- 먼저 DB에서 해당 ID화 GamedID 존재 유무 파악--------

	// 쿼리문 작성
	sprintf_s( query, "select * from member where ID = \"%s\" AND Name = \"%s\"", szID, szGameID);

	// 쿼리 적용
	if( 0 != mysql_query( &m_dbMysql, query ) )
	{
		MessageBox( NULL, L"NetLobbyServer::JoinMember - mysql_query() Failed", L"Faield", MB_OK );        
		return;		
	}

	// 결과를 얻는다.
	sql_result = mysql_store_result( &m_dbMysql );

	//	몇개의 투플인지 row값을 얻어오다.
	//  이미 존재하면해당 Client에게 Fail 메시지 전송
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
//	Function	: 클라이언트로부터 접속 메시지 수신 처리
//	Packet		: Size | type | m_nPlayerNo | m_wstrID(Client ID) | 
//	Size		: DWORD + BYTE + BYTE  + (char) * USER_ID_LEN(25) = 50BYTE
//-------------------------------------------------------------------------------------

void	NetLobbyServer::Login( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	// 파일 또는 DB 에 있는 회원정보를 이용하여 존재 유무 확인
	
	// DB에 존재 유무 파악
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

	// 쿼리문 작성
	sprintf_s( query, "select * from %s where ID = \"%s\" AND PW = \"%s\"; ", MEMBER_TABLE, szID, szPW);

	// 쿼리 적용
	if( 0 != mysql_query( &m_dbMysql, query ) )
	{
		MessageBox( NULL, L"NetLobbyServer::Login - mysql_query() Failed", L"Faield", MB_OK );        
		return;		
	}

	// 결과를 얻는다.
	sql_result = mysql_store_result( &m_dbMysql );
	
	// 존재하는 아이디라면 자료구조에 추가 및 처리 실행
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

		// 신규 접속자 m_nPlayerNo 고유 번호 셋팅
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
		//	Function	: 신규 접속자 정보 각 로비에 있는 Client에게 전송
		//	Packet		: | type | nPlayerNo(플레이어번호) | m_wstrID( 플레이어 게임 아이디 )
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
			
			// 신규 접속자에 대한 정보를 기존 Lobby에 있는 Client들에게 전송
			for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
			{
				Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );
			}
		}
		
		// 신규 접속자를 자료구조에 추가
		m_listPlayersInLobby.push_back( pConnectPlayer );
		m_listPlayersInLobby.sort();

		LeaveCriticalSection(&m_csListPlayersInLobby );
		
		//-------------------------------------------------------------------------------------
		//
		//	Function	: 각각의 클라이언트에게 새로운 Client의 정보 전송 다했고 로그인이 승인 되었으면 고유번호를 보내준다.
		//	Packet		: | type | nPlayerNo(플레이어번호) 
		//	Size		: BYTE + BYTE 1byte
		//-------------------------------------------------------------------------------------
		::ZeroMemory( pTempBuf, BUFSIZE );
		dwSize = sizeof(BYTE)*2 + sizeof(wchar_t)*USER_ID_LEN;
		type = NetLobbyServer::STOC_LOGIN_OK;
		
		::CopyMemory( pTempBuf 					,	&type						,	sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)	,	&pConnectPlayer.m_nPlayerNo,	sizeof(BYTE)					);
		::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&pConnectPlayer.m_wstrID	,	sizeof(wchar_t) * USER_ID_LEN	);

		Send( &pConnectPlayer.m_SockAddr, pTempBuf, (int)dwSize );
		
		::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// 다이얼로그 정보 업데이트
	}
}



//-------------------------------------------------------------------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트에게 로비서버 생성된 방 리스트 정보 송신
//	Packet		: | type | nRoomNo(방번호) | m_nJoiners(현재참여자) | m_nMaxJoiners(방제한인원) |  m_bRoomState(방상태) | m_nMapInfo(맵정보) | m_wstrRoomName(방이름)	
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

	// 남은 Data 전송
	if( cntRoom >= 1  && dwSize > (sizeof(DWORD) + sizeof(BYTE)) )
	{
		::CopyMemory( pTempBuf + sizeof(BYTE), &dwSize, sizeof(DWORD) );
		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
	}


	//-------------------------------------------------------------------------------------
	//
	//	Function	: 룸리스트 완료 Message 송신 Client는 이 정보를 받은후 내부 자료구조의 Data를 MFC 인터페이스에 출력 작업 수행
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
//	Function	: 해당 클라이언트에게 로비서버 접속한 Client 리스트 정보 송신
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

	// 남은 Data 전송
	if( cntPlayer >= 1  && dwSize > sizeof(DWORD) + sizeof(BYTE) )
	{
		::CopyMemory( pTempBuf + sizeof(BYTE), &dwSize, sizeof(DWORD) );
		Send( &pSocketInfo->m_SockAddr, pTempBuf, dwSize );
	}

	//-------------------------------------------------------------------------------------
	//
	//	Function	: 플레이어 리스트 완료 Message 송신 Client는 이 정보를 받은후 내부 자료구조의 Data를 MFC 인터페이스에 출력 작업 수행
	//	Packet		: Size | type | 
	//	Size		: DWORD + BYTE 0byte
	//-------------------------------------------------------------------------------------

	// 전송 다했으면 전송완료 메세지 전송
	ZeroMemory( pTempBuf, BUFSIZE );
	dwSize = sizeof(BYTE);
	type = NetLobbyServer::STOC_PLAYERLIST_SEND_OK;

	::CopyMemory( pTempBuf,	&type	,	sizeof(BYTE)	);

	Send( &pSocketInfo->m_SockAddr, pTempBuf, (int)dwSize );
}

//-------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트의 방 생성 요청을 수신하여 방 생성 처리-	
//	Packet		: Size | type | nPlayerNo(요청한 사용자 번호) | 방이름
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
	BYTE		nPlayerInRoomNo;		// 방에 접속한 고유번호
	BYTE		nPlayerInLobbyNo;		// 로비에 접속한 고유번호

	std::list< RoomInfo >::iterator iterRoom;
	std::list< PlayerInfo >::iterator iterPlayerInLobby;
	std::list< PlayerInfo >::iterator iterPlayerInRoom;

	::CopyMemory( &nPlayerInLobbyNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE) );	
	stTmpPlayer.m_nPlayerNo = nPlayerInLobbyNo;

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayersInRoom );
	EnterCriticalSection( &m_csListPlayersInLobby );

	// 요청한 Client가 로비에서 어떤 Client인지 찾는다.
	if( std::binary_search( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer ) )
	{
		iterPlayerInLobby = std::lower_bound( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer);	

		// 방안에 있는 유저의 번호 정보 설정
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

		// 방 고유 번호 부여
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

		// 방 접속자 리스트에 추가 & 정렬
		m_listPlayersInRoom.push_back( *iterPlayerInLobby );
		m_listPlayersInRoom.sort();

		// 로비 접속자 리스트에 삭제 
		m_listPlayersInLobby.erase( iterPlayerInLobby );
				
		::CopyMemory( &stTmpRoom.m_nMaxJoiners	, pBuf + sizeof(BYTE)*2	, sizeof(BYTE)						);
		::CopyMemory( stTmpRoom.m_wstrRoomName	, pBuf + sizeof(BYTE)*3	, sizeof(wchar_t) * ROOM_NAME_LEN	);		

		m_listRooms.push_back( stTmpRoom );
		m_listRooms.sort();

		stTmpPlayer.m_nPlayerNo	= nPlayerInRoomNo;
		iterPlayerInRoom = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer);
		iterPlayerInRoom->m_nPosAtRoom = 0;	// 접속한 방에서의 위치
		iterPlayerInRoom->m_nRoomNo = stTmpRoom.m_nRoomNo;

		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );
		iterRoom->m_pJoiner[ 0 ] = &(*iterPlayerInRoom);		
		iterRoom->m_nMapInfo	= 0;

	
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//
		//	Function	: 방 생성이 완료후 로비에 있는 각각의 Client에게 방 정보 Message 송신
		//	Packet		: | type | m_nRoomNo(방 번호) | m_bRoomState(방 상태) | m_nJoiners( 현재 참여인원 ) | m_nMaxJoiners( 전체 참여인원 ) | m_wstrRoomName( 방 이름 )
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
		//	Function	: 방 생성이 완료후 해당 클라이언트에게 자신의 방에서의 상태와 방화면에 구성에 필요한 정보 송신
		//	Packet		: Size | type | nPayerNo(방접속 유저 리스트 고유번호) 
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

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// 다이얼로그 정보 업데이트
//	LeaveCriticalSection( &m_csListRooms );
//	LeaveCriticalSection( &m_csListPlayersInRoom );
//	LeaveCriticalSection( &m_csListPlayersInLobby );

}

//-------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트의 방 입장 요청을 수신하여 입장 처리
//	Packet		: Size | type | nPlayerNo(플레이어번호), nRoomNo(방번호) 
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
	BYTE		nPlayerInRoomNo = 0;	// 방에 접속한 고유번호
	BYTE		nPlayerInLobbyNo = 0;	// 로비에 접속한 고유번호
	BYTE		nPosInRoom = 0;			// 로비에 접속한 고유번호
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
			//  대기 상태이므로 입장허용
			
			// 3. 해당 방에 들어가고, PlaerInRoom 리스트에 추가
			// 4. 해당 방에 있는 기존 유저들에게 새로운 유저 정보 송신
			// 5. 해당 방에 상태를 로비 유저들에게 전송
			// 1. 현재 로비에서 유저 목록에서 제거
			// 2. 이를 각 유저들에게 알린다.

			// 방안에 있는 유저의 번호 정보 설정
			nPlayerInRoomNo = 1;		
			for( iterPlayer = m_listPlayersInRoom.begin(); iterPlayer != m_listPlayersInRoom.end(); ++iterPlayer )
			{
				if( nPlayerInRoomNo != iterPlayer->m_nPlayerNo )
					break;
				else
					++nPlayerInRoomNo;
			}
			
			// 로비에서 해당 유저의 번호를 찾아서 리스트에서 제거후 로비에 가각의 Client에게 지워진 Client 넘버 송신
			if( std::binary_search( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(),stTmpPlayer ) )
			{
				iterPlayer = std::lower_bound( m_listPlayersInLobby.begin(), m_listPlayersInLobby.end(), stTmpPlayer );	
				
				// 로비에서의 번호 저장
				nPlayerInLobbyNo = iterPlayer->m_nPlayerNo;
			
				// 방 리스트에서의 번호로 교체
				iterPlayer->m_nPlayerNo = nPlayerInRoomNo;
				iterPlayer->m_nRoomNo	= stTmpRoom.m_nRoomNo;

				wcscpy_s( stTmpPlayer.m_wstrID, iterPlayer->m_wstrID );

				// 방에 있는 유저 리스트에 추가
				m_listPlayersInRoom.push_back( *iterPlayer );
				m_listPlayersInRoom.sort();
				stTmpPlayer.m_nPlayerNo = nPlayerInRoomNo;

				iterPlayerInRoom = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );

				// 로비에 있는 유저 리스트에서 제거
				m_listPlayersInLobby.erase( iterPlayer );
				
				// 접속자 증가
				if( ++iterRoom->m_nJoiners ==  iterRoom->m_nMaxJoiners )
					iterRoom->m_bRoomState = ROOM_FULL;
				
				// 방에서의 위치값 결정
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
				//	Function	: 로비에 있는 유저 각각에게 방안으로 들어간 유저에 대해서 리스트에서  제거 메세지 송신
				//				  그리고 변경된 방정보와 인원수를 갱신						
				//	Packet		: | type | nPlayerNo(플레이어번호) |
				//				: | type | m_nRoomNo(방번호) | m_nJoiners(현재참여인원 | 
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
				//	Function	: 방안에 있는 기존 멤버들에게 새로운 유저의 정보 송신
				//	Packet		: | type | nPosInRoom(방안에서의 위치) | newUserState(뉴비 상태) | newUserState(플레이어 상태), m_wstrID(ID) 
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
				//	Function	: 방안에 들어온 신규 유저에게 기존 유저들에 대한 정보 전송
				//	Packet		:	| type | nPlayerInRoomNo(방에 있는 고유번호) | stTmpRoom.m_nRoomNo(방번호) | nPosInRoom(방에서의 위치) |
				//					| m_nJoiners | nState(유저 상태) | m_wstrID(ID) | m_strConnRoomName( 방이름 ) |
				//	Size		: BYTE + BYTE + BYTE + BYTE + BYTE + wchar_t * USER_ID_LEN + wchar_t * ROOM_NAME_LEN = 114byte
				//----------------------------------------------------------------------------------------------------------------------
				::ZeroMemory( pTempBuf, BUFSIZE );
				DWORD dwPacketPalyerSize = sizeof(BYTE) + sizeof(wchar_t)*USER_ID_LEN;
				dwSize = sizeof(BYTE)*6 + sizeof(wchar_t) * ROOM_NAME_LEN;
				type = NetLobbyServer::STOC_ENTER_ROOM;			// 방에 있었던 기존 유저에 대한 정보 송신과 방입장 허용;
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

				// 신규유저 RoomInfo에 추가 갱신				
				iterRoom->m_pJoiner[ nPosInRoom ] = &(*iterPlayerInRoom);				

			}

			break;

		case ROOM_FULL:
			//-------------------------------------------------------------------------------------
			//
			//	Function	: 정원(6명)이 다 찬 상태
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
			//	Function	: 게임 시작 상태
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
//	Function	: 해당 클라이언트 방 나가기 요청을 수신하여 방 나가기 처리
//	Packet		: | type | m_PlayNo( 고유번호 ) | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) | nCaptain(방장여부) |
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

//	BYTE		nPlayerInRoomNo;		// 방에 접속한 고유번호
	BYTE		nPlayerInLobbyNo;		// 로비에 접속한 고유번호
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
		
		// 로비에서 사용될 보유 번호 구하기
		nPlayerInLobbyNo = 1;		
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			if( nPlayerInLobbyNo != iterPlayer->m_nPlayerNo )
				break;
			else
				++nPlayerInLobbyNo;
		}

//		stTmpPlayer.m_nPlayerNo = iterRoom->m_pJoiner[ nPosInRoom ]->m_nPlayerNo;

		// 룸에서 해당 유저의 번호를 찾아서 리스트에서 제거후 
		if( std::binary_search( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );	
						
			// 방 리스트에서의 번호로 교체
			iterPlayer->m_nPlayerNo = nPlayerInLobbyNo;
			iterPlayer->m_nRoomNo	= 0;

			//-------------------------------------------------------------------------------------
			//
			//	Function	: 신규 접속자 정보 각 로비에 있는 Client에게 전송
			//	Packet		: | type | nPlayerNo(플레이어번호) | m_wstrID( 플레이어 게임 아이디 )
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
					
				// 신규 접속자에 대한 정보를 기존 Lobby에 있는 Client들에게 전송
				for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
				{
					Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
				}
			}			
			
			// 접속자 감소
		//	nPosInRoom = iterPlayer->m_nPosAtRoom ;
			iterRoom->m_pJoiner[ nPosInRoom ] = NULL;
			for( int iJoiner = nPosInRoom + 1; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
			{
				iterRoom->m_pJoiner[ iJoiner - 1 ] = iterRoom->m_pJoiner[ iJoiner ];
			}			
			iterRoom->m_pJoiner[ --iterRoom->m_nJoiners ] = NULL;
			iterRoom->m_bRoomState = ROOM_STAND;		
			
			// 방에 마지막 인원이였다면
			if( 0 >= iterRoom->m_nJoiners )
			{

				//-------------------------------------------------------------------------------------
				//
				//	Function	: 로비에 있는 유저들에게 방 정보 제거 송신
				//	Packet		: | type | nRoomNo( 방번호 ) |
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
			// 방에 남아있는 유저들에게 나간 유저에 정보 제거 메시지 송신
			else
			{
			
				
				//-------------------------------------------------------------------------------------
				//
				//	Function	: 방에 남아있는 유저들에게 나간 유저에 정보 제거 메시지 송신
				//	Packet		: | type | nPosInRoom( 플레이어 방에서 위치값 ) | m_wstrID( 플레이어 ID )
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
				//	Function	: 로비에 있는 유저 각각에게 방안으로 들어간 유저에 대해서 리스트에서  제거 메세지 송신
				//				  그리고 변경된 방정보와 인원수를 갱신						
				//	Packet		: | type | nPlayerNo(플레이어번호) | m_wstrID( 플레이어 ID )
				//				: | type | m_nRoomNo(방번호) | m_nJoiners(현재참여인원 | m_bRoomState(방상태) |
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


				// 나간 유저가 방장이라면
				if( m_bCaptain )
				{
					//-------------------------------------------------------------------------------------
					//
					//	Function	: 방장이라면 상태 정보 갱신 메시지 송신
					//	Packet		: | type | iJoiner( 방에서 위치값 ) |
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

			// 방에 있는 유저 리스트에서 제거
			m_listPlayersInRoom.erase( iterPlayer );	

			// 방 나가기 승인
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

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// 다이얼로그 정보 업데이트

}


//-------------------------------------------------------------------------------------
//
//	Function	: 방장이 지정한 유저 1명 강퇴시키기 처리
//	Packet		: | type | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) 
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

//	BYTE		nPlayerInRoomNo;		// 방에 접속한 고유번호
	BYTE		nPlayerInLobbyNo;		// 로비에 접속한 고유번호
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
		
		// 로비에서 사용될 보유 번호 구하기
		nPlayerInLobbyNo = 1;		
		for( iterPlayer = m_listPlayersInLobby.begin(); iterPlayer != m_listPlayersInLobby.end(); ++iterPlayer )
		{
			if( nPlayerInLobbyNo != iterPlayer->m_nPlayerNo )
				break;
			else
				++nPlayerInLobbyNo;
		}

		stTmpPlayer.m_nPlayerNo = iterRoom->m_pJoiner[ nPosInRoom ]->m_nPlayerNo;

		// 룸에서 해당 유저의 번호를 찾아서 리스트에서 제거후 
		if( std::binary_search( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer ) )
		{
			iterPlayer = std::lower_bound( m_listPlayersInRoom.begin(), m_listPlayersInRoom.end(), stTmpPlayer );	
						
			// 방 리스트에서의 번호로 교체
			iterPlayer->m_nPlayerNo = nPlayerInLobbyNo;
			iterPlayer->m_nRoomNo	= 0;

			//-------------------------------------------------------------------------------------
			//
			//	Function	: 신규 접속자 정보 각 로비에 있는 Client에게 전송
			//	Packet		: | type | nPlayerNo(플레이어번호) | m_wstrID( 플레이어 게임 아이디 )
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
					
				// 신규 접속자에 대한 정보를 기존 Lobby에 있는 Client들에게 전송
				for( iterPlayerInLobby = m_listPlayersInLobby.begin(); iterPlayerInLobby != m_listPlayersInLobby.end(); ++iterPlayerInLobby )
				{
					Send( &iterPlayerInLobby->m_SockAddr, pTempBuf, dwSize );
				}
			}			
			
			// 접속자 감소
		//	nPosInRoom = iterPlayer->m_nPosAtRoom ;
			iterRoom->m_pJoiner[ nPosInRoom ] = NULL;
			for( int iJoiner = nPosInRoom + 1; iJoiner < iterRoom->m_nMaxJoiners; iJoiner++ )
			{
				iterRoom->m_pJoiner[ iJoiner - 1 ] = iterRoom->m_pJoiner[ iJoiner ];
			}			
			iterRoom->m_pJoiner[ --iterRoom->m_nJoiners ] = NULL;
			iterRoom->m_bRoomState = ROOM_STAND;		
			
			// 방에 마지막 인원이였다면
			if( 0 >= iterRoom->m_nJoiners )
			{

				//-------------------------------------------------------------------------------------
				//
				//	Function	: 로비에 있는 유저들에게 방 정보 제거 송신
				//	Packet		: | type | nRoomNo( 방번호 ) |
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
			// 방에 남아있는 유저들에게 나간 유저에 정보 제거 메시지 송신
			else
			{
			
				
				//-------------------------------------------------------------------------------------
				//
				//	Function	: 방에 남아있는 유저들에게 나간 유저에 정보 제거 메시지 송신
				//	Packet		: | type | nPosInRoom( 플레이어 방에서 위치값 ) | m_wstrID( 플레이어 ID )
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
				//	Function	: 로비에 있는 유저 각각에게 방안으로 들어간 유저에 대해서 리스트에서  제거 메세지 송신
				//				  그리고 변경된 방정보와 인원수를 갱신						
				//	Packet		: | type | nPlayerNo(플레이어번호) | m_wstrID( 플레이어 ID )
				//				: | type | m_nRoomNo(방번호) | m_nJoiners(현재참여인원 | m_bRoomState(방상태) |
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


					// 나간 유저가 방장이라면
				if( m_bCaptain )
				{
				//-------------------------------------------------------------------------------------
				//
				//	Function	: 방장이라면 상태 정보 갱신 메시지 송신
				//	Packet		: | type | iJoiner( 방에서 위치값 ) |
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

		
		
			// 방 나가기 승인
			::ZeroMemory( pTempBuf, BUFSIZE );
			type = NetLobbyServer::STOC_OUT_ROOM;
			dwSize = sizeof(BYTE)*2;

			::CopyMemory( pTempBuf					, &type					, sizeof(BYTE) );
			::CopyMemory( pTempBuf + sizeof(BYTE)	, &nPlayerInLobbyNo		, sizeof(BYTE) );			

			Send( &iterPlayer->m_SockAddr, pTempBuf, dwSize );

			m_listPlayersInLobby.push_back( *iterPlayer );
			m_listPlayersInLobby.sort();

				// 방에 있는 유저 리스트에서 제거
			m_listPlayersInRoom.erase( iterPlayer );	

		}
	}

	LeaveCriticalSection( &m_csListPlayersInLobby );
	LeaveCriticalSection( &m_csListPlayersInRoom );
	LeaveCriticalSection( &m_csListRooms );

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// 다이얼로그 정보 업데이트

}


//-------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트의 접속 종료요청을 수신 처리
//	Packet		: Size | type | nPlayerNo(플레이어번호) | nRoomNo(방번호) 
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

	// 로비에서 접속을 끊었을때
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

	// 방에서 접속을 끊었을때
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

				// 방에서 해당 Disconnect한 Client를 제거
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
					//	Function	: 방장이였고, 마지막 인원이 나갔을때 로비에서 각각의 Client에게 방제거 Event 송신.
					//	Packet		: | type | m_nRoomNo(방번호)
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
					//	Function	: 해당 클라이언트의 접속 종료요청을 수신 처리
					//	Packet		: | type | m_nPosAtRoom(방에서 위치) 
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
					//	Function	: 방에 남아있는 유저들에게 나간 유저에 정보 제거 메시지 송신
					//	Packet		: | type | nPosInRoom( 플레이어 방에서 위치값 ) | m_wstrID( 플레이어 ID )
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


					// 나간 유저가 방장이라면				
					if( bCaptain )
					{
						//-------------------------------------------------------------------------------------
						//
						//	Function	: 방장이라면 상태 정보 갱신 메시지 송신
						//	Packet		: | type | iJoiner( 방에서 위치값 ) |
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

	::SendMessage( m_hDlg, UM_UPDATEDATA, 0, 0 );		// 다이얼로그 정보 업데이트
}

//		해당 클라이언트 chat 정보를 Lobby에 있는 각각의 클라이언트들에게 전송
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


//		해당 클라이언트의 chat 정보를 현재 클라이언트의 방안에 접속한 각각의 클라이언트들에게 전송
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
//	Function	: 방안에 있는 유저의 게임 레디 요청 상태를 수신 처리
//	Packet		: | type | m_PlayNo( 고유번호 ) | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) | nPushType(준비완료 여부) |
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
		//	Function	: 방안에 있는 각각의 유저들에게 특정 유저의 레디 상태 정보를 송신
		//	Packet		: | type | nPosInRoom( 방에서의 위치 ) | nPushType( 레디 상태 ) |
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


//		방안에 있는 유저의 맵 정보를 갱신 처리
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
		//	Function	: 방장이 맵을 교체 요청
		//	Packet		: | type | nMapIndex( 맵번호 ) |
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
		//	Function	: 로비에 있는 각각의 유저에게 맵을 교체 정보 송신
		//	Packet		: | type | tmpRoom.m_nRoomNo(방 번호) | nMapIndex( 맵번호 ) |
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
//	Function	: 동물농장 Game Process를 생성하여 각각의 클라이언트 정보를 해당 클라이언트(방장)에게 전송
//	Packet		: | type | m_nPosInRoom( 방장 위치 ) | m_RoomNo( 방번호 ) | 
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
	
		//	로비에 있는 유저들에게 방 상태 정보 갱신 송신
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
		//	Function	: 방장에게 게임에 입장할 각각의 유저의 주소 정보를 송신
		//	Packet		: | type | m_nJoiners(인원 수) | SOCKADDR_IN * n명 |
		//	Size		:  BYTE + BYTE + SOCKADDR_IN * N명=  1byte
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
		//	Function	: 게임에 입장할 각각의 유저에게 방장 주소 정보 송신
		//	Packet		: | type | SOCKADDR_IN  |
		//	Size		:  BYTE + BYTE + SOCKADDR_IN * N명=  1byte
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
//	Function	: 로비에 있는 유저들에게 초대 메시지 송신 처리
//	Packet		: | type | m_RoomNo( 방번호 ) | 
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
//	Function	: 로비에서 Chat Message 송신
//	Packet		: | type | nSendMsg( 채팅 메시지 길이) | strSendMsg
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
//	Function	: 방장 위임 메시지 송신 요청
//	Packet		: | type | m_nPosInRoom( 현재 방장 ) | nPosInRoom( 새로운 방장 ) |
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
		//	Function	: 방장 위임 메시지 송신 요청
		//	Packet		: | type | nOldCaptain( 현재 방장 ) | nNewCaptain( 새로운 방장 ) |
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
//	Function	: 방에 있는 유저 1명의 정보 수신 처리
//	Packet		: | type | m_RoomNo( 방 번호 ) | nPosInRoom( 유저 위치 ) |
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

		// 쿼리 적용
		if( 0 != mysql_query( &m_dbMysql, query ) )
		{
			MessageBox( NULL, L"NetLobbyServer::JoinMember - mysql_query() Failed", L"Faield", MB_OK );        
			goto End;
		}

		// 결과를 얻는다.
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
			//	Function	: 유저 1명 정보 송신
			//	Packet		: | type | nPosInRoom( 방 번호 ) | nWin( 승 ) | nFail( 패 ) | nLevel( 레벨 ) | nExp( 경험치 ) |
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