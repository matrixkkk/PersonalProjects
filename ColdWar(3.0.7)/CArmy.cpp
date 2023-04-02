#include "CArmy.h"

CArmy*	CArmy::	mp_Army = NULL;


CArmy::CArmy()
{
	m_pkEquipWeapon     = NULL;
	m_pkPrimaryWeapon	= NULL;
	m_kCrossHairObject	= NULL;

	m_SuccessiveCount   = 0;				//연속 발사수 카운터
	m_AccuracyRateBonus = 0;			//정확도 증가 보너스 보병 -1

	m_fSpeed		  = 0.0f;			//이동속도
	m_fReloadSpeed	  = 0.0f;			//장전속도
	m_fSumAccuracy	  = 0.0f;			//정확도 합
	m_fEffectAccuracy = 0.0f;			//정확도에 영향을 주는 요인 기본 5
	m_fVital		  = 0.0f;		
}

CArmy::~CArmy()
{
	if(m_pkPrimaryWeapon)
		NiDelete m_pkPrimaryWeapon;
	m_pkPrimaryWeapon = NULL;

	if(m_kCrossHairObject)
		NiDelete m_kCrossHairObject;
	m_kCrossHairObject = NULL;
}
	