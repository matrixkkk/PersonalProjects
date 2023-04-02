#include "NetworkManager.h"
#include "CharacterManager.h"
#include "CameraManager.h"
#include "WorldManager.h"
//#include "ZFLog.h"

CharacterManager::CharacterManager()
{
	//�ӽ��ڵ�
	NiCollisionData::SetEnableVelocity(true);
	m_pkUserCharacter = NULL;
	InitializeCriticalSection(&m_csCharacters);
}

CharacterManager::~CharacterManager()
{
	if(m_pkUserCharacter)
		NiDelete m_pkUserCharacter;

	m_pkUserCharacter = NULL;
	
	//CharacterSolider��ü �޸� ����
	for(unsigned int i=0;i<m_pkVecCharacter.size();i++)
	{
		NiDelete m_pkVecCharacter[i];
	}
	for(unsigned int i=0;i<m_pkVecLoadedCharacter.size();i++)
	{
		NiDelete m_pkVecLoadedCharacter[i];
	}
	
	m_pkVecCharacter.clear();
	m_pkVecLoadedCharacter.clear();
	m_pkStartPoints.clear();
	DeleteCriticalSection(&m_csCharacters);
}

bool CharacterManager::Initialize(NiNode* pkNode)
{
	NiActorManager* pkActorManager;
	
	pkActorManager = NiActorManager::Create("Data/Character/gerilla/gerilla.kfm");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- gerilla.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);		//�����̳ʿ� �߰�

	pkActorManager = NiActorManager::Create("Data/Character/EU/EU/EU.kfm");

	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- EU.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);		//�����̳ʿ� �߰�

	pkActorManager = NiActorManager::Create("Data/Character/EU/EU_Sniper/EU.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- EU.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/EU/EU_Medic/EU.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- EU.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/EU/EU_ASP/EU.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- EU.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/USA/USA/USA.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- USA.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/USA/USA_Sniper/USA.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- USA.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/USA/USA_Medic/USA.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- USA.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);

	pkActorManager = NiActorManager::Create("Data/Character/USA/USA_ASP/USA.KFM");
	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- USA.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);
	
	return true;
}

void CharacterManager::Update(float fTime, bool bNetMode )
{

	// �̱� ���
	if( !bNetMode )
	{
		if( m_pkUserCharacter )
		{
			m_pkUserCharacter->Update(fTime);
			return;
		}
	}
	// ��Ʈ��ũ ���
	else
	{
		// ��Ʈ��ũ ���
		BYTE nID = NetworkManager::GetInstance()->GetID();
		PlayersMap::iterator iterPlayer;
		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		// ----���� ĳ���� ������Ʈ ----
		//������� ��� ������Ʈ �� �ʿ䰡 ���� �ú�	
		if(m_pkUserCharacter)
		{
			m_pkUserCharacter->Update(fTime);	

			//ĳ���� ���� ����
			NiNode* pkNode = m_pkUserCharacter->GetCharRoot();

			CameraManager* pkCM = CameraManager::Get();

			//============================================================
			// ���ÿ��� ���������� �� ������Ʈ �� ������ ��Ŷ�� ���� ����
			//============================================================
			struct UserInfo info;
			info.Heading	= pkCM->GetCameraHeading();
			info.pos		= pkNode->GetTranslate();
			info.Vertical	= pkCM->GetCameraPitch();
			info.animation	= m_pkUserCharacter->GetActorManager()->GetTargetAnimation();
			info.state		= m_pkUserCharacter->GetStatus();
			//=============================================================
			
			//=============================================================
			// ���� ���� ���¿��� ������ Ÿ�� ó��
			//=============================================================
			if( DEATH == m_pkUserCharacter->GetStatus() )
			{				
				// ������ �Ǿ����� ���� ��Ŷ�� �� ����
				if( NetworkManager::GetInstance()->ProcessRespone( fTime) )
				{						
					// �������� �ϳ��� ���� �����̸� ������ġ���� ����
					if(  conPlayers[ nID ].userInfo.territory == nsPlayerInfo::TERRITORY_NO )
					{
						info.pos = WorldManager::GetWorldManager()->GetRespone()->GetStartPosition( 
							conPlayers[ nID ].userInfo.camp );					 
					}
					// �������� 1���� ������ �ش� �������� ������ ������ ��ġ ����
					else
					{
						info.pos = WorldManager::GetWorldManager()->GetRespone()->GetResponePosition( 
							conPlayers[ nID ].userInfo.camp, conPlayers[ nID ].userInfo.territory );
					}

					info.animation	= Animation_Enum::IDLE_RIFLE;
					info.Heading	= 0.0f;							
					info.Vertical	= 0.0f;						
		
					// ĳ���� �⺻ ���� ����( �Ѿ�, �ִϸ��̼�, ��ġ, ��Ÿ ���� ��� )
					m_pkUserCharacter->Reset( info.pos, fTime );
				}
					
			}		
			// �������̸� SEnd �����̸� �ٸ� ��鿡�� ���� ����
			NetworkManager::GetInstance()->UpdateSelfData(info); 		
		}

		//=================================================================
		// �ٸ� �÷��̾� ĳ���� Transfrom ������Ʈ
		//=================================================================	
		iterPlayer = conPlayers.begin();	
		for(unsigned int i = 0; i < m_pkVecCharacter.size(); i++ )
		{
			if( iterPlayer->second.userInfo.id == nID )
			{
				++iterPlayer;
			}
			
			GAME_USER_INFO	&userInfo = iterPlayer->second;

			NiPoint3	kPos = userInfo.userInfo.pos;
			NiMatrix3	kRot,kSpineRot;
			kRot.MakeZRotation(userInfo.userInfo.Heading);

			NiNode* pkChar = m_pkVecCharacter[ i ]->GetCharRoot();
			pkChar->SetTranslate(kPos);
			pkChar->SetRotate(kRot);
			m_pkVecCharacter[ i ]->GetActorManager()->SetTargetAnimation(userInfo.userInfo.animation);	//�ִϸ��̼� ����
			kSpineRot.MakeZRotation(-userInfo.userInfo.Vertical);
			m_pkVecCharacter[ i ]->GetSpineBone()->SetRotate(kSpineRot);
			m_pkVecCharacter[ i ]->SetStatus(userInfo.userInfo.state);
			m_pkVecCharacter[ i ]->Update(fTime);	

			++iterPlayer;
		}

		NetworkManager::GetInstance()->UnLockPlayers();	
	}
}

void CharacterManager::DrawSoldier(NiCamera* pkCamera)
{
	if(!m_pkUserCharacter)
		return;

	m_pkUserCharacter->Draw(pkCamera);

	
	for(unsigned int i=0; i<m_pkVecCharacter.size(); i++)
	{
		m_pkVecCharacter[i]->Draw(pkCamera);
	}
}

void CharacterManager::AddCharacter(unsigned int uiClass,const NiTransform& kSpawn, bool bSelf )
{
	NiActorManager* pkActor = NULL;
	NiActorManager* pkClonActor = NULL;

	//������ ���� actormanager�� Ŭ�δ��ؼ� CharacterSoldierŬ������ �Ѱ��ش�.
	
	pkActor = m_pkVecLoadedCharacter[uiClass];			//ĳ���� �����̳ʿ��� actorŬ������ ������
	pkClonActor = pkActor->Clone();						//actorManager Ŭ�δ�

//	ZFLog::g_sLog->Log("���� ���� ID: %d",uiClass);
	/*using namespace BranchOfMilitary;

	switch(uiClass)
	{
		case Gerilla :				//�Ը��� ����
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ�δ�
			break;
		case EU_Soldier:
			pkActor = m_pkVecLoadedCharacter[1];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case EU_Sniper:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case EU_Medic:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case EU_ASP:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case US_Soldier:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case US_Sniper:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case US_Medic:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
		case US_ASP:
			pkActor = m_pkVecLoadedCharacter[0];		//�Ը��� actor������
			pkClonActor = pkActor->Clone();				//actorManager Ŭ��
			break;
	}	
	*/
	
	if( bSelf ) //���� ĳ���� �߰�
	{
		//�ƴ��÷��̾� �߰�
		m_pkUserCharacter = NiNew CUserCharacter;	
		m_pkUserCharacter->Initialize(pkClonActor,kSpawn);	
	}
	else
	{
		//�ƴ��÷��̾� �߰�
		m_pkVecCharacter.push_back(NiNew CCharacter);	
		m_pkVecCharacter.back()->Init(pkClonActor,kSpawn);	
	}
}


void CharacterManager::AllResetIntersectionRecord()
{
	m_pkUserCharacter->ResetIntersectionRecord();	
}

