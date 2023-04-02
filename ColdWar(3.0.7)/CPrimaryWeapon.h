#ifndef CPRIMARYWEAPON_H_
#define CPRIMARYWEAPON_H_

#include "main.h"
#include "Weapon.h"

class CPrimaryWeapon : public CWeapon
{
public:
	enum Name{M4A1,L85A2};

	CPrimaryWeapon(Name eName);

	void	FireStart();	//�� �߻� ����
	void	FireEnd();				//�� �߻� ��
	void	Reload();					//������
	
	bool 	UseWeapon(float fTime,bool* bEmptyBullet);
	
	unsigned int		GetAccuracy();	//��Ȯ�� ����	
	unsigned int		GetGunReaction();
	unsigned int		GetGunDamage();				//�� ������ ����

	NiNode*				GetObject();				//�� ������Ʈ�� ����
	BulletData			GetBulletData();			//--�߰�


private:
	NiNode*			pkGunGeometry;			//�� ������Ʈ��

	unsigned int	m_uiDamage;				//������
	float			m_fSecPerFire;			//����ӵ� 

	BulletData		m_BulletData;		//�Ѿ˿� ���� ����

	bool			m_bFire;			//�߻������� �ƴ����� ��Ÿ����.
	float			m_lastFireTime;		//�߻� ���� Ÿ��.
	unsigned int 	m_GunReaction;		//�� �ݵ�
	unsigned int	m_AccuracyRate;		//���� ��Ȯ��  
};

#endif

