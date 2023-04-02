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
	// MAP	맵 용
	// STATIC_WITH_COLLISION 충돌 박스가 있는 오브젝트
	// STATIC_ONLY_RENDERING 충돌 박스가 없는 장식용 오브젝트

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

	//		컬링
	void	SetAppCulled( bool bCull );

	//		메쉬 로드하기
	BOOL	LoadMesh();

	//		바운딩 박스 그리기 여부
	void	SetDrawObjectBB( bool bDraw );	//{	m_bDrawBB = bDraw; }

	//		바운딩 박스와 충돌 체크 가장 가까운 MeshObject 선별한다.
	BOOL	CheckCollisionBBWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *fDis );
	
	//		메쉬 충돌 체크
	BOOL	CheckCollisionWithRay( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, NiPoint3* vPickPos, NiPoint3 *vNormal, float *fDis );

	//		MeshObject 하나 추가한다.
	BOOL	AddMeshObject(  const NiFixedString& strFileName );

	//		메쉬 Object 월드에 추가
	void	AddMeshObjectAtWorld( const NiPoint3& pos );

	BYTE	GetObjectType(){ return m_nObjectType; }

	//		선택되어진 MeshObject의 Matrix 하나 제거
	BOOL	DeleteSelectedObject();

	//		파일 이름을 얻음
	NiFixedString GetFileName() const	{ return m_strFileName; }

	//		바운딩 박스 출력
	void	SetDrawCollsionMesh( BOOL bCollision ){	m_bPickDrawMesh = bCollision; }

	//		선택되어진 MatWorld 있는지 여부
	BOOL	IsSelectedObject(){		return m_pSelectedObject != NULL; }

	//		World에 선택되어진 MeshObject을 NULL로 셋팅
	void	SetSelectedObjIsNULL();

	//		Object Dlg에서 입력된 World 좌표로 셋팅
	void	SetSelectedObjPos( const NiPoint3& vPos );
	//	
	void	Translation( float x, float y, float z );
	void	Scaling( float fDel );
	void	RotationX( float fAngle );
	void	RotationY( float fAngle );
	void	RotationZ( float fAngle );
	void	SetDefault();

	//----------------------------------------------------------------------------------

	//		렌더링 용 그룹으로 공간 분할
	void	SpaceDivisionForRendering( Terrain *pTerrain );

	//		컬리전 용 그룹으로 공간 분할
	void	SpaceDivisionForCollision( Terrain *pTerrain );

	//			Nif 파일에 있는 "start" Node로 부터 케릭터 시작위치를 얻음
	NiNode*		GetCharacterStartPosition();

	void	SetRenderingTransform( Terrain *pTerrain );

	//		
	void	SetCollisionTransform( Terrain *pTerrain );
	

//	void SaveData( CArchive& ar );
	void LoadData( FILE *file, int nCntMeshInfo );


};


#endif