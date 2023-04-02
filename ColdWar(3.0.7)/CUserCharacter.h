/*
	Ŭ���� �� : CUserCharacter Ŭ����
	Ŭ���� �뵵: ���۰����� ĳ���� Ŭ���� - �� Ŭ������ ���ӻ󿡼� �� 1���� �ʿ��ϹǷ� �̱������� �ۼ�

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


// 1.0f ���� 0.0f ����
const float SlopeMaxLimit			= cosf( D3DXToRadian(45.0f) );	// �ִ�  ���� ���� ����
const float SlopeMinLimit			= cosf( D3DXToRadian(20.0f) );	// �ּ� ����
const float GRAVITY					= -9.8f;		//�߷°��ӵ�
const float JUMP_MAX				= 1.0f;			//�ִ� ����
//const float JUMP_VELOCITY			= 2.5f;			//���� ��

class CUserCharacter : public CCharacter
{
public:
	CUserCharacter();
	virtual ~CUserCharacter();
	
	bool	Initialize(NiActorManager* pkActor,const NiTransform &kSpawn);					//�ø��� �ʱ�ȭ
	void	Update(float fTime);
	void	Draw(NiCamera *pkCamera);
	void    ResetIntersectionRecord();			//�浹 Record ����
	
	void	Reset(const NiPoint3& newPos,float fTime);

	//�� ���� �޼ҵ�
	void	StartFire();					//��� ����
	void	EndFire();						//��� ��
	void	DecreaseReaction();				//�ݵ� ����
			
	bool	ShootGun(float fTime);		//���

	NiPoint2*		GetFirePoint();
	CrossHair*		GetCrossHairDisplay();
	CWeapon*		GetEquipWeapon();
		
	CollisionManager::IntersectRecord& GetIntersectionRecord();

	void	SetVertical(float vertical) { m_fHeadVertical += vertical; }

protected:
	virtual void InterpretInput(float fTime);			//ĳ������ �⺻ ���� ����
	void		 UpdateBipedRootTransform(float fTime); //Biped Transform
	void		 UpdateCollisionBasePosition(float fTime, const NiMatrix3& kDesireRot,NiPoint3& kLocVelocity);
	void		 UpdateAccuracy();
	void		 RecoveryBuddy(float fTime);		//������ ���� ã��
	bool		 CreateCrossHair();						//ũ�ν���� ����

	//�浹 ����-----------------------------------------------------------
	//CollsionManager::CharacterCollisionCallback�� ���� ä������.
	CollisionManager::IntersectRecord	m_kIntersectionRecord;
	bool				m_bCollision;					//�̹��� �浹�� �Ǿ�����
	NiPoint3			m_kLastSafeSlideN;

	
	//---------------------------------------------------------------------
	//ĳ���� ����
	CWeapon*			m_pkEquipWeapon;				//������ ����
	CPrimaryWeapon*		m_pkWeapon;						//ĳ���� �ֹ���

	//------�� �߻� ���� ������ ------------
	CrossHair*			m_kCrossHairObject;					//ũ�ν���� ǥ��
	NiPoint2			m_kFirePoint;						//�߻���
	float				m_fAccuracy;						//��Ȯ��
	float				m_fEffectAccuracy;					//��Ȯ���� ������ �ִ� ���� �⺻ 5
	
	unsigned int		m_SuccessiveCount;				//���� �߻�� ī����
	unsigned int		m_AccuracyRateBonus;			//��Ȯ�� ���� ���ʽ� ���� -1
	//--------------------------------------
	
	//ĳ���� �ִϸ��̼� ���� ���� ----------------------------------------
	ProcessAnimation	m_ProcessAnimation;					
	float				m_fHeadVertical;
	
	//============= ���� ȸ���� ���� ���� ==================
	NiPick				m_RecoveryPick;
	float				m_fRecoveryTime;						//ȸ�� ���ӽð�
	bool				m_bRecover;

	class CharacterCallbackObject : public NiActorManager::CallbackObject
	{
	public:
		//�ݹ� �������̵� �Լ�
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
		
		bool	m_bJumpData;	//���� �����ߴ��� ����
		bool	m_bJumpable;	//���� ���� �÷���
	
		float	m_fJumpForce;	//�����ϴ� ��
		float	m_fCurrTime;	//���� �ð�
		float	m_fEndTime;		//
	};

	CharacterCallbackObject*	m_pkCallBack;
};

#include "CUserCharacter.inl"

#endif