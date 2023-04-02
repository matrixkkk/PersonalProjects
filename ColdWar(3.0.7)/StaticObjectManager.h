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

	NiPick						*m_pPick;	// ������Ʈ ��ŷ

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


	// ��ŷ NiPick ����
	void	CreatePick();

	//		MeshObject �ϳ� �߰��Ѵ�.
	BOOL	AddMeshObject(  const NiFixedString& strFileName );

	//		MeshObject �ϳ� �߰��Ѵ�.
	void	AddMeshObjectAtWorld( const NiPoint3& vPickPos );

	//		MeshObject �ϳ� �߰��Ѵ�.
	BOOL	DeleteMeshObject( const NiFixedString& strFileName );

	//		���õǾ��� MeshObject�� Matrix �ϳ� ����
	BOOL	DeleteSelectedObject();

	//		���ϸ�� ��ġ�Ǵ� MeshObject�� ã�´�.
	StaticObject* GetMeshObject( const NiFixedString& strFileName );

	//		���ϸ�� ��ġ�Ǵ� MeshObject�� ã�Ƽ� m_pCurrentObject�� ����
	BOOL	SetCurrentMeshObject(  const NiFixedString& strFileName );

	//		������Ʈ�� ��ŷ�Ѵ�.
	BOOL	PickObject( const NiPoint3& pos, const NiPoint3& direction );

	//		�ٿ�� �ڽ� ��� ����
	void	SetDrawObjectBB( bool bDraw );

	//		���õǾ��� MeshObject�� MatWorld�� �ִ��� ����
	BOOL	IsSelectedObject();

	//		World�� ���õǾ��� MeshObject�� NULL�� ����
	void	SetSelectedObjIsNULL();

	//		Object Dlg���� �Էµ� World ��ǥ�� ����
	void	SetSelectedObjPos( const NiPoint3& vPos );

	void	SetTerrain( Terrain *pTerrain );

	//		World���� ���õǾ��� MeshObject������ ��´�.
	StaticObject*	GetSelectedMatOfObj();

	//			ź��, ���� ���� ��Į�� ������Ʈ�� Attach ��Ų��.
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
	//			������ �� �׷����� ���� ����
	void		SpaceDivisionForRendering( Terrain *pTerrain );

	//			�ø��� �� �׷����� ���� ����
	void		SpaceDivisionForCollision( Terrain *pTerrain );

	//			������ �� Local Transform�� ����			
	void		SetRenderingTransform( Terrain *pTerrain );
	
	//			�ø��� �� Local Transform�� ����
	void		SetCollisionTransform( Terrain *pTerrain  );

	NiNode*		GetCharacterStartPosition();
	
	void		LoadData( FILE *file, std::list<int>& listEachofMeshCount );
	void		LoadNameData( FILE *file, std::list< int >& listEachofMeshCount, std::list<NiFixedString>& listFileName );

};

#endif
