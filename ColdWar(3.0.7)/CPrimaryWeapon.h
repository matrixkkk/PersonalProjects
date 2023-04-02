#ifndef CPRIMARYWEAPON_H_
#define CPRIMARYWEAPON_H_

#include "main.h"
#include "Weapon.h"

class CPrimaryWeapon : public CWeapon
{
public:
	enum Name{M4A1,L85A2};

	CPrimaryWeapon(Name eName);

	void	FireStart();	//총 발사 시작
	void	FireEnd();				//총 발사 끝
	void	Reload();					//재장전
	
	bool 	UseWeapon(float fTime,bool* bEmptyBullet);
	
	unsigned int		GetAccuracy();	//정확도 리턴	
	unsigned int		GetGunReaction();
	unsigned int		GetGunDamage();				//총 데미지 리턴

	NiNode*				GetObject();				//총 지오메트리 리턴
	BulletData			GetBulletData();			//--추가


private:
	NiNode*			pkGunGeometry;			//총 지오메트리

	unsigned int	m_uiDamage;				//데미지
	float			m_fSecPerFire;			//연사속도 

	BulletData		m_BulletData;		//총알에 대한 정보

	bool			m_bFire;			//발사중인지 아닌지를 나타낸다.
	float			m_lastFireTime;		//발사 시작 타임.
	unsigned int 	m_GunReaction;		//총 반동
	unsigned int	m_AccuracyRate;		//총의 정확도  
};

#endif

