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
//	member		- ���� ȸ�� ���� ����
//		- Column List | ID VARCHAR(25)| PW VARCHAR(25) | Name VARCHAR(25) |
//
//--------------------------------------------------------------------------------


//-----------------------------------
//
//		MFC ����� ���� Message
//
#define		UM_UPDATEDATA	(WM_USER + 11)
//-----------------------------------


#define		USER_ID_LEN		25
#define		ROOM_NAME_LEN	30
#define		MAX_ROOM_PLAYER	6


//-----------------------------------------------------------
//
//			Player 1�� ���� ������ ǥ���� ����ü
//
//-----------------------------------------------------------

struct PlayerInfo{
	
//	SocketInfo	*m_pSocketInfo;				// ���� �ּ� ����
	SOCKET		m_Sock;
	SOCKADDR_IN	m_SockAddr;
	wchar_t		m_wstrID[ USER_ID_LEN ];	// ID
	BYTE		m_nPlayerNo;				// Client ��ȣ
	BYTE		m_nRoomNo;					// ��� �濡 �ִ��� ����
	BYTE		m_nPosAtRoom;				// �濡���� ��ġ 0 ~ 5

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
//			�� 1���� ���� ������ ǥ�� ����ü 
//
//-----------------------------------------------------------

//------- �� ���� ���� --------
typedef enum _RoomState
{
	ROOM_STAND = 0,		//  ��� ����
	ROOM_FULL,			//  ����(6��)�� �� �� ����
	ROOM_RUNNING_GAME,	//	���� ���� ����
	ROOM_DEFAULT		//	�ʱ�ȭ

}RoomState;


typedef enum _UserInRoomState{
	
	STATE_DEFAULT	= 1,	// �⺻ ����
	STATE_READY		= 2,	// ���� ����
	STATE_GAMMING	= 4,	// ������ ����
	STATE_CAPTAIN	= 8		// ����

}UserInRoomState;


//---------�� ����ü-----------
struct RoomInfo{
		
	PlayerInfo		*m_pJoiner[ MAX_ROOM_PLAYER ];		//�� ������ ����
	wchar_t			m_wstrRoomName[ ROOM_NAME_LEN ];	//�� �̸�	
	BYTE			m_nCaptain;							//���� �ε���
	BYTE			m_nRoomNo;							//�� ��ȣ
	BYTE			m_nJoiners;							//�� �����ο�
	BYTE			m_nMaxJoiners;						//�� Max �����ο�
	BYTE			m_nMapInfo;							//�� �÷��� �� �� �ε���
	RoomState		m_bRoomState;						//�� ����
	UserInRoomState	m_nJoinerState[	MAX_ROOM_PLAYER ];	//�� ������ ����		

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
		CTOS_GAME_INVITATION,	// �濡�� �ʴ� �޽��� �۽� ��û
		CTOS_GAME_FORCE_OUT_ROOM,	// ���� 1�� �����Ű��
		CTOS_GAME_GIVE_CAPTAIN,	// ���� ���� ��û
		CTOS_GAME_JOINER_INFO,	// ���� 1�� ���� �۽� ��û


		//	-----------------------------------------------------------		
		//	Server - > Client
		//	-----------------------------------------------------------

		STOC_JOIN_MEMBERSHIP_FAIL,	// ȸ������ ����( ���̵� ���� )
		STOC_JOIN_MEMBERSHIP_OK,	// ȸ������ ����
		STOC_LOGIN_OK,				// ���� ���� ���
		STOC_ADD_ROOMLIST, 			// �� ����Ʈ ���� �۽�
		STOC_ADD_PlAYERLIST, 		// ������ ����Ʈ ���� �۽�
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

	HWND						m_hDlg;					// ���̾�α��� �ڵ�(�޼�������)
	
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

	virtual BOOL Begin();		// ���� ����
	virtual BOOL End();			// ���� ����

	void	SetDlgHandle( HWND hDlg ){	m_hDlg = hDlg;	}
	int		GetNumOfPlayers()	{ return static_cast< int >( m_listPlayersInLobby.size() + m_listPlayersInRoom.size() );	 }
	int		GetNumOfRooms()		{ return static_cast< int >( m_listRooms.size() );		}

	//--------- Packet Analysis & Handling -----------

	//	Recv �Ǿ�� ��Ŷ�� �м� �׿� �°� ó���ϴ� �Լ�
	void ProcessPacket( SocketInfo *pSocketInfo, const char* pBuf, int nLen );
	
	//	������ ���������� �̿��Ͽ� �ڷᱸ���� �Ҵ�
	virtual void ProcessConnect( SocketInfo *pSocketInfo );
	
	//	���� ������ ���������� �ڷᱸ���κ��� ����
	virtual void ProcessDisConnect( SocketInfo *pSocketInfo );

	void	SendMsgOfManager( CString &msg );


private:

	//-------------------------------------------------------------------------
	//
	//								Recv Functions
	//
	//	Desc : Client�� ���� ��û �޽����� �޾Ƽ� ó���ϴ� �Լ� ����
	//-------------------------------------------------------------------------

	//		Ŭ���̾�Ʈ�κ��� ���� �޽��� ���� ó��
	void	JoinMember	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		Ŭ���̾�Ʈ�κ��� ���� �޽��� ���� ó��
	void	Login		( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ���� �κ񼭹� ������ �� ����Ʈ ���� �۽�
	void	RoomList	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ���� �κ񼭹� ������ Client ����Ʈ ���� �۽�
	void	PlayList	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ�� �� ���� ��û�� �����Ͽ� �� ���� ó��-	
	void	CreateRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ�� �� ���� ��û�� �����Ͽ� ���� ó��
	void	JoinRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ �� ������ ��û�� �����Ͽ� �� ������ ó��
	void	OutRoom		( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		������ ������ ���� 1�� �����Ű�� ó��
	void	OutRoomByCaptain( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ chat ������ Lobby�� �ִ� ������ Ŭ���̾�Ʈ�鿡�� ����
	void	ChatInLobby	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ�� chat ������ ���� Ŭ���̾�Ʈ�� ��ȿ� ������ ������ Ŭ���̾�Ʈ�鿡�� ����
	void	ChatInRoom	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�ش� Ŭ���̾�Ʈ�� ���� �����û�� ���� ó��
	void	Disconnect	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		��ȿ� �ִ� ������ ���� ���� ��û ���¸� ���� ó��
	void	GameReady	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		��ȿ� �ִ� ������ �� ������ ���� ó��
	void	MapChange	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�������� Game Process�� �����Ͽ� ������ Ŭ���̾�Ʈ ������ �ش� Ŭ���̾�Ʈ(����)���� ����
	void	GameStart	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�κ� �ִ� �����鿡�� �ʴ� �޽��� �۽� ó��
	void	Invitation	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		���� ���� ��û�� ���� ó��
	void	GiveCaptain	( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

	//		�濡 �ִ� ���� 1���� ���� �۽�
	void	JoinerInfoAtRoom( SocketInfo *pSocketInfo, const char* pBuf, int nLen );

};


#endif