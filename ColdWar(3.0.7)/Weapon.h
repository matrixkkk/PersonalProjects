/*----CWeapon Ŭ����-----
��� ���� Ŭ������ �θ� Ŭ�������� �߻� Ŭ�����̴�.
ĳ���Ͱ� � ���⸦ �����ߴ����� ���� ����������� ��Ÿ���� ����
��,���� ���� Ŭ������ ����Ű�� ���� ���
*/


#ifndef WEAPON_H_
#define WEAPON_H_

//--�߰�
struct BulletData
{
	unsigned int remainBullet;		//���� �Ѿ� ��
	unsigned int maxBullet;			//�ִ� �Ѿ� ��
};


class CWeapon : public NiMemObject
{
public:
	CWeapon() {};

	virtual void	FireStart() = 0;		//�� �߻� ����
	virtual void	FireEnd() = 0;					//�� �߻� ��
	virtual void	Reload()  = 0;					//������

	virtual NiNode* GetObject() = 0;
	virtual bool	UseWeapon(float fTime,bool* bEmptyBullet)  = 0;	//������ Ȯ��	
	
	virtual unsigned int		GetAccuracy() = 0;				//��Ȯ�� ����
	virtual unsigned int		GetGunReaction() = 0;			//�ݵ� ����
	virtual unsigned int		GetGunDamage() = 0;				//�� ������ ����

	virtual BulletData			GetBulletData() = 0;			//--�߰�

};
	

#endif