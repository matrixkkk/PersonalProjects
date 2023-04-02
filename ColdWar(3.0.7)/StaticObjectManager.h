#ifndef STATICOBJECTMANAGER_H
#define STATICOBJECTMANAGER_H

//#include "afxmt.h"

#include "GBObject.h"
#include "StaticObject.h"
#include "Terrain.h"

#include <list>

class StaticObjectManager : public GBObject
{

protected:

	NiNodePtr					m_spObjectRoot;

	std::list< StaticObject* >	m_listMeshObject;

	int							m_nObjectCount;
	StaticObject*				m_pCurrentObject;
	StaticObject*				m_pPickedObject;				
	StaticObject*				m_pCurrentDeleteObject;

	NiPick						*m_pPick;	// 오브젝트 픽킹

	Terrain						*m_pTerrain;

	static StaticObjectManager	*m_pStaticObjectManager;

protected:

	StaticObjectManager();
	~StaticObjectManager();


public:


	NiNode* GetObject(){ return  m_spObjectRoot; }
	BOOL	UpdateObject( float fElapsedTime );
	BOOL	ReleaseObject();

	static	StaticObjectManager*	CreateManager();
	static	StaticObjectManager*	GetManager();
	static  BOOL					IsManager();
	static	void					ReleaseManager();


	// 픽킹 NiPick 생성
	void	CreatePick();

	//		MeshObject 하나 추가한다.
	BOOL	AddMeshObject(  const NiFixedString& strFileName );

	//		MeshObject 하나 추가한다.
	void	AddMeshObjectAtWorld( const NiPoint3& vPickPos );

	//		MeshObject 하나 추가한다.
	BOOL	DeleteMeshObject( const NiFixedString& strFileName );

	//		선택되어진 MeshObject의 Matrix 하나 제거
	BOOL	DeleteSelectedObject();

	//		파일명과 일치되는 MeshObject를 찾는다.
	StaticObject* GetMeshObject( const NiFixedString& strFileName );

	//		파일명과 일치되는 MeshObject를 찾아서 m_pCurrentObject에 셋팅
	BOOL	SetCurrentMeshObject(  const NiFixedString& strFileName );

	//		오브젝트를 픽킹한다.
	BOOL	PickObject( const NiPoint3& pos, const NiPoint3& direction );

	//		바운딩 박스 출력 여부
	void	SetDrawObjectBB( bool bDraw );

	//		선택되어진 MeshObject의 MatWorld가 있는지 여부
	BOOL	IsSelectedObject();

	//		World에 선택되어진 MeshObject을 NULL로 셋팅
	void	SetSelectedObjIsNULL();

	//		Object Dlg에서 입력된 World 좌표로 셋팅
	void	SetSelectedObjPos( const NiPoint3& vPos );

	void	SetTerrain( Terrain *pTerrain );

	//		World에서 선택되어진 MeshObject포인터 얻는다.
	StaticObject*	GetSelectedMatOfObj();

	//			탄흔, 혈흔 등의 데칼을 오브젝트에 Attach 시킨다.
	BOOL		AttachDecal( const NiPoint3& pos, const NiPoint3& direction );

	void		Translation( float x, float y, float z );
	void		Scaling( float fDel );
	void		RotationX( float fAngle );
	void		RotationY( float fAngle );
	void		RotationZ( float fAngle );
	void		SetDefault();

	void		SetPosition( const NiPoint3& vPos );
/*
	void		SaveHeader( CArchive& ar );
	void		SaveData( CArchive& ar );
	void		SaveNameData( std::ofstream& fwrite );
*/
	//			렌더링 용 그룹으로 공간 분할
	void		SpaceDivisionForRendering( Terrain *pTerrain );

	//			컬리전 용 그룹으로 공간 분할
	void		SpaceDivisionForCollision( Terrain *pTerrain );

	//			렌더링 용 Local Transform을 셋팅			
	void		SetRenderingTransform( Terrain *pTerrain );
	
	//			컬리전 용 Local Transform을 셋팅
	void		SetCollisionTransform( Terrain *pTerrain  );

	NiNode*		GetCharacterStartPosition();
	
	void		LoadData( FILE *file, std::list<int>& listEachofMeshCount );
	void		LoadNameData( FILE *file, std::list< int >& listEachofMeshCount, std::list<NiFixedString>& listFileName );

};

#endif
