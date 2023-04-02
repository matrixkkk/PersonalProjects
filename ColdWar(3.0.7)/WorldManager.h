/*
	클래스 명 : WorldManager
	클래스 용도 : 월드를 구성하고 관리하는 클래스
				  CameraManager,ChacterManager,TerrainManager,CollisionManager를 가지고 있다

*/
#ifndef WORLDMANAGER_H_
#define WORLDMANAGER_H_

#include <NiMain.h>

#include "CameraManager.h"
#include "CharacterManager.h"
#include "CollisionManager.h"
#include "GameSystemManager.h"
#include "ObjectPick.h"

#include "Terrain.h"
#include "StaticObjectManager.h"
#include "BillBoardManager.h"
#include "DecalManager.h"
#include "SkyBox.h"
#include "Respone.h"
#include "CTerritory.h"
#include "DustManager.h"
//#include "ShadowObject.h"

#include <cstdio>
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <Functional>
#include <algorithm>

NiSmartPointer(NiFastAlphaAccumulator);

class CObjectPick;



class WorldManager : public NiMemObject
{

public:

	static bool Create( bool bNetMode );	//월드 매니저 생성
	static void Destroy();					//원드 매니저 파괴

	void	Draw();
	void	DrawScene(NiCamera* pkCamera);							//렌더링 부분
	void	DrawSkyBox(NiCamera* pkCamera);							//스카이 박스 렌더
	void	PartialRender(NiCamera* pkCamera,NiAVObject* pkObject);	//부분 렌더 - 캐릭터 그릴때 사용
	void	RenderScreenItem();
	void	Update();

	void	PerformPick();			//피킹 처리
	void	FireResultProcessing();	//fire정보가 있으면 결과 계산 후 결과 전송
	void	ProcessInput();			//input process
	bool	Initialize();			//월드매니저 초기화

	void	AddCharacter(DWORD type,const NiTransform& kSpawn);
	
	void	SetActiveMenuMode( bool bActive ){ m_bActiveMenu = bActive; }
	bool	IsActiveMenuMode(){ return m_bActiveMenu; }
	void	SetNetMode( bool bNetMode ){ m_bNetMode = bNetMode; }
	bool	IsNetMode(){ return m_bNetMode; }
	
	//Get
	CameraManager*				GetCameraManager();
	CharacterManager*			GetCharacterManager();
	CollisionManager*			GetCollisionManager();
	GameSystemManager*			GetGameSystemManager();
	Terrain*					GetTerrain();
	CRespone*					GetRespone();

	static WorldManager*		GetWorldManager();
	static NiNodePtr			OpenNif(const char* pcNifName);


	//------------ 안개 함수 ------------
	void	CreateFog();
	void	SetFog( bool bFog );
	void	SetFogDepth( float fDepth );
	float	GetFogDepth();
	//-----------------------------------

	void AttachLight1AffectedNode( NiNode* pObject);
	void AttachLight2AffectedNode( NiNode* pObject);

protected:

	//------------ 씬에 어태치, 디태치 함수 ------------

	void AddObjectAtRoot( NiNode *pObject );
	void AddObject_AffectLightScene( NiNode *pObject );
	void AddObject_NotAffectLightScene( NiNode *pObject );	
	
	void RemoveObjectAtRoot( NiNode *pObject );
	void RemoveObject_AffectLightScene( NiNode *pObject );
	void RemoveObject_NotAffectLightScene( NiNode *pObject );

	//--------------------------------------------------

	
	
private:
	WorldManager();
	~WorldManager();
	
	bool	CreateCamera();			//카메라 생성
	bool	CreateScene();			//씬 생성
	bool	CreateCharacter();		//캐릭터 생성
	bool	CreateLight();			//라이트 생성
	bool	CreateGameSystem();		//게임 시스템 생성
	bool	CreateRespone();	

	void	VectorFireDataProcess(FireInfo* pFireInfo);			//fireData 처리
	void	VectorFireResultProcess(FireResult* pFireResult);	//fireResult 처리
	
	//		파일로 부터 지형 정보를 로드한다.
	bool	LoadScene( FILE *file, std::string& cFileName );

	//		오브젝트 리스트 제거
	bool	ReleaseObject();		

	//		Light 생성
	void	EnableDefaultLights( bool bEnable );

	//		MaterialToolKit 생성
	BOOL	CreateMaterialToolKit();

	//		Material 속성값 셋팅
	void	SetMaterialColor( const NiColor& Ambient, const NiColor& Diffuse, const NiColor& Specular, const float& fShine );
	
	//캐릭터에 Fick광선을 쏴서 맞추는 정보를 가져온다.
	// 기존 - bool	PickFire( NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir,NiNode* &pkObject);
	bool	PickFire(FIRE_INTERSECT& vecIntersect, NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir);
private:
	//씬 그래프
	NiNodePtr						m_spScene;
	NiNode							*m_pkLightNode;			//빛 영향 O
	NiNode							*m_pkNotLightNode;		//빛 영향 X

	NiNodePtr						m_spStartPosition;	//캐릭터의 시작위치 노드
	
	//Sub-managers
	CameraManager*					m_pkCameraManager;		//캐릭터에게 붙일 카메라
	CharacterManager*				m_pkCharacterManager;	//캐릭터 관리자
	CollisionManager*				m_pkCollisionManager;	//충돌 관리자
	GameSystemManager*				m_pkGameSystemManager;	//게임 관리자

	Terrain*						m_pkTerrain;				//지형
	SkyBox*							m_pkSkyBox;					// 스카이 박스
	StaticObjectManager*			m_pkStaticObjectManager;	// 스태틱 오브젝트	
	BillBoardManager*				m_pkBillBoardManager;		// 빌보드	
	DecalManager*					m_pkDecalManager;			// 데칼
	CRespone*						m_pkRespone;				// 리스폰
	CTerritory*						m_pkTerritory;				// 정렴지

	std::vector< GBObject* >		m_vecSceneObjects;		// 전체 씬 오브젝트 리스트

	static WorldManager*			m_pkWorldManager;	
	
	//그리기 도구
	//알파 소팅을 위한 변수
	 NiFastAlphaAccumulatorPtr		m_spFastAlphaSorter;

	//Culling    
	NiVisibleArray					m_kVisible;
	NiCullingProcess				m_kCuller;
	
	CObjectPick*					m_pkPicker;				//칼라 피커
	NiPick							m_kFirePick;				//사격 피커
		
	bool	m_bColorPickProcess;	//피킹 처리 플래그
	bool	m_bClicked;				//마우스 클릭 확인
	bool	m_bActiveMenu;			//메뉴창 활성 모드
	bool	m_bNetMode;				// 싱글, 네트워크 모드

	unsigned int					m_uiCntObjects;			// 전체 추가된 NiNode 개수	
	unsigned int					m_uiVisibleObjCount;


	//-----------------------------------------------------------------------
	//							Property 모음

	NiFogPropertyPtr		m_spFog;
	NiMaterialPropertyPtr	m_spMaterialProp;
	NiWireframePropertyPtr	m_spWireframeProperty;
	

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
	//							Light
	
	NiDirectionalLightPtr	m_spDirectionalLignt1;
	NiDirectionalLightPtr	m_spDirectionalLignt2;

	//-----------------------------------------------------------------------
};

#include "WorldManager.inl"

#endif
