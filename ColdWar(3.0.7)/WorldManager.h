/*
	Ŭ���� �� : WorldManager
	Ŭ���� �뵵 : ���带 �����ϰ� �����ϴ� Ŭ����
				  CameraManager,ChacterManager,TerrainManager,CollisionManager�� ������ �ִ�

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

	static bool Create( bool bNetMode );	//���� �Ŵ��� ����
	static void Destroy();					//���� �Ŵ��� �ı�

	void	Draw();
	void	DrawScene(NiCamera* pkCamera);							//������ �κ�
	void	DrawSkyBox(NiCamera* pkCamera);							//��ī�� �ڽ� ����
	void	PartialRender(NiCamera* pkCamera,NiAVObject* pkObject);	//�κ� ���� - ĳ���� �׸��� ���
	void	RenderScreenItem();
	void	Update();

	void	PerformPick();			//��ŷ ó��
	void	FireResultProcessing();	//fire������ ������ ��� ��� �� ��� ����
	void	ProcessInput();			//input process
	bool	Initialize();			//����Ŵ��� �ʱ�ȭ

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


	//------------ �Ȱ� �Լ� ------------
	void	CreateFog();
	void	SetFog( bool bFog );
	void	SetFogDepth( float fDepth );
	float	GetFogDepth();
	//-----------------------------------

	void AttachLight1AffectedNode( NiNode* pObject);
	void AttachLight2AffectedNode( NiNode* pObject);

protected:

	//------------ ���� ����ġ, ����ġ �Լ� ------------

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
	
	bool	CreateCamera();			//ī�޶� ����
	bool	CreateScene();			//�� ����
	bool	CreateCharacter();		//ĳ���� ����
	bool	CreateLight();			//����Ʈ ����
	bool	CreateGameSystem();		//���� �ý��� ����
	bool	CreateRespone();	

	void	VectorFireDataProcess(FireInfo* pFireInfo);			//fireData ó��
	void	VectorFireResultProcess(FireResult* pFireResult);	//fireResult ó��
	
	//		���Ϸ� ���� ���� ������ �ε��Ѵ�.
	bool	LoadScene( FILE *file, std::string& cFileName );

	//		������Ʈ ����Ʈ ����
	bool	ReleaseObject();		

	//		Light ����
	void	EnableDefaultLights( bool bEnable );

	//		MaterialToolKit ����
	BOOL	CreateMaterialToolKit();

	//		Material �Ӽ��� ����
	void	SetMaterialColor( const NiColor& Ambient, const NiColor& Diffuse, const NiColor& Specular, const float& fShine );
	
	//ĳ���Ϳ� Fick������ ���� ���ߴ� ������ �����´�.
	// ���� - bool	PickFire( NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir,NiNode* &pkObject);
	bool	PickFire(FIRE_INTERSECT& vecIntersect, NiNode* pkPickRoot,const NiPoint3& kOrigin,const NiPoint3& kDir);
private:
	//�� �׷���
	NiNodePtr						m_spScene;
	NiNode							*m_pkLightNode;			//�� ���� O
	NiNode							*m_pkNotLightNode;		//�� ���� X

	NiNodePtr						m_spStartPosition;	//ĳ������ ������ġ ���
	
	//Sub-managers
	CameraManager*					m_pkCameraManager;		//ĳ���Ϳ��� ���� ī�޶�
	CharacterManager*				m_pkCharacterManager;	//ĳ���� ������
	CollisionManager*				m_pkCollisionManager;	//�浹 ������
	GameSystemManager*				m_pkGameSystemManager;	//���� ������

	Terrain*						m_pkTerrain;				//����
	SkyBox*							m_pkSkyBox;					// ��ī�� �ڽ�
	StaticObjectManager*			m_pkStaticObjectManager;	// ����ƽ ������Ʈ	
	BillBoardManager*				m_pkBillBoardManager;		// ������	
	DecalManager*					m_pkDecalManager;			// ��Į
	CRespone*						m_pkRespone;				// ������
	CTerritory*						m_pkTerritory;				// ������

	std::vector< GBObject* >		m_vecSceneObjects;		// ��ü �� ������Ʈ ����Ʈ

	static WorldManager*			m_pkWorldManager;	
	
	//�׸��� ����
	//���� ������ ���� ����
	 NiFastAlphaAccumulatorPtr		m_spFastAlphaSorter;

	//Culling    
	NiVisibleArray					m_kVisible;
	NiCullingProcess				m_kCuller;
	
	CObjectPick*					m_pkPicker;				//Į�� ��Ŀ
	NiPick							m_kFirePick;				//��� ��Ŀ
		
	bool	m_bColorPickProcess;	//��ŷ ó�� �÷���
	bool	m_bClicked;				//���콺 Ŭ�� Ȯ��
	bool	m_bActiveMenu;			//�޴�â Ȱ�� ���
	bool	m_bNetMode;				// �̱�, ��Ʈ��ũ ���

	unsigned int					m_uiCntObjects;			// ��ü �߰��� NiNode ����	
	unsigned int					m_uiVisibleObjCount;


	//-----------------------------------------------------------------------
	//							Property ����

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
