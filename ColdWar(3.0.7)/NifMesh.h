#ifndef NIFMESH_H
#define NIFMESH_H

#include "BoundBox.h"
#include <vector>

class Terrain;

struct stRenderingIncludeInfo
{
	BYTE	m_nObjectIndex;
	WORD	m_nTileNum;
	BYTE	m_nIncludeType;
};

struct stCollisionIncludeInfo
{
	BYTE	m_nObjectIndex;
	WORD	m_nTileNum;	
	BYTE	m_nIncludeType;
};


class NifMesh : public NiMemObject
{

	NiNodePtr		m_spObject;
	BoundBox		*m_pBoundBox;

	NiFixedString	m_strFileName;
	bool			m_bDrawBB;
	bool			m_bWireFrame;

	NiPoint3		m_vMin, m_vMax;

	std::vector< stCollisionIncludeInfo >		m_vecInculdeCollsionGroup;
	std::vector< stRenderingIncludeInfo >		m_vecInculdeRenderingGroup;


public:

	NifMesh();
	NifMesh( const NiFixedString &fileName );
	~NifMesh();

	NiNode*		GenerateMesh();
	BOOL		GenerateAABB();
	BOOL		GenerateAABB( const NiPoint3& vMin, const NiPoint3& vMax );
	BOOL		ReleaseObject();

	NiNode*		ClonObject( NiNode *pObject );
	NiNode*		ClonObject( NifMesh *pNifMehs );

	NiNode*		GetRootObject();
	void		GetMinMax( NiPoint3 *vMin, NiPoint3 *vMax );
	void		SetMinMax( const NiPoint3& vMin, const NiPoint3& vMax );
	void		CalculateWorldMinMax( NiPoint3 *vMin, NiPoint3 *vMax );
	void		SetAppCulled( bool bCull );
	void		DrawBB( bool bDraw );
	void		SetName( const NiFixedString& strName );
	void		SetObject( NiNode* pNode );
	void		SetWireFrame( bool bWire );

	BOOL		RayCollision( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *vPickPos, NiPoint3 *vNormal, NiPick *pPick, float *fDist );
	BOOL		RayCollisionBB( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *dis );

	void		Translation( float x, float y, float z );
	void		Scaling( float fDel );
	void		RotationX( float fAngle );
	void		RotationY( float fAngle );
	void		RotationZ( float fAngle );
	void		SetDefault();

	void		AddCollisionGroup( unsigned int &objectIndex, int &index, BYTE type );
	void		AddRenderingGroup( unsigned int &objectIndex, int &index, BYTE type );
	void		ResetRenderingGroup();
	void		ResetCollsionGroup();
	void		ResetRenderingAndCollsionGroup();

	//			�� ����Ÿ ����
	BOOL		IsMapData();

	//				�� ����Ÿ Ÿ�� �ٿ�� �ڽ��� ���� ���� ����
	virtual	void	SpaceDivision( Terrain *pTerran );

	//				Nif �� ������ Extra Data�� �̿��Ͽ� �ش� Ÿ���� RenderGroup�� �߰�		
	virtual void	SpaceDivisionForRenderingFromMapData( Terrain *pTerran );

	//				Nif �� ������ Extra Data�� �̿��Ͽ� �ش� Ÿ���� CollisionManager�� �߰�		
	virtual void	SpaceDivisionForCollisionFromMapData( Terrain *pTerran );

	//				Nif �Ϲ� Object�� Extra Data�� �̿��Ͽ� �ش� Ÿ���� CollisionManager�� �߰�	
	virtual void	SpaceDivisionForCollision( Terrain *pTerran );

	virtual void	SpaceDivisionForRendering( Terrain *pTerran );

	virtual void	SetRenderingTransform( Terrain *pTerran );

	virtual void	SetCollisionTransform( Terrain *pTerran );

	//			Nif ���Ͽ� �ִ� "start" Node�� ���� �ɸ��� ������ġ�� ����
	virtual NiNode*	GetCharacterStartPosition();

	void		SetPosition( const NiPoint3& vPos );

//	void		SaveData( CArchive& ar );
	void		LoadData( FILE *file );

};


#endif