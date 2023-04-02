#include "CPrimaryWeapon.h"

//������
CPrimaryWeapon::CPrimaryWeapon(Name eName)
{
	NiStream kStream;

	bool bLoaded;
	
	//M4A1�� ����
	if(eName == M4A1)
	{
		bLoaded = kStream.Load("Data/object/M4A1.nif");
		
		pkGunGeometry = (NiNode*) kStream.GetObjectAt(0);			//�� ������Ʈ��

		m_uiDamage = 20;				//������
		m_fSecPerFire = 1.0f/15.0f;		//����ӵ�  ��/�߻��

		m_BulletData.maxBullet = 30;	//��ź��
		m_BulletData.remainBullet = 30; //��źâ�� �ִ� ���� �߼�

		m_AccuracyRate = 5;				//���� ��Ȯ��
		m_GunReaction = 5;				//���� �ݵ�
	}

	m_lastFireTime = 0.0f;
	m_bFire = false;

}

unsigned int CPrimaryWeapon::GetAccuracy()
{
	return m_AccuracyRate;
}

unsigned int CPrimaryWeapon::GetGunReaction()
{
	return m_GunReaction;
}

unsigned int CPrimaryWeapon::GetGunDamage()
{
	return m_uiDamage;
}

//�� �߻� ����
void CPrimaryWeapon::FireStart()
{
	m_bFire = true;				//�߻���·� ����
	m_lastFireTime = 0.0f;		//�ð� ����
}

//���� �߻��ϴ��� ���ϴ��� üũ
bool CPrimaryWeapon::UseWeapon(float fTime,bool* bEmptyBullet)
{
	if(m_bFire)		//�߻� ���� ��쿡�� 
	{	
		//���� �ӵ� üũ
		if( (fTime - m_lastFireTime) > m_fSecPerFire || m_lastFireTime == 0)
		{
			//��źȮ��
			if(m_BulletData.remainBullet > 0 )
			{
				m_lastFireTime = fTime;			//������ �߻� �ð� ����
				m_BulletData.remainBullet--;	//�Ѿ� �� ����
				return true;
			}
			else
				*bEmptyBullet = true;
		}
	}
	return false;
}

//�������
void CPrimaryWeapon::FireEnd()
{
	m_bFire = false;
	m_lastFireTime = 0.0f;
}

//������
void CPrimaryWeapon::Reload()
{
	m_BulletData.remainBullet = m_BulletData.maxBullet;
}

//������Ʈ�� ��ȯ
NiNode* CPrimaryWeapon::GetObject()
{
	return pkGunGeometry;
}

BulletData	CPrimaryWeapon::GetBulletData()
{
	return m_BulletData;
}


