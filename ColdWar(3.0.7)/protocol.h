#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <NiPoint3.h>
#include <NiMatrix3.h>
#include <CEGUI.h>
/*
	�����ؾ� ����: CNetSystem �迭�� ����� ���Խ�ų���� �׻� �� ���� #include �Ұ�
	�װ� ��Ű�� ������ winsock2.h �� windows.h�� �浹�� �����Ѽ� �����ǵǾ��ٰ� ���.

*/
const int USER_ID_LEN	=	25;
const int MAX_CHAT_LEN	=	100;

const float RESPONE_TIME	=	5.0f;	//5��
const float TERRITORY_TIME	=	3.0f;	//���� �ð�
const float RECORVERY_TIME  =   8.0f;	//ȸ�� �ð�
const int	QUIT_DELAY_TIME	=	3000;	//���� �¸��� ������ �ð�(3��)

//char	*szFont			= "ahn_m";

namespace nsPlayerInfo
{

	enum _ePLAYER_STATE_AT_ROOM
	{
		DEFAULT = 0,
		READY
	};
		
	// Ŭ���̾�Ʈ ���� �� ��ġ
	enum{ 

		USA = 10, 
		EU, 
		GEURRILLA0, 
		GEURRILLA1
	};

	enum _ePLAYER_JOB
	{
		SOLIDIER,
		SNIPER,
		MEDIC,
		ASP
	};

	enum _eTerritory
	{ 
		TERRITORY_1 = 0, // 1���� ���ɽ�
		TERRITORY_2,  // 2���� 
		TERRITORY_3, // 3���� ���ɽ�
		TERRITORY_MAX, 
		TERRITORY_NO  
	};

	// ��Ʈ��ũ ó�� ��ġ
	enum _eNet_State
	{ 
		NET_ROOM_STATE, 
		NET_GAME_STATE 
	};
}

//���� ����
enum PartOfBody
{
	HEAD = 0,
	LEFT_ARM,
	RIGHT_ARM,
	BODY,
	LEFT_LEG,
	RIGHT_LEG		
};
//����
enum Condition
{
	NORMAL = 10,
	INCAPACITY,
	DEATH
};

//����
enum PointOfView
{
	FIRST_PERSON,
	THIRD_PERSON
};

//���� ����
namespace eSoundCode
{
	enum
	{
		SOUND_BG_MENU,
		SOUND_BG_LOBBY,
		SOUND_BG_PLAY,
		SOUND_WAV_CAPTUREFLAG,
		SOUND_WAV_LOSTFLAG,
		SOUND_WAV_BULLETTAIR,
		SOUND_WAV_BULLETIMPACT,
		SOUND_WAV_CLIPOUT,
		SOUND_WAV_CLIPIN,
		SOUND_WAV_SOUT,
		SOUND_WAV_EMPTY,	
		SOUND_WAV_DEATH,
		SOUND_WAV_HIT,
		SOUND_WAV_STEPLEFT,
		SOUND_WAV_STEPRIGHT,
		SOUND_WAV_AKFIRE,
		SOUND_WAV_FAMASFIRE,
		SOUND_WAV_AWPFIRE,
		SOUND_WAV_M4FIRE,
		
	};
};



#pragma pack (push,1)

typedef	unsigned char	BYTE;
typedef unsigned long	DWORD;

//��ŷ �浹 ����
struct FireIntersectInfo
{
	float			damageDecrease;		//������ ������
	BYTE			part;				//���� ����
};

typedef std::vector<FireIntersectInfo>		FIRE_INTERSECT;


//��� ����
struct FireInfo
{
	NiPoint3	kOrigin;		//������ ������
	NiPoint3	kDirection;		//������ ����

	DWORD		damageUserID;	//���� �÷��̾��� ���̵�
	DWORD		fireUserID;		//�� ����� ���̵�
	
	//�ִ� 3���� ����
	DWORD		intersectInfoSize;		//���� ���� ����
	DWORD		part[3];	
	DWORD		damage[3];			//������ 
	
	bool		bHitPlayer;

	FireInfo()
	{
		::ZeroMemory(this,sizeof(FireInfo));
	}
};

//��ݰ��
struct FireResult
{
	DWORD				m_dwFireUserID;	//�� ��� ���̵�
	DWORD				m_dwResultID;	//���� ����
	DWORD				m_dwCondition;	//����
};

//���� ���� ����ü
struct SoundData
{
	unsigned int	eSoundCode;		//���� ���� �ڵ�
	NiPoint3		kSoundSpot;		//�Ҹ��� �� ����
	NiPoint3		kListenPos;		//��� ����� ��ġ
	NiPoint3		kDir;			//���� ����
	NiPoint3		kUp;			//up����

	bool			b2DSound;		//2D�������� ����		
	
	SoundData()
	{
		ZeroMemory(this,sizeof(SoundData));
	}
};


struct UserInfo
{
	//�Ϲ� ����
	BYTE	id;					//���� ���̵�
	BYTE	BranchOfMilitary;	//����
	BYTE	camp;				//����
	DWORD	animation;			//�ִϸ��̼� ����
	
	//���� ����	
	BYTE	state;				// �÷��̾� ����( Ȱ��, ������, ����, ��Ÿ��� ) �ʱ� : Ȱ�����
	BYTE	territory;			// ���� ������ �ʱ� :  ������ ����
	float	deathTime;			// ���� �ð� �ʱ� : 0.0f;

	BYTE	kill;				// ���� Ƚ�� �ʱ� : 0
	BYTE	death;				// ���� Ƚ�� �ʱ� : 0

	//��ġ ����
	NiPoint3	pos;			//��ġ
	float		Heading;		//�Ӹ� ���� z�� ȸ����
	float		Vertical;		//���� ���콺 �̵�,x�� ȸ����

};

typedef struct _UserRoomInfo
{
	// ����½� �ʿ� ����
	BYTE			m_nPosAtRoom;		// �濡���� ��ġ 0 ~ 5
	BYTE			m_nStateAtRoom;		// �濡���� ����
	BYTE			m_nJob;				// ����
	bool			m_bInitialize;		// �� �ε�
	CEGUI::String	m_strID;

	_UserRoomInfo()
	{
		::ZeroMemory( this, sizeof(_UserRoomInfo) );
		m_nStateAtRoom	= 0;
		m_nJob			= nsPlayerInfo::SOLIDIER;
		m_bInitialize	= false;
	}
}UserRoomInfo, *LPUserRoomInfo;



enum PACKET_TYPE
{
	//LOGIN_CHECK = 100,
	GIVE_ID = 100,				//���̵� �ο�
	UNIT_INFO,					//ĳ���� ����
	ADD_USER,					//�ٸ� ���� �߰�
	ANOTHER_DISCONNECT,
	GUN_FIRE,					//�÷��̾� �� �߻�
	FIRE_RESULT,				//�� �߻� ��� ��Ŷ
	PLAY_TIME,					//�÷��� Ÿ�� ����
	SOUND_DATA,					//���� ������
	CHANGE_STATUS,				//���� ����

	CTOS_CONNECT_ROOM,		// �� ���� ��û
	CTOS_DISCONNECT_ROOM,	// �� ������ ��û
	CTOS_CHAT_IN_GAME,		// �κ� ä�� �޽��� �۽�
	CTOS_CHAT_IN_ROOM,		// �� ä�� �޽��� �۽�
	CTOS_GAME_READY,		// ���� �غ� ��û
	CTOS_CHANGE_TEAM,		// �濡�� �� ����
	CTOS_CHANGE_JOB,		// ���� ���� ��û
	CTOS_GAME_JOIN,			// ���� �ε带 ��ġ��, ���� ���� ���¸� ������ �˷���

	STOC_INIT_INFO,			// �� ���� �� ��ġ�� ����
	STOC_CHANGE_GAMESTATE,	// ���ӻ��·� ��ȯ
	STOC_EXIT_GAMELOBBY,	// ������ �濡�� ���� ����
	STOC_EXIT_GAMEPLAY,		// ������ �����߿� ���� ����

	STOC_ADD_PLAYER,		// ������ �߰� �۽�
	STOC_CHAT_IN_ROOM,		// �� ä�� �޽��� �۽�
	STOC_CHAT_IN_GAME,		// �κ� ä�� �޽��� �۽�
	STOC_GAME_MAP_CHANGE,	// �濡�� �� ��ü

	UPDATE_KILL,
	UPDATE_DEATH,
	UPDATE_TERRITORY,
	WIN_GAME
};


//���� -> Ŭ���̾�Ʈ ��Ŷ
struct give_id
{
	BYTE	size;
	BYTE	type;

	DWORD	id;		//���̵� �ο�
};

struct login_check
{
	BYTE	size;
	BYTE	type;

	DWORD	id;			//���̵�
};
	
struct add_other_user
{
	BYTE	size;
	BYTE	type;

	DWORD	id;			
};

struct another_disconnect
{
	BYTE	size;
	BYTE	type;

	DWORD	id;
};

struct _PACKET_PLAYTIME
{
	BYTE	size;
	BYTE	type;

	float	time;

	_PACKET_PLAYTIME()
	{
		::ZeroMemory(this,sizeof(_PACKET_PLAYTIME));
		size = sizeof(_PACKET_PLAYTIME);
		type = PLAY_TIME;
	}
};

//����  <- Ŭ���̾�Ʈ 
struct user_info			//���� ��ġ ���� ��Ŷ
{
	BYTE	size;
	BYTE	type;

	struct UserInfo info;	
};

//���� ����
struct _PACKET_CHANGE_STATUS
{
	BYTE	size;
	BYTE	type;

	BYTE	status;			//����� ����
	BYTE	id;				//����� ���� ���̵�

	_PACKET_CHANGE_STATUS()
	{
		::ZeroMemory(this,sizeof(_PACKET_CHANGE_STATUS));
		type = CHANGE_STATUS;
		size = sizeof(_PACKET_CHANGE_STATUS);
	}
};

//���� ������
struct _PACKET_SOUND_DATA
{
	BYTE	size;
	BYTE	type;
	
	unsigned int	eSoundCode;		//���� ���� �ڵ�
	NiPoint3		kSoundSpot;		//�Ҹ��� �� ����

	bool			b2DSound;		//���尡 2D���� 3D����

	_PACKET_SOUND_DATA()
	{
		::ZeroMemory(this,sizeof(_PACKET_SOUND_DATA));
		type = SOUND_DATA;
		size = sizeof(_PACKET_SOUND_DATA);
	}
};

//�÷��̾ ���� �߻����� �� �����ϴ� ��Ŷ
struct _PACKET_GUN_FIRE
{
	BYTE	 size;
	BYTE	 type;
	
	FireInfo fireInfo;		//���� ����		

	_PACKET_GUN_FIRE()
	{
		::ZeroMemory(this,sizeof(_PACKET_GUN_FIRE));
		type = GUN_FIRE;
		size = sizeof(_PACKET_GUN_FIRE);
	}
};

//�÷��̾ _PACKET_GUN_FIRE�� ���� Ŭ���̾�Ʈ�� �ٽ� ������ ��Ŷ
struct _PACKET_FIRE_RESULT
{
	BYTE	size;
	BYTE	type;

	FireResult	fireResult;			//��� enum
	
	_PACKET_FIRE_RESULT()
	{
		::ZeroMemory(this,sizeof(_PACKET_FIRE_RESULT));
		type = FIRE_RESULT;
		size = sizeof(_PACKET_FIRE_RESULT);
	}
};

// �� ���� ��û
typedef struct _PACKET_CONNECT_ROOM
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	char	strGameID[ USER_ID_LEN ];
	
	_PACKET_CONNECT_ROOM()
	{
		::ZeroMemory( this, sizeof(_PACKET_CONNECT_ROOM) );
		type = CTOS_CONNECT_ROOM;
		size = sizeof(_PACKET_CONNECT_ROOM);
	}

}PACKET_CONNECT_ROOM, *LPACKET_CONNECT_ROOM;


// �ű� ���� �߰�
typedef struct _PACKET_ADD_PLAYER
{
	BYTE	size;
	BYTE	type;
	
	BYTE	nID;
	BYTE	nPosAtRoom;	
	BYTE	nStateAtRoom;	
	BYTE	nJob;
	char	strGameID[ USER_ID_LEN ];
	
	_PACKET_ADD_PLAYER()
	{
		::ZeroMemory( this, sizeof(_PACKET_ADD_PLAYER) );
		type = STOC_ADD_PLAYER;
		size = sizeof(_PACKET_ADD_PLAYER);
		nStateAtRoom = nsPlayerInfo::DEFAULT;
	}

}PACKET_ADD_PLAYER, *LPACKET_ADD_PLAYER;


//	������ ��û
typedef struct _PACKET_DISCONNECT_PLAYER_AT_ROOM
{
	BYTE	size;
	BYTE	type;
	
	BYTE	nID;
	
	_PACKET_DISCONNECT_PLAYER_AT_ROOM()
	{
		::ZeroMemory( this, sizeof(_PACKET_DISCONNECT_PLAYER_AT_ROOM) );
		type = CTOS_DISCONNECT_ROOM;
		size = sizeof(_PACKET_DISCONNECT_PLAYER_AT_ROOM);
	}

}PACKET_DISCONNECT_PLAYER_AT_ROOM, *LPACKET_DISCONNECT_PLAYER_AT_ROOM;

//	������ ��û
typedef struct _PACKET_CHATMSG_AT_ROOM
{
	BYTE	size;
	BYTE	type;
	
	char	strMsg[ MAX_CHAT_LEN ];
	
	_PACKET_CHATMSG_AT_ROOM()
	{
		::ZeroMemory( this, sizeof(_PACKET_CHATMSG_AT_ROOM) );
		type = CTOS_CHAT_IN_ROOM;
		size = sizeof(_PACKET_CHATMSG_AT_ROOM);
	}

}PACKET_CHATMSG_AT_ROOM, *LPACKET_CHATMSG_AT_ROOM;


typedef struct _PACKET_CHATMSG_AT_GAME
{
	BYTE	size;
	BYTE	type;
	
	char	strMsg[ MAX_CHAT_LEN ];
	
	_PACKET_CHATMSG_AT_GAME()
	{
		::ZeroMemory( this, sizeof(_PACKET_CHATMSG_AT_GAME) );
		type = CTOS_CHAT_IN_GAME;
		size = sizeof(_PACKET_CHATMSG_AT_GAME);
	}

}PACKET_CHATMSG_AT_GAME, *LPACKET_CHATMSG_AT_GAME;


//	���� ��û
typedef struct _PACKET_GAME_READY
{
	BYTE	size;
	BYTE	type;
	
	BYTE	nID;
	BYTE	nState;

	_PACKET_GAME_READY()
	{
		::ZeroMemory( this, sizeof(_PACKET_GAME_READY) );
		type	= CTOS_GAME_READY;
		size	= sizeof(_PACKET_GAME_READY);
		nState	= nsPlayerInfo::DEFAULT;
	}

}PACKET_GAME_READY, *LPACKET_GAME_READY;

//	�� ����
typedef struct _PACKET_CHANGE_TEAM
{
	BYTE	size;
	BYTE	type;
	
	BYTE	nID;
	BYTE	nTeam;

	_PACKET_CHANGE_TEAM()
	{
		::ZeroMemory( this, sizeof(_PACKET_CHANGE_TEAM) );
		type	= CTOS_CHANGE_TEAM;
		size	= sizeof(_PACKET_CHANGE_TEAM);
	}

}PACKET_CHANGE_TEAM, *LPACKET_CHANGE_TEAM;

typedef struct _PACKET_EXIT_GAMELOBBY
{
	BYTE	size;
	BYTE	type;

	_PACKET_EXIT_GAMELOBBY()
	{
		::ZeroMemory( this, sizeof(_PACKET_EXIT_GAMELOBBY) );
		type	= STOC_EXIT_GAMELOBBY;
		size	= sizeof(_PACKET_EXIT_GAMELOBBY);
	}

}PACKET_EXIT_GAMELOBBY, *LPACKET_EXIT_GAMELOBBY;


typedef struct _PACKET_EXIT_GAMEPLAY
{
	BYTE	size;
	BYTE	type;

	_PACKET_EXIT_GAMEPLAY()
	{
		::ZeroMemory( this, sizeof(_PACKET_EXIT_GAMEPLAY) );
		type	= STOC_EXIT_GAMEPLAY;
		size	= sizeof(_PACKET_EXIT_GAMEPLAY);
	}

}PACKET_EXIT_GAMEPLAY, *LPACKET_EXIT_GAMEPLAY;



typedef struct _PACKET_CHANGE_JOB
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	BYTE	nJob;

	_PACKET_CHANGE_JOB()
	{
		::ZeroMemory( this, sizeof(_PACKET_CHANGE_JOB) );
		type	= CTOS_CHANGE_JOB;
		size	= sizeof(_PACKET_CHANGE_JOB);
	}

}PACKET_CHANGE_JOB, *LPACKET_CHANGE_JOB;

typedef struct _PACKET_CHANGE_GAMESTATE
{
	BYTE	size;
	BYTE	type;

	_PACKET_CHANGE_GAMESTATE()
	{
		::ZeroMemory( this, sizeof(_PACKET_CHANGE_GAMESTATE) );
		type	= STOC_CHANGE_GAMESTATE;
		size	= sizeof(_PACKET_CHANGE_GAMESTATE);
	}

}PACKET_CHANGE_GAMESTATE, *LPACKET_CHANGE_GAMESTATE;


typedef struct _PACKET_GAME_JOIN
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;

	_PACKET_GAME_JOIN()
	{
		::ZeroMemory( this, sizeof(_PACKET_GAME_JOIN) );
		type	= CTOS_GAME_JOIN;
		size	= sizeof(_PACKET_GAME_JOIN);
	}

}PACKET_GAME_JOIN, *LPACKET_GAME_JOIN;

typedef struct _PACKET_INIT_INFO
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	BYTE	nPosAtRoom;
	BYTE	nStateAtRoom;
	BYTE	nJob;

	_PACKET_INIT_INFO()
	{
		::ZeroMemory( this, sizeof(_PACKET_INIT_INFO) );
		type	= STOC_INIT_INFO;
		size	= sizeof(_PACKET_INIT_INFO);

		nPosAtRoom = 0;
		nStateAtRoom = nsPlayerInfo::DEFAULT;
		nJob = nsPlayerInfo::SOLIDIER;
	}

}PACKET_INIT_INFO, *LPACKET_INIT_INFO;

typedef struct _PACKET_UPDATE_KILL
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	BYTE	nKill;

	_PACKET_UPDATE_KILL()
	{
		::ZeroMemory( this, sizeof(_PACKET_UPDATE_KILL) );
		type	= UPDATE_KILL;
		size	= sizeof(_PACKET_UPDATE_KILL);
	}

}PACKET_UPDATE_KILL, *LPACKET_UPDATE_KILL;

typedef struct _PACKET_UPDATE_DEATH
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	BYTE	nDeath;

	_PACKET_UPDATE_DEATH()
	{
		::ZeroMemory( this, sizeof(_PACKET_UPDATE_DEATH) );
		type	= UPDATE_DEATH;
		size	= sizeof(_PACKET_UPDATE_DEATH);
	}

}PACKET_UPDATE_DEATH, *LPACKET_UPDATE_DEATH;


typedef struct _PACKET_UPDATE_TERRITORY
{
	BYTE	size;
	BYTE	type;

	BYTE	nID;
	BYTE	nCamp;
	BYTE	nTerritory;

	BYTE	nRenderCamp;
	BYTE	nRenderTerritory;

	_PACKET_UPDATE_TERRITORY()
	{
		::ZeroMemory( this, sizeof(_PACKET_UPDATE_TERRITORY) );
		type	= UPDATE_TERRITORY;
		size	= sizeof(_PACKET_UPDATE_TERRITORY);
	}

}PACKET_UPDATE_TERRITORY, *LPACKET_UPDATE_TERRITORY;


typedef struct _PACKET_WIN_GAME
{
	BYTE	size;
	BYTE	type;

	BYTE	nCamp;

	_PACKET_WIN_GAME()
	{
		::ZeroMemory( this, sizeof(_PACKET_WIN_GAME) );
		type	= WIN_GAME;
		size	= sizeof(_PACKET_WIN_GAME);
	}

}PACKET_WIN_GAME, *LPACKET_WIN_GAME;


#pragma pack(pop)

#endif
