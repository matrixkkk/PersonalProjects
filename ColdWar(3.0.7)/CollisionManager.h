
/*
	클래스 명: CollisionManager 
	클래스 용도 : 캐릭터와 오브젝트간의 충돌을 처리하는 클래스


*/
#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_


#include <NiCollisionGroup.h>
#include <NiPick.h>				//피킹을 통해 캐릭터의 높이를 설정하기 위해
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

	//	충돌 체크 Collidee에 오브젝트 1개 추가 제거
	void AddCollidee( NiNode *pCollidee, const NiTransform& transform  );
	void AddCollideeForMap( NiNode *pCollidee );

	void RemoveCollidee( NiAVObject *pCollidee );

	//	픽킹 타겟 지정
	void SetPickTarget(  NiNode* pkPickRoot );
	
	//함수의 용도 = Pick에 등록된 씬 그래프에 광선을 쏴서 처리할 일이 있을 경우
	//NiPick에 씬 그래프를 등록해서 그에 따른 픽킹 처리를 하는 함수
	//바운딩 박스에서 높이를 가져오기 위해 쓰인다.
	bool PickScene(const NiPoint3& kStart,const NiPoint3& kDir);
	

	NiPick::Results& CollisionManager::GetPickResults();		//피킹 결과 리턴

	//오브젝트의 높이를 피킹광선을 이용해서 구한다.
	bool GetObjectHeight(float x,float y,float& z); 

	
	void	SetCollisionTransform( const NiTransform &transform );
	
	struct IntersectRecord
	{
		bool		m_bInCollision;
		float		m_fTime;
		NiPoint3	m_kPoint;
		NiPoint3	m_kCharNormal;		//캐릭터 노멀		
		NiPoint3	m_kObjectNormal;	//오브젝트 노멀
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
