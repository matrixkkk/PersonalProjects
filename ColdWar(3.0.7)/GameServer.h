/*
	클래스 명   : CGameServer
	클래스 용도 : CNetSystemServer를 상속을 받는 본 클래스는 패킷에 대한 처리 함수를 오버라이딩
				   처리를 위한 용도인 클래스이다.

*/

#ifndef CGAMESERVER_H_
#define CGAMESERVER_H_

#include "CNetSystemServer.h"
#include <vector>
#include <map>
#include <list>

#define BUFSIZE			20		//	버퍼 사이즈
//#define MAX_ID		20		//	ID의 최대 길이

const int MAX_TEAM_PLAYER = 6;

class NetworkManager;

class CGameServer : public CNetSystemServer
{

public:
	

private:

	unsigned char	m_pBuf[BUFSIZE];		//버퍼 사이즈
	unsigned int	m_nGlobal_id;			//유저에게 부여될 아이디
	unsigned int	idx;

	GAME_USER_INFO		m_stCaptain;

	BYTE				m_nCurNetState;	
	BOOL				m_nPlayerPosAtRoom[ MAX_TEAM_PLAYER * 2 + 2 ];
	BYTE				m_bCntInitPlyers;

	//============================================================================================
	//				점령 처리 관련 Data
	//============================================================================================
	BYTE				m_nStateTerritory[nsPlayerInfo::TERRITORY_MAX];	// 점령지 점령되어진 상태	
	std::list<BYTE>		m_listUSA_Territory;				// 팀별 점령지
	std::list<BYTE>		m_listEU_Territory;					// 팀별 점령지
	float				m_fTerritoryTime;					// 점령한 시간

	//---------------------------------------
	//				동 기 화
	//---------------------------------------
	CRITICAL_SECTION	m_csStateTerritory;
	CRITICAL_SECTION	m_csUSA_Territory;
	CRITICAL_SECTION	m_csEU_Territory;
	CRITICAL_SECTION	m_csPosAtRoom;

public:

	CGameServer();
	~CGameServer();

	bool	Begin();	// 서버시작
	bool	End();		// 서버 종료

	void	SetCaptaionID( CEGUI::String& strName );
	BYTE	GetPosAtRoom();

	bool	ConnectProcess(CLIENTINFO* pClientInfo);
	bool	DisConnectProcess(CLIENTINFO* pClientInfo);
	bool	PacketProcess(const UCHAR* pBuf,CLIENTINFO* pClientInfo);	

	void	SendChatMsg( const CEGUI::String& strChat, BYTE chatType);	
	void	ChangePos( BYTE nTeam );
	void	SendExitAtRoom();
	void	SendChangeJob(BYTE nJob);
	void	SendStartGame();
	bool	StartGame();
	void	SendUpdateTerritory(BYTE nTerritory);
	bool	WinGame( BYTE nCamp );
	void	CheckWinTeam();		// 게임 시간 종료시 승리 여부 결정

	//서버 플레이어에서 발생된 데이터를 처리
	void	SendSelfGunfireData( const unsigned char* pBuf, DWORD byteSize);
	void	SendSelfFireResult( const unsigned char* pBuf, DWORD byteSize);
	void	SelfSoundData( const unsigned char* pBuf, DWORD byteSize);
	void	SelfChangeStatus( const unsigned char* pBuf, DWORD byteSize);

	//서버에서 단 방향으로 보내는 메시지
	void	SendTimeToClient(float fTime);		//시간을 모든 클라이언트에게 전송
protected:

	//==============================================================================
	//		로비, 게임 패킷 처리
	//==============================================================================	
	bool	PacketProcessAtRoom(const UCHAR* pBuf,CLIENTINFO* pClientInfo);
	bool	PacketProcessAtGame(const UCHAR* pBuf,CLIENTINFO* pClientInfo);
	//==============================================================================


	//==============================================================================
	//		로비 패킷 처리
	//==============================================================================
	bool	ProcessLogin( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChatMsg( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessReady( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChangeTeam( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChangeJob( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessGameJoin( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	//==============================================================================


	//==============================================================================
	//		게임 패킷 처리
	//==============================================================================
	void	ProcessAddUserInfo(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//새로운 유저를 추가
	void	ProcessUpdateUser(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//유저의 데이터를 업데이트
	void	ProcessChatMsgInGame(const unsigned char* pBuf,CLIENTINFO* pClientInfo);//게임중 채팅 처리
	void	ProcessGunFire(const unsigned char* pBuf,CLIENTINFO* pClientInfo);		//총의 발사 처리
	void	ProcessFireResult(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//발사 결과 처리
	void	ProcessTerritory(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//점령 메시지 처리
	void	ProcessSoundData(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//사운드 메시지 처리
	void	ProcessChangeStatus(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//상태 변경 메시지 처리
	//==============================================================================


	//==============================================================================
	//		로직 알고리즘 함수
	//==============================================================================
	BYTE	GetEmptyPosAtRoom();	
	BYTE	SearchEmptyPos( BYTE nType, BYTE nOldPos, BYTE* nNewPos );
	

	
private:
	
	

};


#endif
