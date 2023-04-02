#ifndef NETLOBBYCLIENT_H
#define NETLOBBYCLIENT_H

#include "NetSystem.h"


#include "PlayerList.h"
#include "JoinerList.h"
#include "RoomList.h"
#include "JoinerList.h"

#include "MapManager.h"

#include <process.h>

#include <algorithm>
#include <functional>
#include <list>
#include <string>

#define		USER_ID_LEN			25
#define		ROOM_NAME_LEN		30
#define		ROOM_MAX_PLAYER		6

//			Spawn시 Arg 갯수
//#define		_SPAWN_AGR1

const std::string	g_strGameApp("동물농장PLUS.exe");
//const std::string	g_strGameAppPath("../../Game/동물농장PLUS.exe");
const std::string	g_strGameAppPath("TestProcess.exe");

//-----------------------------------------------------------
//
//			Player 1명에 대한 정보를 표현한 구조체
//
//-----------------------------------------------------------
struct PlayerInfo{
	
	SocketInfo	*m_pSocketInfo;				// 소켓 주소 정보
	wchar_t		m_wstrID[ USER_ID_LEN ];	// ID
	BYTE		m_nPlayerNo;				// Client 번호
	BYTE		m_nRoomNo;					// 몇번 방에 있는지 여부

	BYTE		m_nListNo;					// UI 리스트에서 Row Number

	PlayerInfo(){

		m_pSocketInfo	= NULL;
		::ZeroMemory( m_wstrID, sizeof(m_wstrID) );
		m_nRoomNo		= 0;
		m_nPlayerNo		= 0;				// Client 번호
		m_nListNo		= 0;

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

}RoomState;

//---------방 구조체-----------
struct RoomInfo{
		
	SocketInfo	*m_pJoiner[ 6 ];					//방 접속자 여부
	wchar_t		m_wstrRoomName[ ROOM_NAME_LEN ];	//방 이름
	BYTE		m_nCaptain;							//방장 인덱스
	BYTE		m_nRoomNo;							//방 번호
	BYTE		m_nJoiners;							//방 참여인원
	BYTE		m_nMaxJoiners;						//방 Max 참여인원
	BYTE		m_nMapInfo;							//방 플레이 될 맵 인덱스
	RoomState	m_nRoomState;						//방 상태


	BYTE		m_nListNo;							//UI 리스트에서 Row Number

	RoomInfo(){

		::ZeroMemory( m_pJoiner		, sizeof(m_pJoiner)			);
		::ZeroMemory( m_wstrRoomName, sizeof(m_wstrRoomName)	);
		m_nCaptain		= 0;
		m_nRoomNo		= 0;
		m_nJoiners		= 0;
		m_nListNo		= 0;
		m_nMapInfo		= 0;
		m_nRoomState	= ROOM_STAND;
	}

	BOOL operator == ( const RoomInfo& _RoomInfo ) const { return this->m_nRoomNo == _RoomInfo.m_nRoomNo; }
	BOOL operator >  ( const RoomInfo& _RoomInfo ) const { return this->m_nRoomNo >  _RoomInfo.m_nRoomNo; }
	BOOL operator <  ( const RoomInfo& _RoomInfo ) const { return this->m_nRoomNo <  _RoomInfo.m_nRoomNo; }
	BOOL operator <= ( const RoomInfo& _RoomInfo ) const { return this->m_nRoomNo <= _RoomInfo.m_nRoomNo; }
	BOOL operator >= ( const RoomInfo& _RoomInfo ) const { return this->m_nRoomNo >= _RoomInfo.m_nRoomNo; }

};


//---------방안 상태-----------

typedef enum _UserInRoomState{
	
	STATE_DEFAULT	= 1,	// 기본 상태
	STATE_READY		= 2,	// 레디 상태
	STATE_GAMMING	= 4,	// 게임중 상태
	STATE_CAPTAIN	= 8		// 방장

}UserInRoomState;



struct UserInRoomInfo {

	static BYTE			m_nJoiner;
	wchar_t				m_wstrID[ USER_ID_LEN ];	// ID 
	UserInRoomState		m_MyState;
};


class NetLobbyClient : public NetSystem
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
		CTOS_GAME_INVITATION,	// 방에서 로비 유저들에게 초대 메시지 보내기
		CTOS_GAME_FORCE_OUT_ROOM,// 유저 1명 강퇴시키기
		CTOS_GAME_GIVE_CAPTAIN,	// 방장 위임 요청
		CTOS_GAME_JOINER_INFO,	// 유저 1명 정보 송신 요청


		//	-----------------------------------------------------------		
		//	Server - > Client
		//	-----------------------------------------------------------

		STOC_JOIN_MEMBERSHIP_FAIL,	// 회원가입 실패( 아이디 존재 )
		STOC_JOIN_MEMBERSHIP_OK,	// 회원가입 성공
		STOC_LOGIN_OK,				// 서버 접속 허용
		STOC_ADD_ROOMLIST, 			// 방 리스트 정보 송신
		STOC_ADD_PlAYERLIST,		// 접속자 리스트 정보 송신
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
	
	HWND					m_hWnd;								// 윈도우 핸들
	
	BYTE					m_PlayerNo;							// 내 고유 번호
	BYTE					m_RoomNo;							// 접속한 방 번호
	BYTE					m_nCntRoom;							// CListView에 표시될 Index
	BYTE					m_nPosInRoom;						// 룸 안에서의 나의 위치
	BOOL					m_bCaptain;							// 방장 여부
	BOOL					m_bReady;							// 준비완료 여부

	CString					m_strMyGameID;						// 내 아이디
	CString					m_strConnRoomName;					// 접속한 방 이름

	UserInRoomInfo			m_GamePlayer[ ROOM_MAX_PLAYER ];	// 방 안 접속 유저 상태

	CRITICAL_SECTION		m_csListRooms;						// m_listRooms		접근 임계영역
	CRITICAL_SECTION		m_csListPlayers;					// m_listPlayers	접근 임계영역
	CRITICAL_SECTION		m_csJoinerInRoom;					// m_listChat		접근 임계영역

	std::list< RoomInfo >	m_listRooms;						// 방 리스트
	std::list< PlayerInfo >	m_listPlayers;						// 유저 리스트
	std::list< CString >	m_listChat;							// 방 참여자 리스트

public:

	NetLobbyClient();
	~NetLobbyClient();

public:

	//--------- Packet Analysis & Handling -----------

	//		Recv 되어온 패킷을 분석 그에 맞게 처리하는 함수
	virtual void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//		접속한 소켓정보를 이용하여 자료구조에 할당
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//		접속 종료한 소켓정보를 자료구조로부터 제거
	virtual void ProcessDisConnect( SocketInfo *pSocketInfo );

	virtual BOOL	Begin();
	virtual	BOOL	End();

	BOOL	Release();

	BOOL	ChangeDisplay( UINT nID );

	CString	GetMyGameID()		{ return m_strMyGameID;			}
	CString GetConnRoomName()	{ return m_strConnRoomName;		}
	HWND	GetMyHwnd()			{ return m_hWnd;				}
	BOOL	IsCaptain()			{ return m_bCaptain;			}
	BOOL	IsReady()			{ return m_bReady;				}
	BOOL&	GetReadyState()		{ return m_bReady;				}
	BYTE	GetPosInRoom()		{ return m_nPosInRoom;			}
	void	GetJoinerName( CString &strJoiner, BYTE nIndex );	


	//-------------------------------------------------------------------------
	//
	//							MFC UI Update Functions
	//
	//	Desc : MFC의 CListView, CFromView 등의 UI System의 Update 관련 함수 모음
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	//		로비 인터페이스
	//-------------------------------------------------------------------------

	//		유저 리스트를 이용하여 ListView에 각 튜플값 갱신
	BOOL	UpdatePlayerListInLobby		( PlayerList *pPlayerList = NULL );

	//		방 리스트를 이용하여 ListView에 각 튜플값 갱신
	BOOL	UpdateRoomListInLobby		( RoomList *pRoomList = NULL );

	//		방 리스트에 방 1개 추가 갱신
	BOOL	UpdateOneRoomListInLobby	( BYTE nRoomNo, RoomList *pRoomList = NULL );

	//		채팅 메시지 CEdit Controller에 추가
	void	UpdateChat					( CEdit &pEdit );

	//		접속한 새로운 유저 1명을 추가한다.
	BOOL	UpdateOnePlayerListInLobby	( BYTE nPlayerNo, PlayerList *pPlayerList = NULL );

	//		방 1개에 대한 상태 정보를 갱신
	void	UpdateRoomStateInLobby		( const RoomInfo *pRoomInfo, RoomList *pRoomList );

	//		방 안에 있는 모든 유저의 리스트 정보를 갱신
	void	UpdateAllJoinerList			( JoinerList *pJoinerList );


	//		접속한 유저중 1명의 Disconnect 했을때 List 상태 갱신
//	void	DeletePlayerInLobby			( BYTE nPlayerNo, PlayerList *pPlayerList = NULL );

	//-------------------------------------------------------------------------
	//		룸 인터페이스
	//-------------------------------------------------------------------------

	//		방안에 접속한 1명을 대해서 추가 갱신
	BOOL	UpdateOneJoinerListInRoom		( BYTE nJoinerNo, JoinerList *pJoinerList = NULL );

	//		방에 들어가서 기존 유저정보를 포함하여 추가 갱신
	BOOL	UpdateJoinerListInRoom			( JoinerList *pJoinerList = NULL );

	//		방에 나간 1명을 리스트에서 제거 갱신
	BOOL	UpdateDeleteJoinerInRoom		( BYTE nPosInRoom, JoinerList *pJoinerList = NULL );

	//		방 참여인원 1명의 상태 갱신
	void	UpdateJoinerListState			( BYTE nPosInRoom, JoinerList *pJoinerList = NULL );




	//-------------------------------------------------------------------------
	//
	//								Request Functions
	//
	//	Desc : Client가 Server 에게 요청 메시지 전송하는 함수 
	//-------------------------------------------------------------------------

	//		로비서버 회원가입 요청
	BOOL	RequestJoinMemeber( CString &ID, CString &PW, CString &GameID );

	//		로비서버 접속 요청
	BOOL	RequestLogin( CString &IP, CString &ID, CString &PW );

	//		로비서버의 개설된 방 리스트 요청
	BOOL	RequestRoomList();

	//		로비서버의 접속한 클라이언트 리스트 요청
	BOOL	RequestPlayerList();

	//		선택한 방 번호를 이용하여 방 입장 요청
	BOOL	RequestJoinRoom( BYTE nRoomNo );

	//		로비에서 Chat Message 송신
	BOOL	RequestChatInLobby( CString &strSendMsg );

	//		방안에서 Chat Message 송신
	BOOL	RequestChatInRoom( CString &strSendMsg);

	//		방안에서 나가기 요청
	BOOL	RequestOutRoom();

	//		방장이 방안 유저 1명을 강퇴시킴 요청
	BOOL	RequesCompulsiontOutRoom( BYTE nPosInRoom );

	//		방 만들기 요청
	BOOL	RequestCreateRoom( CString &strRoomName, BYTE nMaxPlayer );

	//		게임 준비 요청
	BOOL	RequestReadyGame();

	//		게임 시작 요청
	BOOL	RequestStartGame();

	//		방장이 맵을 교체 요청
	BOOL	RequestMapChange( BYTE nMapIndex );

	//		초대 메시지 송신 요청
	BOOL	RequestInvitation();

	//		방장 위임 요청
	BOOL	RequestGiveCaptain( BYTE nPosInRoom );

	//		방안 유저 1명 정보 송신 요청
	BOOL	RequestJoinerInfo( BYTE nPosInRoom );



private:

	//-------------------------------------------------------------------------
	//
	//							Recv Functions
	//
	//	Desc : Server로 부터 메시지 수신하여 처리하는 함수 모음
	//-------------------------------------------------------------------------

	//-		방 1개 리스트 추가
	BOOL	AddRoom					( const char *pBuf, int nLen );

	//-		유저 1명 리스트 추가
	BOOL	AddPlayer				( const char *pBuf, int nLen );

	//-		방 n개 리스트 추가
	BOOL	AddRoomList				( const char *pBuf, int nLen );

	//		접속자 1명 리스트 추가
	BOOL	AddPlayerList			( const char *pBuf, int nLen );

	//		방 1개 리스트에서 제거
	BOOL	DeleteRoom				( const char *pBuf, int nLen );

	//		접속자 1명 리스트에서 제거
	BOOL	DeletePlayer			( const char *pBuf, int nLen );

	//		방안에서 나간 접속자 1명 리스트에서 제거
	BOOL	DeleteJoinerAtRoom		( const char *pBuf, int nLen );

	//		방 접속
	BOOL	JoinRoom				( const char *pBuf, int nLen );

	//		방 나가기
	BOOL	OutRoom					( const char *pBuf, int nLen );

	//		방 만들기
	BOOL	CreateRoom				( const char *pBuf, int nLen );

	//		방 상태 정보 갱싱
	BOOL	UpdateRoomInfo			( const char *pBuf, int nLen );

	//		방에 있는 유저의 상태 정보 갱신
	BOOL	UpdateJoinerState		( const char *pBuf, int nLen );

	//		새로 방에 들어온 유정 정보 추가
	BOOL	AddJoinerInRoom			( const char *pBuf, int nLen );

	//		로비 채팅 메시지 추가
	BOOL	AddChatMessageAtLobby	( const char *pBuf, int nLen );

	//		방 채팅 메시지 추가
	BOOL	AddChatMessageAtRoom	( const char *pBuf, int nLen );

	//		로그인 성공후 고유번호와 게임아이디 얻음
	BOOL	LoginOK					( const char *pBuf, int nLen );

	//		맵 정보 변경 갱신
	BOOL	MapInfoChange			( const char *pBuf, int nLen ); 

	//		맵 정보 변경 갱신
	BOOL	MapInfoChangeAtLobby	( const char *pBuf, int nLen ); 

	//		방장 변경 메시지 수신 갱신
	BOOL	ChangeCaptain			( const char *pBuf, int nLen );

	//		유저 주소정보를 전송 받은 후 게임 시작
	BOOL	GameStartCaptain		( const char *pBuf, int nLen );				

	//		방장 정보를 수신하여 방장에 접속
	BOOL	GameStartJoiner			( const char *pBuf, int nLen );

	//		초대 메시지 수신
	BOOL	Invitation				( const char *pBuf, int nLen );

	//		방장 초대 메시지 결과 수신
	BOOL	InvitationResult		( const char *pBuf, int nLen );

	//		방장 변경 메시지 수신 갱신
	BOOL	GiveCaptain				( const char *pBuf, int nLen );

	//		방안 유저 정보 수신 출력
	BOOL	ShowJoinerInfo			( const char *pBuf, int nLen );

	//		각종 이벤트 Message 창 출력
	BOOL	ShowInfoMessage			( BYTE msgType );

	
	


};


#endif