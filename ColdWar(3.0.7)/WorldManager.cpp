#include "NetworkManager.h"
#include "WorldManager.h"
#include "GameApp.h"
#include "CLoadingDisplay.h"
#include "CTerritory.h"
//#include "CharacterSoldier.h"
#include "NiFastAlphaAccumulator.h"

#include <NiMaterialToolkit.h>

WorldManager*		WorldManager::m_pkWorldManager = NULL;	

WorldManager::WorldManager() :  m_kVisible(1024, 1024),m_kCuller(&m_kVisible)
{
	m_bClicked			= false;
	m_bColorPickProcess = false;
	m_bActiveMenu		= false;
	m_bNetMode			= false;

	m_uiVisibleObjCount = 0;
	m_uiCntObjects		= 0;

	m_spFastAlphaSorter = 0;
	m_pkCameraManager  = NULL;
	m_pkCollisionManager = NULL;
	m_pkCharacterManager = NULL;
	m_pkGameSystemManager = NULL;

	m_pkLightNode	= NULL;				// 씬 어태치 노드 (빛 적용)
	m_pkNotLightNode= NULL;				// 씬 어태치 노드 (빛 노적용)

	m_pkTerrain	= NULL;					// 지형

	m_pkSkyBox = NULL;					// 스카이 박스

	m_pkStaticObjectManager = NULL;		// 스태틱 오브젝트	

	m_pkBillBoardManager	= NULL;		// 빌보드

	m_pkDecalManager		= NULL;		// 데칼

	m_pkPicker				= NULL;		//피커

	m_pkRespone				= NULL;

	m_pkTerritory			= NULL;

	//피킹 셋팅
	//Fire Pick
	m_kFirePick.SetPickType(NiPick::FIND_ALL);
	m_kFirePick.SetSortType(NiPick::NO_SORT);
	m_kFirePick.SetIntersectType(NiPick::BOUND_INTERSECT);
	m_kFirePick.SetCoordinateType(NiPick::WORLD_COORDINATES);
	m_kFirePick.SetFrontOnly(true);
	m_kFirePick.SetObserveAppCullFlag(false);
    m_kFirePick.SetReturnTexture(false);
    m_kFirePick.SetReturnNormal(true);
    m_kFirePick.SetReturnSmoothNormal(false);
    m_kFirePick.SetReturnColor(false);	
}

WorldManager::~WorldManager()
{
	ReleaseObject();

	NiDelete	m_pkCameraManager;		//캐릭터에게 붙일 카메라
	NiDelete	m_pkCollisionManager;	//충돌관리자
	NiDelete	m_pkCharacterManager;	//캐릭터 매니저
	
	//시스템 관리자 제거
	if(m_pkGameSystemManager)
		NiDelete m_pkGameSystemManager;

	if(m_pkRespone)
		NiDelete m_pkRespone;
	
	if(m_pkPicker)
		delete		m_pkPicker;
	
	m_pkCameraManager = NULL;
	m_pkCharacterManager = NULL;
	m_pkCollisionManager = NULL;
	m_pkPicker = NULL;


	m_spFog				  = 0;
	m_spMaterialProp	  = 0;
	m_spWireframeProperty = 0;		//와이어 프레임 프로퍼티
	m_spScene = 0;			//메인 씬

	//FirePick 생성
	m_kFirePick.SetPickType(NiPick::FIND_FIRST);
	m_kFirePick.SetSortType(NiPick::NO_SORT);
	m_kFirePick.SetIntersectType(NiPick::BOUND_INTERSECT);
	m_kFirePick.SetCoordinateType(NiPick::WORLD_COORDINATES);
	m_kFirePick.SetFrontOnly(true);
	m_kFirePick.SetObserveAppCullFlag(false);
    m_kFirePick.SetReturnTexture(false);
    m_kFirePick.SetReturnNormal(true);
    m_kFirePick.SetReturnSmoothNormal(false);
    m_kFirePick.SetReturnColor(false);	
}

bool WorldManager::Initialize()
{
	// 로딩 화면 생성
	if( !CLoadingDisplay::Create( 0, 632, 1024, 12,"LoadingSplash.bmp", "Loadingbar.tga" ) )
		return false;

	CLoadingDisplay::GetInstance()->Render(0.2f);

	// 월드 전체 쉐이더 라이브러리 로드
	if( !CreateMaterialToolKit() )
	{
		NiMessageBox(" CreateScene() - CreateMaterialToolKit() Failed", "Failed", MB_OK );
		return FALSE;
	}

	CLoadingDisplay::GetInstance()->Render(0.3f);
	
	//--------피킹 초기화----------
	/*m_pkPicker = new CObjectPick();		//피킹 객체 생성

	if(!m_pkPicker->Init())				//피커 초기화
		return false;
	*/
	//-----------------------------
	CLoadingDisplay::GetInstance()->Render(0.5f);

	if(!CreateCamera())		//카메라 생성
		return false;

	CLoadingDisplay::GetInstance()->Render(1.5f);

	if(!CreateScene())		//메인 씬 생성
		return false;

	CLoadingDisplay::GetInstance()->Render(77.0f);

	if(!CreateGameSystem())
		return false;
	
	CLoadingDisplay::GetInstance()->Render(79.0f);

	if(!CreateRespone())
		return false;

	CLoadingDisplay::GetInstance()->Render(82.0f);


	if(!CreateCharacter())	//캐릭터 생성
		return false;	

	CLoadingDisplay::GetInstance()->Render(94.0f);

	return true;
}

//월드 매니저 생성
bool WorldManager::Create( bool bNetMode )
{
	WorldManager* pkWorld = NiNew WorldManager;

	if(!pkWorld)
	{
		NiMessageBox("WorldManager::Create  - Failed",NULL);
		return false;
	}
	pkWorld->m_spFastAlphaSorter = NiNew NiFastAlphaAccumulator;

	if(!pkWorld->m_spFastAlphaSorter)
	{
		NiMessageBox("WorldManager::Create - Failed FastAlphaSorter Create",NULL);
		return false;
	}

	pkWorld->SetNetMode( bNetMode );

	m_pkWorldManager = pkWorld;

	return true;
}


bool WorldManager::CreateGameSystem()
{
	m_pkGameSystemManager = NiNew GameSystemManager;

	if(!m_pkGameSystemManager)
	{
		NiMessageBox("WorldManager::CreateGameSystem() -- GameSystemManager객체 생성 실패\n",NULL);
		return false;
	}
	
	float fPlayTime = 0;

	FILE *pFile = NULL;
	fopen_s(&pFile,"Data/script/PlayTime.dat","r");

	fscanf_s(pFile,"%f",&fPlayTime);
		//게임 시스템 초기화
	if(!m_pkGameSystemManager->Init(BranchOfMilitary::Gerilla,fPlayTime))	//두번째 인자. 게임 플레이 시간 s 단위
		return false;

	fclose(pFile);

	return true;
}

bool WorldManager::CreateRespone()
{
	m_pkRespone = NiNew CRespone;
	assert(m_pkRespone);

	if(!m_pkRespone->Create())
		return false;

	return true;
}



bool WorldManager::CreateCamera()
{
	//캐릭터에 붙일 카메라 생성
	m_pkCameraManager = NiNew CameraManager;

	if(!m_pkCameraManager)
	{
		NiMessageBox("WorldManager::CreateCamera -- 메모리 할당 실패\n",NULL);
		return false;
	}	

	return true;
}

//캐릭터 피킹
bool WorldManager::PickFire(FIRE_INTERSECT& vecIntersect, NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir)
{
	FireIntersectInfo	kFireIntersectInfo;

	//피킹 광선을 쏠 오브젝트
	m_kFirePick.SetTarget(pkPickRoot);
	
	if(m_kFirePick.PickObjects(kOrigin,kDir))
	{
		//충돌된 오브젝트가 있음 - - 여러개임
		//충돌된 결과들의 가져옴
		NiPick::Results& kPickResult = m_kFirePick.GetResults();
		
		//충돌 
		for(unsigned int i=0;i<kPickResult.GetSize();i++)
		{
			NiPick::Record* pkRecord = kPickResult.GetAt(i);
			NiNode* pkParent = pkRecord->GetParent();
			if(pkParent == NULL) continue;
			
			//"pick" 키값을 가지는 extra 데이터를 가진 오브젝트가 pick대상 오브젝트이다
			NiIntegerExtraData* pkED = (NiIntegerExtraData*) pkParent->GetExtraData("pick");
			if(pkED == NULL) continue;
			
			//캐릭터에 맞은 부위가 있다 ===
			unsigned int idx = vecIntersect.size();		//맞은 부위 순서대로 벡터에 넣는다.
			
			float fDamageDecrease = 1.0f/(float)(idx + 1); //감소율
			
			//맞은 부위 저장
			unsigned int uiPart = pkED->GetValue();
			
			kFireIntersectInfo.damageDecrease = fDamageDecrease;
			kFireIntersectInfo.part			  =	(BYTE) uiPart;
			//컨테이너에 맞은 부위 순서대로 저장
			vecIntersect.push_back(kFireIntersectInfo);
			
		}
		m_kFirePick.RemoveTarget();
	
		return !vecIntersect.empty();
	}
	m_kFirePick.RemoveTarget();
	return false;
}

//----------------------------------------------------
//
//	케릭터 생성 및 공간 분할한 컬리전 그룹에 케릭터 추가
//
//----------------------------------------------------
bool WorldManager::CreateCharacter()
{
	m_pkCharacterManager = NiNew CharacterManager;
	
	if(!m_pkCharacterManager->Initialize(m_spStartPosition))
	{
		NiMessageBox("WorldManager::CreateCharacter -- Create Failed\n",NULL);
		return false;
	}

	//--------------------------------------------------------------------------
	//	서버로 부터 받은 게임 클라이언트들의 정보를 이용하여 캐릭터를 생성한다.
	//--------------------------------------------------------------------------
	PlayersMap::iterator iterPlayer;
	
	NiMatrix3 rot;
	NiTransform kSpawn;  
	BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	iterPlayer = conPlayers.find( nID );

	//	내 케릭터 생성
	if( iterPlayer != conPlayers.end() )
	{
		DWORD	dwCamp = iterPlayer->second.userInfo.camp;
		DWORD	dwArmy = iterPlayer->second.userInfo.BranchOfMilitary;

		NiPoint3 kTrans = m_pkRespone->GetStartPosition(dwCamp);	//EU위치로 임시 생성
		NiTransform kTransform;
		NiMatrix3	kRot;

		kRot.MakeIdentity();

		kTransform.m_Translate = kTrans;
		kTransform.m_Rotate = kRot;
		kTransform.m_fScale = 1.0f;
		m_pkCharacterManager->AddCharacter(dwArmy,kTransform,true );
	}

	CLoadingDisplay::GetInstance()->Render(82.0f);

	if( m_bNetMode ) // 네트워크 모드일떄
	{
		// 같이 플레이할 다른 유저 케릭터 생성
		for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			if( iterPlayer->second.userInfo.id == nID )
				continue;
				
			rot.MakeZRotation(iterPlayer->second.userInfo.Heading);		
			kSpawn.m_Rotate = rot;
			kSpawn.m_Translate =iterPlayer->second.userInfo.pos;
			kSpawn.m_fScale = 1.0f;

			m_pkCharacterManager->AddCharacter( iterPlayer->second.userInfo.BranchOfMilitary, kSpawn );	
		}
	}

	NetworkManager::GetInstance()->UnLockPlayers();


	CLoadingDisplay::GetInstance()->Render(85.0f);


	//--------------------------------------------------------------------------
	//		나의 케릭터로 카메라를 셋팅해 준다.
	//--------------------------------------------------------------------------
	if(!m_pkCameraManager->Initialize(m_pkCharacterManager->GetUserCharacter()->GetCharRoot()))
	{
		NiMessageBox("WorldManager::CreateCamera -- 카메라 초기화 실패\n",NULL);
		return false;
	}	

	CLoadingDisplay::GetInstance()->Render(87.0f);
	
	// 지형 생성 및 초기화 후 케릭터를 지형의 컬리전 그룹에 추가
	if( m_pkTerrain )
	{
		m_pkTerrain->SetCharacterAtCollisionGroup( m_pkCharacterManager );

		m_pkTerrain->SetPlayer( m_pkCharacterManager->GetUserCharacter()->GetCharRoot() );
	}

	// 점령지와 케릭터 충돌관계를 설정한다.
	CTerritory::GetInstance()->Initialize();

	CLoadingDisplay::GetInstance()->Render(90.0f);

	return true;
}

void WorldManager::AddCharacter(DWORD type,const NiTransform& kSpawn)
{
	if(m_pkCharacterManager == NULL)
	{
		NiMessageBox("Do Not Create CharacterManager",NULL);
		return;
	}
	m_pkCharacterManager->AddCharacter(type,kSpawn);
}


bool WorldManager::CreateScene()
{
	m_spScene = NiNew NiNode;

	if(!m_spScene)
		return false;

	m_spScene->SetName("SceneRoot");


	//------------------------------------------------------------
	//				필수 Node 설정
	//------------------------------------------------------------				

	// Prepare scene for UpdateSelected
    bool bUpdate = true;
    bool bRigid = false;
    m_spScene->SetSelectiveUpdateFlags(bUpdate, true, bRigid);
    m_spScene->SetSelectiveUpdate(true);
    m_spScene->SetSelectiveUpdateTransforms(true);
    m_spScene->SetSelectiveUpdatePropertyControllers(true);
    m_spScene->SetSelectiveUpdateRigid(false);
    m_spScene->SetName("Application Root");


	m_pkLightNode = NiNew NiNode;
    m_pkLightNode->SetName("Light Node");
    m_pkLightNode->SetSelectiveUpdate(true);
    m_pkLightNode->SetSelectiveUpdateTransforms(true);
    m_pkLightNode->SetSelectiveUpdatePropertyControllers(true);
    m_pkLightNode->SetSelectiveUpdateRigid(false);
    AddObjectAtRoot(m_pkLightNode);

	m_pkNotLightNode = NiNew NiNode;
    m_pkNotLightNode->SetName("Not Light Node");
    m_pkNotLightNode->SetSelectiveUpdate(true);
    m_pkNotLightNode->SetSelectiveUpdateTransforms(true);
    m_pkNotLightNode->SetSelectiveUpdatePropertyControllers(true);
    m_pkNotLightNode->SetSelectiveUpdateRigid(false);
    AddObjectAtRoot(m_pkNotLightNode);

	//------------------------------------------------------------	


	//------------------------------------------------------------
	//				기본 속성 생성 & 설정
	//------------------------------------------------------------	

	// 씬에 기본 메터리얼 속성 어태치
	m_spMaterialProp = NiTCreate<NiMaterialProperty>();
    m_spMaterialProp->SetAmbientColor(0.5f*NiColor::WHITE);
    m_spMaterialProp->SetDiffuseColor(NiColor::WHITE);
    m_spMaterialProp->SetEmittance(NiColor::BLACK);
    m_spMaterialProp->SetSpecularColor(NiColor::WHITE);
    m_spScene->AttachProperty(m_spMaterialProp);	

	//와이어프레임 속성 추가
	m_spWireframeProperty = NiNew NiWireframeProperty;
	if(!m_spWireframeProperty)
		return false;

	//메인 씬에 어태치
	m_spScene->AttachProperty(m_spWireframeProperty);
	m_spWireframeProperty->SetWireframe(false);				//일단 와이어프레임 속성 끔

	NiDitherProperty* pD = NiNew NiDitherProperty;
    pD->SetDithering(true);
    m_pkLightNode->AttachProperty(pD);
	
	CLoadingDisplay::GetInstance()->Render(2.0f);

	CreateFog();

	CLoadingDisplay::GetInstance()->Render(2.2f);

	// Light Attach
	CreateLight();

	CLoadingDisplay::GetInstance()->Render(3.0f);

	//------------------------------------------------------------	


	//------------------------------------------------------------
	//			 맵을  로드 & 씬 초기화		
	//------------------------------------------------------------

	// 맵을  로드한다.
	FILE *fstream; 
	//errno_t err; 

	//fstream = fopen( "./Data/Map/testtest4.map", "rb" );
	fopen_s(&fstream, "./Data/Map/FinalTest.map", "rb" );
	//if( err == 0 )
	//	return false;
	if( fstream == NULL )
		return false;

	// 각 타일들에 버텍스 정보를 셋팅하고, 텍스쳐 정보를 셋팅한다.
	if( LoadScene( fstream, std::string("Data/Map/FinalTest.map") ) )
	{
		fclose(fstream);
	}
	
	// 오브젝트를 공간 분할
	if( m_pkTerrain ) 
	{
		// 렌더링 그룹으로 분할
		m_pkStaticObjectManager->SpaceDivisionForRendering( m_pkTerrain );

		CLoadingDisplay::GetInstance()->Render(65.0f);
		// 컬리전 그룹으로 분할
		m_pkStaticObjectManager->SpaceDivisionForCollision( m_pkTerrain );

		CLoadingDisplay::GetInstance()->Render(70.0f);
	}		
	m_spScene->Update(0.0f);
	m_spScene->UpdateNodeBound();
	m_spScene->UpdateProperties();
	m_spScene->UpdateEffects();
	
	
	CLoadingDisplay::GetInstance()->Render(75.0f);
	
	return true;
}


bool WorldManager::CreateLight()
{
	if( !m_spScene )
		return FALSE;

	m_spDirectionalLignt1 = NiTCreate<NiDirectionalLight>();

    m_spDirectionalLignt1->SetAmbientColor(NiColor::BLACK);
    m_spDirectionalLignt1->SetDiffuseColor(NiColor::WHITE);
    m_spDirectionalLignt1->SetSpecularColor(NiColor::WHITE);

    NiMatrix3 kRot1(
        NiPoint3(0.44663f, 0.496292f, -0.744438f),
        NiPoint3(0.0f, -0.83205f, -0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f) );

    m_spDirectionalLignt1->SetRotate(kRot1);
    m_spDirectionalLignt1->SetName("Default Light");

    m_spDirectionalLignt2 = NiTCreate<NiDirectionalLight>();

    m_spDirectionalLignt2->SetAmbientColor(NiColor::BLACK);
    m_spDirectionalLignt2->SetDiffuseColor(NiColor::WHITE);
    m_spDirectionalLignt2->SetSpecularColor(NiColor::WHITE);

    NiMatrix3 kRot2(
        NiPoint3(-0.44663f, -0.496292f, 0.744438f),
        NiPoint3(0.0f, 0.83205f, 0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f));

    m_spDirectionalLignt2->SetRotate(kRot2);
    m_spDirectionalLignt2->SetName("Default Light");

    m_spDirectionalLignt1->AttachAffectedNode(m_pkLightNode);
    m_spDirectionalLignt2->AttachAffectedNode(m_pkLightNode);

    m_spScene->AttachChild(m_spDirectionalLignt1);
    m_spScene->AttachChild(m_spDirectionalLignt2);
	
	EnableDefaultLights(true);
	
	return TRUE;
}

void WorldManager::AttachLight1AffectedNode( NiNode* pObject)
{
	m_spDirectionalLignt1->AttachAffectedNode(pObject);
	m_spDirectionalLignt1->Update(0.0f);
	m_spDirectionalLignt1->UpdateProperties();
}

void WorldManager::AttachLight2AffectedNode( NiNode* pObject)
{
	m_spDirectionalLignt2->AttachAffectedNode(pObject);
	m_spDirectionalLignt2->Update(0.0f);
	m_spDirectionalLignt2->UpdateProperties();
}

// 기본 조명을 키고 끈다.
void WorldManager::EnableDefaultLights( bool bEnable )
{
	if(NULL == m_pkLightNode)
		return;

	unsigned int uiCntObjects = m_pkLightNode->GetArrayCount();

    for (unsigned int ui = 0; ui < uiCntObjects; ui++)
    {
        NiLight* pkLight = NiDynamicCast(NiLight, m_pkLightNode->GetAt(ui));

        if (pkLight && pkLight->GetName() && strcmp(pkLight->GetName(), "Default Light") == 0)
        {
            pkLight->SetSwitch(bEnable);
        }
    }
}



BOOL WorldManager::CreateMaterialToolKit()
{

	if( !CGameApp::mp_Appication->GetRenderer() )
		return FALSE;

	// 쉐이더 폴더에 각종 쉐이더 정보를 관리 하는 툴킷을 생성
	NiMaterialToolkit* pkShaderKit = NiMaterialToolkit::CreateToolkit();

	std::string strShaderPath("./Data/Shaders/");

	if ( !strShaderPath.size() )
	{
		NiMessageBox("The environment variable NDL_SHADER_LIBRARY_PATH ",
			"must\nbe defined for this application to properly execute.Missing Environment Variable"
			, MB_OK );
	}
	else
	{
		NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
		pkToolkit->LoadFromDLL( strShaderPath.c_str() );

		std::string strShaderDir = strShaderPath + "\\Data\\" +
#if defined(_DX9)
			"DX9\\";
#endif  //#if defined(_DX9)
		pkToolkit->SetMaterialDirectory( strShaderDir.c_str() );
	}

	NiMaterialHelpers::RegisterMaterials(m_spScene, CGameApp::mp_Appication->GetRenderer() );

 return TRUE;
}

void WorldManager::PartialRender(NiCamera* pkCamera, NiAVObject* pkObject)
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	assert(pkRenderer);
	if(!pkRenderer)
		return;

	//Cull first.
	m_kVisible.RemoveAll();
	//float fStartTime = NiGetCurrentTimeInsec();
	m_kCuller.Process(pkCamera,pkObject,0);
	//m_fCullTime += NiGetCurrenTimeInSec() - fStartTime; 
	m_uiVisibleObjCount += m_kVisible.GetCount();		//그릴 오브젝트의 수
	
	NiDrawVisibleArrayAppend(m_kVisible);
	//m_fRenderTime += NiGetCurrentTimeInSec() - fStartTime;
}

//씬을 그린다.
void WorldManager::DrawScene(NiCamera* pkCamera)
{
	assert(m_spScene && m_spScene->IsVisualObject());
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();		//렌더러를 가져옴
	
	//Disabled alpha sorter : 스카이 박스 그릴때
	//스카이박스 처리 루틴
    DrawSkyBox(pkCamera);

	//Enable alpha sorter
	assert(m_spFastAlphaSorter);
	m_spFastAlphaSorter->StartAccumulating(pkCamera);
	assert(m_spFastAlphaSorter->IsAccumulating());
			
	//-------------------------------------------------------------
	//						전체 오브젝트 씬
	//-------------------------------------------------------------

	m_kVisible.RemoveAll();
	// 컬링

	NiNode* pkCity = (NiNode*) m_spScene->GetObjectByName("ObjectGroup");
	assert(pkCity);

//	NiCullScene(pkCamera , pkCity, m_kCuller, m_kVisible);	
	NiCullScene(pkCamera , m_spScene, m_kCuller, m_kVisible);	

	if( m_kVisible.GetCount() > 0 )
	{
		NiDrawVisibleArray( pkCamera , m_kVisible );
	}
	pkRenderer->SetSorter(m_spFastAlphaSorter);

	//-------------------------------------------------------------
	
	//모든 캐릭터를 그린다.
	//pkRenderer->SetSorter(NULL);
	if( m_pkCharacterManager ) m_pkCharacterManager->DrawSoldier(pkCamera);	

	// 점령지 렌더
	CTerritory::GetInstance()->Render( pkCamera );
	
	//-------------------------------------------------------------
	//  Effect Rendering
	//-------------------------------------------------------------
	CDustManager::GetInstance()->Render( pkCamera );

	//Finish accumulationg
	//pkRenderer->SetSorter(m_spFastAlphaSorter);
	pkRenderer->GetSorter()->FinishAccumulating();
	pkRenderer->SetSorter(NULL);
}

void WorldManager::DrawSkyBox(NiCamera* pkCamera)
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();		//렌더러를 가져옴

	assert(!pkRenderer->GetSorter());
	
	//기존 프러스텀을 기억한다.
	NiFrustum kOldFrustum = pkCamera->GetViewFrustum();

	//스카이 박스까지 보일 far거리를 지정해줘서 카메라에 frustum에 set한다.
	pkCamera->SetViewFrustum(NiFrustum(-1.0f,1.0f,1.0f,-1.0f, 0.1f, 10000.0f));
	pkRenderer->SetCameraData(pkCamera);

	NiNode* pkSkyBox = m_pkSkyBox->GetObject();
	NiNode* pkGeom = (NiNode*) pkSkyBox->GetObjectByName("SkyBox");
	
	//스카이박스 지오메트리를 즉시 렌더한다.
	for(unsigned int i=0;i<pkGeom->GetArrayCount();i++)
	{
		NiGeometry* geom = (NiGeometry*) pkGeom->GetAt(i);
		geom->RenderImmediate(pkRenderer);
	}
	
	//렌더링 후 기존 프러스텀 set
	pkCamera->SetViewFrustum(kOldFrustum);
	pkRenderer->SetCameraData(pkCamera);
}

//스크린 아이템 그리기
void WorldManager::RenderScreenItem()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	assert(pkRenderer);
	
	pkRenderer->SetScreenSpaceCameraData();

	//유저 캐릭터 객체를 가져온다.
	CUserCharacter* pkUserCharacter = m_pkCharacterManager->GetUserCharacter();
	assert(pkUserCharacter);
	
	//크로스헤어 객체를 가져온다.
	CrossHair* pkCross = pkUserCharacter->GetCrossHairDisplay();
	assert(pkCross);
	
	//크로스헤어 그리기
	pkCross->Draw();

	//게임 시스템 그리기
	m_pkGameSystemManager->DrawUI();

	CTerritory::GetInstance()->RenderScreenItem();

	//머즐 플래쉬 드로우
	m_pkCharacterManager->GetUserCharacter()->DrawMuzel();
}

void WorldManager::Draw()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	NiCamera*	pkCamera = m_pkCameraManager->GetCamera();
	
	DrawScene(pkCamera);

	//스크린 아이템 그리기
	RenderScreenItem();
}

void WorldManager::Update()
{
	ProcessInput();

	//피킹 수행
	PerformPick();
	FireResultProcessing();

	float fTime = CGameApp::mp_Appication->GetAccumTime();

	// 카메라 업데이트
	m_pkCameraManager->UpdateCamera( fTime );

	// 월드 씬 오브젝트 업데이트
	std::for_each( m_vecSceneObjects.begin(), m_vecSceneObjects.end(), std::bind2nd( std::mem_fun( &GBObject::UpdateObject ), fTime ) );

	// 캐릭터 업데이트
	if( m_pkCharacterManager ) m_pkCharacterManager->Update(fTime, m_bNetMode);

	//게임 시스템 업데이트 ( 네트워크 모드일때만 )
	if(m_pkGameSystemManager ) m_pkGameSystemManager->Update(fTime);

	//Dust Effect 업데이트
	CDustManager::GetInstance()->Update( fTime );

	// 점령지 충돌 처리
	CTerritory::GetInstance()->Update( fTime );

	//사운드 업데이트
	CSoundManager::GetInstance()->Update();
}

void WorldManager::VectorFireDataProcess(FireInfo* pFireInfo)	//fireData 처리
{
	BYTE	selfID	  = NetworkManager::GetInstance()->GetID();	

	if(pFireInfo!=NULL)
	{
		//다른 사람의 사격 정보를 알기 때문에.. 그 쏜 사람의 머즐 플래시를 보여주기 위해 여기서
		//처리 한다.
		DWORD dwFireUser = pFireInfo->fireUserID;	//쏜 사람의 아이디
		if(dwFireUser != selfID)					//본인일 경우 처리 안함
		{
			dwFireUser > selfID ?	dwFireUser-- : dwFireUser;
			
			//총을 쏜 캐릭터 오브젝트를 가져온다.
			CCharacter* pkFireCharacter = m_pkCharacterManager->GetCharacterIndex(dwFireUser);
			//머즐 업데이트
			pkFireCharacter->UpdateMuzel(CGameApp::mp_Appication->GetAccumTime());
		}
		//총을 쏴 서 사람이 맞았고 그 사람이 현재 유저인 경우
		if(pFireInfo->bHitPlayer == true && pFireInfo->damageUserID == selfID)
		{
			//현재 나의 상태
			DWORD dwCondition = m_pkCharacterManager->GetUserCharacter()->GetStatus();
			
			//내가 총에 맞았는데~ 난 이미 죽은 상태엿어!! 그니깐 전송을 안해
			if(dwCondition != DEATH) 
			{
				//---------- 데미지 계산 -----------
				//GameSystemManager에 데미지와 맞은 부위를 넘겨주어 캐릭터 상태 받아옴
				DWORD dwCondition = m_pkGameSystemManager->ProcessCondition(pFireInfo);

				struct _PACKET_FIRE_RESULT packet;
				packet.fireResult.m_dwFireUserID = pFireInfo->fireUserID;
				packet.fireResult.m_dwCondition  = dwCondition;
				packet.fireResult.m_dwResultID   = (DWORD)NetworkManager::GetInstance()->GetID();

				//발사 결과를 전송
				NetworkManager::GetInstance()->SendFireResult(packet);
			}			
		}
	}
}
void WorldManager::VectorFireResultProcess(FireResult* pFireResult)	//fireResult 처리
{
	BYTE	selfID	  = NetworkManager::GetInstance()->GetID();	
	
	if(pFireResult != NULL)
	{
		DWORD dwCondition = pFireResult->m_dwCondition;
		BYTE damageID = (BYTE)pFireResult->m_dwResultID; 

		//캐릭터 상태 변경
		if(selfID == damageID) // 맞은게 본인이라면
		{
			m_pkCharacterManager->GetUserCharacter()->ProcessFireResult(pFireResult);

		}
		else if(selfID >= damageID) //맞은 놈이 나보다 일찍 접속한 놈이면 
		{
			m_pkCharacterManager->GetCharacterIndex(damageID)->ProcessFireResult(pFireResult);
		}
		else
			m_pkCharacterManager->GetCharacterIndex(damageID-1)->ProcessFireResult(pFireResult);
	}
}

//fire정보가 있으면 결과 계산 후 결과 전송
void WorldManager::FireResultProcessing()
{
	//fireInfo vector를 가져온다.
	VecFireInfo vecFireInfo = NetworkManager::GetInstance()->LockFireInfoVec();

	for(unsigned int i=0;i<vecFireInfo.size();i++)
	{
		FireInfo fireInfo = vecFireInfo[i];
		//벡터에서 가져온 firedata 처리
		VectorFireDataProcess(&fireInfo);
	}	
	NetworkManager::GetInstance()->ClearFireInfo();
	NetworkManager::GetInstance()->UnLockFireInfo();
	
	//FireResult vector에서 데이터를 가져와서 처리
	VecFireResult vecFireResult = NetworkManager::GetInstance()->LockFireResultVec();
	
	for(unsigned int i=0;i<vecFireResult.size();i++)
	{
		FireResult fireResult = vecFireResult[i];
		//벡터에서 가져온 firedata 처리
		VectorFireResultProcess(&fireResult);
	}	
	NetworkManager::GetInstance()->ClearFireResult();
	NetworkManager::GetInstance()->UnLockFireResult();
	
}



//발사 광선 패킷이 도착하면 발생하는 함수!
void WorldManager::PerformPick()			//피킹 처리
{	
	if(!m_bColorPickProcess) return ;

	DWORD	part  = 0;		//맞은 부위
	DWORD	damagedID = 0;	//맞은 유저 아이디
	DWORD	dwSelfID = NetworkManager::GetInstance()->GetID();

	struct _PACKET_GUN_FIRE		packet;
		
	//다른 캐릭터 벡터컨테이너에서 캐릭터 맞는지 확인
	std::vector<CCharacter*>& vecCharacter = m_pkCharacterManager->GetColliderVector();
	for(unsigned int i=0;i<vecCharacter.size();i++)
	{
		bool	bPicked = false;	//캐릭터 피킹 되엇는지
		
		//발사 광선
		NiPoint3 kOrigin,kDir;

		int iX,iY;

		NiPoint2 *kPoint2 = m_pkCharacterManager->GetUserCharacter()->GetFirePoint();

		iX = (int) kPoint2->x;
		iY = (int) kPoint2->y;
		
		//피킹 광선을 계산
		m_pkCameraManager->GetCamera()->WindowPointToRay(iX,iY,kOrigin,kDir);
		
		//충돌 정보 가져올 컨테이너
		FIRE_INTERSECT	 vecIntersect;
		
		//패킷 처리하기 위한 구조체
		FireInfo	fireInfo;

		//총의 데미지
		//무기에 정보를 가져옴
		CWeapon* pkWeapon	   = m_pkCharacterManager->GetUserCharacter()->GetEquipWeapon();
		unsigned  int uiDamage = pkWeapon->GetGunDamage();
		
		//충돌 체크
		if(PickFire(vecIntersect,vecCharacter[i]->GetNIFRoot(),kOrigin,kDir))
		{
			//충돌 정보 있음
			for(unsigned int j=0;j<vecIntersect.size();j++)
			{
				if(j == 2) break;		//정보는 최대 3개만.

				fireInfo.intersectInfoSize++;
				fireInfo.damage[j] = (DWORD) (vecIntersect[j].damageDecrease * uiDamage);
				fireInfo.part[j]   = vecIntersect[j].part;
			}	

			if(i >= dwSelfID)	fireInfo.damageUserID = i + 1;
			else				fireInfo.damageUserID = i;

			bPicked = true;
		}		
				
		//총 발사 패킷 패킹
		fireInfo.fireUserID   = dwSelfID;		//쏜 사람 ID
		fireInfo.kDirection   = kDir;			//광선의 방향
		fireInfo.kOrigin	  = kOrigin;		//광선의 시작점
		fireInfo.bHitPlayer   = bPicked;		//사람이 맞았는지 여부		
		
		packet.fireInfo		= fireInfo;
 
		//서버로 전송한다.
		NetworkManager::GetInstance()->SendFireData(packet);
	}
	
	m_bColorPickProcess = false;
}

void WorldManager::ProcessInput()
{
	CGameApp  *app = CGameApp::mp_Appication;
	assert(app);

	int		iX = 0;
	int		iY = 0;
	NiPoint3 vPos, vDir;

	if( !m_pkCharacterManager )
		return; 

	assert(m_pkCharacterManager);
	CUserCharacter* pkSoldier = m_pkCharacterManager->GetUserCharacter();
	
	if(!pkSoldier)
		return;
	//마우스 가져옴
	NiInputMouse* pkMouse = app->GetInputSystem()->GetMouse();
	if(pkMouse)
	{
		if( !m_bActiveMenu )
		{	
			if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT))
			{
				m_bClicked = true;
				pkSoldier->StartFire();		//사격시작을 알림						
			}
			if(pkMouse->ButtonWasReleased(NiInputMouse::NIM_LEFT))
			{
				m_bClicked = false;
				pkSoldier->EndFire();
			}		
			//마우스가 눌러진 상태를 처리합니다.
			if(m_bClicked)
			{						
				//사격 중 확인
				if(pkSoldier->ShootGun(app->GetAccumTime()))
				{
					//발사점을 가져와서 커서를 이동 시킴
					app->MoveCursor( (int)pkSoldier->GetFirePoint()->x, (int)pkSoldier->GetFirePoint()->y);

					//------------------ Picking Ray 생성 -------------------------
					CameraManager::Get()->GetCamera()->WindowPointToRay( (int)pkSoldier->GetFirePoint()->x, (int)pkSoldier->GetFirePoint()->y , vPos, vDir);
					
					//	씬에 픽킹 광선을 쏜다.
					NiPoint3	kPickPoint;		//피킹지점을 받아올 변수
					if( Terrain::GetManager()->PickRayAtScene( vPos, vDir,kPickPoint ) )
					{
						//벽에 총에 튕겼다면 총알이 벽에 튕기는 사운드 재생 및 전송
						//패킷을 구성
						_PACKET_SOUND_DATA	packet;
						packet.b2DSound   = false;
						packet.eSoundCode = eSoundCode::SOUND_WAV_BULLETIMPACT;
						packet.kSoundSpot = kPickPoint;//m_pkCharacterManager->GetUserCharacter()->GetCharRoot()->GetTranslate();
						
						//네트워크 매니저로 전송
						NetworkManager::GetInstance()->SendSoundData(packet);
					}				
					m_bColorPickProcess = true;
				}			
			}
			else 	//사격 중이 아니라면 크로스헤어 감소
				pkSoldier->DecreaseReaction();
		}
	}
}

void WorldManager::AddObjectAtRoot( NiNode *pObject )
{
	if( m_spScene != NULL )
	{
		m_spScene->AttachChild( pObject );
		m_spScene->Update(0.0f);
		m_spScene->UpdateNodeBound();
		m_spScene->UpdateProperties();
		m_spScene->UpdateEffects();
	}

	++m_uiCntObjects;

}

void WorldManager::AddObject_AffectLightScene( NiNode *pObject )
{
	if( m_pkLightNode != NULL )
	{
		m_pkLightNode->AttachChild( pObject );
		m_pkLightNode->Update(0.0f);
		m_pkLightNode->UpdateNodeBound();
		m_pkLightNode->UpdateProperties();
		m_pkLightNode->UpdateEffects();
	}

	++m_uiCntObjects;

}

void WorldManager::AddObject_NotAffectLightScene( NiNode *pObject )
{
	if( m_pkNotLightNode != NULL )
	{
		m_pkNotLightNode->AttachChild( pObject );
		m_pkNotLightNode->Update(0.0f);
		m_pkNotLightNode->UpdateNodeBound();
		m_pkNotLightNode->UpdateProperties();
		m_pkNotLightNode->UpdateEffects();
	}

	++m_uiCntObjects;


}

void WorldManager::RemoveObjectAtRoot( NiNode *pObject )
{
	if( m_spScene != NULL )
	{
		m_spScene->DetachChild( pObject );
		m_spScene->Update(0.0f);
		m_spScene->UpdateNodeBound();
		m_spScene->UpdateProperties();
		m_spScene->UpdateEffects();
	}

	--m_uiCntObjects;

}

void WorldManager::RemoveObject_AffectLightScene( NiNode *pObject )
{
	if( m_pkLightNode != NULL )
	{
		m_pkLightNode->DetachChild( pObject );
		m_pkLightNode->Update(0.0f);
		m_pkLightNode->UpdateNodeBound();
		m_pkLightNode->UpdateProperties();
		m_pkLightNode->UpdateEffects();
	}

	--m_uiCntObjects;

}

void WorldManager::RemoveObject_NotAffectLightScene( NiNode *pObject )
{
	if( m_pkNotLightNode != NULL )
	{
		m_pkNotLightNode->DetachChild( pObject );
		m_pkNotLightNode->Update(0.0f);
		m_pkNotLightNode->UpdateNodeBound();
		m_pkNotLightNode->UpdateProperties();
		m_pkNotLightNode->UpdateEffects();
	}

	--m_uiCntObjects;

}

NiNodePtr WorldManager::OpenNif(const char *pcNifName)
{
	NiStream kStream;
	
	bool bLoaded = kStream.Load(pcNifName);
	if(!bLoaded)
	{
		return NULL;
	}
	assert(NiIsKindOf(NiNode,kStream.GetObjectAt(0)));
	NiNodePtr spNode = (NiNode*)kStream.GetObjectAt(0);

	return spNode;
}

void WorldManager::Destroy()
{	
	NiDelete	m_pkWorldManager;	
	m_pkWorldManager = 0;
}


void	WorldManager::SetMaterialColor( const NiColor& Ambient, const NiColor& Diffuse, const NiColor& Specular, const float& fShine )
{

	m_spMaterialProp->SetAmbientColor( Ambient );	// NiColor m_clrAmbient;
	m_spMaterialProp->SetDiffuseColor( Diffuse );	// NiColor m_clrDiffuse;
	m_spMaterialProp->SetSpecularColor( Specular );	// NiColor m_clrSpecular;
	m_spMaterialProp->SetShineness( fShine );		// float m_fShineness;
	
}

//------------------------------------------------------------------------
//
//						안개 관련 함수 모음
//
//------------------------------------------------------------------------

void	WorldManager::CreateFog()
{
	m_spFog = NiTCreate<NiFogProperty>();
	m_spFog->SetFog( true );
	m_spFog->SetFogColor(NiColor(0.60f,0.65f,0.5f));//CGameApp::mp_Appication->GetBackGroundColor() );
	//m_spFog->SetFogFunction( NiFogProperty::FOG_Z_LINEAR );
	m_spFog->SetFogFunction( NiFogProperty::FOG_RANGE_SQ);
	
	m_spFog->SetDepth( 0.8f );

	m_pkLightNode->AttachProperty( m_spFog );
	m_pkLightNode->Update( 0.0f );
	m_pkLightNode->UpdateProperties();

}

void	WorldManager::SetFog( bool bFog )
{
	if( !m_spFog )
		CreateFog();

	m_spFog->SetFog( bFog );
}

void	WorldManager::SetFogDepth( float fDepth )
{
	if( m_spFog ) m_spFog->SetDepth( fDepth );
}

float	WorldManager::GetFogDepth()
{
	return m_spFog->GetDepth();
}
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------


//------------------------------------------------------------------------
//
//					씬에 모든 오브젝트 Release
//
//------------------------------------------------------------------------

bool	WorldManager::ReleaseObject()
{
	if( !m_spScene || !m_spScene->GetChildCount() )
		return FALSE;

	if( m_pkLightNode && m_pkLightNode->GetChildCount() )
	{
		for( unsigned int ui = 0; ui < m_pkLightNode->GetChildCount(); ui++ )
		{
			m_pkLightNode->DetachChildAt( ui );
		}
	}

	if( m_pkNotLightNode && m_pkNotLightNode->GetChildCount() )
	{
		for( unsigned int ui = 0; ui < m_pkNotLightNode->GetChildCount(); ui++ )
		{
			m_pkNotLightNode->DetachChildAt( ui );
		}
	}

	
	m_spScene->Update( 0.0f );
	m_spScene->UpdateEffects();
	m_spScene->UpdateNodeBound();
	m_spScene->UpdateProperties();

	if( !m_vecSceneObjects.empty() )
	{
		std::for_each( m_vecSceneObjects.begin(), m_vecSceneObjects.end(), std::mem_fun( &GBObject::ReleaseObject ) );
		m_vecSceneObjects.clear();
	}

	if( m_pkTerrain != NULL )
	{
		NiDelete m_pkTerrain;
		m_pkTerrain = NULL;
	}

	if( m_pkSkyBox != NULL )
	{
		NiDelete m_pkSkyBox;
		m_pkSkyBox = NULL;
	}

	if( m_pkStaticObjectManager != NULL && StaticObjectManager::IsManager() )
	{
		StaticObjectManager::ReleaseManager();
		m_pkStaticObjectManager = NULL;
	}

	if( m_pkBillBoardManager != NULL && BillBoardManager::IsManager() )
	{
		BillBoardManager::ReleaseManager();
		m_pkBillBoardManager = NULL;
	}

	if( m_pkDecalManager != NULL && DecalManager::IsManager() )
	{
		DecalManager::ReleaseManager();
		m_pkDecalManager = NULL;
	}

	CDustManager::DestroyManager();

	// 점령지 제거
	CTerritory::Destroy();

	return true;

}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//
//					맵 정보를 파일로 부터 읽어서 Scene에 Attach
//
//------------------------------------------------------------------------

bool	WorldManager::LoadScene( FILE *file, std::string& cFileName )
{
	//-------------------------------------------------
	//            기존의 할당된 리소스 제거 
	//-------------------------------------------------
	ReleaseObject();

	CLoadingDisplay::GetInstance()->Render(3.5f);
	
	//-------------------------------------------------
	//		Load 하여 Object Class 들 재생성
	//-------------------------------------------------
	std::string str("");

	NiFixedString strName;

	std::list<NiFixedString> listFileName;
	std::vector<char> vChar;
	cFileName.replace( cFileName.end() - 3, cFileName.end(), "name" );
//	cFileName.Replace( _T(".MAP"), _T(".NAME") );
	std::ifstream fileread( cFileName.c_str() );
	std::copy( std::istreambuf_iterator<char>(fileread), std::istreambuf_iterator<char>(), std::back_inserter(vChar) );
		
	std::vector<char>::iterator iter;
	for( iter = vChar.begin() ; iter != vChar.end() ; ++iter )
	{
		if( *iter == '\n' )
		{
			strName = str.c_str();
			listFileName.push_back( strName ); 	
			str = "";
		}
		else
		{
			str += *iter;
		}
	}
	vChar.clear();
	fileread.close();

	cFileName.replace( cFileName.end() - 4, cFileName.end(), "map" );

	CLoadingDisplay::GetInstance()->Render(4.0f);

	//-----------------------------------------------------------------------------------
	//							 지형 지오메트리, 텍스쳐
	//-----------------------------------------------------------------------------------

	int			nCellCount, nTileCount, nShaderMapCount;
	float		fWidthSize, fHeightSize, fSlodRange, fPlaneEpsilon, fCameraFar;
	NiPoint3	vSkyExtents, vSkyPos;
	char		buf[256];
	::ZeroMemory(buf,sizeof(buf));

	fread( buf, 1, sizeof(int) * 2, file );
	memcpy( &nCellCount, buf + sizeof(int) * 0, sizeof(int) ); // 타일 하나가 보유한 (가로줄) 셀의 갯수
	memcpy( &nTileCount, buf + sizeof(int) * 1, sizeof(int) ); // 전체 Terrain Tile 갯수

	fread( buf, 1, sizeof(float) * 3, file );
	memcpy( &fWidthSize	, buf + sizeof(float) * 0, sizeof(float) ); // 전체 Terrain 가로 길이
	memcpy( &fHeightSize, buf + sizeof(float) * 1, sizeof(float) ); // 전체 Terrain 세로 길이
	memcpy( &fSlodRange	, buf + sizeof(float) * 2, sizeof(float) ); // SLOD 적용 범위 정도

	fread( buf, 1, sizeof(int), file );
	memcpy( &nShaderMapCount, buf , sizeof(int) );					// 쉐이더 맵 카운트

	fread( buf, 1, sizeof(float) * 2, file );
	memcpy( &fPlaneEpsilon	, buf					, sizeof(float) ); // 쉐이더 맵 카운트
	memcpy( &fCameraFar		, buf + sizeof(float)	, sizeof(float) ); // 쉐이더 맵 카운트

	CLoadingDisplay::GetInstance()->Render(4.5f);

	// 카메라 Far 거리 설정
	m_pkCameraManager->SetFrustumFarDistance( fCameraFar );

	if ( !Terrain::IsManager() )
	{
		m_pkTerrain = Terrain::CreateManager(  nTileCount, nCellCount, fWidthSize, fHeightSize );

		if( m_pkTerrain )
		{
			// 지형 생성
			m_pkTerrain->InitTerrain( CGameApp::mp_Appication->GetRenderer() );

			CLoadingDisplay::GetInstance()->Render(20.0f);

			// 베이스 텍스쳐 설정
			m_pkTerrain->CreateTexture( listFileName.front() );
			listFileName.pop_front();

			// 카메라 설정
			if( m_pkCameraManager ) m_pkTerrain->SetCamera( m_pkCameraManager->GetCamera() );

			// 쉐이더 맵 갯수 설정
			m_pkTerrain->SetShderMapCount( nShaderMapCount );

			CLoadingDisplay::GetInstance()->Render(25.0f);
		
			// 프러스텀 컬링 범위 설정
			m_pkTerrain->SetFrustumEpsilion( fPlaneEpsilon );

			// Slod 적용 거리 설정
			m_pkTerrain->SetSlodRange( fSlodRange );

			CLoadingDisplay::GetInstance()->Render(30.0f);

			// 쉐이더 텍스쳐 설정
			m_pkTerrain->LoadNameData( nShaderMapCount, listFileName );

			// 빛의 영향을 받을 노드에 Attach!!
			AddObject_AffectLightScene( m_pkTerrain->GetObject() );

			// 자료구조 vector 컨테이너에 추가
			m_vecSceneObjects.push_back( m_pkTerrain );
		}
	}


	//-----------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------
	//						스카이 박스
	//-----------------------------------------------------------------------------------
	
	fread( buf, 1, sizeof(float) * 6, file );

	// 스카이 박스 크기
	memcpy( &vSkyExtents.x	, buf + sizeof(float) * 0, sizeof(float) ); 
	memcpy( &vSkyExtents.y	, buf + sizeof(float) * 1, sizeof(float) ); 
	memcpy( &vSkyExtents.z	, buf + sizeof(float) * 2, sizeof(float) ); 

	// 스카이 박스 위치
	memcpy( &vSkyPos.x	, buf + sizeof(float) * 3, sizeof(float) ); // 전체 Terrain 가로 길이
	memcpy( &vSkyPos.y	, buf + sizeof(float) * 4, sizeof(float) ); // 전체 Terrain 세로 길이
	memcpy( &vSkyPos.z	, buf + sizeof(float) * 5, sizeof(float) ); // SLOD 적용 범위 정도

	
	if( !m_pkSkyBox ) 
	{
		m_pkSkyBox = NiNew SkyBox();

		if( m_pkSkyBox )
		{
			// 스카이 박스 생성
//			m_pkSkyBox->Generate( listFileName.front(), vSkyExtents );
			m_pkSkyBox->Generate( "Mountain", vSkyExtents );
			m_pkSkyBox->SetAppCulled( false );
			m_pkSkyBox->SetCamera( m_pkCameraManager->GetCamera() );
			m_pkSkyBox->SetPosition( vSkyPos );

			m_vecSceneObjects.push_back( m_pkSkyBox );

			//AddObject_NotAffectLightScene( m_pkSkyBox->GetRootObject() );

			listFileName.pop_front();
		}
		else
		{
			NiMessageBox( "MyTerrainApp::Load() - GenerateSkyBox() Failed", "Failed" );
			return FALSE;
		}
	}

	CLoadingDisplay::GetInstance()->Render(32.0f);

	//-----------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------	
	//									스태틱 오브젝트 로드 
	//-----------------------------------------------------------------------------------
	std::list< int > listEachofMeshCount;

	if( !StaticObjectManager::IsManager() )
	{
		m_pkStaticObjectManager = StaticObjectManager::CreateManager();
	
		if( m_pkTerrain ) m_pkStaticObjectManager->SetTerrain( m_pkTerrain );

		m_vecSceneObjects.push_back( m_pkStaticObjectManager );

		AddObject_AffectLightScene( StaticObjectManager::GetManager()->GetObject() );		

		m_pkStaticObjectManager->LoadNameData( file, listEachofMeshCount, listFileName );
	}

	CLoadingDisplay::GetInstance()->Render(37.0f);

	//-----------------------------------------------------------------------------------


	//------------------------------------------------------------------------------------
	//									전체 씬 Data Load
	//------------------------------------------------------------------------------------

	m_pkTerrain->LoadData( file );	
	m_pkStaticObjectManager->LoadData( file, listEachofMeshCount );

	CLoadingDisplay::GetInstance()->Render(55.0f);

	//-----------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------	
	//									빌보드 로드 
	//-----------------------------------------------------------------------------------

	std::map< int, BillBoardObject* >  mapObjects;
	if( !BillBoardManager::IsManager())
	{	
		m_pkBillBoardManager = BillBoardManager::CreateManager();

		m_pkBillBoardManager->SetCamera( m_pkCameraManager->GetCamera() );
		
		m_vecSceneObjects.push_back( m_pkBillBoardManager );

		AddObject_AffectLightScene( BillBoardManager::GetManager()->GetObject() );	

		m_pkBillBoardManager->LoadNameData( file, mapObjects, listFileName );

		m_pkBillBoardManager->LoadData( file, mapObjects, m_pkTerrain );		
	}

	CLoadingDisplay::GetInstance()->Render(57.0f);
	//---------------------------------------------------------------------------
	//
	//					Manager 생성
	//
	//---------------------------------------------------------------------------

	// 데칼 메니저 생성
	if( !DecalManager::IsManager() )
	{
		m_pkDecalManager = DecalManager::CreateManager();

		if( m_pkTerrain ) DecalManager::GetManager()->SetTerrain( m_pkTerrain );
		
		AddObject_NotAffectLightScene( DecalManager::GetManager()->GetObject() );

		m_vecSceneObjects.push_back( m_pkDecalManager );
	}

	CLoadingDisplay::GetInstance()->Render(60.0f);

	//더스트 매니저 생성
	CDustManager::CreateManager( 0.01f, 1.0f, 5, "dust.nif", "dust.tga");
	CLoadingDisplay::GetInstance()->Render(61.0f);


	m_pkTerritory = CTerritory::GetInstance();

	if( !m_pkTerritory )
		return false;
	
	return true;

}


//------------------------------------------------------------------------
//
//-----------------------------------------------------------------