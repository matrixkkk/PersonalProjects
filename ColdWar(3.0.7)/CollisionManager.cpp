#include "CollisionManager.h"
#include "WorldManager.h"
//#include "CharacterSoldier.h"
//#include "ZFLog.h"

#include <NiCollision.h>


CollisionManager::CollisionManager()
{
	//���� PICK
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

	
	//���� �ø��� �ý��� ���
	//UpdateWorldData���� ���� �ӵ��� ��������� ���θ� ���ϰ� �����մϴ�
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
	
	//���ø����̼��� ������ ��ŷ ������ ���� �� �׷��� ����Ʈ���� �����ϰų� �����ϰ� ���ݴϴ�. 
	//NiPick ��ü�� �� ���� �� ���� Ÿ�� ��常 ������, ���� �ٸ� ��ü��� SetTarget�� ���� �� 
	//ȣ���ص� �� Ÿ�길 �ٲ�ٴ� ���� �����Ͻʽÿ�.
	m_pkCharacterABV = pkCharABV;

	NiCollisionData* pkData = NiDynamicCast(NiCollisionData,m_pkCharacterABV->GetCollisionObject());
	if(!pkData)
	{
		NiMessageBox("CollisionManager::Initialize -- Failure\n",NULL);
		return false;
	}
	
	pkData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
	pkData->SetCollisionMode(NiCollisionData::USE_ABV);
	//�ݹ��Լ� �߰�
	pkData->SetCollideCallback(CharacterCollisionsCallback);
	pkData->SetCollideCallbackData(WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter());
	
	m_pkCharacterCollisionGroup->AddCollider(m_pkCharacterABV);
	m_pkCharacterCollisionGroup->AddCollidee(pkCollisionRoot);

	//�ٸ� ĳ���͸� Collider�� �߰�
	for(unsigned int i=0;i<vecCollider.size();i++)
	{
		m_pkCharacterCollisionGroup->AddCollider(vecCollider[i]->GetABVRoot());
	}

	//ABV�� ������ �ִ� collider Ʈ���� collidee Ʈ���� ���� �����̽� ABV�� ������Ʈ �մϴ�.
	//�����̴� collidee�� NiCollisionData::UpdateWorldData�� ���� ���������� ������Ʈ �ؾ� �մϴ�.
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
	//�ݹ��Լ� �߰�
	pkData->SetCollideCallback(CharacterCollisionsCallback);
	pkData->SetCollideCallbackData(WorldManager::GetWorldManager()->GetCharacterManager()->GetUserCharacter());
	
	m_pkCharacterCollisionGroup->AddCollider( m_pkCharacterABV );
	m_pkCharacterCollisionGroup->AddCollidee( m_pkCollisionRoot );

	//�ٸ� ĳ���͸� Collider�� �߰�
	for(unsigned int i=0;i<vecCollider.size();i++)
	{
		m_pkCharacterCollisionGroup->AddCollider(vecCollider[i]->GetABVRoot());
	}

	//ABV�� ������ �ִ� collider Ʈ���� collidee Ʈ���� ���� �����̽� ABV�� ������Ʈ �մϴ�.
	//�����̴� collidee�� NiCollisionData::UpdateWorldData�� ���� ���������� ������Ʈ �ؾ� �մϴ�.
	m_pkCharacterCollisionGroup->UpdateWorldData();

	return true;

}

//	��ŷ Ÿ�� ����
void CollisionManager::SetPickTarget(  NiNode* pkPickRoot )
{
	m_kScenePick.SetTarget(pkPickRoot);
}



//	�浹 üũ Collidee�� ������Ʈ 1�� �߰� ����
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

//������ collidee�� collider�� �浹�ƴ��� Ȯ��
void CollisionManager::FindCollision(float fDelta)
{
	NiCollisionData* pkData = NiGetCollisionData(m_pkCharacterABV);
	assert(pkData);
	pkData->UpdateWorldData();

	CUserCharacter* pkCharacterSolder = (CUserCharacter*)pkData->GetCollideCallbackData();
	assert(pkCharacterSolder);
	pkCharacterSolder->ResetIntersectionRecord();		// ĳ������ intersectRecord �ʱ�ȭ
	
	m_pkCharacterCollisionGroup->FindCollisions(fDelta);
}

//�����̴� collider�� collidee�� �浹 ó�� 
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

	//�ӽ÷� �⺻ ���� ĳ���͸� �޾Ƽ� �浹�� ó���Ѵ� �Ŀ� ���� ĳ���Ͱ� �߰��� �Ǹ� 
	//�߰����� ó���� �ʿ��ϴ�
	NiCollisionData*	pkData = NiGetCollisionData(pkSoldierABVNode);
	CUserCharacter*	pkSoldier = (CUserCharacter*)pkData->GetCollideCallbackData();
	assert(pkSoldier);

	//������ IntersectRecord�� �����ϱ� ���� �����´�.
	CollisionManager::IntersectRecord& kRecord = 
		pkSoldier->GetIntersectionRecord();

	kRecord.m_bInCollision = true;	//�浹�Ǿ��ٴ� ���� ���ο��� �˸���
	
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

//�ǹ� �ٿ�� �ڽ��� �ö�Ÿ�� ���� ������ ���.
bool CollisionManager::PickScene(const NiPoint3 &kStart, const NiPoint3 &kDir)
{
	if(m_kScenePick.PickObjects(kStart,kDir))
	{
		//�浹�� ������Ʈ�� ���� ���
		NiPick::Record* pkRecord = m_kScenePick.GetResults().GetAt(0);
		return pkRecord !=0;
	}

	return false;
}

bool CollisionManager::GetObjectHeight(float x,float y,float& z)
{
	if(m_kScenePick.PickObjects( NiPoint3( x , y , z + 1.65f ), -NiPoint3::UNIT_Z ) )
	{
		//��� ����
		NiPick::Record* pkRecord = m_kScenePick.GetResults().GetAt(0);

		if(pkRecord)
		{
			float fHeight = pkRecord->GetIntersection().z;		//���̸� �о�´�.

			if(fHeight < z + 2.0f)	//������Ʈ�� ���̰� ĳ������ ���� + 2m���� ���� ��츸 �ö󰣴�.
			{
				z = fHeight;
				return true;
			}			
		}
	}

	return false;
}
