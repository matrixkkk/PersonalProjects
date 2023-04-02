/*
	클래스 명 : GameSystemManager
	클래스 용도 : 게임의 흐름을 제어, 시작 - 진행 - 끝, 게임의 정보를 디스플레이한다.

*/

#ifndef GAMESYSTEMMANAGER_H_
#define GAMESYSTEMMANAGER_H_

#include "main.h"
#include "CPrimaryWeapon.h"
#include "CUserCharacter.h"
#include "CRadar.h"

//부위별 추가 데미지
const float fHeadAddDamage  = 6.0f;
const float fChestAddDamage = 3.0f;


//부위 별 체력
struct Healths
{
	short	head;		//머리
	short	chest;		//가슴
	short	leftArm;	//왼팔
	short	rightArm;	//오른팔
	short	leftLeg;	//왼발
	short   rightLeg;	//오른발

	Healths()
	{
		head = chest = leftArm = rightArm = leftLeg = rightLeg = 100;
	}
	void Reset()
	{
		head = chest = leftArm = rightArm = leftLeg = rightLeg = 100;
	}
};

//스코어보드
struct SCORE
{
	BYTE	kill;		//죽인 수
	BYTE	death;		//죽은 수

	SCORE()
	{
		ZeroMemory(this,sizeof(*this));
	}
};

typedef std::map<DWORD,SCORE>	SCOREMAP;

//데미지 바디 영역
struct HealthBody
{
	NiRect<unsigned char>	m_kHead;
	NiRect<unsigned char>	m_kChest;
	NiRect<unsigned char>	m_kLeftArm;
	NiRect<unsigned char>	m_kRightArm;
	NiRect<unsigned char>	m_kLeftLeg;
	NiRect<unsigned char>	m_kRightLeg;

	HealthBody()
	{
		m_kHead		= NiRect<unsigned char>(19,35,2,16);
		m_kChest	= NiRect<unsigned char>(16,39,16,54);
		m_kLeftArm	= NiRect<unsigned char>(7,16,16,54);
		m_kRightArm	= NiRect<unsigned char>(39,48,16,54);
		m_kLeftLeg	= NiRect<unsigned char>(12,27,54,92);
		m_kRightLeg	= NiRect<unsigned char>(27,44,54,92);
	}
};


class GameSystemManager : public NiMemObject
{
public:
	GameSystemManager();
	~GameSystemManager();

	static GameSystemManager* GetInstance();

	bool	Init(DWORD	dwArmy,float fTotalTime);
	bool	CreateFont();
	bool	CreateScreenTexture();
	bool	CreateRadar();
	void	DrawUI();
	void	Update(float fTime);				//업데이트

	//==========================================
	//======게임 플레이 시간 관련 함수==========
	void	StartGame();		//게임 시간 시작
	void	EndGame();			//종료
	void	ResetPlay();		//리셋
	void	MeasureTime(float fTime);		//서버에서만 사용
	void	SetGameTime(float fCurrTime);	//클라에서만 사용
	//==========================================

	//======= 체력 관련 함수 ===================
	void	RecoveryHealth();								//체력 모두 회복
	DWORD	ProcessCondition(FireInfo* pFireInfo);			//데미지를 바탕으로 캐릭터 상태 결정
	void	CalculateDamageByPart(BYTE part,short sDamage); //부위별 데미지 계산
	void	DamageByInjury(float fTime);					//부상으로 인한 Tick데미지
	Healths* GetHealths();									//체력 리턴

	//====== 스코어 관련 함수 ==================
	void	AddKill(DWORD id);
	void	AddDeath(DWORD id);
	SCOREMAP& GetScoreBoard(){ return m_kScoreboard; }
	void	ProcessResultScore();

	//====== 데미지 이펙트 함수 ================
	bool	CreateDamageEffect();
	void	DisplayDamageEffect();


private:
	void	DrawFont(NiRenderer*	pkRenderer);	//폰트 드로우
	void	HealthBodyUpdate();						//헬스 바디 업데이트
	void	ConditionUpdate();						//전체 컨디션 업데이트
	void	ChangeTexture(NiPixelData* kPixel,NiRect<unsigned char> kBody,unsigned short kHealth);	//해당 영역의 텍스처 변화

	//====== static this포인터변수 ======
	static GameSystemManager*		ms_gameSystemManager;

	//====== 스크린 텍스쳐 목록 =======
	NiScreenTextureArray	m_kScreenTextureArray;	//스크린 텍스쳐 그룹	

	//====== 체력 표시기 변수 ===========================
	NiSourceTexturePtr		m_spOrigin;
	NiSourceTexturePtr		m_spBodyTex;

	//====== 폰트 관련 변수 ==============================
	NiFontPtr				m_spFont;				//폰트
	NiString2DPtr			m_spText;
	
	//폰트 위치
	NiPoint2				m_kPosTime;
	NiPoint2				m_kPosBulletData;	//남은 발수 폰트 위치 --추가

	//========= 클래스 목록 ==============
	CRadar*					m_pkRadar;
	NiScreenElementsArray	m_kRadarPoint;			//레이더상의 적의 위치를 그릴 엘리먼츠
	
	//========= 구조체 목록 ==============
	//체력 관련 변수
	HealthBody				m_kHealthBody;		//체력 바디 영역
	Healths					m_kHealth;			//캐릭터의 체력 상태

	//========= 컨테이너 목록 ============
	SCOREMAP				m_kScoreboard;		//킬뎃 스코어 맵
	
	
	unsigned int			m_uiTotalCondition;	//전체 체력 상태 퍼센트
	unsigned int			m_uiArmor;			//아머

	
	//========= 게임 시간 관련 변수 ===========
	float					m_fTotalTime;		//게임 플레이 시간
	float					m_fCurrTime;		//현재 시간
	float					m_fPlayAccumTime;	//게임플레이됐을 경우의 누적 시간
	float					m_fTimer;			//시간 메시지를 보내는 타이머
	bool					m_bIsServer;		//true:현재 어플이 서버 false: 현재 어플이 클라
	
	//========= 데미지 이펙트 변수 ============
	NiScreenElements*		m_pkDamageEffect;	//데미지 이펙트
	bool					m_bDamageEffectOn;	
	
	float					m_fLastDamage;
};


#endif
