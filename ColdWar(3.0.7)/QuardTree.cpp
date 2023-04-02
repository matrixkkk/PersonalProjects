#include "main.h"
#include "QuardTree.h"


QuadTree::QuadTree( Terrain *pTerrain )
{
	int nTiles = pTerrain->GetCntTiles();
	int nCells = pTerrain->GetCntCells();
	int	nVtx	= nTiles * nCells + 1;

	m_nCorner[ BL ] = 0;							//TL������TR    TL, TR, BL, BR�� ������ Ÿ�� ��ȣ
	m_nCorner[ BR ] = nTiles;						//��	  ��	
	m_nCorner[ TL ] = nTiles * (nTiles + 1 );		//��      ��
	m_nCorner[ TR ] = (nTiles+1) * (nTiles+1) - 1;	//BL������BR

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

//----------------------- �ڳʰ� ���� -------------------------------------------------------
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


//----------------------- �ڽ� �߰� ---------------------------------------------------------
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

	// ��ܺ� ���
	nTopEdgeCenter		= ( m_nCorner[ TL ] + m_nCorner[ TR ] ) / 2;
	// �ϴܺ� ��� 
	nBottomEdgeCenter	= ( m_nCorner[ BL ] + m_nCorner[ BR ] ) / 2;
	// ������ ���
	nLeftEdgeCenter		= ( m_nCorner[ TL ] + m_nCorner[ BL ] ) / 2;
	// ������ ���
	nRightEdgeCenter	= ( m_nCorner[ TR ] + m_nCorner[ BR ] ) / 2;
	// �Ѱ��
	nCentralPoint		= ( m_nCorner[ TL ] + m_nCorner[ TR ] + 
							m_nCorner[ BL ] + m_nCorner[ BR ] ) / 4;

	// ���̻� ������ �Ұ����Ѱ�? �׷��ٸ� SubDivide() ����
	if( (m_nCorner[ TR ] - m_nCorner[ TL ]) <= 1 )
	{
		return FALSE;
	}

	// 4���� �ڽĳ�� �߰�

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
		// ������ܰ�, ���� �ϴ��� �Ÿ��� ���Ѵ�.		
		int nTiles  = pTerrain->GetCntTiles();
		int nCells  = pTerrain->GetCntCells();
		int nVtx	= nTiles * nCells + 1;

		int nVecTL =  ((m_nCorner[ TL ] / (nTiles + 1)) * nVtx * nCells ) + ((m_nCorner[ TL ] % (nTiles + 1))* nCells);
		int nVecBR =  ((m_nCorner[ BR ] / (nTiles + 1)) * nVtx * nCells ) + ((m_nCorner[ BR ] % (nTiles + 1))* nCells);

		NiPoint3 vecTL, vecBR, vecDiff;
		pTerrain->GetVec3Terrain( nVecTL, vecTL ); // Terrain �� HeightMap �迭�� ���� ��ġ���� ���´�.
		pTerrain->GetVec3Terrain( nVecBR, vecBR );

		vecDiff = vecTL - vecBR;
		
		// vec�� �Ÿ����� �� ��带 ���δ� ��豸�� �����̹Ƿ�, 
		// 2�� ������ �������� ���Ѵ�.
		m_fRadius	= vecDiff.Length() / 2.0f;

		m_pChildTree[ TL ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ TR ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ BL ]->GnerateQuadTree( pTerrain );
		m_pChildTree[ BR ]->GnerateQuadTree( pTerrain );
	}
	return TRUE;

}


// �����尡 �������ҿ� ���ԵǴ°�?
int QuadTree::IsInFrustum(  Terrain* pTerrain, FrustumEx* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere = FALSE;

	bInSphere = pFrustum->IsInSphere( m_vecCenter, m_fRadius );

	if( !bInSphere ) return FRUSTUM_OUT;	// ��豸 �ȿ� ������ �������� �������� �׽�Ʈ ����

	// ����Ʈ���� 4���� ��� �������� �׽�Ʈ
	b[0] = pFrustum->InInFrustum( m_vecBL );
	b[1] = pFrustum->InInFrustum( m_vecBR );
	b[2] = pFrustum->InInFrustum( m_vecTL );
	b[3] = pFrustum->InInFrustum( m_vecTR );

	// 4����� �������� �ȿ� ����
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) 
	{
		// else if ��ƾ�� �߰� �Ǿ�� �� �ϴ� �ӽ÷� �ּ�
		return FRUSTUM_COMPLETELY_IN;
	}

	// �Ϻκ��� �������ҿ� �ִ� ���
	return FRUSTUM_PARTIALLY_IN;
}


// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
void QuadTree::FrustumCull( Terrain* pTerrain, FrustumEx* pFrustum )
{
	if( NULL == pFrustum )
		return;

	int ret;

	ret = IsInFrustum( pTerrain, pFrustum );
	
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// �������ҿ� ��������, ������� �˻� �ʿ����
			m_bCulling = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN :		// �������ҿ� �Ϻ�����, ������� �˻� �ʿ���
			m_bCulling = FALSE;
			break;
		case FRUSTUM_OUT :				// �������ҿ��� �������, ������� �˻� �ʿ����
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



