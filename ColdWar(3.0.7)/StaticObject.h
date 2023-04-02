#ifndef StaticOBJECT_H
#define StaticOBJECT_H

#include "NifMesh.h"
#include "Terrain.h"
#include <list>
#include <cstdio>
#include <algorithm>



class StaticObject : public NiMemObject
{

public:

	enum{ MAP = 1, STATIC_WITH_COLLISION, STATIC_ONLY_RENDERING };
	// MAP	�� ��
	// STATIC_WITH_COLLISION �浹 �ڽ��� �ִ� ������Ʈ
	// STATIC_ONLY_RENDERING �浹 �ڽ��� ���� ��Ŀ� ������Ʈ

private:

	NiNodePtr				m_spObjects;
	NifMesh					*m_pSelectedObject;
	NifMesh					*m_pOriginMesh;

	NiFixedString			m_strFileName;
	unsigned short			m_usObjectCnt;

	BOOL					m_bDrawBB;
	BOOL					m_bPickDrawMesh;
	NiPoint3				m_vMin, m_vMax;

	BYTE					m_nObjectType;

	std::list< NifMesh* >	m_listNifMeshs;

public:

	StaticObject();
	StaticObject( const NiFixedString& strObjectName );
	~StaticObject();

	BOOL ReleaseObject();

	NiNode*	GetRootObject();

	NifMesh* GetNifMesh();

	int		GetObjectCount();

	//		�ø�
	void	SetAppCulled( bool bCull );

	//		�޽� �ε��ϱ�
	BOOL	LoadMesh();

	//		�ٿ�� �ڽ� �׸��� ����
	void	SetDrawObjectBB( bool bDraw );	//{	m_bDrawBB = bDraw; }

	//		�ٿ�� �ڽ��� �浹 üũ ���� ����� MeshObject �����Ѵ�.
	BOOL	CheckCollisionBBWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *fDis );
	
	//		�޽� �浹 üũ
	BOOL	CheckCollisionWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, NiPoint3* vPickPos, NiPoint3 *vNormal, float *fDis );

	//		MeshObject �ϳ� �߰��Ѵ�.
	BOOL	AddMeshObject(  const NiFixedString& strFileName );

	//		�޽� Object ���忡 �߰�
	void	AddMeshObjectAtWorld( const NiPoint3& pos );

	BYTE	GetObjectType(){ return m_nObjectType; }

	//		���õǾ��� MeshObject�� Matrix �ϳ� ����
	BOOL	DeleteSelectedObject();

	//		���� �̸��� ����
	NiFixedString GetFileName() const	{ return m_strFileName; }

	//		�ٿ�� �ڽ� ���
	void	SetDrawCollsionMesh( BOOL bCollision ){	m_bPickDrawMesh = bCollision; }

	//		���õǾ��� MatWorld �ִ��� ����
	BOOL	IsSelectedObject(){		return m_pSelectedObject != NULL; }

	//		World�� ���õǾ��� MeshObject�� NULL�� ����
	void	SetSelectedObjIsNULL();

	//		Object Dlg���� �Էµ� World ��ǥ�� ����
	void	SetSelectedObjPos( const NiPoint3& vPos );
	//	
	void	Translation( float x, float y, float z );
	void	Scaling( float fDel );
	void	RotationX( float fAngle );
	void	RotationY( float fAngle );
	void	RotationZ( float fAngle );
	void	SetDefault();

	//----------------------------------------------------------------------------------

	//		������ �� �׷����� ���� ����
	void	SpaceDivisionForRendering( Terrain *pTerrain );

	//		�ø��� �� �׷����� ���� ����
	void	SpaceDivisionForCollision( Terrain *pTerrain );

	//			Nif ���Ͽ� �ִ� "start" Node�� ���� �ɸ��� ������ġ�� ����
	NiNode*		GetCharacterStartPosition();

	void	SetRenderingTransform( Terrain *pTerrain );

	//		
	void	SetCollisionTransform( Terrain *pTerrain );
	

//	void SaveData( CArchive& ar );
	void LoadData( FILE *file, int nCntMeshInfo );


};


#endif