#ifndef CGAMECLIENT_H_
#define CGAMECLIENT_H_

#include "CClientThread.h"
#include <map>


//-----------------------------------------------------------
//
//			Player 1명에 대한 정보를 표현한 구조체
//
//-----------------------------------------------------------

class CGameClient : public NiMemObject
{
public:

//	typedef std::map<BYTE, GAME_USER_INFO >	PlayersMap;

	CGameClient();
	~CGameClient();

	bool	Init();
	bool	Start();
	bool	End();
	bool	Connect();
	bool	DisConnect();

	bool	GetConnecting();
		
	bool				SendData(void* pData,unsigned char pSendbytes); //데이터 보냄 딜레이 잇음 
	bool				SendDelayData(void* pData,unsigned char pSendbytes);//딜레이잇음
	static	SOCKET				GetSocket();
	static  CGameClient*		GetInstance();

protected:	// Internal Function 
	
	// Util
	static void		Err_Display(char* msg);		//에러 출력	


public: // Function

//	void	SetMyID( BYTE nID ){ m_nID = nID; }

	//==============================================
	//		로비 패킷 처리
	//==============================================
	void	MySelfInfoAtRoom( const char *pBuf );

	void	AddPlayer( const char *pBuf );	

	void	DisconnectPlayer( const char *pBuf );	

	void	AddChatMsg( const char *pBuf );

	void	ReadyPlayer( const char *pBuf );

	void	ChangeTeam( const char *pBuf );

	void	ChangeJob( const char *pBuf );

	void	ExitLobby( const char *pBuf );

	void	ChangeGameState( const char *pBuf );

	void	StartGame( const char *pBuf );



	//==============================================
	//		게임 패킷 처리
	//==============================================
	void AddChatMsgInGame( const char *pBuf );

	void UpdateKill( const char *pBuf );

	void UpdateDeath( const char *pBuf );

	void FirePerform( const char *pBuf );

	void UpdateTerritory( const char *pBuf );

	void WinGame( const char *pBuf );

	void ExitGame( const char *pBuf );
	
	//==============================================
	//		로비 패킷 처리 요청
	//==============================================
	void	RequestSendChat( const CEGUI::String& strChatMsg, BYTE stateType );
	void	RequestReady();
	void	RequestChangeTeam( BYTE nTeam );
	void	RequestChangeJob( BYTE nJob );
	void	RequestGameJoin();

	//==============================================
	//		게임 패킷 처리 요청
	//==============================================
//	void	RequestUpdateKill();
//	void	RequestUpdateDeath();
	void	RequestTerritory( BYTE nCampm, BYTE nTerritory );

	//====================================
	BYTE	GetPosAtRoom();
	BYTE	GetCurrentState();

	
protected:	// Attribute

	//	Default 
	CClientThread*		m_pClientThread;
	WSADATA				m_wsa;		
	bool				m_bConnect;		
	static SOCKET		m_socket;
	static CGameClient*	m_gClient;

	float				m_fLastSendData;	//마지막으로 데이터를 보낸 시간

	// Data Structure
//	PlayersMap	m_mapPlayers;

//	BYTE		m_nID;

	// Sync
//	CRITICAL_SECTION	m_csPlayers;	

	

};
#endif
