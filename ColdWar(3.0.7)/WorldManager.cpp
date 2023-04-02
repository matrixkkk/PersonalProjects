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

	m_pkLightNode	= NULL;				// �� ����ġ ��� (�� ����)
	m_pkNotLightNode= NULL;				// �� ����ġ ��� (�� ������)

	m_pkTerrain	= NULL;					// ����

	m_pkSkyBox = NULL;					// ��ī�� �ڽ�

	m_pkStaticObjectManager = NULL;		// ����ƽ ������Ʈ	

	m_pkBillBoardManager	= NULL;		// ������

	m_pkDecalManager		= NULL;		// ��Į

	m_pkPicker				= NULL;		//��Ŀ

	m_pkRespone				= NULL;

	m_pkTerritory			= NULL;

	//��ŷ ����
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

	NiDelete	m_pkCameraManager;		//ĳ���Ϳ��� ���� ī�޶�
	NiDelete	m_pkCollisionManager;	//�浹������
	NiDelete	m_pkCharacterManager;	//ĳ���� �Ŵ���
	
	//�ý��� ������ ����
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
	m_spWireframeProperty = 0;		//���̾� ������ ������Ƽ
	m_spScene = 0;			//���� ��

	//FirePick ����
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
	// �ε� ȭ�� ����
	if( !CLoadingDisplay::Create( 0, 632, 1024, 12,"LoadingSplash.bmp", "Loadingbar.tga" ) )
		return false;

	CLoadingDisplay::GetInstance()->Render(0.2f);

	// ���� ��ü ���̴� ���̺귯�� �ε�
	if( !CreateMaterialToolKit() )
	{
		NiMessageBox(" CreateScene() - CreateMaterialToolKit() Failed", "Failed", MB_OK );
		return FALSE;
	}

	CLoadingDisplay::GetInstance()->Render(0.3f);
	
	//--------��ŷ �ʱ�ȭ----------
	/*m_pkPicker = new CObjectPick();		//��ŷ ��ü ����

	if(!m_pkPicker->Init())				//��Ŀ �ʱ�ȭ
		return false;
	*/
	//-----------------------------
	CLoadingDisplay::GetInstance()->Render(0.5f);

	if(!CreateCamera())		//ī�޶� ����
		return false;

	CLoadingDisplay::GetInstance()->Render(1.5f);

	if(!CreateScene())		//���� �� ����
		return false;

	CLoadingDisplay::GetInstance()->Render(77.0f);

	if(!CreateGameSystem())
		return false;
	
	CLoadingDisplay::GetInstance()->Render(79.0f);

	if(!CreateRespone())
		return false;

	CLoadingDisplay::GetInstance()->Render(82.0f);


	if(!CreateCharacter())	//ĳ���� ����
		return false;	

	CLoadingDisplay::GetInstance()->Render(94.0f);

	return true;
}

//���� �Ŵ��� ����
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
		NiMessageBox("WorldManager::CreateGameSystem() -- GameSystemManager��ü ���� ����\n",NULL);
		return false;
	}
	
	float fPlayTime = 0;

	FILE *pFile = NULL;
	fopen_s(&pFile,"Data/script/PlayTime.dat","r");

	fscanf_s(pFile,"%f",&fPlayTime);
		//���� �ý��� �ʱ�ȭ
	if(!m_pkGameSystemManager->Init(BranchOfMilitary::Gerilla,fPlayTime))	//�ι�° ����. ���� �÷��� �ð� s ����
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
	//ĳ���Ϳ� ���� ī�޶� ����
	m_pkCameraManager = NiNew CameraManager;

	if(!m_pkCameraManager)
	{
		NiMessageBox("WorldManager::CreateCamera -- �޸� �Ҵ� ����\n",NULL);
		return false;
	}	

	return true;
}

//ĳ���� ��ŷ
bool WorldManager::PickFire(FIRE_INTERSECT& vecIntersect, NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir)
{
	FireIntersectInfo	kFireIntersectInfo;

	//��ŷ ������ �� ������Ʈ
	m_kFirePick.SetTarget(pkPickRoot);
	
	if(m_kFirePick.PickObjects(kOrigin,kDir))
	{
		//�浹�� ������Ʈ�� ���� - - ��������
		//�浹�� ������� ������
		NiPick::Results& kPickResult = m_kFirePick.GetResults();
		
		//�浹 
		for(unsigned int i=0;i<kPickResult.GetSize();i++)
		{
			NiPick::Record* pkRecord = kPickResult.GetAt(i);
			NiNode* pkParent = pkRecord->GetParent();
			if(pkParent == NULL) continue;
			
			//"pick" Ű���� ������ extra �����͸� ���� ������Ʈ�� pick��� ������Ʈ�̴�
			NiIntegerExtraData* pkED = (NiIntegerExtraData*) pkParent->GetExtraData("pick");
			if(pkED == NULL) continue;
			
			//ĳ���Ϳ� ���� ������ �ִ� ===
			unsigned int idx = vecIntersect.size();		//���� ���� ������� ���Ϳ� �ִ´�.
			
			float fDamageDecrease = 1.0f/(float)(idx + 1); //������
			
			//���� ���� ����
			unsigned int uiPart = pkED->GetValue();
			
			kFireIntersectInfo.damageDecrease = fDamageDecrease;
			kFireIntersectInfo.part			  =	(BYTE) uiPart;
			//�����̳ʿ� ���� ���� ������� ����
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
//	�ɸ��� ���� �� ���� ������ �ø��� �׷쿡 �ɸ��� �߰�
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
	//	������ ���� ���� ���� Ŭ���̾�Ʈ���� ������ �̿��Ͽ� ĳ���͸� �����Ѵ�.
	//--------------------------------------------------------------------------
	PlayersMap::iterator iterPlayer;
	
	NiMatrix3 rot;
	NiTransform kSpawn;  
	BYTE nID = NetworkManager::GetInstance()->GetID();

	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	iterPlayer = conPlayers.find( nID );

	//	�� �ɸ��� ����
	if( iterPlayer != conPlayers.end() )
	{
		DWORD	dwCamp = iterPlayer->second.userInfo.camp;
		DWORD	dwArmy = iterPlayer->second.userInfo.BranchOfMilitary;

		NiPoint3 kTrans = m_pkRespone->GetStartPosition(dwCamp);	//EU��ġ�� �ӽ� ����
		NiTransform kTransform;
		NiMatrix3	kRot;

		kRot.MakeIdentity();

		kTransform.m_Translate = kTrans;
		kTransform.m_Rotate = kRot;
		kTransform.m_fScale = 1.0f;
		m_pkCharacterManager->AddCharacter(dwArmy,kTransform,true );
	}

	CLoadingDisplay::GetInstance()->Render(82.0f);

	if( m_bNetMode ) // ��Ʈ��ũ ����ϋ�
	{
		// ���� �÷����� �ٸ� ���� �ɸ��� ����
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
	//		���� �ɸ��ͷ� ī�޶� ������ �ش�.
	//--------------------------------------------------------------------------
	if(!m_pkCameraManager->Initialize(m_pkCharacterManager->GetUserCharacter()->GetCharRoot()))
	{
		NiMessageBox("WorldManager::CreateCamera -- ī�޶� �ʱ�ȭ ����\n",NULL);
		return false;
	}	

	CLoadingDisplay::GetInstance()->Render(87.0f);
	
	// ���� ���� �� �ʱ�ȭ �� �ɸ��͸� ������ �ø��� �׷쿡 �߰�
	if( m_pkTerrain )
	{
		m_pkTerrain->SetCharacterAtCollisionGroup( m_pkCharacterManager );

		m_pkTerrain->SetPlayer( m_pkCharacterManager->GetUserCharacter()->GetCharRoot() );
	}

	// �������� �ɸ��� �浹���踦 �����Ѵ�.
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
	//				�ʼ� Node ����
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
	//				�⺻ �Ӽ� ���� & ����
	//------------------------------------------------------------	

	// ���� �⺻ ���͸��� �Ӽ� ����ġ
	m_spMaterialProp = NiTCreate<NiMaterialProperty>();
    m_spMaterialProp->SetAmbientColor(0.5f*NiColor::WHITE);
    m_spMaterialProp->SetDiffuseColor(NiColor::WHITE);
    m_spMaterialProp->SetEmittance(NiColor::BLACK);
    m_spMaterialProp->SetSpecularColor(NiColor::WHITE);
    m_spScene->AttachProperty(m_spMaterialProp);	

	//���̾������� �Ӽ� �߰�
	m_spWireframeProperty = NiNew NiWireframeProperty;
	if(!m_spWireframeProperty)
		return false;

	//���� ���� ����ġ
	m_spScene->AttachProperty(m_spWireframeProperty);
	m_spWireframeProperty->SetWireframe(false);				//�ϴ� ���̾������� �Ӽ� ��

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
	//			 ����  �ε� & �� �ʱ�ȭ		
	//------------------------------------------------------------

	// ����  �ε��Ѵ�.
	FILE *fstream; 
	//errno_t err; 

	//fstream = fopen( "./Data/Map/testtest4.map", "rb" );
	fopen_s(&fstream, "./Data/Map/FinalTest.map", "rb" );
	//if( err == 0 )
	//	return false;
	if( fstream == NULL )
		return false;

	// �� Ÿ�ϵ鿡 ���ؽ� ������ �����ϰ�, �ؽ��� ������ �����Ѵ�.
	if( LoadScene( fstream, std::string("Data/Map/FinalTest.map") ) )
	{
		fclose(fstream);
	}
	
	// ������Ʈ�� ���� ����
	if( m_pkTerrain ) 
	{
		// ������ �׷����� ����
		m_pkStaticObjectManager->SpaceDivisionForRendering( m_pkTerrain );

		CLoadingDisplay::GetInstance()->Render(65.0f);
		// �ø��� �׷����� ����
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

// �⺻ ������ Ű�� ����.
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

	// ���̴� ������ ���� ���̴� ������ ���� �ϴ� ��Ŷ�� ����
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
	m_uiVisibleObjCount += m_kVisible.GetCount();		//�׸� ������Ʈ�� ��
	
	NiDrawVisibleArrayAppend(m_kVisible);
	//m_fRenderTime += NiGetCurrentTimeInSec() - fStartTime;
}

//���� �׸���.
void WorldManager::DrawScene(NiCamera* pkCamera)
{
	assert(m_spScene && m_spScene->IsVisualObject());
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();		//�������� ������
	
	//Disabled alpha sorter : ��ī�� �ڽ� �׸���
	//��ī�̹ڽ� ó�� ��ƾ
    DrawSkyBox(pkCamera);

	//Enable alpha sorter
	assert(m_spFastAlphaSorter);
	m_spFastAlphaSorter->StartAccumulating(pkCamera);
	assert(m_spFastAlphaSorter->IsAccumulating());
			
	//-------------------------------------------------------------
	//						��ü ������Ʈ ��
	//-------------------------------------------------------------

	m_kVisible.RemoveAll();
	// �ø�

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
	
	//��� ĳ���͸� �׸���.
	//pkRenderer->SetSorter(NULL);
	if( m_pkCharacterManager ) m_pkCharacterManager->DrawSoldier(pkCamera);	

	// ������ ����
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
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();		//�������� ������

	assert(!pkRenderer->GetSorter());
	
	//���� ���������� ����Ѵ�.
	NiFrustum kOldFrustum = pkCamera->GetViewFrustum();

	//��ī�� �ڽ����� ���� far�Ÿ��� �������༭ ī�޶� frustum�� set�Ѵ�.
	pkCamera->SetViewFrustum(NiFrustum(-1.0f,1.0f,1.0f,-1.0f, 0.1f, 10000.0f));
	pkRenderer->SetCameraData(pkCamera);

	NiNode* pkSkyBox = m_pkSkyBox->GetObject();
	NiNode* pkGeom = (NiNode*) pkSkyBox->GetObjectByName("SkyBox");
	
	//��ī�̹ڽ� ������Ʈ���� ��� �����Ѵ�.
	for(unsigned int i=0;i<pkGeom->GetArrayCount();i++)
	{
		NiGeometry* geom = (NiGeometry*) pkGeom->GetAt(i);
		geom->RenderImmediate(pkRenderer);
	}
	
	//������ �� ���� �������� set
	pkCamera->SetViewFrustum(kOldFrustum);
	pkRenderer->SetCameraData(pkCamera);
}

//��ũ�� ������ �׸���
void WorldManager::RenderScreenItem()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	assert(pkRenderer);
	
	pkRenderer->SetScreenSpaceCameraData();

	//���� ĳ���� ��ü�� �����´�.
	CUserCharacter* pkUserCharacter = m_pkCharacterManager->GetUserCharacter();
	assert(pkUserCharacter);
	
	//ũ�ν���� ��ü�� �����´�.
	CrossHair* pkCross = pkUserCharacter->GetCrossHairDisplay();
	assert(pkCross);
	
	//ũ�ν���� �׸���
	pkCross->Draw();

	//���� �ý��� �׸���
	m_pkGameSystemManager->DrawUI();

	CTerritory::GetInstance()->RenderScreenItem();

	//���� �÷��� ��ο�
	m_pkCharacterManager->GetUserCharacter()->DrawMuzel();
}

void WorldManager::Draw()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	NiCamera*	pkCamera = m_pkCameraManager->GetCamera();
	
	DrawScene(pkCamera);

	//��ũ�� ������ �׸���
	RenderScreenItem();
}

void WorldManager::Update()
{
	ProcessInput();

	//��ŷ ����
	PerformPick();
	FireResultProcessing();

	float fTime = CGameApp::mp_Appication->GetAccumTime();

	// ī�޶� ������Ʈ
	m_pkCameraManager->UpdateCamera( fTime );

	// ���� �� ������Ʈ ������Ʈ
	std::for_each( m_vecSceneObjects.begin(), m_vecSceneObjects.end(), std::bind2nd( std::mem_fun( &GBObject::UpdateObject ), fTime ) );

	// ĳ���� ������Ʈ
	if( m_pkCharacterManager ) m_pkCharacterManager->Update(fTime, m_bNetMode);

	//���� �ý��� ������Ʈ ( ��Ʈ��ũ ����϶��� )
	if(m_pkGameSystemManager ) m_pkGameSystemManager->Update(fTime);

	//Dust Effect ������Ʈ
	CDustManager::GetInstance()->Update( fTime );

	// ������ �浹 ó��
	CTerritory::GetInstance()->Update( fTime );

	//���� ������Ʈ
	CSoundManager::GetInstance()->Update();
}

void WorldManager::VectorFireDataProcess(FireInfo* pFireInfo)	//fireData ó��
{
	BYTE	selfID	  = NetworkManager::GetInstance()->GetID();	

	if(pFireInfo!=NULL)
	{
		//�ٸ� ����� ��� ������ �˱� ������.. �� �� ����� ���� �÷��ø� �����ֱ� ���� ���⼭
		//ó�� �Ѵ�.
		DWORD dwFireUser = pFireInfo->fireUserID;	//�� ����� ���̵�
		if(dwFireUser != selfID)					//������ ��� ó�� ����
		{
			dwFireUser > selfID ?	dwFireUser-- : dwFireUser;
			
			//���� �� ĳ���� ������Ʈ�� �����´�.
			CCharacter* pkFireCharacter = m_pkCharacterManager->GetCharacterIndex(dwFireUser);
			//���� ������Ʈ
			pkFireCharacter->UpdateMuzel(CGameApp::mp_Appication->GetAccumTime());
		}
		//���� �� �� ����� �¾Ұ� �� ����� ���� ������ ���
		if(pFireInfo->bHitPlayer == true && pFireInfo->damageUserID == selfID)
		{
			//���� ���� ����
			DWORD dwCondition = m_pkCharacterManager->GetUserCharacter()->GetStatus();
			
			//���� �ѿ� �¾Ҵµ�~ �� �̹� ���� ���¿���!! �״ϱ� ������ ����
			if(dwCondition != DEATH) 
			{
				//---------- ������ ��� -----------
				//GameSystemManager�� �������� ���� ������ �Ѱ��־� ĳ���� ���� �޾ƿ�
				DWORD dwCondition = m_pkGameSystemManager->ProcessCondition(pFireInfo);

				struct _PACKET_FIRE_RESULT packet;
				packet.fireResult.m_dwFireUserID = pFireInfo->fireUserID;
				packet.fireResult.m_dwCondition  = dwCondition;
				packet.fireResult.m_dwResultID   = (DWORD)NetworkManager::GetInstance()->GetID();

				//�߻� ����� ����
				NetworkManager::GetInstance()->SendFireResult(packet);
			}			
		}
	}
}
void WorldManager::VectorFireResultProcess(FireResult* pFireResult)	//fireResult ó��
{
	BYTE	selfID	  = NetworkManager::GetInstance()->GetID();	
	
	if(pFireResult != NULL)
	{
		DWORD dwCondition = pFireResult->m_dwCondition;
		BYTE damageID = (BYTE)pFireResult->m_dwResultID; 

		//ĳ���� ���� ����
		if(selfID == damageID) // ������ �����̶��
		{
			m_pkCharacterManager->GetUserCharacter()->ProcessFireResult(pFireResult);

		}
		else if(selfID >= damageID) //���� ���� ������ ���� ������ ���̸� 
		{
			m_pkCharacterManager->GetCharacterIndex(damageID)->ProcessFireResult(pFireResult);
		}
		else
			m_pkCharacterManager->GetCharacterIndex(damageID-1)->ProcessFireResult(pFireResult);
	}
}

//fire������ ������ ��� ��� �� ��� ����
void WorldManager::FireResultProcessing()
{
	//fireInfo vector�� �����´�.
	VecFireInfo vecFireInfo = NetworkManager::GetInstance()->LockFireInfoVec();

	for(unsigned int i=0;i<vecFireInfo.size();i++)
	{
		FireInfo fireInfo = vecFireInfo[i];
		//���Ϳ��� ������ firedata ó��
		VectorFireDataProcess(&fireInfo);
	}	
	NetworkManager::GetInstance()->ClearFireInfo();
	NetworkManager::GetInstance()->UnLockFireInfo();
	
	//FireResult vector���� �����͸� �����ͼ� ó��
	VecFireResult vecFireResult = NetworkManager::GetInstance()->LockFireResultVec();
	
	for(unsigned int i=0;i<vecFireResult.size();i++)
	{
		FireResult fireResult = vecFireResult[i];
		//���Ϳ��� ������ firedata ó��
		VectorFireResultProcess(&fireResult);
	}	
	NetworkManager::GetInstance()->ClearFireResult();
	NetworkManager::GetInstance()->UnLockFireResult();
	
}



//�߻� ���� ��Ŷ�� �����ϸ� �߻��ϴ� �Լ�!
void WorldManager::PerformPick()			//��ŷ ó��
{	
	if(!m_bColorPickProcess) return ;

	DWORD	part  = 0;		//���� ����
	DWORD	damagedID = 0;	//���� ���� ���̵�
	DWORD	dwSelfID = NetworkManager::GetInstance()->GetID();

	struct _PACKET_GUN_FIRE		packet;
		
	//�ٸ� ĳ���� ���������̳ʿ��� ĳ���� �´��� Ȯ��
	std::vector<CCharacter*>& vecCharacter = m_pkCharacterManager->GetColliderVector();
	for(unsigned int i=0;i<vecCharacter.size();i++)
	{
		bool	bPicked = false;	//ĳ���� ��ŷ �Ǿ�����
		
		//�߻� ����
		NiPoint3 kOrigin,kDir;

		int iX,iY;

		NiPoint2 *kPoint2 = m_pkCharacterManager->GetUserCharacter()->GetFirePoint();

		iX = (int) kPoint2->x;
		iY = (int) kPoint2->y;
		
		//��ŷ ������ ���
		m_pkCameraManager->GetCamera()->WindowPointToRay(iX,iY,kOrigin,kDir);
		
		//�浹 ���� ������ �����̳�
		FIRE_INTERSECT	 vecIntersect;
		
		//��Ŷ ó���ϱ� ���� ����ü
		FireInfo	fireInfo;

		//���� ������
		//���⿡ ������ ������
		CWeapon* pkWeapon	   = m_pkCharacterManager->GetUserCharacter()->GetEquipWeapon();
		unsigned  int uiDamage = pkWeapon->GetGunDamage();
		
		//�浹 üũ
		if(PickFire(vecIntersect,vecCharacter[i]->GetNIFRoot(),kOrigin,kDir))
		{
			//�浹 ���� ����
			for(unsigned int j=0;j<vecIntersect.size();j++)
			{
				if(j == 2) break;		//������ �ִ� 3����.

				fireInfo.intersectInfoSize++;
				fireInfo.damage[j] = (DWORD) (vecIntersect[j].damageDecrease * uiDamage);
				fireInfo.part[j]   = vecIntersect[j].part;
			}	

			if(i >= dwSelfID)	fireInfo.damageUserID = i + 1;
			else				fireInfo.damageUserID = i;

			bPicked = true;
		}		
				
		//�� �߻� ��Ŷ ��ŷ
		fireInfo.fireUserID   = dwSelfID;		//�� ��� ID
		fireInfo.kDirection   = kDir;			//������ ����
		fireInfo.kOrigin	  = kOrigin;		//������ ������
		fireInfo.bHitPlayer   = bPicked;		//����� �¾Ҵ��� ����		
		
		packet.fireInfo		= fireInfo;
 
		//������ �����Ѵ�.
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
	//���콺 ������
	NiInputMouse* pkMouse = app->GetInputSystem()->GetMouse();
	if(pkMouse)
	{
		if( !m_bActiveMenu )
		{	
			if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT))
			{
				m_bClicked = true;
				pkSoldier->StartFire();		//��ݽ����� �˸�						
			}
			if(pkMouse->ButtonWasReleased(NiInputMouse::NIM_LEFT))
			{
				m_bClicked = false;
				pkSoldier->EndFire();
			}		
			//���콺�� ������ ���¸� ó���մϴ�.
			if(m_bClicked)
			{						
				//��� �� Ȯ��
				if(pkSoldier->ShootGun(app->GetAccumTime()))
				{
					//�߻����� �����ͼ� Ŀ���� �̵� ��Ŵ
					app->MoveCursor( (int)pkSoldier->GetFirePoint()->x, (int)pkSoldier->GetFirePoint()->y);

					//------------------ Picking Ray ���� -------------------------
					CameraManager::Get()->GetCamera()->WindowPointToRay( (int)pkSoldier->GetFirePoint()->x, (int)pkSoldier->GetFirePoint()->y , vPos, vDir);
					
					//	���� ��ŷ ������ ���.
					NiPoint3	kPickPoint;		//��ŷ������ �޾ƿ� ����
					if( Terrain::GetManager()->PickRayAtScene( vPos, vDir,kPickPoint ) )
					{
						//���� �ѿ� ƨ��ٸ� �Ѿ��� ���� ƨ��� ���� ��� �� ����
						//��Ŷ�� ����
						_PACKET_SOUND_DATA	packet;
						packet.b2DSound   = false;
						packet.eSoundCode = eSoundCode::SOUND_WAV_BULLETIMPACT;
						packet.kSoundSpot = kPickPoint;//m_pkCharacterManager->GetUserCharacter()->GetCharRoot()->GetTranslate();
						
						//��Ʈ��ũ �Ŵ����� ����
						NetworkManager::GetInstance()->SendSoundData(packet);
					}				
					m_bColorPickProcess = true;
				}			
			}
			else 	//��� ���� �ƴ϶�� ũ�ν���� ����
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
//						�Ȱ� ���� �Լ� ����
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
//					���� ��� ������Ʈ Release
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

	// ������ ����
	CTerritory::Destroy();

	return true;

}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//
//					�� ������ ���Ϸ� ���� �о Scene�� Attach
//
//------------------------------------------------------------------------

bool	WorldManager::LoadScene( FILE *file, std::string& cFileName )
{
	//-------------------------------------------------
	//            ������ �Ҵ�� ���ҽ� ���� 
	//-------------------------------------------------
	ReleaseObject();

	CLoadingDisplay::GetInstance()->Render(3.5f);
	
	//-------------------------------------------------
	//		Load �Ͽ� Object Class �� �����
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
	//							 ���� ������Ʈ��, �ؽ���
	//-----------------------------------------------------------------------------------

	int			nCellCount, nTileCount, nShaderMapCount;
	float		fWidthSize, fHeightSize, fSlodRange, fPlaneEpsilon, fCameraFar;
	NiPoint3	vSkyExtents, vSkyPos;
	char		buf[256];
	::ZeroMemory(buf,sizeof(buf));

	fread( buf, 1, sizeof(int) * 2, file );
	memcpy( &nCellCount, buf + sizeof(int) * 0, sizeof(int) ); // Ÿ�� �ϳ��� ������ (������) ���� ����
	memcpy( &nTileCount, buf + sizeof(int) * 1, sizeof(int) ); // ��ü Terrain Tile ����

	fread( buf, 1, sizeof(float) * 3, file );
	memcpy( &fWidthSize	, buf + sizeof(float) * 0, sizeof(float) ); // ��ü Terrain ���� ����
	memcpy( &fHeightSize, buf + sizeof(float) * 1, sizeof(float) ); // ��ü Terrain ���� ����
	memcpy( &fSlodRange	, buf + sizeof(float) * 2, sizeof(float) ); // SLOD ���� ���� ����

	fread( buf, 1, sizeof(int), file );
	memcpy( &nShaderMapCount, buf , sizeof(int) );					// ���̴� �� ī��Ʈ

	fread( buf, 1, sizeof(float) * 2, file );
	memcpy( &fPlaneEpsilon	, buf					, sizeof(float) ); // ���̴� �� ī��Ʈ
	memcpy( &fCameraFar		, buf + sizeof(float)	, sizeof(float) ); // ���̴� �� ī��Ʈ

	CLoadingDisplay::GetInstance()->Render(4.5f);

	// ī�޶� Far �Ÿ� ����
	m_pkCameraManager->SetFrustumFarDistance( fCameraFar );

	if ( !Terrain::IsManager() )
	{
		m_pkTerrain = Terrain::CreateManager(  nTileCount, nCellCount, fWidthSize, fHeightSize );

		if( m_pkTerrain )
		{
			// ���� ����
			m_pkTerrain->InitTerrain( CGameApp::mp_Appication->GetRenderer() );

			CLoadingDisplay::GetInstance()->Render(20.0f);

			// ���̽� �ؽ��� ����
			m_pkTerrain->CreateTexture( listFileName.front() );
			listFileName.pop_front();

			// ī�޶� ����
			if( m_pkCameraManager ) m_pkTerrain->SetCamera( m_pkCameraManager->GetCamera() );

			// ���̴� �� ���� ����
			m_pkTerrain->SetShderMapCount( nShaderMapCount );

			CLoadingDisplay::GetInstance()->Render(25.0f);
		
			// �������� �ø� ���� ����
			m_pkTerrain->SetFrustumEpsilion( fPlaneEpsilon );

			// Slod ���� �Ÿ� ����
			m_pkTerrain->SetSlodRange( fSlodRange );

			CLoadingDisplay::GetInstance()->Render(30.0f);

			// ���̴� �ؽ��� ����
			m_pkTerrain->LoadNameData( nShaderMapCount, listFileName );

			// ���� ������ ���� ��忡 Attach!!
			AddObject_AffectLightScene( m_pkTerrain->GetObject() );

			// �ڷᱸ�� vector �����̳ʿ� �߰�
			m_vecSceneObjects.push_back( m_pkTerrain );
		}
	}


	//-----------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------
	//						��ī�� �ڽ�
	//-----------------------------------------------------------------------------------
	
	fread( buf, 1, sizeof(float) * 6, file );

	// ��ī�� �ڽ� ũ��
	memcpy( &vSkyExtents.x	, buf + sizeof(float) * 0, sizeof(float) ); 
	memcpy( &vSkyExtents.y	, buf + sizeof(float) * 1, sizeof(float) ); 
	memcpy( &vSkyExtents.z	, buf + sizeof(float) * 2, sizeof(float) ); 

	// ��ī�� �ڽ� ��ġ
	memcpy( &vSkyPos.x	, buf + sizeof(float) * 3, sizeof(float) ); // ��ü Terrain ���� ����
	memcpy( &vSkyPos.y	, buf + sizeof(float) * 4, sizeof(float) ); // ��ü Terrain ���� ����
	memcpy( &vSkyPos.z	, buf + sizeof(float) * 5, sizeof(float) ); // SLOD ���� ���� ����

	
	if( !m_pkSkyBox ) 
	{
		m_pkSkyBox = NiNew SkyBox();

		if( m_pkSkyBox )
		{
			// ��ī�� �ڽ� ����
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
	//									����ƽ ������Ʈ �ε� 
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
	//									��ü �� Data Load
	//------------------------------------------------------------------------------------

	m_pkTerrain->LoadData( file );	
	m_pkStaticObjectManager->LoadData( file, listEachofMeshCount );

	CLoadingDisplay::GetInstance()->Render(55.0f);

	//-----------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------	
	//									������ �ε� 
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
	//					Manager ����
	//
	//---------------------------------------------------------------------------

	// ��Į �޴��� ����
	if( !DecalManager::IsManager() )
	{
		m_pkDecalManager = DecalManager::CreateManager();

		if( m_pkTerrain ) DecalManager::GetManager()->SetTerrain( m_pkTerrain );
		
		AddObject_NotAffectLightScene( DecalManager::GetManager()->GetObject() );

		m_vecSceneObjects.push_back( m_pkDecalManager );
	}

	CLoadingDisplay::GetInstance()->Render(60.0f);

	//����Ʈ �Ŵ��� ����
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