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

//			Spawn�� Arg ����
//#define		_SPAWN_AGR1

const std::string	g_strGameApp("��������PLUS.exe");
//const std::string	g_strGameAppPath("../../Game/��������PLUS.exe");
const std::string	g_strGameAppPath("TestProcess.exe");

//-----------------------------------------------------------
//
//			Player 1�� ���� ������ ǥ���� ����ü
//
//-----------------------------------------------------------
struct PlayerInfo{
	
	SocketInfo	*m_pSocketInfo;				// ���� �ּ� ����
	wchar_t		m_wstrID[ USER_ID_LEN ];	// ID
	BYTE		m_nPlayerNo;				// Client ��ȣ
	BYTE		m_nRoomNo;					// ��� �濡 �ִ��� ����

	BYTE		m_nListNo;					// UI ����Ʈ���� Row Number

	PlayerInfo(){

		m_pSocketInfo	= NULL;
		::ZeroMemory( m_wstrID, sizeof(m_wstrID) );
		m_nRoomNo		= 0;
		m_nPlayerNo		= 0;				// Client ��ȣ
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
//			�� 1���� ���� ������ ǥ�� ����ü 
//
//-----------------------------------------------------------

//------- �� ���� ���� --------
typedef enum _RoomState
{
	ROOM_STAND = 0,		//  ��� ����
	ROOM_FULL,			//  ����(6��)�� �� �� ����
	ROOM_RUNNING_GAME,	//	���� ���� ����

}RoomState;

//---------�� ����ü-----------
struct RoomInfo{
		
	SocketInfo	*m_pJoiner[ 6 ];					//�� ������ ����
	wchar_t		m_wstrRoomName[ ROOM_NAME_LEN ];	//�� �̸�
	BYTE		m_nCaptain;							//���� �ε���
	BYTE		m_nRoomNo;							//�� ��ȣ
	BYTE		m_nJoiners;							//�� �����ο�
	BYTE		m_nMaxJoiners;						//�� Max �����ο�
	BYTE		m_nMapInfo;							//�� �÷��� �� �� �ε���
	RoomState	m_nRoomState;						//�� ����


	BYTE		m_nListNo;							//UI ����Ʈ���� Row Number

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


//---------��� ����-----------

typedef enum _UserInRoomState{
	
	STATE_DEFAULT	= 1,	// �⺻ ����
	STATE_READY		= 2,	// ���� ����
	STATE_GAMMING	= 4,	// ������ ����
	STATE_CAPTAIN	= 8		// ����

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

		CTOS_CONNECT,			// ���� ����
		CTOS_DISCONNECT, 		// ���� ���� ����
		CTOS_ROOM_LIST,			// �� ����Ʈ ���� �۽� ��û
		CTOS_PLAYER_LIST,		// ������ ����Ʈ ���� �۽� ��û
		CTOS_CREATE_ROOM,		// �� ���� ��û
		CTOS_ENTER_ROOM,		// �� ���� ��û
		CTOS_OUT_ROOM,			// �� ������ ��û
		CTOS_CHAT_IN_LOBBY,		// �κ� ä�� �޽��� �۽�
		CTOS_CHAT_IN_ROOM,		// �� ä�� �޽��� �۽�
		CTOS_CAPTAIN_GAME_START,// ���� ���� ���� ��û 
		CTOS_LOGIN,				// �α��� ��û
		CTOS_JOIN_MEMBERSHIP,	// ȸ������ ��û
		CTOS_GAME_READY,		// ���� �غ� ��û
		CTOS_GAME_START,		// ���� ���� ��û
		CTOS_GAME_MAP_CHANGE,	// �濡�� �� ��ü
		CTOS_GAME_INVITATION,	// �濡�� �κ� �����鿡�� �ʴ� �޽��� ������
		CTOS_GAME_FORCE_OUT_ROOM,// ���� 1�� �����Ű��
		CTOS_GAME_GIVE_CAPTAIN,	// ���� ���� ��û
		CTOS_GAME_JOINER_INFO,	// ���� 1�� ���� �۽� ��û


		//	-----------------------------------------------------------		
		//	Server - > Client
		//	-----------------------------------------------------------

		STOC_JOIN_MEMBERSHIP_FAIL,	// ȸ������ ����( ���̵� ���� )
		STOC_JOIN_MEMBERSHIP_OK,	// ȸ������ ����
		STOC_LOGIN_OK,				// ���� ���� ���
		STOC_ADD_ROOMLIST, 			// �� ����Ʈ ���� �۽�
		STOC_ADD_PlAYERLIST,		// ������ ����Ʈ ���� �۽�
		STOC_ROOMLIST_SEND_OK,		// �� ����Ʈ ���� �۽� �Ϸ�
		STOC_PLAYERLIST_SEND_OK,	// �� ����Ʈ ���� �۽� �Ϸ�
		STOC_ADD_ROOM,				// �κ� �� �߰� �۽�
		STOC_ADD_PLAYER,			// ������ �߰� �۽�
		STOC_ENTER_ROOM,			// �� ���� �㰡 �� �濡 �ִ� ���� ���� ���� �۽�
		STOC_ROOM_CREATE_OK,		// �� ���� ��� �۽�
		STOC_OUT_ROOM,				// �� ������ ���
		STOC_CHAT_IN_ROOM,			// �� ä�� �޽��� �۽�
		STOC_CHAT_IN_LOBBY,			// �κ� ä�� �޽��� �۽�
		STOC_REMOVE_ROOM,			// �κ� �� ���� �۽�		
		STOC_REMOVE_PLAYER_IN_LOBBY,// ������ ���� �۽�
		STOC_REMOVE_PLAYER_IN_ROOM, // ������ ���� �۽�
		STOC_UPDATE_ROOMSTATE_IN_LOBBY,// ���� ���� ���¸� ���� �۽�
		STOC_NOT_ENTER_FOOL,		// ���� �����϶� �޽��� �۽�
		STOC_NOT_ENTER_GAMMING,		// ���� �������϶� �޽��� �۽�
		STOC_JOINROOM_PLAYERINFO,	// �濡 ���ο� ������ ���� ���� �۽�		
		STOC_JOINER_READY_STATE,	// �濡 ������ ������ ���� ���� ���� ���� �۽�
		STOC_GAME_MAP_CHANGE,		// �濡�� �� ��ü
		STOC_GAME_CAPTAIN_CHANGE,	// ���� ���� �޼��� �۽�
		STOC_GAME_MAP_CHANGE_IN_LOBBY,// �濡�� �� ��ü���� �κ� ����ڵ鿡�� ���� �۽�
		STOC_GAME_JOINER_ADDR_LIST,	// �濡 ������ �Ϲ� ���� �ּ� ������ ���忡�� �۽�
		STOC_GAME_CAPTAIN_ADDR,		// �濡 �����ϴ� ������ �������� ���� �ּ� ���� �۽�
		STOC_GAME_INVITATION,		// �κ� �ִ� �����鿡�� �ʴ� �޽��� �۽�
		STOC_GAME_INVITATION_OK,	// ���忡�� �ʴ� �޽��� �۽� ���� �۽�
		STOC_MANAGER_MESSAGE,		// ��� ������ �����鿡�� ��� �޽��� �۽�
		STOC_GAME_GIVE_CAPTAIN,		// ���� ���� ���� ��� �����鿡�� �۽�
		STOC_GAME_JOINER_INFO		// ���� 1�� ���� �۽�
		

	};
	
	HWND					m_hWnd;								// ������ �ڵ�
	
	BYTE					m_PlayerNo;							// �� ���� ��ȣ
	BYTE					m_RoomNo;							// ������ �� ��ȣ
	BYTE					m_nCntRoom;							// CListView�� ǥ�õ� Index
	BYTE					m_nPosInRoom;						// �� �ȿ����� ���� ��ġ
	BOOL					m_bCaptain;							// ���� ����
	BOOL					m_bReady;							// �غ�Ϸ� ����

	CString					m_strMyGameID;						// �� ���̵�
	CString					m_strConnRoomName;					// ������ �� �̸�

	UserInRoomInfo			m_GamePlayer[ ROOM_MAX_PLAYER ];	// �� �� ���� ���� ����

	CRITICAL_SECTION		m_csListRooms;						// m_listRooms		���� �Ӱ迵��
	CRITICAL_SECTION		m_csListPlayers;					// m_listPlayers	���� �Ӱ迵��
	CRITICAL_SECTION		m_csJoinerInRoom;					// m_listChat		���� �Ӱ迵��

	std::list< RoomInfo >	m_listRooms;						// �� ����Ʈ
	std::list< PlayerInfo >	m_listPlayers;						// ���� ����Ʈ
	std::list< CString >	m_listChat;							// �� ������ ����Ʈ

public:

	NetLobbyClient();
	~NetLobbyClient();

public:

	//--------- Packet Analysis & Handling -----------

	//		Recv �Ǿ�� ��Ŷ�� �м� �׿� �°� ó���ϴ� �Լ�
	virtual void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//		������ ���������� �̿��Ͽ� �ڷᱸ���� �Ҵ�
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//		���� ������ ���������� �ڷᱸ���κ��� ����
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
	//	Desc : MFC�� CListView, CFromView ���� UI System�� Update ���� �Լ� ����
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	//		�κ� �������̽�
	//-------------------------------------------------------------------------

	//		���� ����Ʈ�� �̿��Ͽ� ListView�� �� Ʃ�ð� ����
	BOOL	UpdatePlayerListInLobby		( PlayerList *pPlayerList = NULL );

	//		�� ����Ʈ�� �̿��Ͽ� ListView�� �� Ʃ�ð� ����
	BOOL	UpdateRoomListInLobby		( RoomList *pRoomList = NULL );

	//		�� ����Ʈ�� �� 1�� �߰� ����
	BOOL	UpdateOneRoomListInLobby	( BYTE nRoomNo, RoomList *pRoomList = NULL );

	//		ä�� �޽��� CEdit Controller�� �߰�
	void	UpdateChat					( CEdit &pEdit );

	//		������ ���ο� ���� 1���� �߰��Ѵ�.
	BOOL	UpdateOnePlayerListInLobby	( BYTE nPlayerNo, PlayerList *pPlayerList = NULL );

	//		�� 1���� ���� ���� ������ ����
	void	UpdateRoomStateInLobby		( const RoomInfo *pRoomInfo, RoomList *pRoomList );

	//		�� �ȿ� �ִ� ��� ������ ����Ʈ ������ ����
	void	UpdateAllJoinerList			( JoinerList *pJoinerList );


	//		������ ������ 1���� Disconnect ������ List ���� ����
//	void	DeletePlayerInLobby			( BYTE nPlayerNo, PlayerList *pPlayerList = NULL );

	//-------------------------------------------------------------------------
	//		�� �������̽�
	//-------------------------------------------------------------------------

	//		��ȿ� ������ 1���� ���ؼ� �߰� ����
	BOOL	UpdateOneJoinerListInRoom		( BYTE nJoinerNo, JoinerList *pJoinerList = NULL );

	//		�濡 ���� ���� ���������� �����Ͽ� �߰� ����
	BOOL	UpdateJoinerListInRoom			( JoinerList *pJoinerList = NULL );

	//		�濡 ���� 1���� ����Ʈ���� ���� ����
	BOOL	UpdateDeleteJoinerInRoom		( BYTE nPosInRoom, JoinerList *pJoinerList = NULL );

	//		�� �����ο� 1���� ���� ����
	void	UpdateJoinerListState			( BYTE nPosInRoom, JoinerList *pJoinerList = NULL );




	//-------------------------------------------------------------------------
	//
	//								Request Functions
	//
	//	Desc : Client�� Server ���� ��û �޽��� �����ϴ� �Լ� 
	//-------------------------------------------------------------------------

	//		�κ񼭹� ȸ������ ��û
	BOOL	RequestJoinMemeber( CString &ID, CString &PW, CString &GameID );

	//		�κ񼭹� ���� ��û
	BOOL	RequestLogin( CString &IP, CString &ID, CString &PW );

	//		�κ񼭹��� ������ �� ����Ʈ ��û
	BOOL	RequestRoomList();

	//		�κ񼭹��� ������ Ŭ���̾�Ʈ ����Ʈ ��û
	BOOL	RequestPlayerList();

	//		������ �� ��ȣ�� �̿��Ͽ� �� ���� ��û
	BOOL	RequestJoinRoom( BYTE nRoomNo );

	//		�κ񿡼� Chat Message �۽�
	BOOL	RequestChatInLobby( CString &strSendMsg );

	//		��ȿ��� Chat Message �۽�
	BOOL	RequestChatInRoom( CString &strSendMsg);

	//		��ȿ��� ������ ��û
	BOOL	RequestOutRoom();

	//		������ ��� ���� 1���� �����Ŵ ��û
	BOOL	RequesCompulsiontOutRoom( BYTE nPosInRoom );

	//		�� ����� ��û
	BOOL	RequestCreateRoom( CString &strRoomName, BYTE nMaxPlayer );

	//		���� �غ� ��û
	BOOL	RequestReadyGame();

	//		���� ���� ��û
	BOOL	RequestStartGame();

	//		������ ���� ��ü ��û
	BOOL	RequestMapChange( BYTE nMapIndex );

	//		�ʴ� �޽��� �۽� ��û
	BOOL	RequestInvitation();

	//		���� ���� ��û
	BOOL	RequestGiveCaptain( BYTE nPosInRoom );

	//		��� ���� 1�� ���� �۽� ��û
	BOOL	RequestJoinerInfo( BYTE nPosInRoom );



private:

	//-------------------------------------------------------------------------
	//
	//							Recv Functions
	//
	//	Desc : Server�� ���� �޽��� �����Ͽ� ó���ϴ� �Լ� ����
	//-------------------------------------------------------------------------

	//-		�� 1�� ����Ʈ �߰�
	BOOL	AddRoom					( const char *pBuf, int nLen );

	//-		���� 1�� ����Ʈ �߰�
	BOOL	AddPlayer				( const char *pBuf, int nLen );

	//-		�� n�� ����Ʈ �߰�
	BOOL	AddRoomList				( const char *pBuf, int nLen );

	//		������ 1�� ����Ʈ �߰�
	BOOL	AddPlayerList			( const char *pBuf, int nLen );

	//		�� 1�� ����Ʈ���� ����
	BOOL	DeleteRoom				( const char *pBuf, int nLen );

	//		������ 1�� ����Ʈ���� ����
	BOOL	DeletePlayer			( const char *pBuf, int nLen );

	//		��ȿ��� ���� ������ 1�� ����Ʈ���� ����
	BOOL	DeleteJoinerAtRoom		( const char *pBuf, int nLen );

	//		�� ����
	BOOL	JoinRoom				( const char *pBuf, int nLen );

	//		�� ������
	BOOL	OutRoom					( const char *pBuf, int nLen );

	//		�� �����
	BOOL	CreateRoom				( const char *pBuf, int nLen );

	//		�� ���� ���� ����
	BOOL	UpdateRoomInfo			( const char *pBuf, int nLen );

	//		�濡 �ִ� ������ ���� ���� ����
	BOOL	UpdateJoinerState		( const char *pBuf, int nLen );

	//		���� �濡 ���� ���� ���� �߰�
	BOOL	AddJoinerInRoom			( const char *pBuf, int nLen );

	//		�κ� ä�� �޽��� �߰�
	BOOL	AddChatMessageAtLobby	( const char *pBuf, int nLen );

	//		�� ä�� �޽��� �߰�
	BOOL	AddChatMessageAtRoom	( const char *pBuf, int nLen );

	//		�α��� ������ ������ȣ�� ���Ӿ��̵� ����
	BOOL	LoginOK					( const char *pBuf, int nLen );

	//		�� ���� ���� ����
	BOOL	MapInfoChange			( const char *pBuf, int nLen ); 

	//		�� ���� ���� ����
	BOOL	MapInfoChangeAtLobby	( const char *pBuf, int nLen ); 

	//		���� ���� �޽��� ���� ����
	BOOL	ChangeCaptain			( const char *pBuf, int nLen );

	//		���� �ּ������� ���� ���� �� ���� ����
	BOOL	GameStartCaptain		( const char *pBuf, int nLen );				

	//		���� ������ �����Ͽ� ���忡 ����
	BOOL	GameStartJoiner			( const char *pBuf, int nLen );

	//		�ʴ� �޽��� ����
	BOOL	Invitation				( const char *pBuf, int nLen );

	//		���� �ʴ� �޽��� ��� ����
	BOOL	InvitationResult		( const char *pBuf, int nLen );

	//		���� ���� �޽��� ���� ����
	BOOL	GiveCaptain				( const char *pBuf, int nLen );

	//		��� ���� ���� ���� ���
	BOOL	ShowJoinerInfo			( const char *pBuf, int nLen );

	//		���� �̺�Ʈ Message â ���
	BOOL	ShowInfoMessage			( BYTE msgType );

	
	


};


#endif