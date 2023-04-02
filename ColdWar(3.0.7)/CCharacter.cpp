#include "CCharacter.h"
#include "WorldManager.h"
#include "GameApp.h"
#include "NetWorkManager.h"
#include "GameSystemManager.h"
//#include "CGamePlayState.h"
//#include "CStateManager.h"

CCharacter::CCharacter() : CBiped()
{
	m_fLastUpdate = 0.0f;
	m_kMoveDir = NiPoint2::ZERO;
	m_fSpeed = 6.0f;								//�÷��̾� �⺻ ���ǵ�

	m_Gravity	= NiPoint3( 0.0f, 0.0f, -9.8f );	//�߷�
	m_dwStatus  = NORMAL;

	m_pkMuzle   = NULL;
}

CCharacter::~CCharacter()
{
	if(m_pkMuzle)
		NiDelete m_pkMuzle;

	NiDelete m_spActorManager;
}


bool CCharacter::Init(NiActorManager* pkActor,const NiTransform &kSpawn)
{		
	m_spActorManager = pkActor;
	assert(pkActor);

	//muZel �Ҵ�
	m_pkMuzle = NiNew CMuzelSplash(0.01f);

	//------------Collision setting-------------------

	//ABV������Ʈ collision�� ������ ����
	NiAVObject* pkABVRoot = m_spActorManager->GetNIFRoot()->GetObjectByName("root");
	assert(pkABVRoot);
	//�θ� ��带 �Ѱ��ݽô�.
	m_spABVRoot	= NiDynamicCast(NiNode,pkABVRoot);
	if(!m_spABVRoot)
	{
		NiMessageBox("CharacterSoldier::Initialize - ABV Root�� ã���� �����ϴ�.",NULL);
		return false;
	}
	NiCollisionData*	pkCD = NiGetCollisionData(pkABVRoot);
	assert(pkCD);
	assert(pkCD->GetModelSpaceABV());	//������Ʈ�� �� �����̽� ABV�� ���ؿɴϴ�.
	assert(pkCD->GetModelSpaceABV()->Type() == NiBoundingVolume::CAPSULE_BV);		//�ٿ�� ���� Ÿ�� Ȯ��
	((NiCapsuleBV*)pkCD->GetModelSpaceABV())->SetAxis(NiPoint3::UNIT_Z);
	NiPoint3 kCenter = ((NiCapsuleBV*)pkCD->GetModelSpaceABV())->GetCenter();
	kCenter.x = kCenter.z = 0.0f;
	((NiCapsuleBV*)pkCD->GetModelSpaceABV())->SetCenter(kCenter);
	

	//�⺻ �ִϸ��̼� ����
	m_spActorManager->Update(0.0f);
	m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	
	//ĳ������ �Ӹ��� ����, ���� ���� �ʱ�ȭ
	m_fHeadHorizon = 0.0f;
	m_fHeadVertical = 0.0f;
	
	//�ʱ� ĳ������ ȸ�� �̵� ��� ����
	m_kSpawnTranslate = kSpawn.m_Translate;
	m_kSpawnRotate = kSpawn.m_Rotate;
	m_spCharRoot->SetTranslate(m_kSpawnTranslate);
	m_spCharRoot->SetRotate(m_kSpawnRotate);

	//������ �����̱� ���� dummy��� ����
	m_spSpineBone = NiNew NiNode;

	m_spSpineBone->SetName("lookat");
	
	//m_spSpineBone�� ������ ���� ��� ã��
	NiNode* pkSpine1 = NiDynamicCast(NiNode,m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine1"));
	NiNode* pkSpine2 = NiDynamicCast(NiNode,m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine2"));

	//m_spSpineBone�� �߰��� ���� ��带 ����
	pkSpine1->AttachChild(m_spSpineBone);
	m_spSpineBone->AttachChild(pkSpine2);

	//��带 ���������Ƿ� transform�� �����ǰ� update
	pkSpine1->Update(0.0f);

	m_spSpineBone = NiDynamicCast(NiNode,m_spSpineBone);
	m_kBaseSpineRotate = m_spSpineBone->GetRotate();				//���� �����̼�
			
	CBiped::Init();
	
	if(!m_pkMuzle->Initialize( (NiNode*)pkActor->GetNIFRoot(),
		"MuzelSplash_front.tga", 
		"MuzelSplash_HV.tga") )
	{
		return false;
	}	

	return true;
}

void CCharacter::Reset()
{	
	//=========================================
	// �⺻ ���·� �ǵ�����
	//=========================================
	m_dwStatus = NORMAL;
	m_spActorManager->GetControllerManager()->DeactivateAll();
	//=========================================
	m_spActorManager->SetTargetAnimation( Animation_Enum::IDLE_RIFLE );
	
	CBiped::Reset();
}

void CCharacter::UpdateMuzel(float fAccumTime)
{
	m_pkMuzle->Update( fAccumTime );
}

void CCharacter::Update(float fTime)
{
	CBiped::Update(fTime);
	//--------------------------------------------------------------------------------------

	//ĳ���� ���콺�� ���� ���� ȸ���� ����
	//�丣�� ��� ���� ����� ���� �����̼��� ���ؿ´�.
	NiAVObject* pkSpine2 = m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine2");
	
	//���� �����̼��� ���ؿ´�.	
	float fPitch   = CameraManager::Get()->GetCameraPitch(); 
	//ȸ���� �ִ� �ּҸ� ���Ѵ�.
	float fPitchMin,fPitchMax;
	fPitchMax = NI_PI * 0.5f;
	fPitchMin = -NI_PI * 0.2f;
	if(fPitch < fPitchMin)	fPitch = fPitchMin;
	else if(fPitch > fPitchMax) fPitch = fPitchMax;	 
	
	NiMatrix3	kSpineZRotate,kSpineYRotate;

	kSpineYRotate.MakeYRotation(fPitch * 0.5f);		//����ġ
	kSpineZRotate.MakeZRotation(-fPitch);
	m_spSpineBone->SetRotate(kSpineZRotate * kSpineYRotate);
}

void CCharacter::DrawMuzel()
{
	if(m_pkMuzle) m_pkMuzle->RenderClick();
}

void CCharacter::Draw(NiCamera* pkCamera)
{
	WorldManager::GetWorldManager()->PartialRender(pkCamera, GetNIFRoot());

	if(m_pkMuzle) m_pkMuzle->RenderClick();
}


void CCharacter::ProcessFireResult( FireResult* pResult)
{
	if(m_dwStatus == pResult->m_dwCondition)		//���� ���¿� ���� ���°� ���ٸ� ����
			return;

	NiControllerSequence* pkSequence = NULL;
	NiControllerManager*  pkCM = m_spActorManager->GetControllerManager();

	DWORD eCode = m_spActorManager->GetTargetAnimation();
	pkSequence = m_spActorManager->GetSequence(eCode);

	// === ������ ���°� NORMAL ���°� �ƴ϶�� 3��Ī �������� ��ȯ ===
	if(pResult->m_dwCondition != NORMAL && pResult->m_dwResultID == NetworkManager::GetInstance()->GetID()) 
		CameraManager::Get()->ChangeView(THIRD_PERSON);
	
	//���� ����
	if(pResult->m_dwCondition == DEATH)
	{
		if(m_dwStatus == INCAPACITY)		//���� ���°� �λ� �����̸�
		{	
			pkSequence = m_spActorManager->GetSequence(Animation_Enum::INCAPACITY);
			pkSequence->SetCycleType(NiTimeController::CLAMP);
		}
		else if(eCode == Animation_Enum::JUMP)
		{
			NiControllerSequence* pkDeath = m_spActorManager->GetSequence(Animation_Enum::DEATHBACK);
			pkCM->CrossFade(pkSequence,pkDeath,0.6f);			
		}
		else
			m_spActorManager->SetTargetAnimation(Animation_Enum::DEATHBACK);

		// ==== ų ���� ó�� ====
		GameSystemManager* pkGSM = GameSystemManager::GetInstance();
		pkGSM->AddKill(pResult->m_dwFireUserID);
		pkGSM->AddDeath(pResult->m_dwResultID);		
		
		//==================================================================================================================================
	}
	else if(pResult->m_dwCondition == INCAPACITY) //�λ� ����
	{   
		if(m_dwStatus == DEATH)		//���� ���°� �����̸� �ƹ��� ó�� ���� ����
			return;
		m_spActorManager->SetTargetAnimation(Animation_Enum::INCAPACITY);			
	}
	else
	{
		DeactiveState(m_dwStatus);
		m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	}
	//���� ����
	m_dwStatus = pResult->m_dwCondition;;
}


void CCharacter::DeactiveState(unsigned int uiStatus)			//���� ��Ȱ��ȭ
{
	NiControllerManager* pkCM = m_spActorManager->GetControllerManager();

	pkCM->DeactivateAll();	
}

//���� ó�� - �ٸ� ĳ���� ���� ó��
void CCharacter::SetStatus(unsigned int uiStatus)
{
	if(m_dwStatus == uiStatus)		//���� ���¿� ���� ���°� ���ٸ� ����
		return;
	
	NiControllerSequence* pkSequence = NULL;
	NiControllerManager*  pkCM = m_spActorManager->GetControllerManager();

	DWORD eCode = m_spActorManager->GetTargetAnimation();
	pkSequence = m_spActorManager->GetSequence(eCode);
	//���� ����
	if(uiStatus == DEATH)
	{
		if(m_dwStatus == INCAPACITY)		//���� ���°� �λ� �����̸�
		{	
			pkSequence = m_spActorManager->GetSequence(Animation_Enum::INCAPACITY);
			pkSequence->SetCycleType(NiTimeController::CLAMP);
		}
		else if(eCode == Animation_Enum::JUMP)
		{
			NiControllerSequence* pkDeath = m_spActorManager->GetSequence(Animation_Enum::DEATHBACK);
			pkCM->CrossFade(pkSequence,pkDeath,0.6f);			
		}
		else
		{
			m_spActorManager->SetTargetAnimation(Animation_Enum::DEATHBACK);
		}
	}
	else if(uiStatus == INCAPACITY)	//�λ� ����
	{	
		if(m_dwStatus == DEATH)		//���� ���°� �����̸� �ƹ��� ó�� ���� ����
			return;
		m_spActorManager->SetTargetAnimation(Animation_Enum::INCAPACITY);			
	}
	else							//�븻 ����
	{
		//���� ���� ��Ȱ��ȭ
		DeactiveState(m_dwStatus);	
		m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	}

	//���� ����
	m_dwStatus = uiStatus;
}


void CCharacter::InterpretInput(float fTime)
{

}

void CCharacter::UpdateBipedRootTransform(float fTime)	// ��Ʈ ĳ���� ��ȯ
{

}

NiNode* CCharacter::GetSpineBone()
{
	return m_spSpineBone;
}

NiNode* CCharacter::GetABVRoot()
{ 
	return m_spABVRoot; 
}



