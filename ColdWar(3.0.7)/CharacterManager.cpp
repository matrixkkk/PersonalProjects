#include "NetworkManager.h"
#include "CharacterManager.h"
#include "CameraManager.h"
#include "WorldManager.h"
//#include "ZFLog.h"

CharacterManager::CharacterManager()
{
	//임시코드
	NiCollisionData::SetEnableVelocity(true);
	m_pkUserCharacter = NULL;
	InitializeCriticalSection(&m_csCharacters);
}

CharacterManager::~CharacterManager()
{
	if(m_pkUserCharacter)
		NiDelete m_pkUserCharacter;

	m_pkUserCharacter = NULL;
	
	//CharacterSolider객체 메모리 해제
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
	m_pkVecLoadedCharacter.push_back(pkActorManager);		//컨테이너에 추가

	pkActorManager = NiActorManager::Create("Data/Character/EU/EU/EU.kfm");

	if(!pkActorManager)
	{
		NiMessageBox("CharacterManager::Initialize -- EU.kfm Create Failed",NULL);
		return false;
	}
	m_pkVecLoadedCharacter.push_back(pkActorManager);		//컨테이너에 추가

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

	// 싱글 모드
	if( !bNetMode )
	{
		if( m_pkUserCharacter )
		{
			m_pkUserCharacter->Update(fTime);
			return;
		}
	}
	// 네트워크 모드
	else
	{
		// 네트워크 모드
		BYTE nID = NetworkManager::GetInstance()->GetID();
		PlayersMap::iterator iterPlayer;
		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		// ----유저 캐릭터 업데이트 ----
		//비어있을 경우 업데이트 할 필요가 없다 시붕	
		if(m_pkUserCharacter)
		{
			m_pkUserCharacter->Update(fTime);	

			//캐릭터 정보 전송
			NiNode* pkNode = m_pkUserCharacter->GetCharRoot();

			CameraManager* pkCM = CameraManager::Get();

			//============================================================
			// 평상시에는 지속적으로 내 업데이트 된 정보를 패킷에 셋팅 전송
			//============================================================
			struct UserInfo info;
			info.Heading	= pkCM->GetCameraHeading();
			info.pos		= pkNode->GetTranslate();
			info.Vertical	= pkCM->GetCameraPitch();
			info.animation	= m_pkUserCharacter->GetActorManager()->GetTargetAnimation();
			info.state		= m_pkUserCharacter->GetStatus();
			//=============================================================
			
			//=============================================================
			// 만약 죽은 상태에서 리스폰 타임 처리
			//=============================================================
			if( DEATH == m_pkUserCharacter->GetStatus() )
			{				
				// 리스폰 되었을시 기존 패킷을 재 설정
				if( NetworkManager::GetInstance()->ProcessRespone( fTime) )
				{						
					// 점령지가 하나도 없는 상태이면 시작위치에서 시작
					if(  conPlayers[ nID ].userInfo.territory == nsPlayerInfo::TERRITORY_NO )
					{
						info.pos = WorldManager::GetWorldManager()->GetRespone()->GetStartPosition( 
							conPlayers[ nID ].userInfo.camp );					 
					}
					// 점령지가 1개라도 있으면 해당 점령지와 연관된 리스폰 위치 적용
					else
					{
						info.pos = WorldManager::GetWorldManager()->GetRespone()->GetResponePosition( 
							conPlayers[ nID ].userInfo.camp, conPlayers[ nID ].userInfo.territory );
					}

					info.animation	= Animation_Enum::IDLE_RIFLE;
					info.Heading	= 0.0f;							
					info.Vertical	= 0.0f;						
		
					// 캐릭터 기본 사항 리셋( 총알, 애니메이션, 위치, 기타 정보 등등 )
					m_pkUserCharacter->Reset( info.pos, fTime );
				}
					
			}		
			// 내정보이면 SEnd 서버이면 다른 얘들에게 정보 전송
			NetworkManager::GetInstance()->UpdateSelfData(info); 		
		}

		//=================================================================
		// 다른 플레이어 캐릭터 Transfrom 업데이트
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
			m_pkVecCharacter[ i ]->GetActorManager()->SetTargetAnimation(userInfo.userInfo.animation);	//애니메이션 세팅
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

	//병과에 따라 actormanager를 클로닝해서 CharacterSoldier클래스에 넘겨준다.
	
	pkActor = m_pkVecLoadedCharacter[uiClass];			//캐릭터 컨테이너에서 actor클래스를 가져옴
	pkClonActor = pkActor->Clone();						//actorManager 클로닝

//	ZFLog::g_sLog->Log("유닛 생성 ID: %d",uiClass);
	/*using namespace BranchOfMilitary;

	switch(uiClass)
	{
		case Gerilla :				//게릴라 생성
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로닝
			break;
		case EU_Soldier:
			pkActor = m_pkVecLoadedCharacter[1];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case EU_Sniper:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case EU_Medic:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case EU_ASP:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case US_Soldier:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case US_Sniper:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case US_Medic:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
		case US_ASP:
			pkActor = m_pkVecLoadedCharacter[0];		//게릴라 actor가져옴
			pkClonActor = pkActor->Clone();				//actorManager 클로
			break;
	}	
	*/
	
	if( bSelf ) //본인 캐릭터 추가
	{
		//아더플레이어 추가
		m_pkUserCharacter = NiNew CUserCharacter;	
		m_pkUserCharacter->Initialize(pkClonActor,kSpawn);	
	}
	else
	{
		//아더플레이어 추가
		m_pkVecCharacter.push_back(NiNew CCharacter);	
		m_pkVecCharacter.back()->Init(pkClonActor,kSpawn);	
	}
}


void CharacterManager::AllResetIntersectionRecord()
{
	m_pkUserCharacter->ResetIntersectionRecord();	
}

