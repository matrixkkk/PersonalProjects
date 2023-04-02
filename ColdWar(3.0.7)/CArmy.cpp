#include "CArmy.h"

CArmy*	CArmy::	mp_Army = NULL;


CArmy::CArmy()
{
	m_pkEquipWeapon     = NULL;
	m_pkPrimaryWeapon	= NULL;
	m_kCrossHairObject	= NULL;

	m_SuccessiveCount   = 0;				//���� �߻�� ī����
	m_AccuracyRateBonus = 0;			//��Ȯ�� ���� ���ʽ� ���� -1

	m_fSpeed		  = 0.0f;			//�̵��ӵ�
	m_fReloadSpeed	  = 0.0f;			//�����ӵ�
	m_fSumAccuracy	  = 0.0f;			//��Ȯ�� ��
	m_fEffectAccuracy = 0.0f;			//��Ȯ���� ������ �ִ� ���� �⺻ 5
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
	