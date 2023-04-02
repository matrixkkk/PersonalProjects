#ifndef CARMY_H_
#define CARMY_H_

#include "CPrimaryWeapon.h"
#include "Weapon.h"
#include "CrossHair.h"

#include "main.h"

//추상 클래스
class CArmy
{
public:
	CArmy();
	virtual ~CArmy();

	virtual void InterpretInput(float fTime) = 0;	//캐릭터의 기본 동작 구현
	virtual void UseEquipWeapon(float fTime) = 0;	//주장비 아이템 사용
	
	void	ProcessMouse();							//마우스 처리

	static	CArmy* GetInstance();
protected:
	static CArmy*		mp_Army;

	//무기 정보
	CWeapon*			m_pkEquipWeapon;				//장착된 무기
	CPrimaryWeapon*		m_pkPrimaryWeapon;						//캐릭터 주무기
	//CSecondWeapon*	m_pkSecondWeapon;				//보조 무기
	//CNifeWeapon*		m_pkNifeWeapon;					//근접 무기
	//CThrowWeapon*		m_pkThrowWeapon;				//투척 무기
	//CClassItem*		m_pkClassItem;					//직업 아이템

	//============= 총 발사 관련 변수 =================
	CrossHair*			m_kCrossHairObject;					//크로스헤어 표시
	NiPoint2			m_kFirePoint;						//발사점
	
	unsigned int		m_SuccessiveCount;				//연속 발사수 카운터
	unsigned int		m_AccuracyRateBonus;			//정확도 증가 보너스 보병 -1

	float	m_fSpeed;					//이동속도
	float	m_fReloadSpeed;			//장전속도
	float	m_fSumAccuracy;			//정확도 합
	float	m_fEffectAccuracy;		//정확도에 영향을 주는 요인 기본 5
	float	m_fVital;				//생명력 Tic당 감소되는 Hp
};

#endif
