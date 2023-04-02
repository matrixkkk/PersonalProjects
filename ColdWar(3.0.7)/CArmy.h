#ifndef CARMY_H_
#define CARMY_H_

#include "CPrimaryWeapon.h"
#include "Weapon.h"
#include "CrossHair.h"

#include "main.h"

//�߻� Ŭ����
class CArmy
{
public:
	CArmy();
	virtual ~CArmy();

	virtual void InterpretInput(float fTime) = 0;	//ĳ������ �⺻ ���� ����
	virtual void UseEquipWeapon(float fTime) = 0;	//����� ������ ���
	
	void	ProcessMouse();							//���콺 ó��

	static	CArmy* GetInstance();
protected:
	static CArmy*		mp_Army;

	//���� ����
	CWeapon*			m_pkEquipWeapon;				//������ ����
	CPrimaryWeapon*		m_pkPrimaryWeapon;						//ĳ���� �ֹ���
	//CSecondWeapon*	m_pkSecondWeapon;				//���� ����
	//CNifeWeapon*		m_pkNifeWeapon;					//���� ����
	//CThrowWeapon*		m_pkThrowWeapon;				//��ô ����
	//CClassItem*		m_pkClassItem;					//���� ������

	//============= �� �߻� ���� ���� =================
	CrossHair*			m_kCrossHairObject;					//ũ�ν���� ǥ��
	NiPoint2			m_kFirePoint;						//�߻���
	
	unsigned int		m_SuccessiveCount;				//���� �߻�� ī����
	unsigned int		m_AccuracyRateBonus;			//��Ȯ�� ���� ���ʽ� ���� -1

	float	m_fSpeed;					//�̵��ӵ�
	float	m_fReloadSpeed;			//�����ӵ�
	float	m_fSumAccuracy;			//��Ȯ�� ��
	float	m_fEffectAccuracy;		//��Ȯ���� ������ �ִ� ���� �⺻ 5
	float	m_fVital;				//����� Tic�� ���ҵǴ� Hp
};

#endif
