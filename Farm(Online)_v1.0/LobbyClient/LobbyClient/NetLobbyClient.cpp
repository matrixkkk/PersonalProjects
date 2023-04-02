#include "stdafx.h"
#include "NetLobbyClient.h"
#include "MainFrm.h"
#include "PlayerList.h"

BYTE UserInRoomInfo::m_nJoiner = 0;

//------------ 초기화 ------------
NetLobbyClient::NetLobbyClient()
//-------------------------------- 
{
	m_PlayerNo		= 0;
	m_RoomNo		= 0;
	m_nCntRoom		= 0;
	m_nPosInRoom	= 0;
	m_strMyGameID	= _T("");
	
	m_bCaptain		= FALSE;	// 방장 여부
	m_bReady		= FALSE;	// 준비완료 여부

//	UserInRoomInfo::m_nJoiner = 0;
	
	::ZeroMemory( m_GamePlayer, sizeof(m_GamePlayer) );

	InitializeCriticalSection(&m_csListRooms);
	InitializeCriticalSection(&m_csListPlayers);
	InitializeCriticalSection(&m_csJoinerInRoom);

	m_listRooms.clear();
	m_listPlayers.clear();
	m_listChat.clear();

	//	MapManager 초기화
//	m_MapManager.Init();
	
}

//------------ Release ------------
NetLobbyClient::~NetLobbyClient()
//---------------------------------
{
	End();
}


//	접속한 소켓정보를 이용하여 자료구조에 할당
BOOL	NetLobbyClient::Begin()
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

/*
	char ip[20];
	FILE *fp = fopen( "ServerIP.txt", "r+t" );

	if( NULL == fp )
	{
		MessageBox( NULL, L"NetLobbyServer::Begin() - fopen()", L"Failed", MB_OK );
		return FALSE;
	}
	// ip 읽어오기

	fread( ip, sizeof(char), 20, fp );
	fclose( fp );
	
	m_SockAddr.sin_family		= AF_INET;
	m_SockAddr.sin_port			= htons(9000);
	m_SockAddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
*/


//	if( CreateWorkerThread() )
		m_bRunWorkerThread = TRUE;

	CWnd *pWnd = AfxGetMainWnd();
	m_hWnd = pWnd->m_hWnd;

	return TRUE;
}
	
//	접속 종료한 소켓정보를 자료구조로부터 제거
BOOL	NetLobbyClient::End()
{
	if( !Release() )
		return FALSE;

	DeleteCriticalSection(&m_csListRooms);
	DeleteCriticalSection(&m_csListPlayers);
	DeleteCriticalSection(&m_csJoinerInRoom);
	
	m_bRunWorkerThread = FALSE;
//	DestroyWorKerThread();

	
	if( !CloseSocket() )
		return FALSE;

	return TRUE;
}

BOOL	NetLobbyClient::Release()
{
	if( m_PlayerNo != 0 )
	{
		char pTempBuf[BUFSIZE];
		DWORD dwSize = sizeof(BYTE)*5 ;
		BYTE type = NetLobbyClient::CTOS_DISCONNECT;
		BYTE bCaptain = m_bCaptain ? 1 : 0 ;

		::CopyMemory( pTempBuf					, &type			, sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)	, &m_RoomNo		, sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)*2 , &m_PlayerNo	, sizeof(BYTE) );	
		::CopyMemory( pTempBuf + sizeof(BYTE)*3 , &bCaptain		, sizeof(BYTE) );
		::CopyMemory( pTempBuf + sizeof(BYTE)*4 , &m_nPosInRoom	, sizeof(BYTE) );
		

		Send( pTempBuf, dwSize );
	}	

	if( !m_listRooms.empty() )
	{
		m_listRooms.clear();
	}

	if( !m_listPlayers.empty() )
	{	
		m_listPlayers.clear();
	}

	::ZeroMemory( m_GamePlayer, sizeof(m_GamePlayer) );
	
	m_PlayerNo		= 0;
	m_RoomNo		= 0;
	m_nCntRoom		= 0;
	m_nPosInRoom	= 0;
	m_strMyGameID	= _T("");
	m_bCaptain		= FALSE;
	m_bReady		= FALSE;

	UserInRoomInfo::m_nJoiner = 0;

	return TRUE;
}


//--------- Packet Analysis & Handling -----------

//	Recv 되어온 패킷을 분석 그에 맞게 처리하는 함수
void	NetLobbyClient::ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen )
{
	BYTE type = 0;
	::CopyMemory( &type, pBuf, sizeof( BYTE ) );
//	CMainFrame *pFrame = NULL;

	switch( type )
	{

	case STOC_ADD_ROOMLIST:
		// 방 리스트 정보 송신
		AddRoomList( pBuf, nLen );
		break;

	case STOC_ADD_PlAYERLIST:
		// 접속자 리스트 정보 송신
		AddPlayerList( pBuf, nLen );
		break;

	case STOC_LOGIN_OK:
		// 서버 접속 허용
		if( LoginOK( pBuf, nLen ) )
		{		
			RequestRoomList();
			RequestPlayerList();
		}

		break;

	case STOC_ENTER_ROOM:
		// 방 입장 허가
		JoinRoom( pBuf, nLen );
		break;

	case STOC_OUT_ROOM:
		// 방 나가기 허용
		if( OutRoom( pBuf, nLen ) )
		{
			RequestRoomList();
			RequestPlayerList();
		}
		break;
	
	case STOC_ADD_ROOM:
		// 로비 방 추가 송신
		AddRoom( pBuf, nLen );
		break;

	case STOC_ADD_PLAYER:
		// 접속자 추가 송신
		AddPlayer( pBuf, nLen );
		break;

	case STOC_REMOVE_ROOM:
		// 방 1개 제거 송신
		DeleteRoom( pBuf, nLen );
		break;

	case STOC_REMOVE_PLAYER_IN_LOBBY:
		// 접속자 1명 제거 송신		
		DeletePlayer( pBuf, nLen );
		break;

	case STOC_REMOVE_PLAYER_IN_ROOM:
		// 방안에서 나간 접속자 1명 리스트에서 제거
		DeleteJoinerAtRoom( pBuf, nLen );
		break;

	case STOC_ROOMLIST_SEND_OK:
		// 초기 접속하여 룸리스트 갱신
		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_ADD_MORE, 0 );
		break;

	case STOC_PLAYERLIST_SEND_OK:
		// 초기 접속하여 유저리스트 갱신	
		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_ADD_MORE, 0 );
		break;

	case STOC_CHAT_IN_LOBBY:
		// 로비에서 채팅 화면 갱신
		AddChatMessageAtLobby( pBuf, nLen );		
		break;

	case STOC_CHAT_IN_ROOM:
		// 룸에서 채팅 화면 갱신
		AddChatMessageAtRoom( pBuf, nLen );		
		break;

	case STOC_MANAGER_MESSAGE:
		// 운영자의 공지 메시지 수신 화면 갱신
		if( !m_RoomNo )
			AddChatMessageAtLobby( pBuf, nLen );
		else
			AddChatMessageAtRoom( pBuf, nLen );

		break;

	case STOC_ROOM_CREATE_OK:
		// 방을 생성하여 방장으로 접속
		 CreateRoom( pBuf, nLen );
		 break;

	case STOC_JOINROOM_PLAYERINFO:
		// 방에 입장하고 나서 방 기존 유저 정보 수신
		AddJoinerInRoom( pBuf, nLen );
		break;

	case STOC_UPDATE_ROOMSTATE_IN_LOBBY:
		// 방 상태 정보 갱싱
		UpdateRoomInfo( pBuf, nLen );
		break;

	case STOC_JOINER_READY_STATE:
		// 방에 접속한 유저의 레디 상태 정보 갱신 송신
		UpdateJoinerState( pBuf, nLen );
		break;

	case STOC_GAME_MAP_CHANGE:
		// 맵 정보 변경 갱신
		MapInfoChange( pBuf, nLen );
		break;

	case STOC_GAME_MAP_CHANGE_IN_LOBBY:
		// 방에서 맵 교체정보 로비 대기자들에게 갱신 송신
		MapInfoChangeAtLobby( pBuf, nLen ); 
		break;

	case STOC_GAME_CAPTAIN_CHANGE:
		// 방장 변경 메시지 수신 갱신
		ChangeCaptain( pBuf, nLen );
		break;

	case STOC_GAME_JOINER_ADDR_LIST:
		// 방에 있장한 일반 유저 주소 정보를 방장에게 송신		
		GameStartCaptain( pBuf, nLen );				
		break;

	case STOC_GAME_CAPTAIN_ADDR:
		// 방에 입장하는 각각의 유저에게 방정 주소 정보 송신
		GameStartJoiner( pBuf, nLen );
		break;

	case STOC_GAME_INVITATION:
		// 방에서 초대 메시지 정보 수신
		Invitation( pBuf, nLen );
		break;

	case STOC_GAME_INVITATION_OK:
		// 방장에서 초대 메시지 결과 수신
		InvitationResult( pBuf, nLen );
		break;

	case STOC_GAME_GIVE_CAPTAIN:
		//		방장 변경 메시지 수신 갱신
		GiveCaptain( pBuf, nLen );
		break;

	case STOC_GAME_JOINER_INFO:
		//		방안 유저 정보 수신 출력
		ShowJoinerInfo( pBuf, nLen );
		break;

	case STOC_NOT_ENTER_FOOL:		// 방 정원이 차서 방 접속을 못하는 경우				
	case STOC_NOT_ENTER_GAMMING:	// 방 정원이 차서 방 접속을 못하는 경우					
	case STOC_JOIN_MEMBERSHIP_OK:	// 회원 가입 성공
	case STOC_JOIN_MEMBERSHIP_FAIL:	// 회원 가입 실패

		ShowInfoMessage( type );
		break;	
	}
}
	
//	접속한 소켓정보를 이용하여 자료구조에 할당
void	NetLobbyClient::ProcessConnect( SocketInfo *pSocketInfo )
{


}
	
//	접속 종료한 소켓정보를 자료구조로부터 제거
void	NetLobbyClient::ProcessDisConnect( SocketInfo *pSocketInfo )
{


}




//-------------------------------------------------------------------------
//
//								Request Functions
//
//	Desc : Client가 Server 에게 요청 메시지 전송하는 함수 모음
//-------------------------------------------------------------------------

//		로비서버 회원가입 요청
BOOL	NetLobbyClient::RequestJoinMemeber( CString &ID, CString &PW, CString &GameID )
{
	char pTempBuf[ BUFSIZE ];
	char szPW[ USER_ID_LEN ];
	char szID[ USER_ID_LEN ];
	char szGameID[ USER_ID_LEN ];

	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	::ZeroMemory( szPW, sizeof(szPW) );
	::ZeroMemory( szID, sizeof(szID) );
	::ZeroMemory( szGameID, sizeof(szGameID) );

	BYTE type		= NetLobbyClient::CTOS_JOIN_MEMBERSHIP;
	DWORD dwSize	= sizeof(BYTE) + sizeof(char)* USER_ID_LEN * 3;
	
	WideCharToMultiByte(CP_ACP, 0, ID.GetBuffer()		, -1, szID		, sizeof(szID)		, NULL, NULL); 
	WideCharToMultiByte(CP_ACP, 0, PW.GetBuffer()		, -1, szPW		, sizeof(szPW)		, NULL, NULL); 	
	WideCharToMultiByte(CP_ACP, 0, GameID.GetBuffer()	, -1, szGameID	, sizeof(szGameID)	, NULL, NULL); 

	::CopyMemory( pTempBuf 												, &type		, sizeof(BYTE)				);
	::CopyMemory( pTempBuf + sizeof(BYTE)								, szID		, sizeof(char) * USER_ID_LEN);
	::CopyMemory( pTempBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN	, szPW		, sizeof(char) * USER_ID_LEN);
	::CopyMemory( pTempBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN*2, szGameID	, sizeof(char) * USER_ID_LEN);

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 아이디와 비번을 통해 로그인 접속요청 메시지 송신
//	Packet		: | type | szID( 아아디 ) | szPW( 비밀번호 ) 
//	Size		: sizeof(char)* USER_ID_LEN(25) + sizeof(char)* USER_ID_LEN(25) = 50Byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestLogin( CString &IP, CString &ID, CString &PW )
{	
	char pTempBuf[ BUFSIZE ];
	char szPW[ USER_ID_LEN ];
	char szID[ USER_ID_LEN ];

	::ZeroMemory( pTempBuf, BUFSIZE );
	::ZeroMemory( szID, sizeof(szID) );
	::ZeroMemory( szPW, sizeof(szPW) );

	BYTE	type	= NetLobbyClient::CTOS_LOGIN;
	DWORD	dwSize	= sizeof(BYTE) + sizeof(char) * USER_ID_LEN * 2;
		
	WideCharToMultiByte(CP_ACP, 0, ID.GetBuffer()		, -1, szID		, sizeof(szID)		, NULL, NULL); 
	WideCharToMultiByte(CP_ACP, 0, PW.GetBuffer()		, -1, szPW		, sizeof(szPW)		, NULL, NULL); 	

	//------------------------------------------------------------------------------------------
	::CopyMemory( pTempBuf,													&type	,	sizeof(int)					);
	::CopyMemory( pTempBuf + sizeof(BYTE),									szID	,	sizeof(char) * USER_ID_LEN	);
	::CopyMemory( pTempBuf + sizeof(BYTE) + sizeof(char) * USER_ID_LEN,		szPW	,	sizeof(char) * USER_ID_LEN	);

/*
	char *szIP = (LPSTR)(LPCSTR)IP.GetBuffer();
	m_SockAddr.sin_family		= AF_INET;
	m_SockAddr.sin_port			= htons(9000);
	m_SockAddr.sin_addr.s_addr	= inet_addr( szIP );
*/
	Send( pTempBuf, dwSize );

	return TRUE;
}

//		로비서버의 개설된 방 리스트 요청
BOOL	NetLobbyClient::RequestRoomList()
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type	= NetLobbyClient::CTOS_ROOM_LIST;
	//DWORD	dwSize	= sizeof(DWORD) + sizeof(BYTE);
	DWORD	dwSize	= sizeof(BYTE);

	::CopyMemory( pTempBuf,	 &type,	sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//		로비서버의 접속한 클라이언트 리스트 요청
BOOL	NetLobbyClient::RequestPlayerList()
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type	= NetLobbyClient::CTOS_PLAYER_LIST;
	DWORD	dwSize	= sizeof(BYTE);

	::CopyMemory( pTempBuf,	 &type,	sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 선택한 방 번호를 이용하여 방 입장 요청
//	Packet		: Size | type | nPlayerNo(유저번호) | nRoomNo(방번호) 
//	Size		: DWORD + BYTE + BYTE 1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestJoinRoom( BYTE nRoomNo )
{

	std::list< RoomInfo >::iterator iterRoom;
	EnterCriticalSection(&m_csListRooms);
	
	for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
		if( iterRoom->m_nListNo == nRoomNo )
			break;

	if( iterRoom == m_listRooms.end() )
		return FALSE;

	LeaveCriticalSection(&m_csListRooms);


	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type	= NetLobbyClient::CTOS_ENTER_ROOM;
	DWORD	dwSize	= sizeof(BYTE) * 3;

	::CopyMemory( pTempBuf					,	&type					,	sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_PlayerNo				,	sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&iterRoom->m_nRoomNo	,	sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 방 만들기 요청
//	Packet		: | type | m_PlayerNo( 고유번호 ) | nMaxPlayer( 방 인원 ) | strRoomName |
//	Size		:  BYTE + BYTE + BYTE + wchar_t * ROOM_NAME_LEN = 62byte;
//-------------------------------------------------------------------------------------
BOOL NetLobbyClient::RequestCreateRoom( CString &strRoomName, BYTE nMaxPlayer )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type	= NetLobbyClient::CTOS_CREATE_ROOM;
	DWORD	dwSize	= sizeof(BYTE)*3 + sizeof(wchar_t)*ROOM_NAME_LEN;

	::CopyMemory( pTempBuf					,	&type					,		sizeof(BYTE)					);
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_PlayerNo				,		sizeof(BYTE)					);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nMaxPlayer				,		sizeof(BYTE)					);
	::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	strRoomName.GetBuffer()	,		sizeof(wchar_t)*ROOM_NAME_LEN	);

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_DELETE_ALL		, 0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_DELETE_ALL	, 0 );

	Send( pTempBuf, dwSize );

	return TRUE;

}


//-------------------------------------------------------------------------------------
//
//	Function	: 로비에서 Chat Message 송신
//	Packet		: | type | nSendMsg( 채팅 메시지 길이) | strSendMsg
//	Size		:   BYTE + BYTE + sizeof(wchar_t) * nLen = ?byte
//-------------------------------------------------------------------------------------

BOOL	NetLobbyClient::RequestChatInLobby( CString &strSendMsg )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	CString strSendMessage;	
	strSendMessage = m_strMyGameID + strSendMsg;

	BYTE	nMsgLen = static_cast< BYTE >( strSendMessage.GetLength() );
	BYTE	type	= NetLobbyClient::CTOS_CHAT_IN_LOBBY;	
	DWORD	dwSize	= sizeof(BYTE)*2 + sizeof(wchar_t)*nMsgLen;

	::CopyMemory( pTempBuf,						&type						,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE),		&nMsgLen					,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2,	strSendMessage.GetBuffer()	,	sizeof(wchar_t)*nMsgLen	);

	Send( pTempBuf, dwSize );

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 룸에서 Chat Message 송신
//	Packet		: | type | m_RoomNo( 접속한 방번호) | nSendMsg( 채팅 메시지 길이) | strSendMsg(메시지)
//	Size		: BYTE + BYTE + BYTE + sizeof(wchar_t) * nLen = ?byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestChatInRoom( CString &strSendMsg )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	CString strSendMessage;	
	strSendMessage = m_strMyGameID + strSendMsg;

	BYTE	nMsgLen = static_cast< BYTE >( strSendMessage.GetLength() );
	BYTE	type	= NetLobbyClient::CTOS_CHAT_IN_ROOM;	
	DWORD	dwSize	= sizeof(BYTE)*3 + sizeof(wchar_t)*nMsgLen;

	::CopyMemory( pTempBuf					,	&type						,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_RoomNo					,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nMsgLen					,	sizeof(BYTE)			);
	::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	strSendMessage.GetBuffer()	,	sizeof(wchar_t)*nMsgLen	);

	Send( pTempBuf, dwSize );

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 방안에서 나가기 요청
//	Packet		: | type | m_PlayNo( 고유번호 ) | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) | nCaptain(방장여부) |
//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE =  4byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestOutRoom()
{

	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type		= NetLobbyClient::CTOS_OUT_ROOM;
	BYTE	nCaptain	= m_bCaptain ? 1 : 0;
	DWORD	dwSize		= sizeof(BYTE)*5;	

	::CopyMemory( pTempBuf					,	&type			,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_PlayerNo		,		sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&m_RoomNo		,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	&m_nPosInRoom	,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*4	,	&nCaptain		,		sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장이 방안 유저 1명을 강퇴시킴 요청
//	Packet		: | type | m_PlayNo( 고유번호 ) | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) | nCaptain(방장여부) |
//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE =  4byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequesCompulsiontOutRoom( BYTE nPosInRoom )
{

	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type		= NetLobbyClient::CTOS_GAME_FORCE_OUT_ROOM;
	DWORD	dwSize		= sizeof(BYTE)*3;	

	::CopyMemory( pTempBuf					,	&type			,		sizeof(BYTE) );		
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_RoomNo		,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nPosInRoom		,		sizeof(BYTE) );	

	Send( pTempBuf, dwSize );

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 게임 준비 요청
//	Packet		: | type | m_PlayNo( 고유번호 ) | m_RoomNo( 방번호 ) | m_nPosInRoom(방에서의 위치) | nPushType(준비완료 여부) |
//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE =  4byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestReadyGame()
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type		= NetLobbyClient::CTOS_GAME_READY;		
	BYTE	nPushType	= m_bReady ? 1 : 0;
	DWORD	dwSize		= sizeof(BYTE)*5;	

	::CopyMemory( pTempBuf					,	&type			,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_PlayerNo		,		sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&m_RoomNo		,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*3	,	&m_nPosInRoom	,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*4	,	&nPushType		,		sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	EnterCriticalSection( &m_csJoinerInRoom );

	m_GamePlayer[ m_nPosInRoom ].m_MyState = nPushType ? STATE_READY : STATE_DEFAULT;

	LeaveCriticalSection( &m_csJoinerInRoom );

	::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_PLAYER_UPDATE_ONE,	m_nPosInRoom );

	return TRUE;

}


//-------------------------------------------------------------------------------------
//
//	Function	: 방에서 게임 시작 요청
//	Packet		: | type | m_nPosInRoom( 방장 위치 ) | m_RoomNo( 방번호 ) | 
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestStartGame()
{

	// 모두 레디 준비중인지 체크한다.
	BYTE nStateType	 = 0;
	nStateType		|=	STATE_READY;
	nStateType		|=	STATE_CAPTAIN;

	for( int iPlayer = 0; iPlayer < UserInRoomInfo::m_nJoiner; iPlayer++ )
	{
		if( !(m_GamePlayer[ iPlayer ].m_MyState & nStateType) )
			return FALSE;
	}

	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );

	BYTE	type	= NetLobbyClient::CTOS_GAME_START;
	DWORD	dwSize	= sizeof(BYTE)*3;

	::CopyMemory( pTempBuf					,	&type			,		sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_nPosInRoom	,		sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&m_RoomNo		,		sizeof(BYTE) );	

	Send( pTempBuf, dwSize );

	return TRUE;

}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장이 맵을 교체 요청
//	Packet		: | type | m_RoomNo( 방번호 ) | nMapIndex( 맵번호 ) |
//	Size		:  BYTE + BYTE + BYTE=  2byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestMapChange( BYTE nMapIndex )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	
	BYTE	type		= NetLobbyClient::CTOS_GAME_MAP_CHANGE;			
	DWORD	dwSize		= sizeof(BYTE)*3;	

	::CopyMemory( pTempBuf					,	&type		,		sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	,	&m_RoomNo	,		sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	,	&nMapIndex	,		sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장이 초대 메시지 송신 요청
//	Packet		: | type | m_RoomNo( 방번호 ) |
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestInvitation()
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	
	BYTE	type		= NetLobbyClient::CTOS_GAME_INVITATION;			
	DWORD	dwSize		= sizeof(BYTE)*2;	

	::CopyMemory( pTempBuf					, &type		, sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	, &m_RoomNo	, sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장 위임 메시지 송신 요청
//	Packet		: | type | m_RoomNo( 방 번호 ) | nPosInRoom( 새로운 방장 ) |
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestGiveCaptain( BYTE nPosInRoom )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	
	BYTE	type		= NetLobbyClient::CTOS_GAME_GIVE_CAPTAIN;			
	DWORD	dwSize		= sizeof(BYTE)*3;	

	::CopyMemory( pTempBuf					, &type				, sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	, &m_RoomNo			, sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &nPosInRoom		, sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방안 유저 1명 정보 송신 요청
//	Packet		: | type | m_RoomNo( 방 번호 ) | nPosInRoom( 유저 위치 ) |
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::RequestJoinerInfo( BYTE nPosInRoom )
{
	char pTempBuf[BUFSIZE];
	::ZeroMemory( pTempBuf, BUFSIZE );
	
	BYTE	type		= NetLobbyClient::CTOS_GAME_JOINER_INFO;			
	DWORD	dwSize		= sizeof(BYTE)*3;	

	::CopyMemory( pTempBuf					, &type				, sizeof(BYTE) );
	::CopyMemory( pTempBuf + sizeof(BYTE)	, &m_RoomNo			, sizeof(BYTE) );	
	::CopyMemory( pTempBuf + sizeof(BYTE)*2	, &nPosInRoom		, sizeof(BYTE) );

	Send( pTempBuf, dwSize );

	return TRUE;
}

//-------------------------------------------------------------------------
//
//							MFC UI Functions
//
//	Desc : MFC 각종 UI 상태 갱신 및 처리 관련 함수 모음
//-------------------------------------------------------------------------

BOOL NetLobbyClient::ChangeDisplay( UINT nID )
{
	CMainFrame *pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if( NULL != pFrame )
	{
		pFrame->ChangeDisplay( nID );	
		return TRUE;
	}
		
	return FALSE;
}


//		유저 리스트를 이용하여 ListView에 각 튜플값 갱신
BOOL	NetLobbyClient::UpdatePlayerListInLobby( PlayerList *pPlayerList )
{	

	if( NULL == pPlayerList )
		return FALSE;


	std::list< PlayerInfo >::iterator iterPlayer;
	LV_ITEM lvitem;
			
	EnterCriticalSection( &m_csListPlayers );	

	for( iterPlayer = m_listPlayers.begin(); iterPlayer != m_listPlayers.end(); ++iterPlayer )
	{
		lvitem.mask = LVIF_TEXT;					//데이터 스타일 마스크
		lvitem.iItem = iterPlayer->m_nPlayerNo;		//아이템 번호
		lvitem.iSubItem = 0;						//컬럼 번호
		lvitem.pszText = iterPlayer->m_wstrID;		//아이템문자열
		pPlayerList->GetListCtrl().InsertItem( &lvitem );
	}		

	LeaveCriticalSection( &m_csListPlayers );

	return TRUE;
}

//		접속한 새로운 유저 1명을 추가한다.
BOOL	NetLobbyClient::UpdateOnePlayerListInLobby( BYTE nPlayerNo, PlayerList *pPlayerList )
{	
	if( NULL == pPlayerList )
		return FALSE;

	std::list< PlayerInfo >::iterator iterPlayer;
	PlayerInfo tmpPlayer;
	tmpPlayer.m_nPlayerNo = nPlayerNo;

	LV_ITEM lvitem;
			
	EnterCriticalSection( &m_csListPlayers );

	if( std::binary_search( m_listPlayers.begin(), m_listPlayers.end(), tmpPlayer ) )
	{
		iterPlayer = std::lower_bound( m_listPlayers.begin(), m_listPlayers.end(), tmpPlayer );

		lvitem.mask = LVIF_TEXT;									//데이터 스타일 마스크
		lvitem.iItem = pPlayerList->GetListCtrl().GetItemCount();	//아이템 번호
		lvitem.iSubItem = 0;										//컬럼 번호
		lvitem.pszText = iterPlayer->m_wstrID;						//아이템문자열
		pPlayerList->GetListCtrl().InsertItem( &lvitem );
	}

	LeaveCriticalSection( &m_csListPlayers );

	return TRUE;
}

//		룸 리스트를 이용하여 ListView에 각 튜플값 갱신
BOOL	NetLobbyClient::UpdateRoomListInLobby( RoomList *pRoomList )
{
	if( NULL == pRoomList )
		return FALSE;

	std::list< RoomInfo >::iterator iterRoom;
	LV_ITEM lvitem;
	int count = 0;
	CString strNum, strGameState, strJoiner;
			
	EnterCriticalSection( &m_csListRooms );	

	for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
	{
//		strNum.Format( L"%d", iterRoom->m_nRoomNo	);
		strNum.Format( L"%d", ++m_nCntRoom );

		switch( iterRoom->m_nRoomState )
		{
		case ROOM_STAND:
			//  대기 상태
			strGameState = _T("대기중");
			break;

		case ROOM_FULL:
			//  정원(6명)이 다 찬 상태
			strGameState = _T("꽉참");
			break;

		case ROOM_RUNNING_GAME:			
			//	게임 시작 상태
			strGameState = _T("기도중");	
			break;
		}

		strJoiner.Format( L"%d명 / %d명", iterRoom->m_nMaxJoiners, iterRoom->m_nJoiners );

		lvitem.mask		= LVIF_TEXT;				//데이터 스타일 마스크
		lvitem.iItem	= count;					//아이템 번호
		lvitem.iSubItem = 0;						//컬럼 번호
		lvitem.pszText	= strNum.GetBuffer();		//아이템문자열		

		CListCtrl &roomList = pRoomList->GetListCtrl();
		
		// | 방번호 | 방이름 | 방상태 | 참여인원 |
		roomList.InsertItem( &lvitem );		
		roomList.SetItemText( count, 1, iterRoom->m_wstrRoomName	);
		roomList.SetItemText( count, 2, strGameState				);
		roomList.SetItemText( count, 3, strJoiner					);

		++count;
	}

	LeaveCriticalSection( &m_csListRooms );

	return TRUE;
}

//		방 안에 있는 모든 유저의 리스트 정보를 갱신
void	NetLobbyClient::UpdateAllJoinerList( JoinerList *pJoinerList )
{
	if( NULL == pJoinerList )
		return;

	if( 0 >= UserInRoomInfo::m_nJoiner )
		return;

	CString strNum, strState;

	CListCtrl &joinerList = pJoinerList->GetListCtrl();

	EnterCriticalSection(&m_csJoinerInRoom);

	for( BYTE iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{		
		switch( m_GamePlayer[ iJoiner ].m_MyState )
		{
		case STATE_DEFAULT:
			// 기본 상태
			strState = _T("신도");
			break;

		case STATE_READY:
			// 레디 상태
			strState = _T("기도준비");
			break;

		case STATE_GAMMING:
			// 게임중 상태
			strState = _T("기도중");
			break;

		case STATE_CAPTAIN:
			// 방장
			strState = _T("신");
			break;
		}

		joinerList.SetItemText( iJoiner , 2, strState );	
	}

	LeaveCriticalSection(&m_csJoinerInRoom);
}

//		방 리스트에 방 1개 추가 갱신
BOOL	NetLobbyClient::UpdateOneRoomListInLobby( BYTE nRoomNo, RoomList *pRoomList )
{

	if( NULL == pRoomList )
		return FALSE;

	std::list< RoomInfo >::iterator iterRoom;
	LV_ITEM lvitem;
	int count = 0;
	CString strNum, strGameState, strJoiner;

	RoomInfo tmpRoom;
	tmpRoom.m_nRoomNo = nRoomNo;
			
	EnterCriticalSection( &m_csListRooms );	

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );

//		strNum.Format( L"%d", iterRoom->m_nRoomNo	);
		strNum.Format( L"%d", ++m_nCntRoom	);		

		switch( iterRoom->m_nRoomState )
		{
		case ROOM_STAND:
			//  대기 상태
			strGameState = _T("대기중");
			break;

		case ROOM_FULL:
			//  정원(6명)이 다 찬 상태
			strGameState = _T("꽉참");
			break;

		case ROOM_RUNNING_GAME:			
			//	게임 시작 상태
			strGameState = _T("기도중");	
			break;
		}

		strJoiner.Format( L"%d명 / %d명", iterRoom->m_nMaxJoiners, iterRoom->m_nJoiners );

		int nCntRooms = static_cast<int>( m_listRooms.size() - 1 );

		CListCtrl &roomList = pRoomList->GetListCtrl();		

		lvitem.mask		= LVIF_TEXT;				//데이터 스타일 마스크
		lvitem.iItem	= roomList.GetItemCount(); 	//아이템 번호
		lvitem.iSubItem = 0;						//컬럼 번호
		lvitem.pszText	= strNum.GetBuffer();		//아이템문자열				
		
		// | 방번호 | 방이름 | 방상태 | 참여인원 |
		roomList.InsertItem( &lvitem );		
		roomList.SetItemText( nCntRooms, 1, iterRoom->m_wstrRoomName	);
		roomList.SetItemText( nCntRooms, 2, strGameState				);
		roomList.SetItemText( nCntRooms, 3, strJoiner					);
	}		

	LeaveCriticalSection( &m_csListRooms );

	return TRUE;
}

//		방에 나간 1명을 리스트에서 제거 갱신
BOOL	NetLobbyClient::UpdateDeleteJoinerInRoom( BYTE nPosInRoom, JoinerList *pJoinerList )
{	

	CString strNum;
	CListCtrl &JoinerList = pJoinerList->GetListCtrl();
	JoinerList.DeleteItem( nPosInRoom );

	for( int iList = nPosInRoom; iList < JoinerList.GetItemCount(); iList++ )
	{
		strNum.Format(L"%d", iList + 1 );
		JoinerList.SetItemText( iList, 0, strNum );
	}	
	
	return TRUE;
}

//		방 1개에 대한 상태 정보를 갱신
void	NetLobbyClient::UpdateRoomStateInLobby		( const RoomInfo *pRoomInfo, RoomList *pRoomList )
{
	if( NULL == pRoomInfo || NULL == pRoomList )
		return;

	CString strJoiner, strGameState;
	CListCtrl &roomList = pRoomList->GetListCtrl();

	switch( pRoomInfo->m_nRoomState )
	{
		case ROOM_STAND:
			//  대기 상태
			strGameState = _T("대기중");
			break;

		case ROOM_FULL:
			//  정원(6명)이 다 찬 상태
			strGameState = _T("꽉참");
			break;

		case ROOM_RUNNING_GAME:			
			//	게임 시작 상태
			strGameState = _T("기도중");	
			break;
	}

	strJoiner.Format( L"%d명 / %d명", pRoomInfo->m_nMaxJoiners, pRoomInfo->m_nJoiners );	

	// | 방번호 | 방이름 | 방상태 | 참여인원 |
	roomList.SetItemText( pRoomInfo->m_nListNo, 2, strGameState	 );
	roomList.SetItemText( pRoomInfo->m_nListNo, 3, strJoiner	 );	
}


		
//-------------------------------------------------------------------------------------
//
//	Function	: 채팅 메시지 CEdit Controller에 추가
//-------------------------------------------------------------------------------------
void	NetLobbyClient::UpdateChat( CEdit &pEdit )
{

	std::list< CString >::iterator iterMsg;

	for( iterMsg = m_listChat.begin(); iterMsg != m_listChat.end(); ++iterMsg )
	{
		int dwLen = pEdit.GetWindowTextLength();
		pEdit.SetSel(dwLen, dwLen);
		pEdit.ReplaceSel( *iterMsg );
	}

	m_listChat.clear();	
}

//		방안에 접속한 1명을 대해서 추가 갱신
BOOL	NetLobbyClient::UpdateOneJoinerListInRoom( BYTE nJoinerNo, JoinerList *pJoinerList)
{

	if( NULL == pJoinerList )
		return FALSE;
			
	EnterCriticalSection(&m_csJoinerInRoom);

	CString strNum, strState;
	strNum.Format( L"%d", nJoinerNo + 1 );

	switch( m_GamePlayer[ nJoinerNo ].m_MyState )
	{
	case STATE_DEFAULT:
		// 기본 상태
		strState = _T("신도");
		break;

	case STATE_READY:
		// 레디 상태
		strState = _T("기도준비");
		break;

	case STATE_GAMMING:
		// 게임중 상태
		strState = _T("기도중");
		break;

	case STATE_CAPTAIN:
		// 방장
		strState = _T("신");
		break;
	}	

	LV_ITEM lvitem;
	lvitem.mask = LVIF_TEXT;					//데이터 스타일 마스크
	lvitem.iItem = nJoinerNo;					//아이템 번호
	lvitem.iSubItem = 0;						//컬럼 번호
	lvitem.pszText = strNum.GetBuffer();		//아이템문자열

	CListCtrl &joinerList = pJoinerList->GetListCtrl();

	joinerList.InsertItem( &lvitem );
//	joinerList.SetItemText( nJoinNumber +1, 0, strNum );
	joinerList.SetItemText( nJoinerNo , 1, m_GamePlayer[ nJoinerNo ].m_wstrID  );
	joinerList.SetItemText( nJoinerNo , 2, strState );	

	++UserInRoomInfo::m_nJoiner;

	LeaveCriticalSection(&m_csJoinerInRoom);

	return TRUE;
}

//		방에 들어가서 기존 유저정보를 포함하여 추가 갱신
BOOL	NetLobbyClient::UpdateJoinerListInRoom( JoinerList *pJoinerList )
{
	if( NULL == pJoinerList )
		return FALSE;

	if( 0 >= UserInRoomInfo::m_nJoiner )
		return FALSE;
	

	CString strNum, strState;

	CListCtrl &joinerList = pJoinerList->GetListCtrl();

	EnterCriticalSection(&m_csJoinerInRoom);

	for( BYTE iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{
		strNum.Format( L"%d", iJoiner + 1 );

		switch( m_GamePlayer[ iJoiner ].m_MyState )
		{
		case STATE_DEFAULT:
			// 기본 상태
			strState = _T("신도");
			break;

		case STATE_READY:
			// 레디 상태
			strState = _T("기도준비");
			break;

		case STATE_GAMMING:
			// 게임중 상태
			strState = _T("기도중");
			break;

		case STATE_CAPTAIN:
			// 방장
			strState = _T("신");
			break;
		}	

		LV_ITEM lvitem;
		lvitem.mask = LVIF_TEXT;					//데이터 스타일 마스크
		lvitem.iItem = iJoiner;						//아이템 번호
		lvitem.iSubItem = 0;						//컬럼 번호
		lvitem.pszText = strNum.GetBuffer();		//아이템문자열		

		joinerList.InsertItem( &lvitem );
	//	joinerList.SetItemText( nJoinNumber +1, 0, strNum );
		joinerList.SetItemText( iJoiner , 1, m_GamePlayer[ iJoiner ].m_wstrID  );
		joinerList.SetItemText( iJoiner , 2, strState );	

	}

	LeaveCriticalSection(&m_csJoinerInRoom);

	return TRUE;
}

//		방 참여인원 1명의 상태 갱신
void	NetLobbyClient::UpdateJoinerListState( BYTE nPosInRoom, JoinerList *pJoinerList )
{

	if( NULL == pJoinerList )
		return;
			
	CString strState;	

	switch( m_GamePlayer[ nPosInRoom ].m_MyState )
	{
	case STATE_DEFAULT:
		// 기본 상태
		strState = _T("신도");
		break;

	case STATE_READY:
		// 레디 상태
		strState = _T("기도준비");
		break;

	case STATE_GAMMING:
		// 게임중 상태
		strState = _T("기도중");
		break;

	case STATE_CAPTAIN:
		// 방장
		strState = _T("신");
		break;
	}	

	CListCtrl &joinerList = pJoinerList->GetListCtrl();

	joinerList.SetItemText( nPosInRoom , 2, strState );	

}


//-------------------------------------------------------------------------
//
//							Recv Functions
//
//	Desc : Server로 부터 메시지 수신하여 처리하는 함수 모음
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트에게 로비서버 생성된 방 리스트 정보 송신
//	Packet		: | type | nRoomNo(방번호) | m_nJoiners(현재참여자) | m_nMaxJoiners(방제한인원) |  m_nRoomState(방상태) | m_nMapInfo(맵정보) | m_wstrRoomName(방이름)	
//	Size		: BYTE + DWORD + BYTE + BYTE + BYTE + BYTE + (wchat_t) * ROOM_NAME_LEN(30) = 64BYTE		512 / 64
//-------------------------------------------------------------------------------------------------------------------------------------------------
BOOL	NetLobbyClient::AddRoomList	( const char *pBuf, int nLen )
{

	DWORD dwSize = sizeof(DWORD) + sizeof(BYTE);
	DWORD dwPacketRoomSize = sizeof(BYTE)*5 + sizeof(wchar_t) * ROOM_NAME_LEN;

	RoomInfo stRoom;
	int nCnt = 0;

	EnterCriticalSection( &m_csListRooms );

	while( (int)dwSize < nLen  )
	{
		ZeroMemory( &stRoom, sizeof(RoomInfo) );

		::CopyMemory( &stRoom.m_nRoomNo		,   pBuf + dwSize					,	sizeof(BYTE)					);
		::CopyMemory( &stRoom.m_nJoiners	,   pBuf + dwSize + sizeof(BYTE) 	,	sizeof(BYTE)					);
		::CopyMemory( &stRoom.m_nMaxJoiners	,   pBuf + dwSize + sizeof(BYTE)*2 	,	sizeof(BYTE)					);
		::CopyMemory( &stRoom.m_nRoomState	,	pBuf + dwSize + sizeof(BYTE)*3 	,	sizeof(BYTE)					);
		::CopyMemory( &stRoom.m_nMapInfo	,	pBuf + dwSize + sizeof(BYTE)*4 	,	sizeof(BYTE)					);
		::CopyMemory( stRoom.m_wstrRoomName	,	pBuf + dwSize + sizeof(BYTE)*5	,	sizeof(wchar_t)*ROOM_NAME_LEN	);		

		stRoom.m_nListNo = nCnt++;
		
		dwSize += dwPacketRoomSize;				

		m_listRooms.push_back( stRoom );		
	}

	m_listRooms.sort();

	LeaveCriticalSection( &m_csListRooms );

	return TRUE;
}



//-------------------------------------------------------------------------------------
//
//	Function	: 로비서버에서 생성된 방 정보 1개 수신 처리
//	Packet		: Size | type | nRoomNo(방번호) | nRoomNo(방이름)	| (방인원) | (방상태)
//	Size		: DWORD + BYTE + (wchat_t) * ROOM_NAME_LEN(30) + BYTE + BYTE + BYTE = 68BYTE		
//-------------------------------------------------------------------------------------

BOOL	NetLobbyClient::AddRoom( const char *pBuf, int nLen )
{
	RoomInfo stRoom;
	PlayerInfo stPlayer;
	BYTE nPlyaerInLobby = 0;
	std::list< PlayerInfo >::iterator iterPlayer;

	::ZeroMemory( &stRoom, sizeof(RoomInfo) );

	::CopyMemory( &stPlayer.m_nPlayerNo	,	pBuf + sizeof(BYTE)		,	sizeof(BYTE)					);
	::CopyMemory( &stRoom.m_nRoomNo		,   pBuf + sizeof(BYTE)*2	,	sizeof(BYTE)					);
	::CopyMemory( &stRoom.m_nJoiners	,   pBuf + sizeof(BYTE)*3 	,	sizeof(BYTE)					);
	::CopyMemory( &stRoom.m_nMaxJoiners	,   pBuf + sizeof(BYTE)*4 	,	sizeof(BYTE)					);
	::CopyMemory( &stRoom.m_nRoomState	,	pBuf + sizeof(BYTE)*5 	,	sizeof(BYTE)					);
	::CopyMemory( stRoom.m_wstrRoomName	,	pBuf + sizeof(BYTE)*6	,	sizeof(wchar_t)*ROOM_NAME_LEN	);

	EnterCriticalSection( &m_csListRooms );
	EnterCriticalSection( &m_csListPlayers );

	stRoom.m_nListNo = static_cast<BYTE>( m_listRooms.size() );
	m_listRooms.push_back( stRoom );
	m_listRooms.sort();

	if( std::binary_search( m_listPlayers.begin(), m_listPlayers.end(), stPlayer ) )
	{
		iterPlayer = std::lower_bound( m_listPlayers.begin(), m_listPlayers.end(), stPlayer );

		stPlayer.m_nListNo = iterPlayer->m_nListNo;
		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_DELETE_ONE	, iterPlayer->m_nListNo	 );

		m_listPlayers.erase( iterPlayer );

		if( !m_listPlayers.empty() )
		{
			for( iterPlayer = m_listPlayers.begin(); iterPlayer != m_listPlayers.end(); ++iterPlayer )
			{
				if( iterPlayer->m_nListNo > stPlayer.m_nListNo )
					iterPlayer->m_nListNo--;
			}
		}
	}

	LeaveCriticalSection( &m_csListPlayers );
	LeaveCriticalSection( &m_csListRooms );

	//방 리스트 뷰 추가 갱신 
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_ADD_ONE		, stRoom.m_nRoomNo			);	

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 해당 클라이언트에게 로비서버 접속한 Client 리스트 정보 송신
//	Packet		: Size | type | m_nPlayerNo | m_wstrID(Client ID) | 
//	Size		: DWORD + BYTE + BYTE + (wchat_t) * USER_ID_LEN(25) = 52BYTE		512 / 52
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::AddPlayerList( const char *pBuf, int nLen )
{

	DWORD dwSize = sizeof(DWORD) + sizeof(BYTE);
	DWORD dwPacketPlayerize =  sizeof(BYTE) + sizeof(wchar_t) * USER_ID_LEN;

	PlayerInfo	stPlayer;
	int			nCnt = 0;

	EnterCriticalSection( &m_csListPlayers );

	while( (int)dwSize < nLen  )
	{
		ZeroMemory( &stPlayer, sizeof(stPlayer) );

		::CopyMemory( &stPlayer.m_nPlayerNo	, pBuf + dwSize					, sizeof(BYTE)					);				
		::CopyMemory( stPlayer.m_wstrID		, pBuf + dwSize + sizeof(BYTE)	, sizeof(wchar_t) * USER_ID_LEN	);				

		stPlayer.m_nListNo = nCnt++;
		
		dwSize += dwPacketPlayerize;				

		m_listPlayers.push_back( stPlayer );		
	}

	m_listPlayers.sort();

	LeaveCriticalSection( &m_csListPlayers );	

	return TRUE;
}


//-------------------------------------------------------------------------------------
//
//	Function	: 로비서버 접속한 Client 1명 정보 송신
//	Packet		: Size | type | m_nPlayerNo | m_wstrID(Client ID) | 
//	Size		: DWORD + BYTE + BYTE + (wchat_t) * USER_ID_LEN(25) = 52BYTE		512 / 52
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::AddPlayer	( const char *pBuf, int nLen )
{
	
	PlayerInfo stPlayer;
	::ZeroMemory( &stPlayer, sizeof(PlayerInfo) );

	::CopyMemory( &stPlayer.m_nPlayerNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE)					);				
	::CopyMemory( stPlayer.m_wstrID		, pBuf + sizeof(BYTE)*2	, sizeof(wchar_t) * USER_ID_LEN	);	
	
	stPlayer.m_nListNo = static_cast<BYTE>( m_listPlayers.size() );

	EnterCriticalSection( &m_csListPlayers );

	m_listPlayers.push_back( stPlayer );		
	m_listPlayers.sort();

	LeaveCriticalSection( &m_csListPlayers );
	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_ADD_ONE, stPlayer.m_nPlayerNo );

	return TRUE;	
}

//		방 1개 리스트에서 제거
BOOL	NetLobbyClient::DeleteRoom( const char *pBuf, int nLen )
{
	
	RoomInfo stRoom;	
	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( &stRoom.m_nRoomNo		,   pBuf + sizeof(BYTE),	sizeof(BYTE) );

	EnterCriticalSection( &m_csListRooms );

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stRoom );
		stRoom.m_nListNo = iterRoom->m_nListNo;

		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_DELETE_ONE, iterRoom->m_nListNo );

		m_listRooms.erase( iterRoom );		

		if( !m_listRooms.empty() )
		{
			for( iterRoom = m_listRooms.begin(); iterRoom != m_listRooms.end(); ++iterRoom )
			{
				if( iterRoom->m_nListNo > stRoom.m_nListNo )
					iterRoom->m_nListNo--;
			}
		}
	}	

	LeaveCriticalSection( &m_csListRooms );	

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 로비서버 접속 종료한 Client 1명 정보 송신하여 리스트에서 제거
//	Packet		: Size | type | m_nPlayerNo 
//	Size		: DWORD + BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::DeletePlayer( const char *pBuf, int nLen )
{
	
	PlayerInfo stPlayer;	
	::ZeroMemory( &stPlayer, sizeof(stPlayer) );
	std::list< PlayerInfo >::iterator iterPlayer;

	::CopyMemory( &stPlayer.m_nPlayerNo,   pBuf + sizeof(BYTE),	sizeof(BYTE) );

	EnterCriticalSection( &m_csListPlayers );

	if( std::binary_search( m_listPlayers.begin(), m_listPlayers.end(), stPlayer ) )
	{
		iterPlayer = std::lower_bound( m_listPlayers.begin(), m_listPlayers.end(), stPlayer );
		stPlayer.m_nListNo = iterPlayer->m_nListNo;
		// 유저 리스트 갱신 필요
		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_DELETE_ONE, iterPlayer->m_nListNo );
		m_listPlayers.erase( iterPlayer );

		if( !m_listPlayers.empty() )
		{
			for( iterPlayer = m_listPlayers.begin(); iterPlayer != m_listPlayers.end(); ++iterPlayer )
			{
				if( iterPlayer->m_nListNo > stPlayer.m_nListNo )
					iterPlayer->m_nListNo--;
			}
		}
	}	

	LeaveCriticalSection( &m_csListPlayers );	
	
	return TRUE;

}


//-------------------------------------------------------------------------------------
//
//	Function	: 방안에 들어어가서 기존 유저들에 대한 정보를 수신 갱신
//	Packet		:	| type | nPlayerInRoomNo(방에 있는 고유번호) | stTmpRoom.m_nRoomNo(방번호) | nPosInRoom(방에서의 위치) |
//					| m_nJoiners | nState(유저 상태) | m_wstrID(ID) | m_strConnRoomName( 방이름 ) |
//	Size		: BYTE + BYTE + BYTE + BYTE + BYTE + wchar_t * USER_ID_LEN + wchar_t * ROOM_NAME_LEN = 114byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::JoinRoom( const char *pBuf, int nLen )
{
	BYTE nCntJoiner = 0;
	BYTE nMapIndex	= 0;
	wchar_t tmpRoomName[ ROOM_NAME_LEN ];
	::ZeroMemory( tmpRoomName, sizeof(tmpRoomName) );
	DWORD dwSize			= sizeof(BYTE)*6	+ sizeof(wchar_t) * ROOM_NAME_LEN;
	DWORD dwSizeofPlayer	= sizeof(BYTE)		+ sizeof(wchar_t) * USER_ID_LEN;

	m_strConnRoomName = _T("");

	::CopyMemory( &m_PlayerNo						,	pBuf + sizeof(BYTE)		, sizeof(BYTE)						);
	::CopyMemory( &m_RoomNo							,	pBuf + sizeof(BYTE)*2	, sizeof(BYTE)						);
	::CopyMemory( &m_nPosInRoom						,	pBuf + sizeof(BYTE)*3	, sizeof(BYTE)						);	
	::CopyMemory( &nCntJoiner						,	pBuf + sizeof(BYTE)*4	, sizeof(BYTE)						);	
	::CopyMemory( &nMapIndex						,	pBuf + sizeof(BYTE)*5	, sizeof(BYTE)						);	
	::CopyMemory( tmpRoomName						,	pBuf + sizeof(BYTE)*6	, sizeof(wchar_t) * ROOM_NAME_LEN	);		

	m_strConnRoomName = tmpRoomName;
	EnterCriticalSection(&m_csJoinerInRoom);		

	for( BYTE iJoiner = 0;  iJoiner < nCntJoiner; iJoiner++ )
	{
		::CopyMemory( &m_GamePlayer[ iJoiner ].m_MyState	, pBuf + dwSize					, sizeof(BYTE)					);
		::CopyMemory( m_GamePlayer[ iJoiner ].m_wstrID		, pBuf + dwSize + sizeof(BYTE)	, sizeof(wchar_t) * USER_ID_LEN	);

		dwSize += dwSizeofPlayer;

		UserInRoomInfo::m_nJoiner++;
	}

	m_nCntRoom = 0;

	m_GamePlayer[ UserInRoomInfo::m_nJoiner		].m_MyState = STATE_DEFAULT;
	wcscpy_s( m_GamePlayer[ UserInRoomInfo::m_nJoiner	].m_wstrID, m_strMyGameID.GetBuffer() );
	UserInRoomInfo::m_nJoiner++;

	LeaveCriticalSection(&m_csJoinerInRoom);	

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_NAME				, 0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_MYSELF			, 0 );	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_PLAYER_ADD_MORE	, 0 );

	// 현재 방 리스트에 있는 내용들 삭제
	EnterCriticalSection(&m_csListRooms);			
	EnterCriticalSection(&m_csListPlayers);

	if( !m_listRooms.empty() )
		m_listRooms.clear();

	if( !m_listPlayers.empty() )
		m_listPlayers.clear();

	LeaveCriticalSection(&m_csListPlayers);
	LeaveCriticalSection(&m_csListRooms);

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_PLAYER_DELETE_ALL	,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_DELETE_ALL		,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_BUTTON		,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_IS_CAPTAIN			,		m_bCaptain);
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_MAPINFO_CHANGE		,		nMapIndex );


	return ChangeDisplay( ROOM_DISPLAY );
}


//-------------------------------------------------------------------------------------
//
//	Function	: 방에 있는 유저의 상태 정보 갱신
//	Packet		: | type | nPosInRoom( 방에서의 위치 ) | nPushType( 레디 상태 ) |
//	Size		:  BYTE + BYTE + BYTE + BYTE =  3byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::UpdateJoinerState( const char *pBuf, int nLen )
{
	BYTE nPosInRoom = 0;
	BYTE nPushType = 0;
	
	::CopyMemory( &nPosInRoom	,	pBuf + sizeof(BYTE)		,	sizeof(BYTE) );
	::CopyMemory( &nPushType	,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE) );

	EnterCriticalSection( &m_csJoinerInRoom );

	if( nPushType )
		m_GamePlayer[ nPosInRoom ].m_MyState = STATE_READY;
	else
		m_GamePlayer[ nPosInRoom ].m_MyState = STATE_DEFAULT;

	LeaveCriticalSection( &m_csJoinerInRoom );

	::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_PLAYER_UPDATE_ONE,	nPosInRoom );

	if( m_bCaptain )
	{
		BYTE nCntState = 0;

		for( int iJoiner = 0; iJoiner < ROOM_MAX_PLAYER; iJoiner++ )
		{
			if( m_GamePlayer[ iJoiner ].m_MyState & ( STATE_READY | STATE_CAPTAIN ) )
				nCntState++;
		}

		if( nCntState == UserInRoomInfo::m_nJoiner )
			::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_UPDATE_BUTTON,	1 );
		else
			::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_UPDATE_BUTTON,	0 );
	}

	return TRUE;
}


//		새로 방에 들어온 유정 정보 추가
BOOL	NetLobbyClient::AddJoinerInRoom( const char *pBuf, int nLen )
{
	BYTE nPosInRoom = 0;
	CString strMsg;

	::CopyMemory( &nPosInRoom							, pBuf + sizeof(BYTE)	,	sizeof(BYTE)					);
	::CopyMemory( &m_GamePlayer[ nPosInRoom ].m_MyState	, pBuf + sizeof(BYTE)*2	,	sizeof(BYTE)					);
	::CopyMemory( m_GamePlayer[ nPosInRoom ].m_wstrID	, pBuf + sizeof(BYTE)*3	,	sizeof(wchar_t) * USER_ID_LEN	);

	strMsg += m_GamePlayer[ nPosInRoom ].m_wstrID;
	strMsg += _T("님께서 깨달음을 원했습니다.\r\n");

	m_listChat.push_back( strMsg );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_CHAT_UPDATE		,			0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_PLAYER_ADD_ONE	, nPosInRoom  );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_BUTTON	,			0 );

	return TRUE;	
}

//		방 상태 정보 갱싱
BOOL	NetLobbyClient::UpdateRoomInfo( const char *pBuf, int nLen )
{
	RoomInfo tmpRoom;	
	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( &tmpRoom.m_nRoomNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE) );
	::CopyMemory( &tmpRoom.m_nJoiners	, pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );
	::CopyMemory( &tmpRoom.m_nRoomState	, pBuf + sizeof(BYTE)*3	, sizeof(BYTE) );


	EnterCriticalSection(&m_csListRooms);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), tmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), tmpRoom );

		iterRoom->m_nJoiners	= tmpRoom.m_nJoiners;
		iterRoom->m_nRoomState	= tmpRoom.m_nRoomState;
	}

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_UPDATE_ONE, reinterpret_cast< LPARAM >( &(*iterRoom) ) );

	LeaveCriticalSection(&m_csListRooms);	

	return TRUE;
}

//		방 나가기
BOOL	NetLobbyClient::OutRoom( const char *pBuf, int nLen )
{

	::CopyMemory( &m_PlayerNo,	pBuf + sizeof(BYTE), sizeof(BYTE) );
	
	m_RoomNo		= 0;
	m_nPosInRoom	= 0;
		
	m_bCaptain		= FALSE;	// 방장 여부
	m_bReady		= FALSE;	// 준비완료 여부

	UserInRoomInfo::m_nJoiner = 0;

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_MYSELF				,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_PLAYER_DELETE_ALL	,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_BUTTON		,				0 );
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_IS_CAPTAIN			,		m_bCaptain);

	return ChangeDisplay( LOBBY_DISPLAY );	
}


//-------------------------------------------------------------------------------------
//
//	Function	: 방안에서 나간 접속자 1명 리스트에서 제거
//	Packet		: | type | nPosInRoom( 플레이어 방에서 위치값 ) | m_wstrID( 플레이어 ID )
//	Size		:  BYTE + BYTE + wchar_t * USER_ID_LEN = 51Byte;
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::DeleteJoinerAtRoom( const char *pBuf, int nLen )
{
	
	CString strMsg;
	wchar_t tmpName[ USER_ID_LEN ];
	BYTE	nPosInRoom = 0;

	::CopyMemory( &nPosInRoom		,	pBuf + sizeof(BYTE)		,	sizeof(BYTE)					);
	::CopyMemory( tmpName			,	pBuf + sizeof(BYTE)*2	,	sizeof(wchar_t) * USER_ID_LEN	);

	EnterCriticalSection( &m_csJoinerInRoom );

	for( BYTE iJoiner = nPosInRoom + 1; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{
		m_GamePlayer[ iJoiner - 1 ] = m_GamePlayer[ iJoiner ];
	}	

	::ZeroMemory( &m_GamePlayer[ --UserInRoomInfo::m_nJoiner ], sizeof(UserInRoomInfo) );

	LeaveCriticalSection( &m_csJoinerInRoom );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_PLAYER_DELETE_ONE, nPosInRoom  );

	strMsg += tmpName;
	strMsg += _T("님께서 하산하셨습니다.\r\n\r\n");

	if( nPosInRoom < m_nPosInRoom )
		--m_nPosInRoom;

	m_listChat.push_back( strMsg );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_CHAT_UPDATE,					0 );


	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 로비서버 부터 전송된  방 생성 메시지를 수신하여 생성한 방에 입장
//	Packet		:  type | nPlayerInRoomNo | m_nRoomNo | 
//	Size		:  BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::CreateRoom( const char *pBuf, int nLen )
{
	wchar_t roomName[ ROOM_NAME_LEN ];
	::ZeroMemory( roomName, sizeof(roomName) );
	
	::CopyMemory( &m_PlayerNo	, pBuf + sizeof(BYTE)	, sizeof(BYTE)					);
	::CopyMemory( &m_RoomNo		, pBuf + sizeof(BYTE)*2	, sizeof(BYTE)					);	
	::CopyMemory( roomName		, pBuf + sizeof(BYTE)*3	, sizeof(wchar_t)*ROOM_NAME_LEN	);
	
	m_strConnRoomName = _T("");
	m_strConnRoomName = roomName;
	m_GamePlayer[ 0 ].m_MyState = STATE_CAPTAIN;
//	UserInRoomInfo::m_nJoiner = 1;
	m_bCaptain = TRUE;
	m_nCntRoom = 0;

	wcscpy_s( m_GamePlayer[ 0 ].m_wstrID, m_strMyGameID.GetBuffer() );
	
	EnterCriticalSection(&m_csListRooms);
	EnterCriticalSection(&m_csListPlayers);

	m_listRooms.clear();
	m_listPlayers.clear();

	LeaveCriticalSection(&m_csListPlayers);
	LeaveCriticalSection(&m_csListRooms);	

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_NAME				,	0			);
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_MYSELF			,	0			);	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_BUTTON	,	0			);	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_PLAYER_ADD_ONE	,	0			);	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_IS_CAPTAIN		,	m_bCaptain	);

	return ChangeDisplay( ROOM_DISPLAY );
	
}

//-------------------------------------------------------------------------------------
//
//	Function	: 로비서버 부터 전송된  채팅 메시지를 로비 CEdit에 추가
//	Packet		: Size | type | m_nPlayerNo 
//	Size		: DWORD + BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::AddChatMessageAtLobby( const char *pBuf, int nLen )
{
	BYTE nMsgLen = 0;
	wchar_t pwBuf[ BUFSIZE ];
	::ZeroMemory( pwBuf, sizeof( pwBuf ) );

	::CopyMemory( &nMsgLen	, pBuf + sizeof(BYTE)	, sizeof(BYTE)				);
	::CopyMemory( pwBuf		, pBuf + sizeof(BYTE)*2	, sizeof(wchar_t) * nMsgLen	);

	CString strMsg( pwBuf );	
	strMsg += _T("\r\n");

	m_listChat.push_back( strMsg );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_CHAT_UPDATE, 0 );

	return TRUE;
}


//		방 채팅 메시지 추가
BOOL	NetLobbyClient::AddChatMessageAtRoom( const char *pBuf, int nLen )
{
	BYTE nMsgLen = 0;
	wchar_t pwBuf[ BUFSIZE ];
	::ZeroMemory( pwBuf, sizeof( pwBuf ) );

	::CopyMemory( &nMsgLen	, pBuf + sizeof(BYTE)	, sizeof(BYTE)				);
	::CopyMemory( pwBuf		, pBuf + sizeof(BYTE)*2	, sizeof(wchar_t) * nMsgLen	);

	CString strMsg( pwBuf );	
	strMsg += _T("\r\n");

	m_listChat.push_back( strMsg );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_CHAT_UPDATE, 0 );

	return TRUE;
}



//-------------------------------------------------------------------------------------
//
//	Function	: 로그인 성공후 고유번호와 게임아이디 얻음
//	Packet		: | type | m_nPlayerNo | 
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::LoginOK( const char *pBuf, int nLen )
{
	wchar_t strMyGameID[ USER_ID_LEN ];
	::ZeroMemory( strMyGameID, sizeof(strMyGameID) );

	::CopyMemory( &m_PlayerNo, pBuf + sizeof(BYTE)	, sizeof(BYTE)					);
	::CopyMemory( strMyGameID, pBuf + sizeof(BYTE)*2, sizeof(wchar_t)*USER_ID_LEN	);

	m_strMyGameID = _T("[");
	m_strMyGameID += strMyGameID;
	m_strMyGameID += _T("] ");

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_MYSELF, 0 );

	return ChangeDisplay( LOBBY_DISPLAY );
	
}

//-------------------------------------------------------------------------------------
//
//	Function	: 맵 정보 변경 갱신
//	Packet		: | type | nMapIndex | 
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::MapInfoChange( const char *pBuf, int nLen )
{
	BYTE nMapIndex = 0;

	::CopyMemory( &nMapIndex,	pBuf + sizeof(BYTE),	sizeof(BYTE) );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_MAPINFO_CHANGE, nMapIndex );	

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 맵 정보 변경 갱신
//	Packet		: | type | nRoomNo(방번호) | nMapIndex(
//	Size		:  BYTE + BYTE + BYTE =  2byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::MapInfoChangeAtLobby( const char *pBuf, int nLen )
{ 
	RoomInfo stTmpRoom;
	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( &stTmpRoom.m_nRoomNo		,	pBuf + sizeof(BYTE)		,	sizeof(BYTE) );
	::CopyMemory( &stTmpRoom.m_nMapInfo		,	pBuf + sizeof(BYTE)*2	,	sizeof(BYTE) );

	EnterCriticalSection(&m_csListRooms);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );

		iterRoom->m_nMapInfo = stTmpRoom.m_nMapInfo;
	}

	LeaveCriticalSection(&m_csListRooms);

	return TRUE;
}

//		방장 변경 메시지 수신 갱신
BOOL	NetLobbyClient::ChangeCaptain( const char *pBuf, int nLen )
{
	BYTE nPosInRoom = 0;
	
	::CopyMemory( &nPosInRoom,	pBuf + sizeof(BYTE), sizeof(BYTE) );
	
	EnterCriticalSection(&m_csJoinerInRoom);
	m_GamePlayer[ nPosInRoom ].m_MyState = STATE_CAPTAIN;		

	// 내가 방장이 된다면
	if( m_nPosInRoom == nPosInRoom )
	{
		m_bCaptain = TRUE;
		::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_IS_CAPTAIN,	m_bCaptain			);
	}
	
	for( BYTE iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner ; iJoiner++ )
	{
		if( STATE_CAPTAIN != m_GamePlayer[ iJoiner ].m_MyState  )
		{
			m_GamePlayer[ iJoiner ].m_MyState = STATE_DEFAULT;
			::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_PLAYER_UPDATE_ONE,	iJoiner	);
		}
	}	

	LeaveCriticalSection(&m_csJoinerInRoom);

	m_bReady = FALSE;

	::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_PLAYER_UPDATE_ONE	,	nPosInRoom		);		
	::PostMessage( m_hWnd,	UM_UPDATE_DISPLAY,	ROOM_UPDATE_BUTTON		,	0				);
	
	return TRUE;
}	

//-------------------------------------------------------------------------------------
//
//	Function	: 방장에게 게임에 입장할 각각의 유저의 주소 정보를 송신
//	Packet		: | type | m_nJoiners(인원 수) | SOCKADDR_IN * n명 |
//	Size		:  BYTE + BYTE + BYTE + SOCKADDR_IN * N명=  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::GameStartCaptain( const char *pBuf, int nLen )
{

	char pTempBuf[ BUFSIZE ];
	::ZeroMemory( pTempBuf, sizeof(pTempBuf) );
	BYTE	nJoiner = 0;	
	BYTE	nCaptain = 1;
	BYTE	nMapIndex = 0;
	SOCKADDR_IN SockAddrCaptain;
	DWORD	dwSize = sizeof(BYTE) * 3;
	wchar_t wStrUserID[USER_ID_LEN];
	char	strCaptainID[USER_ID_LEN];
	
	::ZeroMemory( wStrUserID	, sizeof(wStrUserID) );		
	::ZeroMemory( strCaptainID	, sizeof(strCaptainID) );		
	::ZeroMemory( &SockAddrCaptain, sizeof(SockAddrCaptain) );

	// wchar_t --> char
//	WideCharToMultiByte(CP_ACP, 0, m_strMyGameID.GetBuffer(), -1, strCaptainID	, sizeof(strCaptainID), NULL, NULL); 	

	::CopyMemory( &SockAddrCaptain, pBuf + sizeof(BYTE)*3	, sizeof(SOCKADDR_IN) );
	SockAddrCaptain.sin_port = 10000;

	char buf[25];
	::ZeroMemory( buf, sizeof(buf) );
	strcpy_s( buf ,inet_ntoa(SockAddrCaptain.sin_addr));
		
/*		
	for( BYTE iJoiner = 0; iJoiner < nJoiner; iJoiner++ )
	{
		::CopyMemory( &pGameUserList[ iJoiner ].SockAddr	, pBuf + dwSize							, sizeof(SOCKADDR_IN)			);
		::CopyMemory( wStrUserID							, pBuf + dwSize + sizeof(SOCKADDR_IN)	, sizeof(wchar_t) * USER_ID_LEN );

		WideCharToMultiByte(CP_ACP, 0, wStrUserID, -1, pGameUserList[ iJoiner ].strGameID	, sizeof(pGameUserList[ iJoiner ].strGameID), NULL, NULL); 

		::CopyMemory( pTempBuf + iJoiner*dwJoinerPacketSize, &pGameUserList[ iJoiner ], sizeof(GameUser) );

		dwSize += dwJoinerPacketSize;
	}
*/

	struct GameUser {

		SOCKADDR_IN		SockAddr;
		char			strGameID[ USER_ID_LEN ];

	};	

	DWORD dwJoinerPacketSize = sizeof(GameUser);
		
	::CopyMemory( &nJoiner	,	pBuf + sizeof(BYTE)		, sizeof(BYTE) );
	::CopyMemory( &nMapIndex,	pBuf + sizeof(BYTE)*2	, sizeof(BYTE) );

	GameUser *pGameUserList = new GameUser[ UserInRoomInfo::m_nJoiner ];


	EnterCriticalSection(&m_csJoinerInRoom);
	for( int iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{
		WideCharToMultiByte(CP_ACP, 0, m_GamePlayer[ iJoiner ].m_wstrID, -1, pGameUserList[ iJoiner ].strGameID	, sizeof(pGameUserList[ iJoiner ].strGameID	), NULL, NULL); 
	}
	LeaveCriticalSection(&m_csJoinerInRoom);
	
//	nJoiner++;
	
	// Packet 처리 Thread 중지
//	SuspendThread(m_hWorkerThread);

#ifdef _SPAWN_AGR1

	char chJoiner[2];
	sprintf_s( chJoiner, "%d", nJoiner );
	
	// Agr 하나로 실행
	//_spawnl( P_WAIT, g_strGameApp.c_str(), g_strGameApp.c_str(), chJoiner, reinterpret_cast< char* >( &pGameUserList[ 0 ] ), NULL );
	//_spawnl( P_WAIT, g_strGameApp.c_str(), g_strGameApp.c_str(), chJoiner, reinterpret_cast< char* >( &pGameUserList[ 0 ] ), NULL );
	//_spawnl( P_WAIT, g_strGameApp.c_str(), g_strGameApp.c_str(), chJoiner, pTempBuf, pGameUserList[ 0 ].strGameID, NULL );
	_spawnl( P_WAIT, "TestProcess.exe", "TestProcess.exe", chJoiner, "sdfdsfafaadsf", NULL );

	
#else // 동물 농장 Process 생성 실행

	char chMapIndex[2], chJoiner[2], chPos[2], chIP[25], chPort[6];

	sprintf_s( chIP,		sizeof(chIP),		"%s", inet_ntoa(SockAddrCaptain.sin_addr)	);
	sprintf_s( chPort,		sizeof(chPort),		"%d", ntohs(SockAddrCaptain.sin_port)		);
	sprintf_s( chMapIndex,	sizeof(chMapIndex), "%d", nMapIndex								);
	sprintf_s( chJoiner,	sizeof(chJoiner),	"%d", UserInRoomInfo::m_nJoiner				);
	sprintf_s( chPos,		sizeof(chPos),		"%d", m_nPosInRoom							);

	switch( UserInRoomInfo::m_nJoiner )
	{
	case 2:
		// 1 : 1
		
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(), 
//			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
//			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
//			reinterpret_cast< char* >( &UserInRoomInfo::m_nJoiner ),	// 참여 인원
//			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치			
//			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
//			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
//			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
/*			(char*)( &nMapIndex ),					// 맵 인덱스 BYTE
			(char*)( &UserInRoomInfo::m_nJoiner ),	// 참여 인원
			(char*)( &m_nPosInRoom ),					// 방에서 위치			
			(char*)( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			(char*)( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
*/
			(char*)( &chIP ),							// IP
			(char*)( &chPort ),							// PORT
			(char*)( &chMapIndex ),						// 맵 인덱스 BYTE
			(char*)( &chJoiner ),						// 참여 인원
			(char*)( &chPos ),							// 방에서 위치			
			(char*)( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			(char*)( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
			NULL  );					
		break;

	case 3:
		// 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &UserInRoomInfo::m_nJoiner ),	// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 2 )
			NULL  );
		break;

	case 4:
		// 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &UserInRoomInfo::m_nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 3 )
			NULL  );
		break;

	case 5:
		// 1 : 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &UserInRoomInfo::m_nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 4 )
			reinterpret_cast< char* >( pGameUserList[ 4 ].strGameID ),	// ID ( 유저 4 )
			NULL  );
		break;

	case 6:
		// 1 : 1 : 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(), 
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 4 )
			reinterpret_cast< char* >( pGameUserList[ 4 ].strGameID ),	// ID ( 유저 5 )
			reinterpret_cast< char* >( pGameUserList[ 5 ].strGameID ),	// ID ( 유저 5 )
			NULL  );
		break;
	}

#endif

//	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_GAME_START, 0 );
	
	delete[] pGameUserList;

	// Packet 처리 Thread 시작
//	ResumeThread(m_hWorkerThread);

	// 대기 상태로 변경
//	RequestReadyGame();

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장 정보를 수신하여 방장에 접속
//	Packet		: | type | SOCKADDR_IN  |
//	Size		:  BYTE + BYTE + SOCKADDR_IN * N명=  1byte
//-------------------------------------------------------------------------------------

BOOL	NetLobbyClient::GameStartJoiner( const char *pBuf, int nLen )
{
	SOCKADDR_IN SockAddrCaptain;
	wchar_t		wstrCaptainID[ USER_ID_LEN ];
//	char		 strCaptainID[ USER_ID_LEN ];
	BYTE		nJoiner	  = 0;
	BYTE		nMapIndex = 0;
	BYTE		nCaptainIndex = 0;

	::ZeroMemory( &SockAddrCaptain, sizeof(SockAddrCaptain) );
	::ZeroMemory( wstrCaptainID, sizeof(wstrCaptainID) );

	::CopyMemory( &nJoiner			, pBuf + sizeof(BYTE)							, sizeof(BYTE)					);	
	::CopyMemory( &nMapIndex		, pBuf + sizeof(BYTE)*2							, sizeof(BYTE)					);	
	::CopyMemory( &nCaptainIndex	, pBuf + sizeof(BYTE)*3							, sizeof(BYTE)					);	
	::CopyMemory( &SockAddrCaptain	, pBuf + sizeof(BYTE)*4							, sizeof(SOCKADDR_IN)			);	
	
	SockAddrCaptain.sin_port = 10000;

	struct GameUser {

		SOCKADDR_IN		SockAddr;
		char			strGameID[ USER_ID_LEN ];

	};	

	GameUser *pGameUserList = new GameUser[ UserInRoomInfo::m_nJoiner ];

	EnterCriticalSection(&m_csJoinerInRoom);
	for( int iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{
		WideCharToMultiByte(CP_ACP, 0, m_GamePlayer[ iJoiner ].m_wstrID, -1, pGameUserList[ iJoiner ].strGameID	, sizeof(pGameUserList[ iJoiner ].strGameID	), NULL, NULL); 
	}
	LeaveCriticalSection(&m_csJoinerInRoom);


	// Packet 처리 Thread 중지
//	SuspendThread(m_hWorkerThread);
	char chMapIndex[2], chJoiner[2], chPos[2], chIP[25], chPort[6];

	sprintf_s( chIP,		sizeof(chIP),		"%s", inet_ntoa(SockAddrCaptain.sin_addr)	);
	sprintf_s( chPort,		sizeof(chPort),		"%d", ntohs(SockAddrCaptain.sin_port)		);
	sprintf_s( chMapIndex,	sizeof(chMapIndex), "%d", nMapIndex								);
	sprintf_s( chJoiner,	sizeof(chJoiner),	"%d", UserInRoomInfo::m_nJoiner				);
	sprintf_s( chPos,		sizeof(chPos),		"%d", m_nPosInRoom							);
	

	switch( UserInRoomInfo::m_nJoiner )
	{
	case 2:
		// 1 : 1
		
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(), 
//			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
//			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
//			reinterpret_cast< char* >( &UserInRoomInfo::m_nJoiner ),	// 참여 인원
//			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치			
//			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
//			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
//			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
/*			(char*)( &nMapIndex ),					// 맵 인덱스 BYTE
			(char*)( &UserInRoomInfo::m_nJoiner ),	// 참여 인원
			(char*)( &m_nPosInRoom ),					// 방에서 위치			
			(char*)( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			(char*)( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
*/
			(char*)( &chIP ),							// IP
			(char*)( &chPort ),							// PORT
			(char*)( &chMapIndex ),						// 맵 인덱스 BYTE
			(char*)( &chJoiner ),						// 참여 인원
			(char*)( &chPos ),							// 방에서 위치			
			(char*)( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			(char*)( pGameUserList[ 1 ].strGameID  ),	// ID ( 유저 1 )
			NULL  );
		break;

	case 3:
		// 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 2 )
			NULL  );
		break;

	case 4:
		// 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 3 )
			NULL  );
		break;

	case 5:
		// 1 : 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(),
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 4 )
			reinterpret_cast< char* >( pGameUserList[ 4 ].strGameID ),	// ID ( 유저 4 )
			NULL  );
		break;

	case 6:
		// 1 : 1 : 1 : 1 : 1 : 1
		_spawnl( P_WAIT, g_strGameAppPath.c_str(), g_strGameAppPath.c_str(), 
			reinterpret_cast< char* >( &SockAddrCaptain ),				// 방장 주소정보
			reinterpret_cast< char* >( &nMapIndex ),					// 맵 인덱스 BYTE
			reinterpret_cast< char* >( &nJoiner ),						// 참여 인원
			reinterpret_cast< char* >( &m_nPosInRoom ),					// 방에서 위치
			reinterpret_cast< char* >( pGameUserList[ 0 ].strGameID  ),	// ID ( 유저 1 )			
			reinterpret_cast< char* >( pGameUserList[ 1 ].strGameID ),	// ID ( 유저 2 )
			reinterpret_cast< char* >( pGameUserList[ 2 ].strGameID ),	// ID ( 유저 3 )
			reinterpret_cast< char* >( pGameUserList[ 3 ].strGameID ),	// ID ( 유저 4 )
			reinterpret_cast< char* >( pGameUserList[ 4 ].strGameID ),	// ID ( 유저 5 )
			reinterpret_cast< char* >( pGameUserList[ 5 ].strGameID ),	// ID ( 유저 5 )
			NULL  );
		break;
	}

//	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_GAME_START, 0 );

	delete[] pGameUserList;

	// Packet 처리 Thread 시작
//	ResumeThread(m_hWorkerThread);

	// 대기 상태로 변경
	RequestReadyGame();

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방으로 부터 초대 메시지 수신
//	Packet		: | type | nRoomNo  |
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::Invitation( const char *pBuf, int nLen )
{

	RoomInfo	stTmpRoom;

	std::list< RoomInfo >::iterator iterRoom;

	::CopyMemory( &stTmpRoom.m_nRoomNo	,	pBuf + sizeof(BYTE)		, sizeof(BYTE) );	

	EnterCriticalSection(&m_csListRooms);

	if( std::binary_search( m_listRooms.begin(), m_listRooms.end(), stTmpRoom ) )
	{
		iterRoom = std::lower_bound( m_listRooms.begin(), m_listRooms.end(), stTmpRoom );

		::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, LOBBY_ROOM_INVITATION, reinterpret_cast< LPARAM >( &(*iterRoom) ) );
	}

	LeaveCriticalSection(&m_csListRooms);

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장 초대 메시지 결과 수신
//	Packet		: | type | nRoomNo  |
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::InvitationResult( const char *pBuf, int nLen )
{
	BYTE		nPlayers = 0;
	CString		strMsg;
	
	::CopyMemory( &nPlayers				,	pBuf + sizeof(BYTE)	, sizeof(BYTE) );	

	strMsg.Format( L" %d명에게 초대 메시지를 발송하였습니다.", nPlayers );

	m_listChat.push_back( strMsg );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_CHAT_UPDATE,					0 );

	return TRUE;
}

//-------------------------------------------------------------------------------------
//
//	Function	: 방장 위임 정보 수신 처리
//	Packet		: | type | nPlayers( 초대 메시지 인원수 )  |
//	Size		:  BYTE + BYTE =  1byte
//-------------------------------------------------------------------------------------
BOOL	NetLobbyClient::GiveCaptain( const char *pBuf, int nLen )
{
	BYTE nNewCaptain = 0;
	
	::CopyMemory( &nNewCaptain,	pBuf + sizeof(BYTE)	, sizeof(BYTE) );		

	m_bCaptain	= FALSE;
	m_bReady	= FALSE;

	if( nNewCaptain == m_nPosInRoom )
		m_bCaptain = TRUE;

	EnterCriticalSection(&m_csJoinerInRoom);

	for( BYTE iJoiner = 0; iJoiner < UserInRoomInfo::m_nJoiner; iJoiner++ )
	{
		m_GamePlayer[ iJoiner ].m_MyState = STATE_DEFAULT;
	}

	m_GamePlayer[ nNewCaptain ].m_MyState = STATE_CAPTAIN;

	LeaveCriticalSection(&m_csJoinerInRoom);
	
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_ALL_STATE		,	0			);
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_UPDATE_BUTTON		,	0			);
	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_IS_CAPTAIN			,	m_bCaptain	);	
	
	return TRUE;
}


//----------------------------------------------------------------------------------------------------------------
//
//	Function	: 방안 유저 정보 수신 출력
//	Packet		: | type | nPosInRoom( 방 번호 ) | nWin( 승 ) | nFail( 패 ) | nLevel( 레벨 ) | nExp( 경험치 ) |
//	Size		:  BYTE + BYTE + BYTE + BYTE + BYTE + USHORT=  6byte
//----------------------------------------------------------------------------------------------------------------
BOOL	NetLobbyClient::ShowJoinerInfo( const char *pBuf, int nLen )
{
	
	char *tmpJoinerInfo = new char[ 7 ];
	::ZeroMemory( tmpJoinerInfo, sizeof(tmpJoinerInfo) );

	::CopyMemory( tmpJoinerInfo, pBuf + sizeof(BYTE), sizeof(BYTE)*6 );

	::PostMessage( m_hWnd, UM_UPDATE_DISPLAY, ROOM_IS_CAPTAIN, reinterpret_cast< LPARAM >( tmpJoinerInfo ) );

	return TRUE;
}

//		각종 이벤트 Message 창 출력
BOOL NetLobbyClient::ShowInfoMessage( BYTE msgType )
{

	switch( msgType )
	{
		case STOC_NOT_ENTER_FOOL:		
			// 방 정원이 차서 방 접속을 못하는 경우				
			
			MessageBox( NULL, L"다 찼으니깐, 우주 플리즈 꺼져 줄래?. '_'", L"ㅗ", MB_OK );
			break;

		case STOC_NOT_ENTER_GAMMING:
			// 방 정원이 차서 방 접속을 못하는 경우	

			MessageBox( NULL, L"시작했으니깐, 우주 플리즈 꺼져 줄래?. '_'", L"ㅗ", MB_OK );
			break;

		case STOC_JOIN_MEMBERSHIP_OK:
			// 회원 가입 성공

			MessageBox( NULL, L"회원 가입을 축하드립니다. '_'", L"ㄳ", MB_OK );
			break;

		case STOC_JOIN_MEMBERSHIP_FAIL:	
			// 회원 가입 실패

			MessageBox( NULL, L"이미 존재하는 아이디 입니다.", L"ㅗ", MB_OK );
			break;
	}

	return TRUE;
}


void	NetLobbyClient::GetJoinerName( CString &strJoiner, BYTE nIndex )
{
	if( NULL != &m_GamePlayer[ nIndex ] )
		return;

	strJoiner = m_GamePlayer[ nIndex ].m_wstrID;
}