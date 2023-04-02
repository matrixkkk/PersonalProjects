/*
	Ŭ���� �� 	: NetWorkManager
	Ŭ�����뵵	: ���ӳ��� ��Ʈ��ũ�� �����ϸ�, ���� �� �ɼǿ� ���� ����/Ŭ���̾�Ʈ �� �ϳ��� �����Ѵ�.

*/

#ifndef NETWORKMANGER_H_
#define NETWORKMANGER_H_

#include "GameServer.h"
#include "GameClient.h"

#include <map>
#include <string>

typedef std::vector<GAME_USER_INFO>			VecUserInfo;
typedef std::map< BYTE, GAME_USER_INFO>		PlayersMap;
typedef std::vector<FireInfo>				VecFireInfo;		//��� ����
typedef std::vector<FireResult>				VecFireResult;		//��� ���



class NetworkManager : public NiMemObject
{
public:
	static bool		Create(bool server = true);		//true�� ���� false  Ŭ���̾�Ʈ
	static bool		Destroy();
	bool			Start();						//��Ʈ��ũ ����
	bool			End();							//��Ʈ��ũ ����
	static void		Reset(bool server = true);		//��Ʈ��ũ ����

	static NetworkManager* GetInstance();
	
	void			SetID(BYTE id);	
	BYTE			GetID();
	BYTE			GetCamp();
	

	void			RegisterFireInfo(FireInfo& fireInfo);			//��� ���� ���
	void			RegisterFireResult(FireResult& fireResult);
	
	VecUserInfo&	GetVecUserInfo();								//���� ���� �����̳� ����
	
	void			SetSelfData(const GAME_USER_INFO&  selfData);	//���� ������ ���� �����͸� ��Ʈ��ũ �Ŵ����� ����	
	void			SetSendData(BYTE id);
	void			UpdateSelfData(const UserInfo& selfInfo);		//�� ������ �����͸� ������Ʈ
	void			AddUserData(const GAME_USER_INFO& userInfo);	//���� �����͸� �߰��Ѵ�.
	void			UpdateUserData(const UserInfo& selfInfo);		//���� �����͸� ������Ʈ �Ѵ�.
	void			SendFireData(const _PACKET_GUN_FIRE& gunFire);	//�߻� ���� ����
	void			SendFireResult(const _PACKET_FIRE_RESULT& fireResult);	//�߻� ��� ����
	void			SendSoundData(const _PACKET_SOUND_DATA& soundData);
	void			SendChangeStatus(const _PACKET_CHANGE_STATUS& changeStatus);
	
//	void			DeleteUser(DWORD id);							//�ش� ���̵��� ������ ����

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

	void			ClearFireInfo();		//��� �����̳ʿ��� ������ ����
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
	//���� ���� - ����������
	VecUserInfo			m_vecUserInfo;
	BYTE				m_nUniqID;				//�� ������ ���� ���̵�	
	GAME_USER_INFO		m_SelfInfo;				//���� ����

	float				m_fLastSendTime;		//���������� �޽����� ������ �ð�
	float				m_fDelay;				//��Ʈ��ũ ������ �ð�

	static bool			m_bCaptain;				// ���� ����
	static HANDLE		m_StartEvent;

	//------------
	PlayersMap			m_mapPlayers;
	CRITICAL_SECTION	m_csPlayers;
	CRITICAL_SECTION	m_csFireInfo;
	CRITICAL_SECTION	m_csFireResult;
	
	//------------------------------
	//    �� �߻� ���� ����
	//------------------------------
	VecFireInfo			m_vecFireInfo;		//��� ����
	VecFireResult		m_vecFireResult;		//��� ���
	
	//��Ʈ��ũ ��ü
	static	CGameServer*			m_pGameServer;
	static	CGameClient*			m_pGameClient;

	static NetworkManager*			m_pkNetworkManager;			

};

#endif