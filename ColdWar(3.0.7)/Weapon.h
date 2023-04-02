/*----CWeapon 클래스-----
모든 무기 클래스의 부모 클래스이자 추상 클래스이다.
캐릭터가 어떤 무기를 장착했는지를 현재 사용중인지를 나타내기 위해
주,보조 무기 클래스를 가르키기 위해 사용
*/


#ifndef WEAPON_H_
#define WEAPON_H_

//--추가
struct BulletData
{
	unsigned int remainBullet;		//남은 총알 수
	unsigned int maxBullet;			//최대 총알 수
};


class CWeapon : public NiMemObject
{
public:
	CWeapon() {};

	virtual void	FireStart() = 0;		//총 발사 시작
	virtual void	FireEnd() = 0;					//총 발사 끝
	virtual void	Reload()  = 0;					//재장전

	virtual NiNode* GetObject() = 0;
	virtual bool	UseWeapon(float fTime,bool* bEmptyBullet)  = 0;	//무기사용 확인	
	
	virtual unsigned int		GetAccuracy() = 0;				//정확도 리턴
	virtual unsigned int		GetGunReaction() = 0;			//반동 리턴
	virtual unsigned int		GetGunDamage() = 0;				//총 데미지 리턴

	virtual BulletData			GetBulletData() = 0;			//--추가

};
	

#endif