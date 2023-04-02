#include "CollisionManager.h"
#include "WorldManager.h"
//#include "CharacterSoldier.h"
//#include "ZFLog.h"

#include <NiCollision.h>


CollisionManager::CollisionManager()
{
	//지형 PICK
	m_kScenePick.SetPickType(NiPick::FIND_FIRST);
    m_kScenePick.SetSortType(NiPick::NO_SORT);
	m_kScenePick.SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	m_kScenePick.SetCoordinateType(NiPick::WORLD_COORDINATES);
    m_kScenePick.SetFrontOnly(true);
    m_kScenePick.SetObserveAppCullFlag(false);
    m_kScenePick.SetReturnTexture(false);
    m_kScenePick.SetReturnNormal(true);
    m_kScenePick.SetReturnSmoothNormal(false);
    m_kScenePick.SetReturnColor(false);	

	
	//동적 컬리션 시스템 허용
	//UpdateWorldData에서 월드 속도를 계산할지의 여부를 구하고 설정합니다
	NiCollisionData::SetEnableVelocity(true);

	m_pkCollisionRoot	= NiNew NiNode();

	m_pkCollisionRoot->SetName( "Collision" );
	
	m_pkPickRoot		= NULL;
	m_pkCharacterABV	= NULL;
	m_pkCharacterCollisionGroup	= NULL;

}

CollisionManager::~CollisionManager()
{		
	NiDelete m_pkCharacterCollisionGroup;
}

bool CollisionManager::Initialize(NiNode *pkCollisionRoot, NiNode *pkCharABV, NiNode* pkPickRoot,std::vector<CCharacter*>& vecCollider)
{
	m_pkCharacterCollisionGroup = NiNew NiCollisionGroup;

	if(!m_pkCharacterCollisionGroup)
	{
		NiMessageBox("CollsionManager::Initialize - NiCollisionGroup Create Failed",NULL);
		return false;
	}

	if(!pkCollisionRoot || !pkCharABV || !pkPickRoot)
	{
		NiMessageBox("CollisionManager::Initialize ",NULL);
		return false;
	}
	m_kScenePick.SetTarget(pkPickRoot);
	
	//어플리케이션이 나중의 피킹 연산을 위해 씬 그래프 서브트리를 설정하거나 제거하게 해줍니다. 
	//NiPick 객체는 한 번에 한 개의 타깃 노드만 가지며, 서로 다른 객체들로 SetTarget을 여러 번 
	//호출해도 그 타깃만 바뀐다는 점에 주의하십시오.
	m_pkCharacterABV = pkCharABV;

	NiCollisionData* pkData = NiDynamicCast(NiCollisionData,m_pkCharacterABV->GetCollisionObject());
	if(!pkData)
	{
		NiMessageBox("CollisionManager::Initialize -- Failure\n",NULL);
		return false;
	}
	
	pkData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
	pkData->SetCollisionMode(NiCollisionData::USE_ABV);
	//콜백함수 추가
	pkData->SetCollideCallback(CharacterCollisionsCallback);
	pkData->SetCollideCallbackData(WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter());
	
	m_pkCharacterCollisionGroup->AddCollider(m_pkCharacterABV);
	m_pkCharacterCollisionGroup->AddCollidee(pkCollisionRoot);

	//다른 캐릭터를 Collider로 추가
	for(unsigned int i=0;i<vecCollider.size();i++)
	{
		m_pkCharacterCollisionGroup->AddCollider(vecCollider[i]->GetABVRoot());
	}

	//ABV를 가지고 있는 collider 트리나 collidee 트리의 월드 스페이스 ABV를 업데이트 합니다.
	//움직이는 collidee는 NiCollisionData::UpdateWorldData를 통해 개별적으로 업데이트 해야 합니다.
	m_pkCharacterCollisionGroup->UpdateWorldData();

	return true;
}

bool CollisionManager::Initialize( NiNode* pkCharABV, std::vector<CCharacter*>& vecCollider )
{
	m_pkCharacterCollisionGroup = NiNew NiCollisionGroup;

	if(!m_pkCharacterCollisionGroup)
	{
		NiMessageBox("CollsionManager::Initialize - NiCollisionGroup Create Failed",NULL);
		return false;
	}

	m_pkCharacterABV = pkCharABV;

	NiCollisionData* pkData = NiDynamicCast(NiCollisionData,m_pkCharacterABV->GetCollisionObject());
	if(!pkData)
	{
		NiMessageBox("CollisionManager::Initialize -- Failure\n",NULL);
		return false;
	}
	
	pkData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
	pkData->SetCollisionMode(NiCollisionData::USE_ABV);
	//콜백함수 추가
	pkData->SetCollideCallback(CharacterCollisionsCallback);
	pkData->SetCollideCallbackData(WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter());
	
	m_pkCharacterCollisionGroup->AddCollider( m_pkCharacterABV );
	m_pkCharacterCollisionGroup->AddCollidee( m_pkCollisionRoot );

	//다른 캐릭터를 Collider로 추가
	for(unsigned int i=0;i<vecCollider.size();i++)
	{
		m_pkCharacterCollisionGroup->AddCollider(vecCollider[i]->GetABVRoot());
	}

	//ABV를 가지고 있는 collider 트리나 collidee 트리의 월드 스페이스 ABV를 업데이트 합니다.
	//움직이는 collidee는 NiCollisionData::UpdateWorldData를 통해 개별적으로 업데이트 해야 합니다.
	m_pkCharacterCollisionGroup->UpdateWorldData();

	return true;

}

//	픽킹 타겟 지정
void CollisionManager::SetPickTarget(  NiNode* pkPickRoot )
{
	m_kScenePick.SetTarget(pkPickRoot);
}



//	충돌 체크 Collidee에 오브젝트 1개 추가 제거
void CollisionManager::AddCollidee( NiNode *pCollidee, const NiTransform& transform )
{
	if( m_pkCollisionRoot )
	{
		NiCloningProcess kProcess;
		kProcess.m_eCopyType = NiObjectNET::COPY_EXACT;
		NiNode *pClon = (NiNode*)pCollidee->Clone(kProcess);

		const NiTransform &vTransformForCollision = transform * pClon->GetLocalTransform();

		pClon->SetTranslate( vTransformForCollision.m_Translate );
		pClon->SetRotate( vTransformForCollision.m_Rotate );
		pClon->SetScale( vTransformForCollision.m_fScale );
		pClon->Update(0.0f);
		pClon->UpdateWorldBound();
		pClon->UpdateProperties();
		pClon->UpdateEffects();

		m_pkCollisionRoot->AttachChild( pClon );//pClon );//pCollidee );
		m_pkCollisionRoot->Update( 0.0f );
		m_pkCollisionRoot->UpdateProperties();
		m_pkCollisionRoot->UpdateEffects();
		m_pkCollisionRoot->UpdateWorldBound();
	}
}

void CollisionManager::AddCollideeForMap( NiNode *pCollidee )
{
	if( m_pkCollisionRoot )
	{
		NiCloningProcess kProcess;
		kProcess.m_eCopyType = NiObjectNET::COPY_EXACT;
		NiNode *pClon = (NiNode*)pCollidee->Clone(kProcess);

		pClon->Update(0.0f);
		pClon->UpdateWorldBound();
		pClon->UpdateProperties();
		pClon->UpdateEffects();

		m_pkCollisionRoot->AttachChild( pClon );//pClon );//pCollidee );
		m_pkCollisionRoot->Update( 0.0f );
		m_pkCollisionRoot->UpdateProperties();
		m_pkCollisionRoot->UpdateEffects();
		m_pkCollisionRoot->UpdateWorldBound();
	}
}

void	CollisionManager::SetCollisionTransform( const NiTransform &transform )
{
	if( m_pkCollisionRoot )
	{
		m_pkCollisionRoot->SetTranslate( transform.m_Translate );
		m_pkCollisionRoot->SetRotate( transform.m_Rotate );
		m_pkCollisionRoot->SetScale( transform.m_fScale );

		m_pkCollisionRoot->Update(0.0f);
		m_pkCollisionRoot->UpdateWorldBound();
	}
}

void CollisionManager::RemoveCollidee( NiAVObject *pCollidee )
{
	if( m_pkCollisionRoot )
	{
		m_pkCollisionRoot->DetachChild( pCollidee );
		m_pkCollisionRoot->Update( 0.0f );
	}
}

//고정된 collidee와 collider와 충돌됐는지 확인
void CollisionManager::FindCollision(float fDelta)
{
	NiCollisionData* pkData = NiGetCollisionData(m_pkCharacterABV);
	assert(pkData);
	pkData->UpdateWorldData();

	CUserCharacter* pkCharacterSolder = (CUserCharacter*)pkData->GetCollideCallbackData();
	assert(pkCharacterSolder);
	pkCharacterSolder->ResetIntersectionRecord();		// 캐릭터의 intersectRecord 초기화
	
	m_pkCharacterCollisionGroup->FindCollisions(fDelta);
}

//움직이는 collider와 collidee와 충돌 처리 
bool CollisionManager::TestCollision(float fDelta)
{
	NiCollisionData* pkData = NiGetCollisionData(m_pkCharacterABV);
	assert(pkData);
	pkData->UpdateWorldData();

	return m_pkCharacterCollisionGroup->TestCollisions(fDelta);
}


int CollisionManager::CharacterCollisionsCallback(
		NiCollisionGroup::Intersect& kIntersect)
{
	NiNode*		pkSoldierABVNode = (NiNode*)kIntersect.pkRoot0;
	NiPoint3	kSoldierNormal = kIntersect.kNormal0;
	NiPoint3	kObjectNormal  = kIntersect.kNormal1;

	//임시로 기본 유저 캐릭터를 받아서 충돌을 처리한다 후에 여러 캐릭터가 추가가 되면 
	//추가적인 처리가 필요하다
	NiCollisionData*	pkData = NiGetCollisionData(pkSoldierABVNode);
	CUserCharacter*	pkSoldier = (CUserCharacter*)pkData->GetCollideCallbackData();
	assert(pkSoldier);

	//군인의 IntersectRecord를 갱신하기 위해 가져온다.
	CollisionManager::IntersectRecord& kRecord = 
		pkSoldier->GetIntersectionRecord();

	kRecord.m_bInCollision = true;	//충돌되었다는 것을 군인에게 알리자
	
	// Keep track of the minimum time at which the Soldier will collide
    // with another object.  Want first intersection only...
	if (kIntersect.fTime < kRecord.m_fTime)
    {
        kRecord.m_fTime = kIntersect.fTime;
        kRecord.m_kPoint = kIntersect.kPoint;
		kRecord.m_kCharNormal = kSoldierNormal;
        kRecord.m_kObjectNormal = kObjectNormal;
    }

	 return NiCollisionGroup::CONTINUE_COLLISIONS;
}

//건물 바운딩 박스에 올라타기 위해 광선을 쏜다.
bool CollisionManager::PickScene(const NiPoint3 &kStart, const NiPoint3 &kDir)
{
	if(m_kScenePick.PickObjects(kStart,kDir))
	{
		//충돌된 오브젝트가 있을 경우
		NiPick::Record* pkRecord = m_kScenePick.GetResults().GetAt(0);
		return pkRecord !=0;
	}

	return false;
}

bool CollisionManager::GetObjectHeight(float x,float y,float& z)
{
	if(m_kScenePick.PickObjects( NiPoint3( x , y , z + 1.65f ), -NiPoint3::UNIT_Z ) )
	{
		//결과 저장
		NiPick::Record* pkRecord = m_kScenePick.GetResults().GetAt(0);

		if(pkRecord)
		{
			float fHeight = pkRecord->GetIntersection().z;		//높이를 읽어온다.

			if(fHeight < z + 2.0f)	//오브젝트의 높이가 캐릭터의 높이 + 2m보다 적을 경우만 올라간다.
			{
				z = fHeight;
				return true;
			}			
		}
	}

	return false;
}
