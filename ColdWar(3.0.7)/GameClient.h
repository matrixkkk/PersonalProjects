#ifndef CGAMECLIENT_H_
#define CGAMECLIENT_H_

#include "CClientThread.h"
#include <map>


//-----------------------------------------------------------
//
//			Player 1�� ���� ������ ǥ���� ����ü
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
		
	bool				SendData(void* pData,unsigned char pSendbytes); //������ ���� ������ ���� 
	bool				SendDelayData(void* pData,unsigned char pSendbytes);//����������
	static	SOCKET				GetSocket();
	static  CGameClient*		GetInstance();

protected:	// Internal Function 
	
	// Util
	static void		Err_Display(char* msg);		//���� ���	


public: // Function

//	void	SetMyID( BYTE nID ){ m_nID = nID; }

	//==============================================
	//		�κ� ��Ŷ ó��
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
	//		���� ��Ŷ ó��
	//==============================================
	void AddChatMsgInGame( const char *pBuf );

	void UpdateKill( const char *pBuf );

	void UpdateDeath( const char *pBuf );

	void FirePerform( const char *pBuf );

	void UpdateTerritory( const char *pBuf );

	void WinGame( const char *pBuf );

	void ExitGame( const char *pBuf );
	
	//==============================================
	//		�κ� ��Ŷ ó�� ��û
	//==============================================
	void	RequestSendChat( const CEGUI::String& strChatMsg, BYTE stateType );
	void	RequestReady();
	void	RequestChangeTeam( BYTE nTeam );
	void	RequestChangeJob( BYTE nJob );
	void	RequestGameJoin();

	//==============================================
	//		���� ��Ŷ ó�� ��û
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

	float				m_fLastSendData;	//���������� �����͸� ���� �ð�

	// Data Structure
//	PlayersMap	m_mapPlayers;

//	BYTE		m_nID;

	// Sync
//	CRITICAL_SECTION	m_csPlayers;	

	

};
#endif
