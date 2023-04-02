/*
	클래스 명 : CUserCharacter 클래스
	클래스 용도: 조작가능한 캐릭터 클래스 - 본 클래스는 게임상에서 꼭 1개만 필요하므로 싱글톤으로 작성

*/
#ifndef CUSERCHARACTER_H_
#define CUSERCHARACTER_H_

#include "CCharacter.h"
#include "CollisionManager.h"
#include "Weapon.h"
#include "CPrimaryWeapon.h"
#include "ProcessAnimation.h"
#include "CrossHair.h"

#include <NiCollision.h>


// 1.0f 평지 0.0f 직각
const float SlopeMaxLimit			= cosf( D3DXToRadian(45.0f) );	// 최대  제한 경사면 각도
const float SlopeMinLimit			= cosf( D3DXToRadian(20.0f) );	// 최소 각도
const float GRAVITY					= -9.8f;		//중력가속도
const float JUMP_MAX				= 1.0f;			//최대 점프
//const float JUMP_VELOCITY			= 2.5f;			//점프 힘

class CUserCharacter : public CCharacter
{
public:
	CUserCharacter();
	virtual ~CUserCharacter();
	
	bool	Initialize(NiActorManager* pkActor,const NiTransform &kSpawn);					//컬리젼 초기화
	void	Update(float fTime);
	void	Draw(NiCamera *pkCamera);
	void    ResetIntersectionRecord();			//충돌 Record 리셋
	
	void	Reset(const NiPoint3& newPos,float fTime);

	//총 관련 메소드
	void	StartFire();					//사격 시작
	void	EndFire();						//사격 끝
	void	DecreaseReaction();				//반동 감소
			
	bool	ShootGun(float fTime);		//사격

	NiPoint2*		GetFirePoint();
	CrossHair*		GetCrossHairDisplay();
	CWeapon*		GetEquipWeapon();
		
	CollisionManager::IntersectRecord& GetIntersectionRecord();

	void	SetVertical(float vertical) { m_fHeadVertical += vertical; }

protected:
	virtual void InterpretInput(float fTime);			//캐릭터의 기본 동작 구현
	void		 UpdateBipedRootTransform(float fTime); //Biped Transform
	void		 UpdateCollisionBasePosition(float fTime, const NiMatrix3& kDesireRot,NiPoint3& kLocVelocity);
	void		 UpdateAccuracy();
	void		 RecoveryBuddy(float fTime);		//쓰러진 동료 찾기
	bool		 CreateCrossHair();						//크로스헤어 생성

	//충돌 변수-----------------------------------------------------------
	//CollsionManager::CharacterCollisionCallback에 의해 채워진다.
	CollisionManager::IntersectRecord	m_kIntersectionRecord;
	bool				m_bCollision;					//이번에 충돌에 되었는지
	NiPoint3			m_kLastSafeSlideN;

	
	//---------------------------------------------------------------------
	//캐릭터 스탯
	CWeapon*			m_pkEquipWeapon;				//장착된 무기
	CPrimaryWeapon*		m_pkWeapon;						//캐릭터 주무기

	//------총 발사 관련 변수들 ------------
	CrossHair*			m_kCrossHairObject;					//크로스헤어 표시
	NiPoint2			m_kFirePoint;						//발사점
	float				m_fAccuracy;						//정확도
	float				m_fEffectAccuracy;					//정확도에 영향을 주는 요인 기본 5
	
	unsigned int		m_SuccessiveCount;				//연속 발사수 카운터
	unsigned int		m_AccuracyRateBonus;			//정확도 증가 보너스 보병 -1
	//--------------------------------------
	
	//캐릭터 애니메이션 관련 변수 ----------------------------------------
	ProcessAnimation	m_ProcessAnimation;					
	float				m_fHeadVertical;
	
	//============= 동료 회복을 위한 변수 ==================
	NiPick				m_RecoveryPick;
	float				m_fRecoveryTime;						//회복 지속시간
	bool				m_bRecover;

	class CharacterCallbackObject : public NiActorManager::CallbackObject
	{
	public:
		//콜백 오버라이드 함수
		virtual void AnimActivated(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime);
        virtual void AnimDeactivated(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
			float fEventTime){}
        virtual void TextKeyEvent(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, 
            const NiFixedString& kTextKey,
            const NiTextKeyMatch* pkMatchObject,
			float fCurrentTime, float fEventTime);
        virtual void EndOfSequence(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime);
		
		virtual ~CharacterCallbackObject(){};

		void	Reset();
		void	Init();
		
		bool	m_bJumpData;	//점프 시작했는지 여부
		bool	m_bJumpable;	//점프 가능 플래그
	
		float	m_fJumpForce;	//점프하는 힘
		float	m_fCurrTime;	//현재 시간
		float	m_fEndTime;		//
	};

	CharacterCallbackObject*	m_pkCallBack;
};

#include "CUserCharacter.inl"

#endif