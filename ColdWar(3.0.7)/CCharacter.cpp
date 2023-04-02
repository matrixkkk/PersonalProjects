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
	m_fSpeed = 6.0f;								//플레이어 기본 스피드

	m_Gravity	= NiPoint3( 0.0f, 0.0f, -9.8f );	//중력
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

	//muZel 할당
	m_pkMuzle = NiNew CMuzelSplash(0.01f);

	//------------Collision setting-------------------

	//ABV오브젝트 collision을 가져와 저장
	NiAVObject* pkABVRoot = m_spActorManager->GetNIFRoot()->GetObjectByName("root");
	assert(pkABVRoot);
	//부모 노드를 넘겨줍시다.
	m_spABVRoot	= NiDynamicCast(NiNode,pkABVRoot);
	if(!m_spABVRoot)
	{
		NiMessageBox("CharacterSoldier::Initialize - ABV Root를 찾을수 없습니다.",NULL);
		return false;
	}
	NiCollisionData*	pkCD = NiGetCollisionData(pkABVRoot);
	assert(pkCD);
	assert(pkCD->GetModelSpaceABV());	//오브젝트의 모델 스페이스 ABV를 구해옵니다.
	assert(pkCD->GetModelSpaceABV()->Type() == NiBoundingVolume::CAPSULE_BV);		//바운딩 볼륨 타입 확인
	((NiCapsuleBV*)pkCD->GetModelSpaceABV())->SetAxis(NiPoint3::UNIT_Z);
	NiPoint3 kCenter = ((NiCapsuleBV*)pkCD->GetModelSpaceABV())->GetCenter();
	kCenter.x = kCenter.z = 0.0f;
	((NiCapsuleBV*)pkCD->GetModelSpaceABV())->SetCenter(kCenter);
	

	//기본 애니메이션 설정
	m_spActorManager->Update(0.0f);
	m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	
	//캐릭터의 머리의 수직, 수평 방향 초기화
	m_fHeadHorizon = 0.0f;
	m_fHeadVertical = 0.0f;
	
	//초기 캐릭터의 회전 이동 행렬 설정
	m_kSpawnTranslate = kSpawn.m_Translate;
	m_kSpawnRotate = kSpawn.m_Rotate;
	m_spCharRoot->SetTranslate(m_kSpawnTranslate);
	m_spCharRoot->SetRotate(m_kSpawnRotate);

	//몸통을 움직이기 위해 dummy노드 생성
	m_spSpineBone = NiNew NiNode;

	m_spSpineBone->SetName("lookat");
	
	//m_spSpineBone를 연결할 상하 노드 찾음
	NiNode* pkSpine1 = NiDynamicCast(NiNode,m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine1"));
	NiNode* pkSpine2 = NiDynamicCast(NiNode,m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine2"));

	//m_spSpineBone가 중간에 끼게 노드를 연결
	pkSpine1->AttachChild(m_spSpineBone);
	m_spSpineBone->AttachChild(pkSpine2);

	//노드를 수정했으므로 transform이 형성되게 update
	pkSpine1->Update(0.0f);

	m_spSpineBone = NiDynamicCast(NiNode,m_spSpineBone);
	m_kBaseSpineRotate = m_spSpineBone->GetRotate();				//몸통 로테이션
			
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
	// 기본 상태로 되돌리기
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

	//캐릭터 마우스에 따른 몸통 회전을 구현
	//토르소 노드 하위 노드의 로컬 로테이션을 구해온다.
	NiAVObject* pkSpine2 = m_spActorManager->GetNIFRoot()->GetObjectByName("Bip01 Spine2");
	
	//로컬 로테이션을 구해온다.	
	float fPitch   = CameraManager::Get()->GetCameraPitch(); 
	//회전의 최대 최소를 정한다.
	float fPitchMin,fPitchMax;
	fPitchMax = NI_PI * 0.5f;
	fPitchMin = -NI_PI * 0.2f;
	if(fPitch < fPitchMin)	fPitch = fPitchMin;
	else if(fPitch > fPitchMax) fPitch = fPitchMax;	 
	
	NiMatrix3	kSpineZRotate,kSpineYRotate;

	kSpineYRotate.MakeYRotation(fPitch * 0.5f);		//가중치
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
	if(m_dwStatus == pResult->m_dwCondition)		//이전 상태와 현재 상태가 같다면 리턴
			return;

	NiControllerSequence* pkSequence = NULL;
	NiControllerManager*  pkCM = m_spActorManager->GetControllerManager();

	DWORD eCode = m_spActorManager->GetTargetAnimation();
	pkSequence = m_spActorManager->GetSequence(eCode);

	// === 본인의 상태가 NORMAL 상태가 아니라면 3인칭 시점으로 변환 ===
	if(pResult->m_dwCondition != NORMAL && pResult->m_dwResultID == NetworkManager::GetInstance()->GetID()) 
		CameraManager::Get()->ChangeView(THIRD_PERSON);
	
	//죽은 상태
	if(pResult->m_dwCondition == DEATH)
	{
		if(m_dwStatus == INCAPACITY)		//이전 상태가 부상 상태이면
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

		// ==== 킬 데스 처리 ====
		GameSystemManager* pkGSM = GameSystemManager::GetInstance();
		pkGSM->AddKill(pResult->m_dwFireUserID);
		pkGSM->AddDeath(pResult->m_dwResultID);		
		
		//==================================================================================================================================
	}
	else if(pResult->m_dwCondition == INCAPACITY) //부상 상태
	{   
		if(m_dwStatus == DEATH)		//이전 상태가 죽음이면 아무런 처리 하지 않음
			return;
		m_spActorManager->SetTargetAnimation(Animation_Enum::INCAPACITY);			
	}
	else
	{
		DeactiveState(m_dwStatus);
		m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	}
	//상태 저장
	m_dwStatus = pResult->m_dwCondition;;
}


void CCharacter::DeactiveState(unsigned int uiStatus)			//동작 비활성화
{
	NiControllerManager* pkCM = m_spActorManager->GetControllerManager();

	pkCM->DeactivateAll();	
}

//상태 처리 - 다른 캐릭터 상태 처리
void CCharacter::SetStatus(unsigned int uiStatus)
{
	if(m_dwStatus == uiStatus)		//이전 상태와 현재 상태가 같다면 리턴
		return;
	
	NiControllerSequence* pkSequence = NULL;
	NiControllerManager*  pkCM = m_spActorManager->GetControllerManager();

	DWORD eCode = m_spActorManager->GetTargetAnimation();
	pkSequence = m_spActorManager->GetSequence(eCode);
	//죽은 상태
	if(uiStatus == DEATH)
	{
		if(m_dwStatus == INCAPACITY)		//이전 상태가 부상 상태이면
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
	else if(uiStatus == INCAPACITY)	//부상 상태
	{	
		if(m_dwStatus == DEATH)		//이전 상태가 죽음이면 아무런 처리 하지 않음
			return;
		m_spActorManager->SetTargetAnimation(Animation_Enum::INCAPACITY);			
	}
	else							//노말 상태
	{
		//이전 상태 비활성화
		DeactiveState(m_dwStatus);	
		m_spActorManager->SetTargetAnimation(Animation_Enum::IDLE_RIFLE);
	}

	//상태 저장
	m_dwStatus = uiStatus;
}


void CCharacter::InterpretInput(float fTime)
{

}

void CCharacter::UpdateBipedRootTransform(float fTime)	// 루트 캐릭터 변환
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



