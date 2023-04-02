/*
	Ŭ���� ��   : CGameServer
	Ŭ���� �뵵 : CNetSystemServer�� ����� �޴� �� Ŭ������ ��Ŷ�� ���� ó�� �Լ��� �������̵�
				   ó���� ���� �뵵�� Ŭ�����̴�.

*/

#ifndef CGAMESERVER_H_
#define CGAMESERVER_H_

#include "CNetSystemServer.h"
#include <vector>
#include <map>
#include <list>

#define BUFSIZE			20		//	���� ������
//#define MAX_ID		20		//	ID�� �ִ� ����

const int MAX_TEAM_PLAYER = 6;

class NetworkManager;

class CGameServer : public CNetSystemServer
{

public:
	

private:

	unsigned char	m_pBuf[BUFSIZE];		//���� ������
	unsigned int	m_nGlobal_id;			//�������� �ο��� ���̵�
	unsigned int	idx;

	GAME_USER_INFO		m_stCaptain;

	BYTE				m_nCurNetState;	
	BOOL				m_nPlayerPosAtRoom[ MAX_TEAM_PLAYER * 2 + 2 ];
	BYTE				m_bCntInitPlyers;

	//============================================================================================
	//				���� ó�� ���� Data
	//============================================================================================
	BYTE				m_nStateTerritory[nsPlayerInfo::TERRITORY_MAX];	// ������ ���ɵǾ��� ����	
	std::list<BYTE>		m_listUSA_Territory;				// ���� ������
	std::list<BYTE>		m_listEU_Territory;					// ���� ������
	float				m_fTerritoryTime;					// ������ �ð�

	//---------------------------------------
	//				�� �� ȭ
	//---------------------------------------
	CRITICAL_SECTION	m_csStateTerritory;
	CRITICAL_SECTION	m_csUSA_Territory;
	CRITICAL_SECTION	m_csEU_Territory;
	CRITICAL_SECTION	m_csPosAtRoom;

public:

	CGameServer();
	~CGameServer();

	bool	Begin();	// ��������
	bool	End();		// ���� ����

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
	void	CheckWinTeam();		// ���� �ð� ����� �¸� ���� ����

	//���� �÷��̾�� �߻��� �����͸� ó��
	void	SendSelfGunfireData( const unsigned char* pBuf, DWORD byteSize);
	void	SendSelfFireResult( const unsigned char* pBuf, DWORD byteSize);
	void	SelfSoundData( const unsigned char* pBuf, DWORD byteSize);
	void	SelfChangeStatus( const unsigned char* pBuf, DWORD byteSize);

	//�������� �� �������� ������ �޽���
	void	SendTimeToClient(float fTime);		//�ð��� ��� Ŭ���̾�Ʈ���� ����
protected:

	//==============================================================================
	//		�κ�, ���� ��Ŷ ó��
	//==============================================================================	
	bool	PacketProcessAtRoom(const UCHAR* pBuf,CLIENTINFO* pClientInfo);
	bool	PacketProcessAtGame(const UCHAR* pBuf,CLIENTINFO* pClientInfo);
	//==============================================================================


	//==============================================================================
	//		�κ� ��Ŷ ó��
	//==============================================================================
	bool	ProcessLogin( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChatMsg( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessReady( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChangeTeam( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessChangeJob( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	bool	ProcessGameJoin( const UCHAR* pBuf,CLIENTINFO* pClientInfo );
	//==============================================================================


	//==============================================================================
	//		���� ��Ŷ ó��
	//==============================================================================
	void	ProcessAddUserInfo(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//���ο� ������ �߰�
	void	ProcessUpdateUser(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//������ �����͸� ������Ʈ
	void	ProcessChatMsgInGame(const unsigned char* pBuf,CLIENTINFO* pClientInfo);//������ ä�� ó��
	void	ProcessGunFire(const unsigned char* pBuf,CLIENTINFO* pClientInfo);		//���� �߻� ó��
	void	ProcessFireResult(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//�߻� ��� ó��
	void	ProcessTerritory(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//���� �޽��� ó��
	void	ProcessSoundData(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//���� �޽��� ó��
	void	ProcessChangeStatus(const unsigned char* pBuf,CLIENTINFO* pClientInfo);	//���� ���� �޽��� ó��
	//==============================================================================


	//==============================================================================
	//		���� �˰��� �Լ�
	//==============================================================================
	BYTE	GetEmptyPosAtRoom();	
	BYTE	SearchEmptyPos( BYTE nType, BYTE nOldPos, BYTE* nNewPos );
	

	
private:
	
	

};


#endif
