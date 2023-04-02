
/*
	Ŭ���� ��: CollisionManager 
	Ŭ���� �뵵 : ĳ���Ϳ� ������Ʈ���� �浹�� ó���ϴ� Ŭ����


*/
#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_


#include <NiCollisionGroup.h>
#include <NiPick.h>				//��ŷ�� ���� ĳ������ ���̸� �����ϱ� ����
#include <vector>
#include "CCharacter.h"

class CollisionManager : public NiMemObject
{
public:
	CollisionManager();
	~CollisionManager();

	bool Initialize(NiNode* pkCollisionRoot, NiNode* pkCharABV,NiNode* pkPickRoot,std::vector<CCharacter*>& vecCollider );

	bool Initialize( NiNode* pkCharABV, std::vector<CCharacter*>& vecCollider );
	
	void FindCollision(float fDelta);
	bool TestCollision(float fDelta);

	//	�浹 üũ Collidee�� ������Ʈ 1�� �߰� ����
	void AddCollidee( NiNode *pCollidee, const NiTransform& transform  );
	void AddCollideeForMap( NiNode *pCollidee );

	void RemoveCollidee( NiAVObject *pCollidee );

	//	��ŷ Ÿ�� ����
	void SetPickTarget(  NiNode* pkPickRoot );
	
	//�Լ��� �뵵 = Pick�� ��ϵ� �� �׷����� ������ ���� ó���� ���� ���� ���
	//NiPick�� �� �׷����� ����ؼ� �׿� ���� ��ŷ ó���� �ϴ� �Լ�
	//�ٿ�� �ڽ����� ���̸� �������� ���� ���δ�.
	bool PickScene(const NiPoint3& kStart,const NiPoint3& kDir);
	

	NiPick::Results& CollisionManager::GetPickResults();		//��ŷ ��� ����

	//������Ʈ�� ���̸� ��ŷ������ �̿��ؼ� ���Ѵ�.
	bool GetObjectHeight(float x,float y,float& z); 

	
	void	SetCollisionTransform( const NiTransform &transform );
	
	struct IntersectRecord
	{
		bool		m_bInCollision;
		float		m_fTime;
		NiPoint3	m_kPoint;
		NiPoint3	m_kCharNormal;		//ĳ���� ���		
		NiPoint3	m_kObjectNormal;	//������Ʈ ���
	};

	static int CharacterCollisionsCallback(
		NiCollisionGroup::Intersect& kIntersect);

protected:

	NiNode*				m_pkCollisionRoot;
	NiNode*				m_pkPickRoot;

	NiNode*				m_pkCharacterABV;
	NiCollisionGroup*	m_pkCharacterCollisionGroup;
	NiPick				m_kScenePick;
	
};

#include "CollisionManager.inl"

#endif
