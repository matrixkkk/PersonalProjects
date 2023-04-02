#ifndef QUARDTREE_H
#define QUARDTREE_H


#include <algorithm>
#include <vector>
//#include "BoundBox.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Tile.h"

class Terrain;
//class CGameApp;
//class GameApp;

/*
struct _TerrainVertex{

	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR2 t;
};
*/

class QuadTree : public NiMemObject
{

	enum CONTER_TYPE{ TL, TR, BL, BR, CENTER };
	enum INTERSECT_TYPE{ FRUSTUM_OUT, FRUSTUM_PARTIALLY_IN, FRUSTUM_COMPLETELY_IN };

	QuadTree	*m_pChildTree[ 4 ];

	int			m_nCorner[ 4 ];
	int			m_nCenter;
	BOOL		m_bCulling;
	float		m_fRadius;
	int			m_nCheckCount;
	NiPoint3	m_vecBL, m_vecBR, m_vecTL, m_vecTR;
//	int			m_nVecCenter;
	NiPoint3	m_vecCenter;

public:

	QuadTree( ){ };
	QuadTree( Terrain *pTerrain );
	QuadTree( QuadTree* pParent );
	~QuadTree();

	BOOL GnerateQuadTree( Terrain* pTerrain );	
	QuadTree*	AddChild( int nCornerBL, int nCornerBR, int nCornerTL, int nCornerTR, Terrain* pTerrain  );	

	BOOL SubDivide( Terrain* pTerrain );	
	void SetCorners( int nCornerBL, int nCornerBR, int nCornerTL, int nCornerTR,  Terrain* pTerrain  );				
	
	// ÄÃ¸µ
	int		IsInFrustum( Terrain* pTerrain, FrustumEx* pFrustum );
	void	FrustumCull( Terrain* pTerrain, FrustumEx* pFrustum );

	void	ValidGetTile( Terrain* pTerrain, std::vector< int >& vecTilesNum );
	void	Update( Terrain* pTerrain, FrustumEx* pFrustum, std::vector<int>& vecTilesNum );

	

};

#endif