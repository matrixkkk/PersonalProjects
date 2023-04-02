#include "main.h"
#include "BoundBox.h"


BoundBox::BoundBox( const NiPoint3& vMin, const NiPoint3& vMax ) : m_vMin( vMin ), m_vMax( vMax )
{

}


BoundBox::~BoundBox()
{
//	ReleaseObject();
}

NiAVObject*	BoundBox::GenerateAABB( const NiPoint3& vMin, const NiPoint3& vMax )
{

	NiPoint3 *pVPos = NiNew NiPoint3[ 8 ];	
	NiColorA* pkBoxColors = NiNew NiColorA[ 8 ];
	unsigned short *index = NiAlloc(unsigned short, 36);

	m_vMin = vMin;
	m_vMax = vMax;

	if( NULL == pVPos || NULL == pkBoxColors || NULL == index )
	{
		goto Exit;
	}

	pVPos[0].x = vMin.x; pVPos[0].y = vMin.y; pVPos[0].z = vMin.z;	
	pVPos[1].x = vMax.x; pVPos[1].y = vMin.y; pVPos[1].z = vMin.z;
	pVPos[2].x = vMin.x; pVPos[2].y = vMin.y; pVPos[2].z = vMax.z;	
	pVPos[3].x = vMax.x; pVPos[3].y = vMin.y; pVPos[3].z = vMax.z;	
	pVPos[4].x = vMin.x; pVPos[4].y = vMax.y; pVPos[4].z = vMin.z;
	pVPos[5].x = vMax.x; pVPos[5].y = vMax.y; pVPos[5].z = vMin.z;
	pVPos[6].x = vMin.x; pVPos[6].y = vMax.y; pVPos[6].z = vMax.z;
	pVPos[7].x = vMax.x; pVPos[7].y = vMax.y; pVPos[7].z = vMax.z;
	

    pkBoxColors[0] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
    pkBoxColors[1] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
    pkBoxColors[2] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
    pkBoxColors[3] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
	pkBoxColors[4] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
	pkBoxColors[5] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
	pkBoxColors[6] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
	pkBoxColors[7] = NiColorA(0.5f, 0.5f, 0.4f, 1.0f);
	
	//앞
	index[0] = 0; index[1] = 1;	index[2] = 2; 
	index[3] = 1; index[4] = 3; index[5] = 2;

	//왼쪽
	index[6]  = 0; index[7]  = 6; index[8]  = 4;
	index[9]  = 0; index[10] = 2; index[11] = 6;

	//오른쪽
	index[12] = 5; index[13] = 3; index[14] = 1;
	index[15] = 5; index[16] = 7; index[17] = 3;

	//뒤
	index[18] = 4; index[19] = 7; index[20] = 5;
	index[21] = 4; index[22] = 6; index[23] = 7;

	// 윗면
	index[24] = 3; index[25] = 6; index[26] = 2;
	index[27] = 3; index[28] = 7; index[29] = 6;

	// 밑면
	index[30] = 5; index[31] = 0; index[32] = 4;
	index[33] = 5; index[34] = 1; index[35] = 0;


	m_spBox = NiNew NiTriShape( 8, pVPos, NULL, pkBoxColors, NULL, 0, NiGeometryData::NBT_METHOD_NONE, 12, index );

	if( !m_spBox )
		goto Exit;

	m_spBox->CalculateNormals();
	m_spBox->SetName("BoundBox");
	m_spBox->SetAppCulled( true );

	// 와이어 프레임	
	m_spWireFrame = NiTCreate<NiWireframeProperty>();
	m_spWireFrame->SetWireframe( true );
	m_spBox->AttachProperty( m_spWireFrame );
	
	m_spBox->Update( 0.0f );
	m_spBox->UpdateNodeBound();
	m_spBox->UpdateProperties();

	return m_spBox;

Exit:

	NiDelete[] pVPos;
	NiDelete[] pkBoxColors;
	NiDelete[] index;

	return NULL;		
	
}

void BoundBox::SetMinMax( const NiPoint3& vMin, const NiPoint3& vMax )
{
	m_vMin = vMin;
	m_vMax = vMax;

	NiTriShapeData *pVtxData = (NiTriShapeData*)m_spBox->GetModelData();

	NiPoint3 *pVtxPos = pVtxData->GetVertices();

	pVtxPos[0].x = vMin.x; pVtxPos[0].y = vMin.y; pVtxPos[0].z = vMin.z;	
	pVtxPos[1].x = vMax.x; pVtxPos[1].y = vMin.y; pVtxPos[1].z = vMin.z;
	pVtxPos[2].x = vMin.x; pVtxPos[2].y = vMin.y; pVtxPos[2].z = vMax.z;	
	pVtxPos[3].x = vMax.x; pVtxPos[3].y = vMin.y; pVtxPos[3].z = vMax.z;	
	pVtxPos[4].x = vMin.x; pVtxPos[4].y = vMax.y; pVtxPos[4].z = vMin.z;
	pVtxPos[5].x = vMax.x; pVtxPos[5].y = vMax.y; pVtxPos[5].z = vMin.z;
	pVtxPos[6].x = vMin.x; pVtxPos[6].y = vMax.y; pVtxPos[6].z = vMax.z;
	pVtxPos[7].x = vMax.x; pVtxPos[7].y = vMax.y; pVtxPos[7].z = vMax.z;	

	pVtxData->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	
}

void BoundBox::UpdateMax( float minY, float maxY )
{
	m_vMax.z = maxY;
	m_vMin.z = minY;

	NiTriShapeData *pData = (NiTriShapeData*)m_spBox->GetModelData();
	NiPoint3 *pVtxPos = pData->GetVertices();

	// 바닥면
	pVtxPos[0].z = minY;
	pVtxPos[1].z = minY;
	pVtxPos[4].z = minY;
	pVtxPos[5].z = minY;

	// 윗면
	pVtxPos[2].z = maxY;
	pVtxPos[3].z = maxY;
	pVtxPos[6].z = maxY;
	pVtxPos[7].z = maxY;

	pData->MarkAsChanged( NiGeometryData::VERTEX_MASK );
/*
	VERTEX_MASK
	Vertex coordinates have been changed.
 
	NORMAL_MASK
	Normals have been changed.
 
	COLOR_MASK
	Colors have been changed.
 
	TEXTURE_MASK
	Texture coordinates have been changed.
*/
}

void BoundBox::SetAppCulled( bool bCull )
{
	if( m_spBox != NULL )
		m_spBox->SetAppCulled( bCull );
}

void BoundBox::RenderImmediate( NiRenderer *pRenderer )
{
	m_spBox->RenderImmediate( pRenderer );
}


// 바운딩 박스의 벡터성분을 넣어 Min Max 값 조절
void BoundBox::Include ( const NiPoint3& vPos )
{

	if( m_vMax.x < vPos.x )
		m_vMax.x = vPos.x;

	if( m_vMax.y < vPos.y )
		m_vMax.y = vPos.y;

	if( m_vMax.z < vPos.z )
		m_vMax.z = vPos.z;

	if( m_vMin.x > vPos.x )
		m_vMin.x = vPos.x;

	if( m_vMin.y > vPos.y )
		m_vMin.y = vPos.y;

	if( m_vMin.z > vPos.z )
		m_vMin.z = vPos.z;

}


BOOL BoundBox::IsCollision( const NiPoint3 &vPoint )
{
	if( m_vMin.x > vPoint.x )
		return FALSE;
	if( m_vMin.y > vPoint.y )
		return FALSE;
	if( m_vMin.z > vPoint.z )
		return FALSE;

	if( m_vMax.x < vPoint.x )
		return FALSE;
	if( m_vMax.y < vPoint.y )
		return FALSE;
	if( m_vMax.z < vPoint.z )
		return FALSE;

	return TRUE;
}

BOOL BoundBox::RayCollision( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *fDist )
{

	if( NULL == pPick )
		return FALSE;

	if (NiIsKindOf(NiAVObject, m_spBox))
	{		
		pPick->SetTarget( (NiAVObject*)m_spBox );		

		if( pPick->PickObjects( pos, dir ) )
		{
			// get pick results
			NiPick::Results& pickResults = pPick->GetResults();
			NiPick::Record* pPickRecord = pickResults.GetAt(0);
			*fDist = NiAbs(pPickRecord->GetDistance());

			pPick->RemoveTarget();
			return TRUE;
		}
	}

	pPick->RemoveTarget();
	return FALSE;
}

NiAVObject* BoundBox::GetBoundObject()
{
	return m_spBox;
}

void BoundBox::ReleaseObject()
{
	if( m_spBox ){
		m_spBox->DetachProperty( m_spWireFrame );
		m_spWireFrame = 0;
		m_spBox = 0;
	}
}

void	BoundBox::Translation( float x, float y, float z )
{
	NiPoint3 vMoveUnit( x, y, z );
	NiPoint3 kObjectPos = m_spBox->GetTranslate();

	kObjectPos += vMoveUnit;

	if( m_spBox )
	m_spBox->SetTranslate( kObjectPos );
}

void	BoundBox::Scaling( float fDel )
{
	if( m_spBox )
		m_spBox->SetScale( fDel );
}

void	BoundBox::RotationX( float fAngle )
{
	NiMatrix3 kRotX;
	kRotX.MakeXRotation( fAngle );

	if( m_spBox )
		m_spBox->SetRotate( kRotX );
}

void	BoundBox::RotationY( float fAngle )
{
	NiMatrix3 kRotY;
	kRotY.MakeYRotation( fAngle );

	if( m_spBox )
		m_spBox->SetRotate( kRotY );

}

void	BoundBox::RotationZ( float fAngle )
{
	NiMatrix3 kRotZ;
	kRotZ.MakeZRotation( fAngle );

	if( m_spBox )
		m_spBox->SetRotate( kRotZ );
}


void	BoundBox::SetPosition( const NiPoint3& vPos )
{
	if( m_spBox )
		m_spBox->SetTranslate( vPos );
}

void	BoundBox::ResetMinMax()
{
	m_vMin.x = m_vMin.y = m_vMin.z = FLT_MAX;
	m_vMax.x = m_vMax.y = m_vMax.z = -FLT_MAX;
}