#include "NetworkManager.h"
#include "CUserCharacter.h"
#include "WorldManager.h"
#include "CStateManager.h"
#include "CGamePlayState.h"
#include "SoundManager.h"
#include "ZFLog.h"


CUserCharacter::CUserCharacter() : CCharacter()
{
	m_kMoveDir = NiPoint2::ZERO;
	m_SuccessiveCount = 0;
	
	m_kFirePoint = NiPoint2::ZERO;

	m_AccuracyRateBonus = 0;						//정확도 보너스. - 보병이라는 전제하에 
	m_bCollision = false;

	m_pkWeapon   = NULL;
	m_kCrossHairObject = NULL;
    
	m_fHeadVertical = 0.0f;
	m_fAccuracy = 0.0f;
	m_fEffectAccuracy = 5.0f;

	//피킹 변수 초기화
	m_RecoveryPick.SetPickType(NiPick::FIND_FIRST);
	m_RecoveryPick.SetIntersectType(NiPick::BOUND_INTERSECT);
	m_RecoveryPick.SetFrontOnly(false);

	m_bRecover = false;
	m_fRecoveryTime = 0.0f;
}

CUserCharacter::~CUserCharacter()
{
	NiDelete m_pkCallBack;

	if(m_pkWeapon)
		NiDelete m_pkWeapon;

	if(m_kCrossHairObject)
		NiDelete m_kCrossHairObject;
	
	m_kCrossHairObject = 0;
	m_pkWeapon = 0;
	m_pkEquipWeapon = 0;
}

bool CUserCharacter::Initialize(NiActorManager* pkActor,const NiTransform &kSpawn)
{	
	m_spActorManager = pkActor;
	assert(pkActor);

	CCharacter::Init(pkActor,kSpawn);
	
	//무기 객체 생성
	m_pkWeapon = NiNew CPrimaryWeapon(CPrimaryWeapon::M4A1);

	m_pkEquipWeapon = m_pkWeapon;		//주무기를 장착
			
	//CBiped::Init();

	//애니메이션 콜백 함수----------------------------------
	//콜백객체 생성
	m_pkCallBack = NiNew CharacterCallbackObject;
	assert(m_pkCallBack);
	//애니메이션 콜백 등록
	pkActor->SetCallbackObject(m_pkCallBack);		//현재 캐릭터 객체를 넘겨줌.
	pkActor->RegisterCallback(NiActorManager::ANIM_ACTIVATED,Animation_Enum::JUMP,"start");
	pkActor->RegisterCallback(NiActorManager::END_OF_SEQUENCE,Animation_Enum::JUMP,"end");
	pkActor->RegisterCallback(NiActorManager::TEXT_KEY_EVENT,Animation_Enum::JUMP,"pick");
	
	//콜백값 초기화
	m_pkCallBack->Init();
	
	//-------------------------------------------------------

	//애니메이션 처리 객체 초기화
	m_ProcessAnimation.Init(pkActor);
	m_ProcessAnimation.SetAnimation(Animation_Enum::IDLE_RIFLE,ProcessAnimation::INTERRUPT,0.0f);
	
	//크로스 헤어 생성
	if(!CreateCrossHair())
	{
		NiMessageBox("크로스헤어 생성 실패",NULL);
		return false;
	}
	
	return true;
}

//크로스헤어 생성
bool CUserCharacter::CreateCrossHair()
{
	//크로스헤어 객체 생성
	m_kCrossHairObject = NiNew CrossHair;
	assert(m_kCrossHairObject);

	if(!m_kCrossHairObject->Create())
		return false;

	return true;
}

void CUserCharacter::UpdateAccuracy()
{
	//정확도는 = 발사 수 x 총의 반동 - 반동보너스
	m_fAccuracy = (float) (m_SuccessiveCount * m_pkEquipWeapon->GetAccuracy() - m_AccuracyRateBonus);
	m_fAccuracy += m_fEffectAccuracy;
}

void CUserCharacter::Draw(NiCamera *pkCamera)
{
	NiNode* pkNifRoot = (NiNode*) GetNIFRoot();
	assert(pkNifRoot);

	//유저 캐릭터일 경우는 시점에 따라 보이는 것을 다르게 그려준다.
	bool bFirstPerson = CameraManager::Get()->GetFirstPerson();
	if(bFirstPerson)
	{
		NiAVObject*	pkLeft  = pkNifRoot->GetObjectByName("L_hand");
		NiAVObject* pkRight = pkNifRoot->GetObjectByName("R_hand");
		NiAVObject* pkGun   = pkNifRoot->GetObjectByName("gunRef");
		
		WorldManager::GetWorldManager()->PartialRender(pkCamera,pkLeft);
		WorldManager::GetWorldManager()->PartialRender(pkCamera,pkRight);
		WorldManager::GetWorldManager()->PartialRender(pkCamera,pkGun);
	}
	else
		WorldManager::GetWorldManager()->PartialRender(pkCamera, GetNIFRoot());		
}

void CUserCharacter::Update(float fTime)
{
	CCharacter::Update(fTime);

	UpdateAccuracy();							//정확도 업데이트
	
	m_kCrossHairObject->Update(m_fAccuracy);	//크로스헤어 업데이트
	
	if(m_bRecover) RecoveryBuddy(fTime);		//구할 친구 있나 찾기 
	
	if(m_dwStatus == INCAPACITY) GameSystemManager::GetInstance()->DamageByInjury(fTime);
	m_fLastUpdate = fTime;
}

void CUserCharacter::Reset(const NiPoint3 &newPos,float fTime)
{	
	//=========================================
	//   캐릭터 Transfrom 리셋
	//========================================= 
	NiMatrix3 kRot,kSpineRot;

	kRot.MakeZRotation(0.0f);
	kSpineRot.MakeZRotation(-0.0f);

	m_spCharRoot->SetTranslate( newPos );
	m_spCharRoot->SetRotate( kRot);
	m_spCharRoot->SetRotate( kSpineRot );

	//=========================================
	// 체력을 원래 상태로
	//=========================================
	WorldManager::GetWorldManager()->GetGameSystemManager()->RecoveryHealth();

	//카메라 리셋 - 1인칭 시점으로 변경
	//CameraManager::Get()->CameraReset();
	CameraManager::Get()->ChangeView(FIRST_PERSON);

	//총 리로드
	m_pkEquipWeapon->Reload();
	
	CCharacter::Reset();

	Update(fTime); 
}

void CUserCharacter::StartFire()
{
	//총에 발사 시작을 알림
	m_pkEquipWeapon->FireStart();
}

void CUserCharacter::EndFire()
{
	m_pkEquipWeapon->FireEnd();
}

//반동 감소
void CUserCharacter::DecreaseReaction()
{
	//좌,우 반동 감소
	m_SuccessiveCount == 0 ? m_SuccessiveCount = 0 : m_SuccessiveCount--;
}

//총의 발사 여부 확인
bool CUserCharacter::ShootGun(float fTime)
{
	//캐릭터가 NORMAL 상태 일때만 총 발사 하게 한다.
	if(m_dwStatus != NORMAL)
		return false;
	//총 발사에 관련된 패킷을 구성하는데는 2가지가 있다.
	//총알을 발사할 때랑, 빈총을 발사 할때 두가지에 대한 처리를 해주어야한다.
	_PACKET_SOUND_DATA	packet;							// 총 관련된 사운드를 전송할 패킷
		
	packet.kSoundSpot	= m_spCharRoot->GetTranslate();	//소리가 발생되는 지점 = 현재 캐릭터의 위치
	packet.b2DSound		= false;		//3D 사운드 입니다.
	bool bEmptyBullet	= false;			//true : 빈총, false : 발사도 아니고 빈총도 아니다.
	
	//발사가 된다면
	if(m_pkEquipWeapon->UseWeapon(fTime,&bEmptyBullet))
	{
		CGameApp* app = CGameApp::mp_Appication;
		m_SuccessiveCount++;		//발사 카운터 증가
		
		int iX,iY,iZ;
		iX = app->GetAppWidth()/2;		//중심의 x좌표
		iY = app->GetAppHeight()/2;		//중심의 y좌표

		//크로스헤어 안에서 발사점 랜덤으로 찾기
		if(m_SuccessiveCount == 1)	//첫발일 경우 정확도 100%
		{
			m_kFirePoint = NiPoint2((float)iX,(float)iY);
		}
		else
		{
			//크로스헤어 영역에 따른 발사점 무작위 설정
			NiRect<int>		CrossHairRect = m_kCrossHairObject->GetCrossRect();

			int randomRangeX = NiRand() % (CrossHairRect.m_right - CrossHairRect.m_left);
			int randomRangeY = NiRand() % (CrossHairRect.m_top - CrossHairRect.m_bottom);

			m_kFirePoint = NiPoint2((float)CrossHairRect.m_left + randomRangeX,(float)CrossHairRect.m_top + randomRangeY); 
		}
		//상하 반동
		NiInputMouse* pkMouse = app->GetInputSystem()->GetMouse();
		pkMouse->GetPositionDelta(iX,iY,iZ);				//마우스의 위치를 얻어옴.
		iY+= m_pkEquipWeapon->GetGunReaction();				//반동만큼 iY값을 수정해준다.
		CameraManager::Get()->UpwardCameraFramePitch(iY);	//카메라의 pitch
		
		//머즐 업데이트
		UpdateMuzel(fTime);
		
		packet.eSoundCode	= eSoundCode::SOUND_WAV_M4FIRE;		
		
		//사운드 데이터를 전송하기 위해 네트워크 매니저로 전달
		NetworkManager::GetInstance()->SendSoundData(packet);
		return true;
	}
	else if(bEmptyBullet)		//빈총 발사 확인
	{
		//빈총 발사 사운드 종류 설정		
		packet.eSoundCode	= eSoundCode::SOUND_WAV_EMPTY;			
		//사운드 데이터를 전송하기 위해 네트워크 매니저로 전달
		NetworkManager::GetInstance()->SendSoundData(packet);
	}

	return false; 
}

void CUserCharacter::UpdateBipedRootTransform(float fTime)
{
	CGameApp *app = CGameApp::mp_Appication;

	//캐릭터의 이동 방향은 키보드 눌름에 따라 결정된다.
	
	NiQuaternion DirQuat;

	float fHeading = CameraManager::Get()->GetCameraHeading();
	NiMatrix3 kHeadingRot;
	kHeadingRot.MakeZRotation(fHeading);
	
	//사용자의 Input으로 캐릭터의 위치를 업데이트 한다.
	float fForward = m_fSpeed * m_kMoveDir.x;
	float fHorz = m_fSpeed * m_kMoveDir.y;
	float fVert = 0.0f;

	NiPoint3 kLocVelocity(-fHorz,-fForward,fVert);
	
	//캐릭터의 LocVelocity를 이용하여 Collision Update
	UpdateCollisionBasePosition(fTime,kHeadingRot,kLocVelocity);

	//-------------------------------------------------------------------------------------------		
	m_spCharRoot->Update(0.0f,false);	
}


void CUserCharacter::UpdateCollisionBasePosition(float fTime, const NiMatrix3 &kDesireRot, NiPoint3 &kLocVelocity)
{
	float fDelta = fTime - m_fLastUpdate;

	bool				bColliWithObject = false;
	NiPoint3			kOldTrans = m_spCharRoot->GetTranslate();
	NiPoint3			kVecByCollision = NiPoint3::ZERO;
	NiCollisionData*	pkCollisionData = NiGetCollisionData(m_spABVRoot);
	
	assert(pkCollisionData);

	//지형보다 우선순위로 건물의 높이를 구한다.
	NiPoint3 kEarthHeightPos,kNormal;		//새로윤 Height
	float fSlopeAffect;
	
	///캐릭터를 rotation한다-- 캐릭터가 NORMAL 상태 일 때만 회전한다.
	if(m_dwStatus == NORMAL)
		m_spCharRoot->SetRotate(kDesireRot);
	
	//속도(velocity를 CollisionData에 set하고 Collision을 찾는다.
	CollisionManager*	pkCollisionManager = WorldManager::GetWorldManager()->GetTerrain()->GetCollisionManager( kOldTrans );
	pkCollisionData->SetLocalVelocity(kLocVelocity);
		
	if( pkCollisionManager ) 
		pkCollisionManager->FindCollision(fDelta);
	else
	{
		pkCollisionData->UpdateWorldData();
	}
	NiPoint3 kWorldVelocity = pkCollisionData->GetWorldVelocity();	//World 속도를 가져온다.
	NiPoint3 kNewTrans = kOldTrans;
			
	//----------------------------------------------------------
	
	//-------------------------충돌 체크--------------------------
	//충돌이 되지 않았을 경우
	if( !pkCollisionManager || !m_kIntersectionRecord.m_bInCollision   )
	{
		//기존 위치에 WorldVelocity 벡터를 더한다.
		//Z 월드 속도에 중력 적용
		kWorldVelocity.z = GRAVITY;		//z 속도에 중력 적용
		kNewTrans += (fDelta * kWorldVelocity);	
	}
	else //충돌 되었을 경우 벽의 노멀과 캐릭터 노멀을 이용해서 미끄러지듯 이동하게 한다.
	{
		//충돌면의 노멀 벡터를 가져온다
		NiPoint3 kNormal = m_kIntersectionRecord.m_kObjectNormal;
	
		//Z의 노말은 0을 대입한다. 안그럼 땅으로 꺼질껄?
		kNormal.z = 0.0f;
		//충돌 후의 새로운 벡터를 구하자
		NiPoint3 kNewCollisionVec = kWorldVelocity - ((kWorldVelocity * kNormal) * kNormal);
		
		//월드 속도를 직접 입력하기 위해 false
		NiCollisionData::SetEnableVelocity(false);
		//월드 속도를 셋한다.
		pkCollisionData->SetWorldVelocity(kNewCollisionVec);		
		
		//위에 월드 속도를 기반으로 2차 충돌이 있는지 확인한다.
		pkCollisionManager->FindCollision(fDelta);

		//다시 로컬 속도를 계산할 수 있게 true
		NiCollisionData::SetEnableVelocity(true);		
		
		
		if(!m_kIntersectionRecord.m_bInCollision)
		{
			kNewTrans += (fDelta * kNewCollisionVec);
			m_kLastSafeSlideN = kNormal;			
		}
		else
		{
			const float fCos = NiCos(NI_HALF_PI * 0.5f);
			kNormal = m_kIntersectionRecord.m_kObjectNormal;
			//kNormal.z = 0.0f;
			float fDot = m_kLastSafeSlideN * kNormal;
			if(fDot >= fCos)
				kNewTrans += (fDelta * kNormal);
		}		

	}	
	//------------------------------------------------------------------------------
	
	// 캐릭터의 x,y 위치를 이용하여 지형의 높이를 피킹 광선을 이용해서 구한다.
	
	//기본 평지의 높이는 캐릭터의 z 높이로 정한다.
	kEarthHeightPos.z = kOldTrans.z;
	// 건물 및 기타 오브젝트로 부터 높이값을 얻는다.
	if( pkCollisionManager )
	{		
		if( pkCollisionManager->GetObjectHeight( kNewTrans.x,kNewTrans.y,kEarthHeightPos.z) )
		{
			bColliWithObject = true;			
		}
	}	
	// 건물 및 기타 오브젝트로 부터 피킹이 없을 경우 지형과 피킹 검사
	if( !bColliWithObject )
	{
		// 지형과 충돌이 안 일어나면 원래 위치로 조정 
		if( !WorldManager::GetWorldManager()->GetTerrain()->GetHeightPos(kNewTrans,kEarthHeightPos,kNormal) )
		{
			kNewTrans = kOldTrans;
		}
		// 피킹이 일어나고 지형의 경사도에 따라 이동을 제한한다.
		else
		{
			fSlopeAffect = kNormal.Dot( NiPoint3::UNIT_Z );

			if( fSlopeAffect < SlopeMaxLimit )
			{
				kNewTrans = kOldTrans;	
				kEarthHeightPos.z = kOldTrans.z;
			}	
		}
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	m_spCharRoot->SetTranslate(kNewTrans);
	m_spCharRoot->Update(0.0f,false);
	
	pkCollisionData->SetLocalVelocity(NiPoint3::ZERO);	
	//--------------------------------------------------------------------------------
	//점프 구현
	float jumpHeight = 0.0f;
	if(m_pkCallBack->m_bJumpData)
	{
		float ElapsedTime = fTime - m_pkCallBack->m_fCurrTime;
		
		jumpHeight = m_pkCallBack->m_fJumpForce * ElapsedTime - (0.5f * 9.8f * ElapsedTime * ElapsedTime);
		if(jumpHeight <= 0)	//점프한 높이가 0보다 작으면 점프는 끝
		{
			m_pkCallBack->Reset();
		}
	}

	//최종 높이 계산
	kNewTrans.z += jumpHeight;
	if(kNewTrans.z < kEarthHeightPos.z)
	{
		kNewTrans.z = kEarthHeightPos.z;
		m_pkCallBack->m_bJumpable = true;	//점프 가능으로 바뀜
	}

	m_spCharRoot->SetTranslate(kNewTrans);
	m_spCharRoot->Update(0.0f,false);

	pkCollisionData->SetLocalVelocity(NiPoint3::ZERO);
	//==================================================================================
	if( pkCollisionManager )
	{
		//정적 충돌 체크
		if(pkCollisionManager->TestCollision(0.0f))
		{	
			/*float fNormalX = m_kIntersectionRecord.m_kObjectNormal.x;
			fNormalX = NiAbs(fNormalX);
			float fNormalY = m_kIntersectionRecord.m_kObjectNormal.y;
			fNormalY = NiAbs(fNormalY);
			float fNormalZ = m_kIntersectionRecord.m_kObjectNormal.z;
			fNormalZ = NiAbs(fNormalZ);

			if(fNormalZ > fNormalY && fNormalZ > fNormalX)
			{
				kVecByCollision = fDelta *  m_kIntersectionRecord.m_kObjectNormal; 
 				//kVecByCollision.z = 0.0f;
				kNewTrans += kVecByCollision; 				//충돌 후 노멀벡터 쪽으로 방향이동
				m_spCharRoot->SetTranslate(kNewTrans);
			}
			else
				m_spCharRoot->SetTranslate(kOldTrans);
			*/
			kVecByCollision = fDelta *  m_kIntersectionRecord.m_kObjectNormal; 
			kVecByCollision.z = 0.0f;
			kNewTrans += kVecByCollision; 				//충돌 후 노멀벡터 쪽으로 방향이동
			m_spCharRoot->SetTranslate(kNewTrans);
			m_spCharRoot->Update(0.0f,false);			
		}	
	}
	
	//---------------------------------------------------------------------------------
}


//키 입력에 따른 캐릭터 애니메이션 처리
void CUserCharacter::InterpretInput(float fTime)
{
	m_kMoveDir = NiPoint2::ZERO;

	//키 입력 전에 캐릭터의 상태를 체크한다.
	if(m_dwStatus == INCAPACITY || m_dwStatus == DEATH)
	{
		return ;
	}

	//키보드 객체를 GameApp에서 가져온다
	NiInputKeyboard* pkKB = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();
	if(!pkKB)
		return;

	bool bShift = (pkKB->KeyIsDown(NiInputKeyboard::KEY_LSHIFT) ||
		pkKB->KeyIsDown(NiInputKeyboard::KEY_RSHIFT));

	bool bDuck = (pkKB->KeyIsDown(NiInputKeyboard::KEY_LCONTROL));
	if(bDuck)
	{
		CameraManager::Get()->SetDuckFlag(true);
	}
	else	
		CameraManager::Get()->SetDuckFlag(false);
		
	//동시 키입력 처리
	bool bMoveKey = false;
	bool bSetAni = false;		//애니메이션이 셋되어 있는지 여부 확인변수
	
	// 메뉴창이 활성화 되어 있다면 키입력을 막는다.
	if( WorldManager::GetWorldManager()->IsActiveMenuMode() ) return;

	// 채팅중이라면 키입력을 막는다.
	if( ((CGamePlayState*)CGameApp::GetStateManager().GetCurrentStateClass())->IsChatMode() )
		return;
	
	//캐릭터 이동 키 입력
	if(pkKB->KeyIsDown(NiInputKeyboard::KEY_A))
	{		
		if(bDuck)
		{
			m_kMoveDir.y = -0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::DUCKLEFT,ProcessAnimation::INTERRUPT,fTime);	
		}
		else if(bShift)
		{
			m_kMoveDir.y = -0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::WALKLEFT,ProcessAnimation::INTERRUPT,fTime);	
		}
		else
		{
			m_kMoveDir.y = -1.0f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::RUNLEFT,ProcessAnimation::INTERRUPT,fTime);	
		}
		bSetAni = true;
		bMoveKey = true;		
	}
	else if(pkKB->KeyIsDown(NiInputKeyboard::KEY_D))
	{
		if(bDuck)
		{
			m_kMoveDir.y = 0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::DUCKRIGHT,ProcessAnimation::INTERRUPT,fTime);	
		}
		else if(bShift)
		{
			m_kMoveDir.y = 0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::WALKRIGHT,ProcessAnimation::INTERRUPT,fTime);
		}
		else
		{
			m_kMoveDir.y = 1.0f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::RUNRIGHT,ProcessAnimation::INTERRUPT,fTime);
		}
		bSetAni = true;
		bMoveKey = true;
	}

	if(pkKB->KeyIsDown(NiInputKeyboard::KEY_W))		//전진
	{		
		if(bDuck)
		{
			m_kMoveDir.x = 0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::DUCKFRONT,ProcessAnimation::INTERRUPT,fTime);
		}
		else if(bShift)
		{
			m_kMoveDir.x = 0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::WALKFRONT,ProcessAnimation::INTERRUPT,fTime);
		}
		else
		{
			m_kMoveDir.x = 1.0f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::RUNFRONT,ProcessAnimation::INTERRUPT,fTime);
			if(bMoveKey)	//동시키 입력일 경우 x,y벡터의 합벡터가 1이 되도록 노멀 벡터로 만들어주자
			{
				m_kMoveDir.Unitize();		//벡터를 단위 벡터화 한다	
			}
		}		
		bSetAni = true;
	}
	else if(pkKB->KeyIsDown(NiInputKeyboard::KEY_S))	//후진
	{
		if(bDuck)
		{
			m_kMoveDir.x = -0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::DUCKFRONT,ProcessAnimation::INTERRUPT,fTime);
		}		
		else
		{
			m_kMoveDir.x = -0.5f;
			m_ProcessAnimation.SetAnimation(Animation_Enum::WALKBACK,ProcessAnimation::INTERRUPT,fTime);
			if(bMoveKey)	//동시키 입력일 경우 x,y벡터의 합벡터가 1이 되도록 노멀 벡터로 만들어주자
			{
				m_kMoveDir.Unitize();	
			}
		}		
		bSetAni = true;
	}

	if(pkKB->KeyWasPressed(NiInputKeyboard::KEY_SPACE) && m_pkCallBack->m_bJumpable)
	{
		m_ProcessAnimation.SetAnimation(Animation_Enum::JUMP,ProcessAnimation::NONINTERRUPT,fTime);
		bSetAni = true;		
	}

	if(pkKB->KeyWasPressed(NiInputKeyboard::KEY_R))
	{
		//재장전
		m_pkEquipWeapon->Reload();

//		m_ProcessAnimation.SetAnimation(Animation_Enum::CHANGECLIP,ProcessAnimation::NONINTERRUPT,fTime);
		bSetAni = true;
	}

	//아군 캐릭터 부활 시키기
	if(pkKB->KeyWasPressed(NiInputKeyboard::KEY_E))
	{
		m_bRecover = true;		
		//리코버리 프로세스에 접근
		CStateManager &rStateManager = CGameApp::GetStateManager();
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
		pGameState->SetRebirthing(true);
	}
	else if(pkKB->KeyWasReleased(NiInputKeyboard::KEY_E))
	{
		m_bRecover = false;
		//리코버리 프로세스에 접근
		CStateManager &rStateManager = CGameApp::GetStateManager();
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
		pGameState->SetRebirthing(false);

		m_fRecoveryTime = 0.0f;
	}

	if(bSetAni == false)
	{
		if(bDuck)
			m_ProcessAnimation.SetAnimation(Animation_Enum::IDLE_DUCK,ProcessAnimation::INTERRUPT,fTime);
		else
			m_ProcessAnimation.SetAnimation(Animation_Enum::IDLE_RIFLE,ProcessAnimation::INTERRUPT,fTime);
	}		
	m_ProcessAnimation.UpdateAnimation();		
}

void CUserCharacter::RecoveryBuddy(float fTime)
{
	CStateManager &rStateManager = CGameApp::GetStateManager();
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
	//if(!m_bRecover) return;				//E버튼(회복)버튼 클릭이 안 되어 있으면 리턴

	float fDelta = fTime - m_fLastUpdate;

	BYTE	selfCamp = NetworkManager::GetInstance()->GetCamp();		//자신의 진영
	BYTE	selfID   = NetworkManager::GetInstance()->GetID();			//자신의 아이디
	
	//캐릭터 중에서 찾아봅시다... 피킹되는 아군 캐릭터가 있는지
	std::vector<CCharacter*>& vecOtherCharacter = WorldManager::GetWorldManager()->GetCharacterManager()->GetColliderVector();
	PlayersMap conPlayer = NetworkManager::GetInstance()->LockPlayers();
	
	//네트워크 매니저의 플레이어 정보에서 같은 진영의 플레이어 아이디를 찾음
	PlayersMap::iterator iter = conPlayer.begin();
	for( ;iter !=conPlayer.end();++iter)
	{
		//치료하려는 캐릭터가 아군이고 부상상태 일 경우만 처리
		const UserInfo&  userInfo = iter->second.userInfo;  
		if(selfCamp == userInfo.camp && userInfo.state == INCAPACITY)
		{
			CCharacter* pkOtherCharacter = NULL;	//다른 플레이어 캐릭터
			NiPoint3	kOrigin,kDir;				//피킹 광선
			BYTE id = userInfo.id;					//다른 플레이어 아이디

			selfID > id ? pkOtherCharacter = vecOtherCharacter[id] :
				          pkOtherCharacter = vecOtherCharacter[id-1];
			m_RecoveryPick.RemoveTarget();
			m_RecoveryPick.SetTarget(pkOtherCharacter->GetCharRoot());	//동료 플레이어를 피킹 타겟으로 설정
			//피킹 광선 설정
			int scrX = CGameApp::mp_Appication->GetAppWidth();
			int scrY = CGameApp::mp_Appication->GetAppHeight();
			CameraManager::Get()->GetCamera()->WindowPointToRay(scrX/2,scrY/2,kOrigin,kDir);	//광선 생성
						
			//피킹 확인
			if(m_RecoveryPick.PickObjects(kOrigin,kDir))
			{
				NiPick::Results& kRecoveryResults = m_RecoveryPick.GetResults();
				NiPick::Record*  kRecord = kRecoveryResults.GetAt(0);

				float fDistance = kRecord->GetDistance();		//피킹된 오브젝트와의 거리
				if(fDistance < 1.0f)							//피킹된 사람의 거리가 1m이하 일경우 회복 가능
				{
					m_fRecoveryTime += fDelta;
					float fValue = 1.0f * m_fRecoveryTime / RESPONE_TIME;
					if(1.0f <= fValue)
					{						
						m_fRecoveryTime = 0;
						iter->second.userInfo.state = NORMAL;	//노멀 상태로 만듬

						//패킷 구성
						_PACKET_CHANGE_STATUS	packet;
						packet.id	  = id;
						packet.status = NORMAL;
						NetworkManager::GetInstance()->SendChangeStatus(packet);
					}
					pGameState->UpdateRebirthProgress(fValue);
				}
			}
		}
	}
	NetworkManager::GetInstance()->UnLockPlayers();
}


//애니메이션 콜백 함수
//애니메이션이 활성화되면 콜백이 불려지게 된다능
void CUserCharacter::CharacterCallbackObject::AnimActivated(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime)
{
	 m_bJumpData = true;		//점프 시작했는지 여부
	 m_fCurrTime = CGameApp::mp_Appication->GetAccumTime();//NiGetCurrentTimeInSec();		//점프 시작 시간.
		 
	 //시퀀스가 끝나는 시간
	 m_fEndTime = pkManager->GetNextEventTime(NiActorManager::TEXT_KEY_EVENT,
			eSequenceID,"end");
}

//애니메이션의 시퀀스 끝에 갈때 생기는 콜백
void CUserCharacter::CharacterCallbackObject::EndOfSequence(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime)
{
	//Reset();	//콜백값 초기화
}

void CUserCharacter::CharacterCallbackObject::TextKeyEvent(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, const NiFixedString& kTextKey,
            const NiTextKeyMatch* pkMatchObject,float fCurrentTime, float fEventTime)
{
	//Reset();	//콜백값 초기화
}

//콜백값 초기화
void CUserCharacter::CharacterCallbackObject::Reset()
{
	m_bJumpData = false;	//점프 시작했는지 여부
	m_bJumpable = false;	

		
	m_fCurrTime	= 0.0f;		//현재 시간
	m_fEndTime	= 0.0f;
}

void CUserCharacter::CharacterCallbackObject::Init()
{
	//점프의 힘을 스크립트에서 가져온다.
	FILE*	pFile = NULL;
	fopen_s(&pFile,"Data/script/JumpForce.dat","r");
	if(pFile == NULL)
	{
		NiMessageBox("JumpForce.dat를 찾을 수가 없습니다.","Error");
	}
	fscanf_s(pFile,"%f",&m_fJumpForce);
	//m_fJumpForce = JUMP_VELOCITY;
	m_fCurrTime	= 0.0f;		//현재 시간
	m_fEndTime	= 0.0f;

	m_bJumpData = false;	//점프 시작했는지 여부
	m_bJumpable = false;	
}

