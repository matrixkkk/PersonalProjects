#include "main.h"
#include "QuardTree.h"


QuadTree::QuadTree( Terrain *pTerrain )
{
	int nTiles = pTerrain->GetCntTiles();
	int nCells = pTerrain->GetCntCells();
	int	nVtx	= nTiles * nCells + 1;

	m_nCorner[ BL ] = 0;							//TL━━━TR    TL, TR, BL, BR은 지형이 타일 번호
	m_nCorner[ BR ] = nTiles;						//┃	  ┃	
	m_nCorner[ TL ] = nTiles * (nTiles + 1 );		//┃      ┃
	m_nCorner[ TR ] = (nTiles+1) * (nTiles+1) - 1;	//BL━━━BR

	m_nCenter	= ( m_nCorner[TL] + m_nCorner[TR] + m_nCorner[BL] + m_nCorner[BR] ) / 4;

	int nVecCenter = ((m_nCenter / (nTiles + 1)) * nVtx * nCells ) + ((m_nCenter % (nTiles + 1))* nCells) ;

	pTerrain->GetVec3Terrain( nVecCenter,m_vecCenter );

	int _BL = ((m_nCorner[BL] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[BL] % (nTiles + 1))* nCells) ;
	int _BR = ((m_nCorner[BR] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[BR] % (nTiles + 1))* nCells) ;
	int _TL = ((m_nCorner[TL] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[TL] % (nTiles + 1))* nCells) ;
	int _TR = ((m_nCorner[TR] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[TR] % (nTiles + 1))* nCells) ;
	
	pTerrain->GetVec3Terrain( _BL, m_vecBL );
	pTerrain->GetVec3Terrain( _BR, m_vecBR );
	pTerrain->GetVec3Terrain( _TL, m_vecTL );
	pTerrain->GetVec3Terrain( _TR, m_vecTR );
	
	for( int i = 0 ; i < 4 ; i++ )
	{
		m_pChildTree[i]	= NULL;
	}

	m_bCulling		= FALSE;
	m_fRadius		= 0.0f;

}

QuadTree::QuadTree( QuadTree* pParent )
{
	m_nCenter		= 0;
	m_bCulling		= FALSE;
	m_fRadius		= 0.0f;
	m_nCheckCount	= 0;

	for( int i = 0 ; i < 4 ; i++ )
	{
		m_pChildTree[i]	= NULL;
		m_nCorner[i]	= 0;
	}
}

QuadTree::~QuadTree()
{
	for( int i = 0 ; i < 4 ; i++ )
	{
		if( m_pChildTree[ i ] != NULL )
			delete m_pChildTree[ i ];
	}
}

//----------------------- 코너값 셋팅 -------------------------------------------------------
void QuadTree::SetCorners( int nCornerBL, int nCornerBR, int nCornerTL, int nCornerTR, Terrain *pTerrain  )
//-------------------------------------------------------------------------------------------
{
	m_nCorner[ TL ] = nCornerTL;
	m_nCorner[ TR ] = nCornerTR;
	m_nCorner[ BL ] = nCornerBL;
	m_nCorner[ BR ] = nCornerBR;

	m_nCenter = ( m_nCorner[ TL ] + m_nCorner[ TR ] + m_nCorner[ BL ] + m_nCorner[ BR ] ) / 4;

	int  nCells = pTerrain->GetCntCells();
	int  nTiles = pTerrain->GetCntTiles();
	int  nVtx  = nCells * nTiles + 1;

	int nVecCenter = ((m_nCenter / (nTiles + 1)) * nVtx * nCells ) + ((m_nCenter % (nTiles + 1))* nCells) ;

	pTerrain->GetVec3Terrain( nVecCenter,m_vecCenter );

	int _BL = ((m_nCorner[BL] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[BL] % (nTiles + 1))* nCells) ;
	int _BR = ((m_nCorner[BR] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[BR] % (nTiles + 1))* nCells) ;
	int _TL = ((m_nCorner[TL] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[TL] % (nTiles + 1))* nCells) ;
	int _TR = ((m_nCorner[TR] / (nTiles + 1)) * nVtx * nCells) + ((m_nCorner[TR] % (nTiles + 1))* nCells) ;
	
	pTerrain->GetVec3Terrain( _BL, m_vecBL );
	pTerrain->GetVec3Terrain( _BR, m_vecBR );
	pTerrain->GetVec3Terrain( _TL, m_vecTL );
	pTerrain->GetVec3Terrain( _TR, m_vecTR );

}


//----------------------- 자식 추가 ---------------------------------------------------------
QuadTree* QuadTree::AddChild( int nCornerBL, int nCornerBR, int nCornerTL, int nCornerTR, Terrain* pTerrain )
//-------------------------------------------------------------------------------------------
{
	QuadTree*	pChild;

	pChild = NiNew QuadTree( this );
	pChild->SetCorners( nCornerBL, nCornerBR, nCornerTL, nCornerTR, pTerrain );

	return pChild;
}


BOOL QuadTree::SubDivide( Terrain *pTerrain )
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 상단변 가운데
	nTopEdgeCenter		= ( m_nCorner[ TL ] + m_nCorner[ TR ] ) / 2;
	// 하단변 가운데 
	nBottomEdgeCenter	= ( m_nCorner[ BL ] + m_nCorner[ BR ] ) / 2;
	// 좌측변 가운데
	nLeftEdgeCenter		= ( m_nCorner[ TL ] + m_nCorner[ BL ] ) / 2;
	// 우측변 가운데
	nRightEdgeCenter	= ( m_nCorner[ TR ] + m_nCorner[ BR ] ) / 2;
	// 한가운데
	nCentralPoint		= ( m_nCorner[ TL ] + m_nCorner[ TR ] + 
							m_nCorner[ BL ] + m_nCorner[ BR ] ) / 4;

	// 더이상 분할이 불가능한가? 그렇다면 SubDivide() 종료
	if( (m_nCorner[ TR ] - m_nCorner[ TL ]) <= 1 )
	{
		return FALSE;
	}

	// 4개의 자식노드 추가

	m_pChildTree[ BL ] = AddChild( m_nCorner[ BL ],		nBottomEdgeCenter,	nLeftEdgeCenter,	nCentralPoint,		pTerrain );
	m_pChildTree[ BR ] = AddChild( nBottomEdgeCenter,	m_nCorner[ BR ],	nCentralPoint,		nRightEdgeCenter,   pTerrain );
	m_pChildTree[ TL ] = AddChild( nLeftEdgeCenter,		nCentralPoint,		m_nCorner[ TL ],	nTopEdgeCenter,		pTerrain );
	m_pChildTree[ TR ] = AddChild( nCentralPoint,		nRightEdgeCenter,	nTopEdgeCenter,		m_nCorner[ TR ],	pTerrain );
	
	return TRUE;
}


BOOL QuadTree::GnerateQuadTree( Terrain* pTerrain )
{

	if( SubDivide( pTerrain ) )
	{
		// 좌측상단과, 우측 하단의 거리를 구한다.		
		int nTiles  = pTerrain->GetCntTiles();
		int nCells  = pTerrain->GetCntCells();
		int nVtx	= nTiles * nCells + 1;

		int nVecTL =  ((m_nCorner[ TL ] / (nTiles + 1)) * nVtx * nCells ) + ((m_nCorner[ TL ] % (nTiles + 1))* nCells);
		int nVecBR =  ((m_nCorner[ BR ] / (nTiles + 1)) * nVtx * nCells ) + ((m_nCorner[ BR ] % (nTiles + 1))* nCells);

		NiPoint3 vecTL, vecBR, vecDiff;
		pTerrain->GetVec3Terrain( nVecTL, vecTL ); // Terrain 의 HeightMap 배열로 부터 위치값을 얻어온다.
		pTerrain->GetVec3Terrain( nVecBR, vecBR );

		vecDiff = vecTL - vecBR;
		
		// vec의 거리값이 이 노드를 감싸는 경계구의 지름이므로, 
		// 2로 나누어 반지름을 구한다.
		m_fRadius	= vecDiff.Length() / 2.0f;

		m_pChildTree[ TL ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ TR ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ BL ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ BR ]->GnerateQuadTree( pTerrain );
	}
	return TRUE;

}


// 현재노드가 프러스텀에 포함되는가?
int QuadTree::IsInFrustum(  Terrain* pTerrain, FrustumEx* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere = FALSE;

	bInSphere = pFrustum->IsInSphere( m_vecCenter, m_fRadius );

	if( !bInSphere ) return FRUSTUM_OUT;	// 경계구 안에 없으면 점단위의 프러스텀 테스트 생략

	// 쿼드트리의 4군데 경계 프러스텀 테스트
	b[0] = pFrustum->InInFrustum( m_vecBL );
	b[1] = pFrustum->InInFrustum( m_vecBR );
	b[2] = pFrustum->InInFrustum( m_vecTL );
	b[3] = pFrustum->InInFrustum( m_vecTR );

	// 4개모두 프러스텀 안에 있음
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) 
	{
		// else if 루틴은 추가 되어야 함 일단 임시로 주석
		return FRUSTUM_COMPLETELY_IN;
	}

	// 일부분이 프러스텀에 있는 경우
	return FRUSTUM_PARTIALLY_IN;
}


// _IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
void QuadTree::FrustumCull( Terrain* pTerrain, FrustumEx* pFrustum )
{
	if( NULL == pFrustum )
		return;

	int ret;

	ret = IsInFrustum( pTerrain, pFrustum );
	
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// 프러스텀에 완전포함, 하위노드 검색 필요없음
			m_bCulling = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN :		// 프러스텀에 일부포함, 하위노드 검색 필요함
			m_bCulling = FALSE;
			break;
		case FRUSTUM_OUT :				// 프러스텀에서 완전벗어남, 하위노드 검색 필요없음
			m_bCulling = TRUE;
			return;
	}

	m_nCheckCount++;

	if( m_pChildTree[BL] ) m_pChildTree[BL]->FrustumCull( pTerrain, pFrustum );
	if( m_pChildTree[BR] ) m_pChildTree[BR]->FrustumCull( pTerrain, pFrustum );
	if( m_pChildTree[TL] ) m_pChildTree[TL]->FrustumCull( pTerrain, pFrustum );
	if( m_pChildTree[TR] ) m_pChildTree[TR]->FrustumCull( pTerrain, pFrustum );
}


void QuadTree::ValidGetTile( Terrain* pTerrain, std::vector< int >& vecTilesNum )
{
	if( m_bCulling )
	{
		m_bCulling = FALSE;
		return;
	}

	if( m_nCorner[ TR ] - m_nCorner[ TL ] <= 1 )
	{
		int nTileIndex = m_nCorner[ TL ] - pTerrain->GetCntTiles() - ( m_nCorner[ TL ] / ( pTerrain->GetCntTiles() + 1 ));
		vecTilesNum.push_back( nTileIndex );
	}

	if( m_pChildTree[BL] ) m_pChildTree[BL]->ValidGetTile( pTerrain, vecTilesNum );
	if( m_pChildTree[BR] ) m_pChildTree[BR]->ValidGetTile( pTerrain, vecTilesNum );
	if( m_pChildTree[TL] ) m_pChildTree[TL]->ValidGetTile( pTerrain, vecTilesNum );
	if( m_pChildTree[TR] ) m_pChildTree[TR]->ValidGetTile( pTerrain, vecTilesNum );
}

void QuadTree::Update( Terrain* pTerrain, FrustumEx* pFrustum, std::vector< int >& vecTilesNum )
{

	FrustumCull( pTerrain, pFrustum );

	ValidGetTile( pTerrain, vecTilesNum );
}



