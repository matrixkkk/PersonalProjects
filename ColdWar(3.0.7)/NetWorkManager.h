/*
	클래스 명 	: NetWorkManager
	클래스용도	: 게임내에 네트워크를 관리하며, 생성 시 옵션에 따라 서버/클라이언트 중 하나로 구동한다.

*/

#ifndef NETWORKMANGER_H_
#define NETWORKMANGER_H_

#include "GameServer.h"
#include "GameClient.h"

#include <map>
#include <string>

typedef std::vector<GAME_USER_INFO>			VecUserInfo;
typedef std::map< BYTE, GAME_USER_INFO>		PlayersMap;
typedef std::vector<FireInfo>				VecFireInfo;		//사격 정보
typedef std::vector<FireResult>				VecFireResult;		//사격 결과



class NetworkManager : public NiMemObject
{
public:
	static bool		Create(bool server = true);		//true는 서버 false  클라이언트
	static bool		Destroy();
	bool			Start();						//네트워크 시작
	bool			End();							//네트워크 종료
	static void		Reset(bool server = true);		//네트워크 리셋

	static NetworkManager* GetInstance();
	
	void			SetID(BYTE id);	
	BYTE			GetID();
	BYTE			GetCamp();
	

	void			RegisterFireInfo(FireInfo& fireInfo);			//사격 정보 등록
	void			RegisterFireResult(FireResult& fireResult);
	
	VecUserInfo&	GetVecUserInfo();								//유저 정보 컨테이너 리턴
	
	void			SetSelfData(const GAME_USER_INFO&  selfData);	//현재 어플의 유저 데이터를 네트워크 매니저에 전달	
	void			SetSendData(BYTE id);
	void			UpdateSelfData(const UserInfo& selfInfo);		//현 유저의 데이터를 업데이트
	void			AddUserData(const GAME_USER_INFO& userInfo);	//유저 데이터를 추가한다.
	void			UpdateUserData(const UserInfo& selfInfo);		//유저 데이터를 업데이트 한다.
	void			SendFireData(const _PACKET_GUN_FIRE& gunFire);	//발사 정보 전송
	void			SendFireResult(const _PACKET_FIRE_RESULT& fireResult);	//발사 결과 전송
	void			SendSoundData(const _PACKET_SOUND_DATA& soundData);
	void			SendChangeStatus(const _PACKET_CHANGE_STATUS& changeStatus);
	
//	void			DeleteUser(DWORD id);							//해당 아이디의 유저를 제거

	bool			ProcessRespone( float fAccumTime);
	bool			ProcessTerritory( BYTE nCamp,  BYTE nTerritory );
		
	BYTE			GetCharacterIndex( BYTE nTeam, BYTE nJob );
	BYTE			GetTeamType( BYTE nPos, bool bRoomSeatMove = true );

	CGameServer*	GetServer(){ return m_pGameServer; }
	CGameClient*	GetClient(){ return m_pGameClient; }
	void			SetCaptain( bool bCaptain ){ m_bCaptain = bCaptain; }
	bool			IsCaptain(){ return m_bCaptain; }

	PlayersMap&		LockPlayers();
	VecFireInfo&	LockFireInfoVec();		
	VecFireResult&	LockFireResultVec();
	
	void			UnLockPlayers();
	void			UnLockFireInfo();
	void			UnLockFireResult();

	void			ClearFireInfo();		//사격 컨테이너에서 데이터 삭제
	void			ClearFireResult();

	DWORD			WaitComplete();
	void			CallStartEvent();
	void			ResetStartEvent();
	static void		SetIP( std::string pIP );
	static const char*	GetIP();
	
private:

	NetworkManager();
	~NetworkManager();

	static			std::string			m_IP;
	//유저 정보 - 공유데이터
	VecUserInfo			m_vecUserInfo;
	BYTE				m_nUniqID;				//현 어플의 고유 아이디	
	GAME_USER_INFO		m_SelfInfo;				//셀프 정보

	float				m_fLastSendTime;		//마지막으로 메시지를 전송한 시간
	float				m_fDelay;				//네트워크 딜레이 시간

	static bool			m_bCaptain;				// 방장 여부
	static HANDLE		m_StartEvent;

	//------------
	PlayersMap			m_mapPlayers;
	CRITICAL_SECTION	m_csPlayers;
	CRITICAL_SECTION	m_csFireInfo;
	CRITICAL_SECTION	m_csFireResult;
	
	//------------------------------
	//    총 발사 관련 변수
	//------------------------------
	VecFireInfo			m_vecFireInfo;		//사격 정보
	VecFireResult		m_vecFireResult;		//사격 결과
	
	//네트워크 객체
	static	CGameServer*			m_pGameServer;
	static	CGameClient*			m_pGameClient;

	static NetworkManager*			m_pkNetworkManager;			

};

#endif