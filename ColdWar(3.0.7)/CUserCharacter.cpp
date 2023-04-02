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

	m_AccuracyRateBonus = 0;						//��Ȯ�� ���ʽ�. - �����̶�� �����Ͽ� 
	m_bCollision = false;

	m_pkWeapon   = NULL;
	m_kCrossHairObject = NULL;
    
	m_fHeadVertical = 0.0f;
	m_fAccuracy = 0.0f;
	m_fEffectAccuracy = 5.0f;

	//��ŷ ���� �ʱ�ȭ
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
	
	//���� ��ü ����
	m_pkWeapon = NiNew CPrimaryWeapon(CPrimaryWeapon::M4A1);

	m_pkEquipWeapon = m_pkWeapon;		//�ֹ��⸦ ����
			
	//CBiped::Init();

	//�ִϸ��̼� �ݹ� �Լ�----------------------------------
	//�ݹ鰴ü ����
	m_pkCallBack = NiNew CharacterCallbackObject;
	assert(m_pkCallBack);
	//�ִϸ��̼� �ݹ� ���
	pkActor->SetCallbackObject(m_pkCallBack);		//���� ĳ���� ��ü�� �Ѱ���.
	pkActor->RegisterCallback(NiActorManager::ANIM_ACTIVATED,Animation_Enum::JUMP,"start");
	pkActor->RegisterCallback(NiActorManager::END_OF_SEQUENCE,Animation_Enum::JUMP,"end");
	pkActor->RegisterCallback(NiActorManager::TEXT_KEY_EVENT,Animation_Enum::JUMP,"pick");
	
	//�ݹ鰪 �ʱ�ȭ
	m_pkCallBack->Init();
	
	//-------------------------------------------------------

	//�ִϸ��̼� ó�� ��ü �ʱ�ȭ
	m_ProcessAnimation.Init(pkActor);
	m_ProcessAnimation.SetAnimation(Animation_Enum::IDLE_RIFLE,ProcessAnimation::INTERRUPT,0.0f);
	
	//ũ�ν� ��� ����
	if(!CreateCrossHair())
	{
		NiMessageBox("ũ�ν���� ���� ����",NULL);
		return false;
	}
	
	return true;
}

//ũ�ν���� ����
bool CUserCharacter::CreateCrossHair()
{
	//ũ�ν���� ��ü ����
	m_kCrossHairObject = NiNew CrossHair;
	assert(m_kCrossHairObject);

	if(!m_kCrossHairObject->Create())
		return false;

	return true;
}

void CUserCharacter::UpdateAccuracy()
{
	//��Ȯ���� = �߻� �� x ���� �ݵ� - �ݵ����ʽ�
	m_fAccuracy = (float) (m_SuccessiveCount * m_pkEquipWeapon->GetAccuracy() - m_AccuracyRateBonus);
	m_fAccuracy += m_fEffectAccuracy;
}

void CUserCharacter::Draw(NiCamera *pkCamera)
{
	NiNode* pkNifRoot = (NiNode*) GetNIFRoot();
	assert(pkNifRoot);

	//���� ĳ������ ���� ������ ���� ���̴� ���� �ٸ��� �׷��ش�.
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

	UpdateAccuracy();							//��Ȯ�� ������Ʈ
	
	m_kCrossHairObject->Update(m_fAccuracy);	//ũ�ν���� ������Ʈ
	
	if(m_bRecover) RecoveryBuddy(fTime);		//���� ģ�� �ֳ� ã�� 
	
	if(m_dwStatus == INCAPACITY) GameSystemManager::GetInstance()->DamageByInjury(fTime);
	m_fLastUpdate = fTime;
}

void CUserCharacter::Reset(const NiPoint3 &newPos,float fTime)
{	
	//=========================================
	//   ĳ���� Transfrom ����
	//========================================= 
	NiMatrix3 kRot,kSpineRot;

	kRot.MakeZRotation(0.0f);
	kSpineRot.MakeZRotation(-0.0f);

	m_spCharRoot->SetTranslate( newPos );
	m_spCharRoot->SetRotate( kRot);
	m_spCharRoot->SetRotate( kSpineRot );

	//=========================================
	// ü���� ���� ���·�
	//=========================================
	WorldManager::GetWorldManager()->GetGameSystemManager()->RecoveryHealth();

	//ī�޶� ���� - 1��Ī �������� ����
	//CameraManager::Get()->CameraReset();
	CameraManager::Get()->ChangeView(FIRST_PERSON);

	//�� ���ε�
	m_pkEquipWeapon->Reload();
	
	CCharacter::Reset();

	Update(fTime); 
}

void CUserCharacter::StartFire()
{
	//�ѿ� �߻� ������ �˸�
	m_pkEquipWeapon->FireStart();
}

void CUserCharacter::EndFire()
{
	m_pkEquipWeapon->FireEnd();
}

//�ݵ� ����
void CUserCharacter::DecreaseReaction()
{
	//��,�� �ݵ� ����
	m_SuccessiveCount == 0 ? m_SuccessiveCount = 0 : m_SuccessiveCount--;
}

//���� �߻� ���� Ȯ��
bool CUserCharacter::ShootGun(float fTime)
{
	//ĳ���Ͱ� NORMAL ���� �϶��� �� �߻� �ϰ� �Ѵ�.
	if(m_dwStatus != NORMAL)
		return false;
	//�� �߻翡 ���õ� ��Ŷ�� �����ϴµ��� 2������ �ִ�.
	//�Ѿ��� �߻��� ����, ������ �߻� �Ҷ� �ΰ����� ���� ó���� ���־���Ѵ�.
	_PACKET_SOUND_DATA	packet;							// �� ���õ� ���带 ������ ��Ŷ
		
	packet.kSoundSpot	= m_spCharRoot->GetTranslate();	//�Ҹ��� �߻��Ǵ� ���� = ���� ĳ������ ��ġ
	packet.b2DSound		= false;		//3D ���� �Դϴ�.
	bool bEmptyBullet	= false;			//true : ����, false : �߻絵 �ƴϰ� ���ѵ� �ƴϴ�.
	
	//�߻簡 �ȴٸ�
	if(m_pkEquipWeapon->UseWeapon(fTime,&bEmptyBullet))
	{
		CGameApp* app = CGameApp::mp_Appication;
		m_SuccessiveCount++;		//�߻� ī���� ����
		
		int iX,iY,iZ;
		iX = app->GetAppWidth()/2;		//�߽��� x��ǥ
		iY = app->GetAppHeight()/2;		//�߽��� y��ǥ

		//ũ�ν���� �ȿ��� �߻��� �������� ã��
		if(m_SuccessiveCount == 1)	//ù���� ��� ��Ȯ�� 100%
		{
			m_kFirePoint = NiPoint2((float)iX,(float)iY);
		}
		else
		{
			//ũ�ν���� ������ ���� �߻��� ������ ����
			NiRect<int>		CrossHairRect = m_kCrossHairObject->GetCrossRect();

			int randomRangeX = NiRand() % (CrossHairRect.m_right - CrossHairRect.m_left);
			int randomRangeY = NiRand() % (CrossHairRect.m_top - CrossHairRect.m_bottom);

			m_kFirePoint = NiPoint2((float)CrossHairRect.m_left + randomRangeX,(float)CrossHairRect.m_top + randomRangeY); 
		}
		//���� �ݵ�
		NiInputMouse* pkMouse = app->GetInputSystem()->GetMouse();
		pkMouse->GetPositionDelta(iX,iY,iZ);				//���콺�� ��ġ�� ����.
		iY+= m_pkEquipWeapon->GetGunReaction();				//�ݵ���ŭ iY���� �������ش�.
		CameraManager::Get()->UpwardCameraFramePitch(iY);	//ī�޶��� pitch
		
		//���� ������Ʈ
		UpdateMuzel(fTime);
		
		packet.eSoundCode	= eSoundCode::SOUND_WAV_M4FIRE;		
		
		//���� �����͸� �����ϱ� ���� ��Ʈ��ũ �Ŵ����� ����
		NetworkManager::GetInstance()->SendSoundData(packet);
		return true;
	}
	else if(bEmptyBullet)		//���� �߻� Ȯ��
	{
		//���� �߻� ���� ���� ����		
		packet.eSoundCode	= eSoundCode::SOUND_WAV_EMPTY;			
		//���� �����͸� �����ϱ� ���� ��Ʈ��ũ �Ŵ����� ����
		NetworkManager::GetInstance()->SendSoundData(packet);
	}

	return false; 
}

void CUserCharacter::UpdateBipedRootTransform(float fTime)
{
	CGameApp *app = CGameApp::mp_Appication;

	//ĳ������ �̵� ������ Ű���� ������ ���� �����ȴ�.
	
	NiQuaternion DirQuat;

	float fHeading = CameraManager::Get()->GetCameraHeading();
	NiMatrix3 kHeadingRot;
	kHeadingRot.MakeZRotation(fHeading);
	
	//������� Input���� ĳ������ ��ġ�� ������Ʈ �Ѵ�.
	float fForward = m_fSpeed * m_kMoveDir.x;
	float fHorz = m_fSpeed * m_kMoveDir.y;
	float fVert = 0.0f;

	NiPoint3 kLocVelocity(-fHorz,-fForward,fVert);
	
	//ĳ������ LocVelocity�� �̿��Ͽ� Collision Update
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

	//�������� �켱������ �ǹ��� ���̸� ���Ѵ�.
	NiPoint3 kEarthHeightPos,kNormal;		//������ Height
	float fSlopeAffect;
	
	///ĳ���͸� rotation�Ѵ�-- ĳ���Ͱ� NORMAL ���� �� ���� ȸ���Ѵ�.
	if(m_dwStatus == NORMAL)
		m_spCharRoot->SetRotate(kDesireRot);
	
	//�ӵ�(velocity�� CollisionData�� set�ϰ� Collision�� ã�´�.
	CollisionManager*	pkCollisionManager = WorldManager::GetWorldManager()->GetTerrain()->GetCollisionManager( kOldTrans );
	pkCollisionData->SetLocalVelocity(kLocVelocity);
		
	if( pkCollisionManager ) 
		pkCollisionManager->FindCollision(fDelta);
	else
	{
		pkCollisionData->UpdateWorldData();
	}
	NiPoint3 kWorldVelocity = pkCollisionData->GetWorldVelocity();	//World �ӵ��� �����´�.
	NiPoint3 kNewTrans = kOldTrans;
			
	//----------------------------------------------------------
	
	//-------------------------�浹 üũ--------------------------
	//�浹�� ���� �ʾ��� ���
	if( !pkCollisionManager || !m_kIntersectionRecord.m_bInCollision   )
	{
		//���� ��ġ�� WorldVelocity ���͸� ���Ѵ�.
		//Z ���� �ӵ��� �߷� ����
		kWorldVelocity.z = GRAVITY;		//z �ӵ��� �߷� ����
		kNewTrans += (fDelta * kWorldVelocity);	
	}
	else //�浹 �Ǿ��� ��� ���� ��ְ� ĳ���� ����� �̿��ؼ� �̲������� �̵��ϰ� �Ѵ�.
	{
		//�浹���� ��� ���͸� �����´�
		NiPoint3 kNormal = m_kIntersectionRecord.m_kObjectNormal;
	
		//Z�� �븻�� 0�� �����Ѵ�. �ȱ׷� ������ ������?
		kNormal.z = 0.0f;
		//�浹 ���� ���ο� ���͸� ������
		NiPoint3 kNewCollisionVec = kWorldVelocity - ((kWorldVelocity * kNormal) * kNormal);
		
		//���� �ӵ��� ���� �Է��ϱ� ���� false
		NiCollisionData::SetEnableVelocity(false);
		//���� �ӵ��� ���Ѵ�.
		pkCollisionData->SetWorldVelocity(kNewCollisionVec);		
		
		//���� ���� �ӵ��� ������� 2�� �浹�� �ִ��� Ȯ���Ѵ�.
		pkCollisionManager->FindCollision(fDelta);

		//�ٽ� ���� �ӵ��� ����� �� �ְ� true
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
	
	// ĳ������ x,y ��ġ�� �̿��Ͽ� ������ ���̸� ��ŷ ������ �̿��ؼ� ���Ѵ�.
	
	//�⺻ ������ ���̴� ĳ������ z ���̷� ���Ѵ�.
	kEarthHeightPos.z = kOldTrans.z;
	// �ǹ� �� ��Ÿ ������Ʈ�� ���� ���̰��� ��´�.
	if( pkCollisionManager )
	{		
		if( pkCollisionManager->GetObjectHeight( kNewTrans.x,kNewTrans.y,kEarthHeightPos.z) )
		{
			bColliWithObject = true;			
		}
	}	
	// �ǹ� �� ��Ÿ ������Ʈ�� ���� ��ŷ�� ���� ��� ������ ��ŷ �˻�
	if( !bColliWithObject )
	{
		// ������ �浹�� �� �Ͼ�� ���� ��ġ�� ���� 
		if( !WorldManager::GetWorldManager()->GetTerrain()->GetHeightPos(kNewTrans,kEarthHeightPos,kNormal) )
		{
			kNewTrans = kOldTrans;
		}
		// ��ŷ�� �Ͼ�� ������ ��絵�� ���� �̵��� �����Ѵ�.
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
	//���� ����
	float jumpHeight = 0.0f;
	if(m_pkCallBack->m_bJumpData)
	{
		float ElapsedTime = fTime - m_pkCallBack->m_fCurrTime;
		
		jumpHeight = m_pkCallBack->m_fJumpForce * ElapsedTime - (0.5f * 9.8f * ElapsedTime * ElapsedTime);
		if(jumpHeight <= 0)	//������ ���̰� 0���� ������ ������ ��
		{
			m_pkCallBack->Reset();
		}
	}

	//���� ���� ���
	kNewTrans.z += jumpHeight;
	if(kNewTrans.z < kEarthHeightPos.z)
	{
		kNewTrans.z = kEarthHeightPos.z;
		m_pkCallBack->m_bJumpable = true;	//���� �������� �ٲ�
	}

	m_spCharRoot->SetTranslate(kNewTrans);
	m_spCharRoot->Update(0.0f,false);

	pkCollisionData->SetLocalVelocity(NiPoint3::ZERO);
	//==================================================================================
	if( pkCollisionManager )
	{
		//���� �浹 üũ
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
				kNewTrans += kVecByCollision; 				//�浹 �� ��ֺ��� ������ �����̵�
				m_spCharRoot->SetTranslate(kNewTrans);
			}
			else
				m_spCharRoot->SetTranslate(kOldTrans);
			*/
			kVecByCollision = fDelta *  m_kIntersectionRecord.m_kObjectNormal; 
			kVecByCollision.z = 0.0f;
			kNewTrans += kVecByCollision; 				//�浹 �� ��ֺ��� ������ �����̵�
			m_spCharRoot->SetTranslate(kNewTrans);
			m_spCharRoot->Update(0.0f,false);			
		}	
	}
	
	//---------------------------------------------------------------------------------
}


//Ű �Է¿� ���� ĳ���� �ִϸ��̼� ó��
void CUserCharacter::InterpretInput(float fTime)
{
	m_kMoveDir = NiPoint2::ZERO;

	//Ű �Է� ���� ĳ������ ���¸� üũ�Ѵ�.
	if(m_dwStatus == INCAPACITY || m_dwStatus == DEATH)
	{
		return ;
	}

	//Ű���� ��ü�� GameApp���� �����´�
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
		
	//���� Ű�Է� ó��
	bool bMoveKey = false;
	bool bSetAni = false;		//�ִϸ��̼��� �µǾ� �ִ��� ���� Ȯ�κ���
	
	// �޴�â�� Ȱ��ȭ �Ǿ� �ִٸ� Ű�Է��� ���´�.
	if( WorldManager::GetWorldManager()->IsActiveMenuMode() ) return;

	// ä�����̶�� Ű�Է��� ���´�.
	if( ((CGamePlayState*)CGameApp::GetStateManager().GetCurrentStateClass())->IsChatMode() )
		return;
	
	//ĳ���� �̵� Ű �Է�
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

	if(pkKB->KeyIsDown(NiInputKeyboard::KEY_W))		//����
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
			if(bMoveKey)	//����Ű �Է��� ��� x,y������ �պ��Ͱ� 1�� �ǵ��� ��� ���ͷ� ���������
			{
				m_kMoveDir.Unitize();		//���͸� ���� ����ȭ �Ѵ�	
			}
		}		
		bSetAni = true;
	}
	else if(pkKB->KeyIsDown(NiInputKeyboard::KEY_S))	//����
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
			if(bMoveKey)	//����Ű �Է��� ��� x,y������ �պ��Ͱ� 1�� �ǵ��� ��� ���ͷ� ���������
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
		//������
		m_pkEquipWeapon->Reload();

//		m_ProcessAnimation.SetAnimation(Animation_Enum::CHANGECLIP,ProcessAnimation::NONINTERRUPT,fTime);
		bSetAni = true;
	}

	//�Ʊ� ĳ���� ��Ȱ ��Ű��
	if(pkKB->KeyWasPressed(NiInputKeyboard::KEY_E))
	{
		m_bRecover = true;		
		//���ڹ��� ���μ����� ����
		CStateManager &rStateManager = CGameApp::GetStateManager();
		CGamePlayState* pGameState = (CGamePlayState*)rStateManager.GetStateClass("GamePlay");
		pGameState->SetRebirthing(true);
	}
	else if(pkKB->KeyWasReleased(NiInputKeyboard::KEY_E))
	{
		m_bRecover = false;
		//���ڹ��� ���μ����� ����
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
	//if(!m_bRecover) return;				//E��ư(ȸ��)��ư Ŭ���� �� �Ǿ� ������ ����

	float fDelta = fTime - m_fLastUpdate;

	BYTE	selfCamp = NetworkManager::GetInstance()->GetCamp();		//�ڽ��� ����
	BYTE	selfID   = NetworkManager::GetInstance()->GetID();			//�ڽ��� ���̵�
	
	//ĳ���� �߿��� ã�ƺ��ô�... ��ŷ�Ǵ� �Ʊ� ĳ���Ͱ� �ִ���
	std::vector<CCharacter*>& vecOtherCharacter = WorldManager::GetWorldManager()->GetCharacterManager()->GetColliderVector();
	PlayersMap conPlayer = NetworkManager::GetInstance()->LockPlayers();
	
	//��Ʈ��ũ �Ŵ����� �÷��̾� �������� ���� ������ �÷��̾� ���̵� ã��
	PlayersMap::iterator iter = conPlayer.begin();
	for( ;iter !=conPlayer.end();++iter)
	{
		//ġ���Ϸ��� ĳ���Ͱ� �Ʊ��̰� �λ���� �� ��츸 ó��
		const UserInfo&  userInfo = iter->second.userInfo;  
		if(selfCamp == userInfo.camp && userInfo.state == INCAPACITY)
		{
			CCharacter* pkOtherCharacter = NULL;	//�ٸ� �÷��̾� ĳ����
			NiPoint3	kOrigin,kDir;				//��ŷ ����
			BYTE id = userInfo.id;					//�ٸ� �÷��̾� ���̵�

			selfID > id ? pkOtherCharacter = vecOtherCharacter[id] :
				          pkOtherCharacter = vecOtherCharacter[id-1];
			m_RecoveryPick.RemoveTarget();
			m_RecoveryPick.SetTarget(pkOtherCharacter->GetCharRoot());	//���� �÷��̾ ��ŷ Ÿ������ ����
			//��ŷ ���� ����
			int scrX = CGameApp::mp_Appication->GetAppWidth();
			int scrY = CGameApp::mp_Appication->GetAppHeight();
			CameraManager::Get()->GetCamera()->WindowPointToRay(scrX/2,scrY/2,kOrigin,kDir);	//���� ����
						
			//��ŷ Ȯ��
			if(m_RecoveryPick.PickObjects(kOrigin,kDir))
			{
				NiPick::Results& kRecoveryResults = m_RecoveryPick.GetResults();
				NiPick::Record*  kRecord = kRecoveryResults.GetAt(0);

				float fDistance = kRecord->GetDistance();		//��ŷ�� ������Ʈ���� �Ÿ�
				if(fDistance < 1.0f)							//��ŷ�� ����� �Ÿ��� 1m���� �ϰ�� ȸ�� ����
				{
					m_fRecoveryTime += fDelta;
					float fValue = 1.0f * m_fRecoveryTime / RESPONE_TIME;
					if(1.0f <= fValue)
					{						
						m_fRecoveryTime = 0;
						iter->second.userInfo.state = NORMAL;	//��� ���·� ����

						//��Ŷ ����
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


//�ִϸ��̼� �ݹ� �Լ�
//�ִϸ��̼��� Ȱ��ȭ�Ǹ� �ݹ��� �ҷ����� �ȴٴ�
void CUserCharacter::CharacterCallbackObject::AnimActivated(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime)
{
	 m_bJumpData = true;		//���� �����ߴ��� ����
	 m_fCurrTime = CGameApp::mp_Appication->GetAccumTime();//NiGetCurrentTimeInSec();		//���� ���� �ð�.
		 
	 //�������� ������ �ð�
	 m_fEndTime = pkManager->GetNextEventTime(NiActorManager::TEXT_KEY_EVENT,
			eSequenceID,"end");
}

//�ִϸ��̼��� ������ ���� ���� ����� �ݹ�
void CUserCharacter::CharacterCallbackObject::EndOfSequence(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, float fCurrentTime,
            float fEventTime)
{
	//Reset();	//�ݹ鰪 �ʱ�ȭ
}

void CUserCharacter::CharacterCallbackObject::TextKeyEvent(NiActorManager* pkManager,
            NiActorManager::SequenceID eSequenceID, const NiFixedString& kTextKey,
            const NiTextKeyMatch* pkMatchObject,float fCurrentTime, float fEventTime)
{
	//Reset();	//�ݹ鰪 �ʱ�ȭ
}

//�ݹ鰪 �ʱ�ȭ
void CUserCharacter::CharacterCallbackObject::Reset()
{
	m_bJumpData = false;	//���� �����ߴ��� ����
	m_bJumpable = false;	

		
	m_fCurrTime	= 0.0f;		//���� �ð�
	m_fEndTime	= 0.0f;
}

void CUserCharacter::CharacterCallbackObject::Init()
{
	//������ ���� ��ũ��Ʈ���� �����´�.
	FILE*	pFile = NULL;
	fopen_s(&pFile,"Data/script/JumpForce.dat","r");
	if(pFile == NULL)
	{
		NiMessageBox("JumpForce.dat�� ã�� ���� �����ϴ�.","Error");
	}
	fscanf_s(pFile,"%f",&m_fJumpForce);
	//m_fJumpForce = JUMP_VELOCITY;
	m_fCurrTime	= 0.0f;		//���� �ð�
	m_fEndTime	= 0.0f;

	m_bJumpData = false;	//���� �����ߴ��� ����
	m_bJumpable = false;	
}

