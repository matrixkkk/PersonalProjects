#include "CPrimaryWeapon.h"

//생성자
CPrimaryWeapon::CPrimaryWeapon(Name eName)
{
	NiStream kStream;

	bool bLoaded;
	
	//M4A1의 설정
	if(eName == M4A1)
	{
		bLoaded = kStream.Load("Data/object/M4A1.nif");
		
		pkGunGeometry = (NiNode*) kStream.GetObjectAt(0);			//총 지오메트리

		m_uiDamage = 20;				//데미지
		m_fSecPerFire = 1.0f/15.0f;		//연사속도  초/발사수

		m_BulletData.maxBullet = 30;	//잔탄량
		m_BulletData.remainBullet = 30; //한탄창의 최대 저장 발수

		m_AccuracyRate = 5;				//총의 정확도
		m_GunReaction = 5;				//총의 반동
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

//총 발사 시작
void CPrimaryWeapon::FireStart()
{
	m_bFire = true;				//발사상태로 만듬
	m_lastFireTime = 0.0f;		//시간 측정
}

//총이 발사하는지 안하는지 체크
bool CPrimaryWeapon::UseWeapon(float fTime,bool* bEmptyBullet)
{
	if(m_bFire)		//발사 중일 경우에만 
	{	
		//연사 속도 체크
		if( (fTime - m_lastFireTime) > m_fSecPerFire || m_lastFireTime == 0)
		{
			//잔탄확인
			if(m_BulletData.remainBullet > 0 )
			{
				m_lastFireTime = fTime;			//마지막 발사 시간 저장
				m_BulletData.remainBullet--;	//총알 수 감소
				return true;
			}
			else
				*bEmptyBullet = true;
		}
	}
	return false;
}

//사격종료
void CPrimaryWeapon::FireEnd()
{
	m_bFire = false;
	m_lastFireTime = 0.0f;
}

//재장전
void CPrimaryWeapon::Reload()
{
	m_BulletData.remainBullet = m_BulletData.maxBullet;
}

//지오메트리 반환
NiNode* CPrimaryWeapon::GetObject()
{
	return pkGunGeometry;
}

BulletData	CPrimaryWeapon::GetBulletData()
{
	return m_BulletData;
}


