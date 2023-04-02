#ifndef NETLOBBYSERVER_H
#define NETLOBBYSERVER_H

#include "NetSystem.h"
#include "MapManager.h"

#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <mysql.h>

#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "ws2_32.lib")

//-----------------------------------
//
//				DB
//
#define		MYSQL_HOST		"localhost"
#define		MYSQL_USER		"root"
#define		MYSQL_PWD		"autoset"
#define		MYSQL_DB		"animalfarm"

#define		MEMBER_TABLE	"member"

//--------------------------------------------------------------------------------
//	table List
//
//	member		- 가입 회원 정보 저장
//		- Column List | ID VARCHAR(25)| PW VARCHAR(25) | Name VARCHAR(25) |
//
//--------------------------------------------------------------------------------


//-----------------------------------
//
//		MFC 사용자 정의 Message
//
#define		UM_UPDATEDATA	(WM_USER + 11)
//-----------------------------------


#define		USER_ID_LEN		25
#define		ROOM_NAME_LEN	30
#define		MAX_ROOM_PLAYER	6


//-----------------------------------------------------------
//
//			Player 1명에 대한 정보를 표현한 구조체
//
//-----------------------------------------------------------

struct PlayerInfo{
	
//	SocketInfo	*m_pSocketInfo;				// 소켓 주소 정보
	SOCKET		m_Sock;
	SOCKADDR_IN	m_SockAddr;
	wchar_t		m_wstrID[ USER_ID_LEN ];	// ID
	BYTE		m_nPlayerNo;				// Client 번호
	BYTE		m_nRoomNo;					// 몇번 방에 있는지 여부
	BYTE		m_nPosAtRoom;				// 방에서의 위치 0 ~ 5

	PlayerInfo(){

		m_Sock			= INVALID_SOCKET;
		::ZeroMemory( &m_SockAddr, sizeof(m_SockAddr) );
		::ZeroMemory( m_wstrID, sizeof(m_wstrID) );
		m_nPlayerNo		= 1;
		m_nRoomNo		= 0;
		m_nPosAtRoom	= MAX_ROOM_PLAYER;
		//		m_pSocketInfo	= NULL;

}

	BOOL operator == ( const PlayerInfo& _playerInfo ) const { return this->m_nPlayerNo == _playerInfo.m_nPlayerNo; }
	BOOL operator >  ( const PlayerInfo& _playerInfo ) const { return this->m_nPlayerNo >  _playerInfo.m_nPlayerNo; }
	BOOL operator <  ( const PlayerInfo& _playerInfo ) const { return this->m_nPlayerNo <  _playerInfo.m_nPlayerNo; }
	BOOL operator <= ( const PlayerInfo& _playerInfo ) const { return this->m_nPlayerNo <= _playerInfo.m_nPlayerNo; }
	BOOL operator >= ( const PlayerInfo& _playerInfo ) const { return this->m_nPlayerNo >= _playerInfo.m_nPlayerNo; }
};

//-----------------------------------------------------------
//
//			방 1개에 대한 정보를 표한 구조체 
//
//-----------------------------------------------------------

//------- 방 상태 정보 --------
typedef enum _RoomState
{
	ROOM_STAND = 0,		//  대기 상태
	ROOM_FULL,			//  정원(6명)이 다 찬 상태
	ROOM_RUNNING_GAME,	//	게임 시작 상태
	ROOM_DEFAULT		//	초기화

}RoomState;


typedef enum _UserInRoomState{
	
	STATE_DEFAULT	= 1,	// 기본 상태
	STATE_READY		= 2,	// 레디 상태
	STATE_GAMMING	= 4,	// 게임중 상태
	STATE_CAPTAIN	= 8		// 방장

}UserInRoomState;


//---------방 구조체-----------
struct RoomInfo{
		
	PlayerInfo		*m_pJoiner[ MAX_ROOM_PLAYER ];		//방 접속자 여부
	wchar_t			m_wstrRoomName[ ROOM_NAME_LEN ];	//방 이름	
	BYTE			m_nCaptain;							//방장 인덱스
	BYTE			m_nRoomNo;							//방 번호
	BYTE			m_nJoiners;							//방 참여인원
	BYTE			m_nMaxJoiners;						//방 Max 참여인원
	BYTE			m_nMapInfo;							//방 플레이 될 맵 인덱스
	RoomState		m_bRoomState;						//방 상태
	UserInRoomState	m_nJoinerState[	MAX_ROOM_PLAYER ];	//방 접속자 상태		

	RoomInfo(){

		::ZeroMemory( m_pJoiner		, sizeof(m_pJoiner)			);
		::ZeroMemory( m_wstrRoomName, sizeof(m_wstrRoomName)	);
		m_nCaptain			= 0;
		m_nMapInfo			= 0;
		m_nRoomNo			= 1;
		m_nJoiners			= 1;
		m_bRoomState		= ROOM_STAND;
		m_nJoinerState[ 0 ]	= STATE_CAPTAIN;
	}

	BOOL operator == ( const RoomInfo &_RoomInfo ) const { return this->m_nRoomNo == _RoomInfo.m_nRoomNo; }
	BOOL operator >	 ( const RoomInfo &_RoomInfo ) const { return this->m_nRoomNo >  _RoomInfo.m_nRoomNo; }
	BOOL operator <	 ( const RoomInfo &_RoomInfo ) const { return this->m_nRoomNo <  _RoomInfo.m_nRoomNo; }
	BOOL operator <= ( const RoomInfo &_RoomInfo ) const { return this->m_nRoomNo <= _RoomInfo.m_nRoomNo; }
	BOOL operator >= ( const RoomInfo &_RoomInfo ) const { return this->m_nRoomNo >= _RoomInfo.m_nRoomNo; }

};


class NetLobbyServer : public NetSystem
{

	enum{

		//	-----------------------------------------------------------		
		//	Client - > Server
		//	-----------------------------------------------------------		

		CTOS_CONNECT,			// 서버 접속
		CTOS_DISCONNECT, 		// 서버 접속 해제
		CTOS_ROOM_LIST,			// 방 리스트 정보 송신 요청
		CTOS_PLAYER_LIST,		// 접속자 리스트 정보 송신 요청
		CTOS_CREATE_ROOM,		// 방 생성 요청
		CTOS_ENTER_ROOM,		// 방 입장 요청
		CTOS_OUT_ROOM,			// 방 나가기 요청
		CTOS_CHAT_IN_LOBBY,		// 로비 채팅 메시지 송신
		CTOS_CHAT_IN_ROOM,		// 방 채팅 메시지 송신
		CTOS_CAPTAIN_GAME_START,// 방장 게임 시작 요청 
		CTOS_LOGIN,				// 로그인 요청
		CTOS_JOIN_MEMBERSHIP,	// 회원가입 요청
		CTOS_GAME_READY,		// 게임 준비 요청
		CTOS_GAME_START,		// 게임 시작 요청
		CTOS_GAME_MAP_CHANGE,	// 방에서 맵 교체
		CTOS_GAME_INVITATION,	// 방에서 초대 메시지 송신 요청
		CTOS_GAME_FORCE_OUT_ROOM,	// 유저 1명 강퇴시키기
		CTOS_GAME_GIVE_CAPTAIN,	// 방장 위임 요청
		CTOS_GAME_JOINER_INFO,	// 유저 1명 정보 송신 요청


		//	-----------------------------------------------------------		
		//	Server - > Client
		//	-----------------------------------------------------------

		STOC_JOIN_MEMBERSHIP_FAIL,	// 회원가입 실패( 아이디 존재 )
		STOC_JOIN_MEMBERSHIP_OK,	// 회원가입 성공
		STOC_LOGIN_OK,				// 서버 접속 허용
		STOC_ADD_ROOMLIST, 			// 방 리스트 정보 송신
		STOC_ADD_PlAYERLIST, 		// 접속자 리스트 정보 송신
		STOC_ROOMLIST_SEND_OK,		// 방 리스트 정보 송신 완료
		STOC_PLAYERLIST_SEND_OK,	// 방 리스트 정보 송신 완료
		STOC_ADD_ROOM,				// 로비 방 추가 송신
		STOC_ADD_PLAYER,			// 접속자 추가 송신
		STOC_ENTER_ROOM,			// 방 입장 허가 와 방에 있던 기존 유저 정보 송신
		STOC_ROOM_CREATE_OK,		// 방 생성 허용 송신
		STOC_OUT_ROOM,				// 방 나가기 허용
		STOC_CHAT_IN_ROOM,			// 방 채팅 메시지 송신
		STOC_CHAT_IN_LOBBY,			// 로비 채팅 메시지 송신
		STOC_REMOVE_ROOM,			// 로비 방 제거 송신		
		STOC_REMOVE_PLAYER_IN_LOBBY,// 접속자 제거 송신
		STOC_REMOVE_PLAYER_IN_ROOM, // 접속자 제거 송신
		STOC_UPDATE_ROOMSTATE_IN_LOBBY,// 룸의 현재 상태를 갱신 송신
		STOC_NOT_ENTER_FOOL,		// 방이 정원일때 메시지 송신
		STOC_NOT_ENTER_GAMMING,		// 방이 게임중일때 메시지 송신
		STOC_JOINROOM_PLAYERINFO,	// 방에 새로운 유저에 대한 정보 송신		
		STOC_JOINER_READY_STATE,	// 방에 접속한 유저의 레디 상태 정보 갱신 송신
		STOC_GAME_MAP_CHANGE,		// 방에서 맵 교체
		STOC_GAME_CAPTAIN_CHANGE,	// 방장 변경 메세지 송신
		STOC_GAME_MAP_CHANGE_IN_LOBBY,// 방에서 맵 교체정보 로비 대기자들에게 갱신 송신		
		STOC_GAME_JOINER_ADDR_LIST,	// 방에 있장한 일반 유저 주소 정보를 방장에게 송신
		STOC_GAME_CAPTAIN_ADDR,		// 방에 입장하는 각각의 유저에게 방정 주소 정보 송신
		STOC_GAME_INVITATION,		// 로비에 있는 유저들에게 초대 메시지 송신
		STOC_GAME_INVITATION_OK,	// 방장에게 초대 메시지 송신 여부 송신
		STOC_MANAGER_MESSAGE,		// 모든 서버의 유저들에게 운영자 메시지 송신
		STOC_GAME_GIVE_CAPTAIN,		// 방장 위임 정보 방안 유저들에게 송신
		STOC_GAME_JOINER_INFO		// 유저 1명 정보 송신
		

	};

	HWND						m_hDlg;					// 다이얼로그의 핸들(메세지전달)
	
	MYSQL						m_dbMysql;				// Mysql DB

	CRITICAL_SECTION			m_csListRooms;
	CRITICAL_SECTION			m_csListPlayersInLobby;
	CRITICAL_SECTION			m_csListPlayersInRoom;

	std::list< RoomInfo >		m_listRooms;
	std::list< PlayerInfo >		m_listPlayersInLobby;
	std::list< PlayerInfo >		m_listPlayersInRoom;

public:

	NetLobbyServer();
	~NetLobbyServer();

public:

	virtual BOOL Begin();		// 서버 시작
	virtual BOOL End();			// 서버 종료

	void	SetDlgHandle( HWND hDlg ){	m_hDlg = hDlg;	}
	int		GetNumOfPlayers()	{ return static_cast< int >( m_listPlayersInLobby.size() + m_listPlayersInRoom.size() );	 }
	int		GetNumOfRooms()		{ return static_cast< int >( m_listRooms.size() );		}

	//--------- Packet Analysis & Handling -----------

	//	Recv 되어온 패킷을 분석 그에 맞게 처리하는 함수
	void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//	접속한 소켓정보를 이용하여 자료구조에 할당
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//	접속 종료한 소켓정보를 자료구조로부터 제거
	virtual void ProcessDisConnect( SocketInfo *pSocketInfo );

	void	SendMsgOfManager( CString &msg );


private:

	//-------------------------------------------------------------------------
	//
	//								Recv Functions
	//
	//	Desc : Client로 부터 요청 메시지를 받아서 처리하는 함수 모음
	//-------------------------------------------------------------------------

	//		클라이언트로부터 접속 메시지 수신 처리
	void	JoinMember	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		클라이언트로부터 접속 메시지 수신 처리
	void	Login		( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트에게 로비서버 생성된 방 리스트 정보 송신
	void	RoomList	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트에게 로비서버 접속한 Client 리스트 정보 송신
	void	PlayList	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트의 방 생성 요청을 수신하여 방 생성 처리-	
	void	CreateRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트의 방 입장 요청을 수신하여 입장 처리
	void	JoinRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트 방 나가기 요청을 수신하여 방 나가기 처리
	void	OutRoom		( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		방장이 지정한 유저 1명 강퇴시키기 처리
	void	OutRoomByCaptain( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트 chat 정보를 Lobby에 있는 각각의 클라이언트들에게 전송
	void	ChatInLobby	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트의 chat 정보를 현재 클라이언트의 방안에 접속한 각각의 클라이언트들에게 전송
	void	ChatInRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		해당 클라이언트의 접속 종료요청을 수신 처리
	void	Disconnect	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		방안에 있는 유저의 게임 레디 요청 상태를 수신 처리
	void	GameReady	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		방안에 있는 유저의 맵 정보를 갱신 처리
	void	MapChange	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		동물농장 Game Process를 생성하여 각각의 클라이언트 정보를 해당 클라이언트(방장)에게 전송
	void	GameStart	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		로비에 있는 유저들에게 초대 메시지 송신 처리
	void	Invitation	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		방장 위임 요청을 수신 처리
	void	GiveCaptain	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		방에 있는 유저 1명의 정보 송신
	void	JoinerInfoAtRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

};


#endif