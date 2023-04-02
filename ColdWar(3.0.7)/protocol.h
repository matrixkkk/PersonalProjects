#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <NiPoint3.h>
#include <NiMatrix3.h>
#include <CEGUI.h>
/*
	주의해야 할점: CNetSystem 계열의 헤더를 포함시킬때는 항상 맨 위에 #include 할것
	그걸 지키지 않으면 winsock2.h 와 windows.h가 충돌을 일으켜서 재정의되었다고 뜬다.

*/
const int USER_ID_LEN	=	25;
const int MAX_CHAT_LEN	=	100;

const float RESPONE_TIME	=	5.0f;	//5초
const float TERRITORY_TIME	=	3.0f;	//점령 시간
const float RECORVERY_TIME  =   8.0f;	//회복 시간
const int	QUIT_DELAY_TIME	=	3000;	//게임 승리후 딜레이 시간(3초)

//char	*szFont			= "ahn_m";

namespace nsPlayerInfo
{

	enum _ePLAYER_STATE_AT_ROOM
	{
		DEFAULT = 0,
		READY
	};
		
	// 클라이언트 현재 팀 위치
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
		TERRITORY_1 = 0, // 1지역 점령시
		TERRITORY_2,  // 2지역 
		TERRITORY_3, // 3지역 점령시
		TERRITORY_MAX, 
		TERRITORY_NO  
	};

	// 네트워크 처리 위치
	enum _eNet_State
	{ 
		NET_ROOM_STATE, 
		NET_GAME_STATE 
	};
}

//맞은 부위
enum PartOfBody
{
	HEAD = 0,
	LEFT_ARM,
	RIGHT_ARM,
	BODY,
	LEFT_LEG,
	RIGHT_LEG		
};
//상태
enum Condition
{
	NORMAL = 10,
	INCAPACITY,
	DEATH
};

//시점
enum PointOfView
{
	FIRST_PERSON,
	THIRD_PERSON
};

//사운드 종류
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

//피킹 충돌 정보
struct FireIntersectInfo
{
	float			damageDecrease;		//데미지 감소율
	BYTE			part;				//맞은 부위
};

typedef std::vector<FireIntersectInfo>		FIRE_INTERSECT;


//사격 정보
struct FireInfo
{
	NiPoint3	kOrigin;		//광선의 시작점
	NiPoint3	kDirection;		//광선의 방향

	DWORD		damageUserID;	//맞은 플레이어의 아이디
	DWORD		fireUserID;		//쏜 사람의 아이디
	
	//최대 3군대 관통
	DWORD		intersectInfoSize;		//맞은 정보 개수
	DWORD		part[3];	
	DWORD		damage[3];			//데미지 
	
	bool		bHitPlayer;

	FireInfo()
	{
		::ZeroMemory(this,sizeof(FireInfo));
	}
};

//사격결과
struct FireResult
{
	DWORD				m_dwFireUserID;	//쏜 사람 아이디
	DWORD				m_dwResultID;	//맞은 유저
	DWORD				m_dwCondition;	//상태
};

//사운드 정보 구조체
struct SoundData
{
	unsigned int	eSoundCode;		//사운드 종류 코드
	NiPoint3		kSoundSpot;		//소리가 난 지점
	NiPoint3		kListenPos;		//듣는 사람의 위치
	NiPoint3		kDir;			//보는 방향
	NiPoint3		kUp;			//up벡터

	bool			b2DSound;		//2D사운드인지 여부		
	
	SoundData()
	{
		ZeroMemory(this,sizeof(SoundData));
	}
};


struct UserInfo
{
	//일반 정보
	BYTE	id;					//고유 아이디
	BYTE	BranchOfMilitary;	//병과
	BYTE	camp;				//진영
	DWORD	animation;			//애니메이션 종류
	
	//게임 정보	
	BYTE	state;				// 플레이어 상태( 활동, 빈사상태, 죽음, 기타등등 ) 초기 : 활성모드
	BYTE	territory;			// 현재 점령지 초기 :  점령지 없음
	float	deathTime;			// 죽은 시간 초기 : 0.0f;

	BYTE	kill;				// 죽인 횟수 초기 : 0
	BYTE	death;				// 죽은 횟수 초기 : 0

	//위치 정보
	NiPoint3	pos;			//위치
	float		Heading;		//머리 방향 z축 회전값
	float		Vertical;		//상하 마우스 이동,x축 회전값

};

typedef struct _UserRoomInfo
{
	// 방상태시 필요 정보
	BYTE			m_nPosAtRoom;		// 방에서의 위치 0 ~ 5
	BYTE			m_nStateAtRoom;		// 방에서의 상태
	BYTE			m_nJob;				// 직업
	bool			m_bInitialize;		// 씬 로드
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
	GIVE_ID = 100,				//아이디 부여
	UNIT_INFO,					//캐릭터 정보
	ADD_USER,					//다른 유저 추가
	ANOTHER_DISCONNECT,
	GUN_FIRE,					//플레이어 총 발사
	FIRE_RESULT,				//총 발사 결과 패킷
	PLAY_TIME,					//플레이 타임 전송
	SOUND_DATA,					//사운드 데이터
	CHANGE_STATUS,				//상태 변경

	CTOS_CONNECT_ROOM,		// 방 입장 요청
	CTOS_DISCONNECT_ROOM,	// 방 나가기 요청
	CTOS_CHAT_IN_GAME,		// 로비 채팅 메시지 송신
	CTOS_CHAT_IN_ROOM,		// 방 채팅 메시지 송신
	CTOS_GAME_READY,		// 게임 준비 요청
	CTOS_CHANGE_TEAM,		// 방에서 팀 변경
	CTOS_CHANGE_JOB,		// 직업 변경 요청
	CTOS_GAME_JOIN,			// 월드 로드를 마치고, 게임 가능 상태를 서버에 알려줌

	STOC_INIT_INFO,			// 방 상태 및 위치값 셋팅
	STOC_CHANGE_GAMESTATE,	// 게임상태로 전환
	STOC_EXIT_GAMELOBBY,	// 방장이 방에서 강제 종료
	STOC_EXIT_GAMEPLAY,		// 방자이 게임중에 강제 종료

	STOC_ADD_PLAYER,		// 접속자 추가 송신
	STOC_CHAT_IN_ROOM,		// 방 채팅 메시지 송신
	STOC_CHAT_IN_GAME,		// 로비 채팅 메시지 송신
	STOC_GAME_MAP_CHANGE,	// 방에서 맵 교체

	UPDATE_KILL,
	UPDATE_DEATH,
	UPDATE_TERRITORY,
	WIN_GAME
};


//서버 -> 클라이언트 패킷
struct give_id
{
	BYTE	size;
	BYTE	type;

	DWORD	id;		//아이디 부여
};

struct login_check
{
	BYTE	size;
	BYTE	type;

	DWORD	id;			//아이디
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

//서버  <- 클라이언트 
struct user_info			//유저 위치 정보 패킷
{
	BYTE	size;
	BYTE	type;

	struct UserInfo info;	
};

//상태 변경
struct _PACKET_CHANGE_STATUS
{
	BYTE	size;
	BYTE	type;

	BYTE	status;			//변경될 상태
	BYTE	id;				//변경될 놈의 아이디

	_PACKET_CHANGE_STATUS()
	{
		::ZeroMemory(this,sizeof(_PACKET_CHANGE_STATUS));
		type = CHANGE_STATUS;
		size = sizeof(_PACKET_CHANGE_STATUS);
	}
};

//사운드 데이터
struct _PACKET_SOUND_DATA
{
	BYTE	size;
	BYTE	type;
	
	unsigned int	eSoundCode;		//사운드 종류 코드
	NiPoint3		kSoundSpot;		//소리가 난 지점

	bool			b2DSound;		//사운드가 2D인지 3D인지

	_PACKET_SOUND_DATA()
	{
		::ZeroMemory(this,sizeof(_PACKET_SOUND_DATA));
		type = SOUND_DATA;
		size = sizeof(_PACKET_SOUND_DATA);
	}
};

//플레이어가 총을 발사했을 때 전송하는 패킷
struct _PACKET_GUN_FIRE
{
	BYTE	 size;
	BYTE	 type;
	
	FireInfo fireInfo;		//맞은 부위		

	_PACKET_GUN_FIRE()
	{
		::ZeroMemory(this,sizeof(_PACKET_GUN_FIRE));
		type = GUN_FIRE;
		size = sizeof(_PACKET_GUN_FIRE);
	}
};

//플레이어가 _PACKET_GUN_FIRE를 받은 클라이언트가 다시 보내는 패킷
struct _PACKET_FIRE_RESULT
{
	BYTE	size;
	BYTE	type;

	FireResult	fireResult;			//결과 enum
	
	_PACKET_FIRE_RESULT()
	{
		::ZeroMemory(this,sizeof(_PACKET_FIRE_RESULT));
		type = FIRE_RESULT;
		size = sizeof(_PACKET_FIRE_RESULT);
	}
};

// 방 접속 요청
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


// 신규 유저 추가
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


//	나가기 요청
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

//	나가기 요청
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


//	레디 요청
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

//	팀 변경
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
