#include "main.h"
#include "BulletDecalFactory.h"
#include "GameApp.h"

// ----------------------------------------------------------------------------------------------------------
//
//
//		��ŷ ������, ����� �̿��Ͽ� �ش���ġ�� ��Į �������� �����ϴ� Factory ������ Ŭ����
//
//
//
//
// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------- //
// CDecal Function
// ----------------------------------------------- //
BulletDecalFactory::BulletDecalFactory()
{
	m_width			= 0.0f;		// ��Į width
	m_height		= 0.0f;		// ��Į height
	m_fdepth		= 0.0f;		// ��Į depth

	CGameApp::SetMediaPath("Data/Object/Decal/");

	m_spTexture = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename( "Decal.tga" ) );
	m_spTexture->SetBaseFilterMode( NiTexturingProperty::FILTER_TRILERP );
	m_spTexture->SetBaseClampMode( NiTexturingProperty::CLAMP_MAX_MODES );
	m_spTexture->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );
}

BulletDecalFactory::~BulletDecalFactory()
{
	if( m_spTexture ) m_spTexture = 0;
}

void BulletDecalFactory::Init(const NiPoint3* center, const NiPoint3* normal, const NiPoint3* tangent, const float &width, const float &height, const float &depth )
{
	///-- center ��ŷ�� �� normal ��ŷ�� �ﰢ���� �̷�� ��ֺ���, tangent �߽ɿ��� �������� ����
	///-- width, height, depth - ��Į�� �ʺ�� ���� ���� - �̰��� ������ ���������ϴ�.		
	m_decalCenter	= *center;
	m_decalNormal	= *normal;
	m_tangent		= *tangent;

	// Begin with empty mesh
	m_width		= width;
	m_height	= height;
	m_fdepth	= depth;

	m_decalNormal.Unitize();
	m_tangent.Unitize();

	m_binormal = normal->Cross( m_tangent );
	m_binormal.Unitize();
	// Calculate boundary planes
	///-- �ٿ�� ������ ���Ѵ�. �տ� ����Ҵ� ����� �������� ��� �ڿ��� d�� 
	float d			= m_decalCenter.Dot( m_tangent );

	m_leftPlane		= NiPlane(m_tangent,  -(width * 0.5F -d));
	m_rightPlane	= NiPlane(-m_tangent,  -(width * 0.5F +d));

	d				= m_decalCenter.Dot(m_binormal);
	m_bottomPlane	= NiPlane(m_binormal,  -(height * 0.5F -d));
	m_topPlane		= NiPlane(-m_binormal, -(height * 0.5F +d));

	///-- ���ܸ�� �Ĵܸ��� ����
	d				= m_decalCenter.Dot(m_decalNormal);
	m_frontPlane	= NiPlane(-m_decalNormal, -(depth + d));
	m_backPlane		= NiPlane(m_decalNormal, -(depth - d));	

	m_vecDecalVertices.clear();
	m_vecDecalIndices.clear();
	
}

void BulletDecalFactory::ClipMesh( std::vector<DecalTriIndex>& vecDecalTri, const NiPoint3 *vertex, const NiPoint3 *normal )
{
	///-- triangleCount - Clip�� �޽��� �� �ﰢ������
	///-- triangle - ���� �ε����� �� ������
	///-- vertex - ���������� �� ������
	///-- normal - �����븻�� �� ������	
	NiPoint3		newVertex[9];
	NiPoint3		newNormal[9];

	// Clip one triangle at a time
	// �ѹ��� �ϳ��� �������� Clip�Ѵ�.
	std::vector< DecalTriIndex >::iterator iterTri;
	::ZeroMemory( newVertex, sizeof(newVertex) );
	::ZeroMemory( newNormal, sizeof(newNormal) );

	for( iterTri = vecDecalTri.begin(); iterTri != vecDecalTri.end(); ++iterTri )
	{
		unsigned short &i1 = iterTri->index[0];		//		3����4
		unsigned short &i2 = iterTri->index[1];		//		��  ��
		unsigned short &i3 = iterTri->index[2];		//		��  ��
													//		0����1
		const NiPoint3 & v1 = vertex[ i1 ];			// ���ε� ���� : �ݽð� ����
		const NiPoint3 & v2 = vertex[ i2 ];			// 4 3 0 ,      1 4 0
		const NiPoint3 & v3 = vertex[ i3 ];
													// 3 -- 4	//      4
													// | /		//   /  |
													// 0		// 0 -- 1			
		NiPoint3 cross = (v2 - v1).UnitCross( (v3 - v1) );
//		cross.Unitize();	

		/// ���� �������� ������ ��Į�� ������ ���Ѵ�.
		/// ���⼭ �ɷ����� �������� ��Į�� ǥ����� �ݴ����� �ٶ󺸰� �ִ� �͵��̴�.
		/// Cos ���� 0.25F ���� �۴ٴ°� -76�� ~ 76�� ���̿� ������ �͵鸸 ���ϰڴٴ� ���̴�.
		/// decalEpsilon ���� ���Ƿ� �����Ҽ� �ִ�.
		if( m_decalNormal.Dot( cross ) > DECALEPSILON )
		{
			newVertex[0] = v1;
			newVertex[1] = v2;
			newVertex[2] = v3;
			
			newNormal[0] = normal[i1];
			newNormal[1] = normal[i2];
			newNormal[2] = normal[i3];
			
			///-- ��鿡 ���� �������� �����ϴ� �Լ��� ȣ��
			long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
			if ((count != 0) && (!AddPolygon(count, newVertex, newNormal)))
			{
				break;
			}
		}
	}

	// Assign texture mapping coordinates
	// �ؽ��� ���� ��ǥ���� ����
	float one_over_w = 1.0F / m_width;
	float one_over_h = 1.0F / m_height;

	std::vector< DECALVERTEX >::iterator iterVtx;
	for( iterVtx = m_vecDecalVertices.begin(); iterVtx != m_vecDecalVertices.end(); ++iterVtx )
	{
		NiPoint3 &v = m_decalCenter - iterVtx->p ;
		float s = v.Dot( m_tangent ) * one_over_w + 0.5F;
		float t = v.Dot( m_binormal) * one_over_h + 0.5F;

		iterVtx->tu = 1.0f - s;
		iterVtx->tv = t;
	}

}

unsigned int BulletDecalFactory::ClipPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal) const
{
	NiPoint3		tempVertex[9];
	NiPoint3		tempNormal[9];
	
	// Clip against all six planes
	// 6�� ���鿡 ���� Clip�Ѵ�.
	unsigned int count = ClipPolygonAgainstPlane(m_leftPlane, vertexCount, vertex, normal, tempVertex, tempNormal);

	if (count != 0)
	{
		count = ClipPolygonAgainstPlane(m_rightPlane, count, tempVertex, tempNormal, newVertex, newNormal);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(m_bottomPlane, count, newVertex, newNormal, tempVertex, tempNormal);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(m_topPlane, count, tempVertex, tempNormal, newVertex, newNormal);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(m_backPlane, count, newVertex, newNormal, tempVertex, tempNormal);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(m_frontPlane, count, tempVertex, tempNormal, newVertex, newNormal);
					}
				}
			}
		}
	}
	
	return (count);
}

bool BulletDecalFactory::AddPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal)
{
	unsigned int count = m_vecDecalVertices.size();

	if (count + vertexCount >= maxDecalVertices ) return (false);
	
	// Add polygon as a triangle fan
	DecalTriIndex stDecalTri;
	for (unsigned int i = 2; i < vertexCount; i++)				///vertexCount �� 4�̸� TriangleCount 2�� �����Ѵ�.
	{
		stDecalTri.index[0] = (unsigned short) count;			///-- 0, 0			
		stDecalTri.index[1] = (unsigned short) (count + i - 1); ///-- 1, 2  
		stDecalTri.index[2] = (unsigned short) (count + i);		///-- 2, 3

		m_vecDecalIndices.push_back( stDecalTri );
	}

	// Assign vertex colors
	float f = 255.0f / (1.0F - DECALEPSILON);

	DECALVERTEX stDecalVtx;
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		stDecalVtx.p = vertex[i];
		stDecalVtx.n = normal[i];
		stDecalVtx.n.Unitize();

		int alpha = (int)(( m_decalNormal.Dot(stDecalVtx.n) - DECALEPSILON) * f);

		///-- �ش� ���İ��� ������ ���� [decalEpsilon, 1]�� ���İ� ���� [0, 255]�� ��� ��Ų���̴�.
		stDecalVtx.diffuse = (float)D3DCOLOR_ARGB( (alpha > 0) ? alpha : 0 , 255 , 255 , 255 );

		m_vecDecalVertices.push_back( stDecalVtx );
	}
	
	return true;
}
	




unsigned int BulletDecalFactory::ClipPolygonAgainstPlane(const NiPlane &plane, unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal )
{
	bool	negative[10];
	
	// Classify vertices
	// �������� �з��Ѵ�.
	int negativeCount = 0;
	bool neg = false;

	for ( unsigned int a = 0; a < vertexCount; a++)
	{
		///-- ���� ������ �������� ������ ������ ������ ����� ���ʿ� ���ϰԵȴ�.
		///-- ���⼭ ������ �����°��� �ٿ�� ������ ��� ���̶�� ���̴�.
		///-- �߽ɰ��� (0.0f, 0.0f, 0.0f)�� �ؼ� �� �ٿ�� ����� ���غ��� �˼��ִ�.
		neg = NiPlane::NEGATIVE_SIDE == plane.WhichSide(vertex[a]) ? true : false;

		negative[a] = neg;
		negativeCount += neg;
	}
	
	// Discard this polygon if it's completely culled
	// �Ϻ��� �ø��Ǿ��ٸ� �ش��������� �����Ѵ�.
	if (negativeCount == vertexCount) 
	{
		return (0);
	}
	
	unsigned int count = 0;
	for (unsigned int b = 0; b < vertexCount; b++)
	{
		// c is the index of the previous vertex
		// c �� ���� ���ؽ��� �ε����̴�.
		int c = (b != 0) ? b - 1 : vertexCount - 1;
		
		if (negative[b])	// �ش���ؽ��� �ٿ�� ������ ��� ��..
		{
			if (!negative[c])  //�������ؽ��� �ٿ�� ������ �� ������
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const NiPoint3 & v1 = vertex[c];
				const NiPoint3 & v2 = vertex[b];
				const NiPoint3 &planeNormal = plane.GetNormal();

				///-- DotProduct(plane, v1) - v1���� ������ ��������
				///-- (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				///-- v1���� v2 ��� ��������
				float t = plane.Distance(v1) / (planeNormal.x * (v1.x - v2.x) + planeNormal.y * (v1.y - v2.y) + planeNormal.z * (v1.z - v2.z));
				///-- ������ �浹����
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				//�浹���� ��������Ʈ�� �ִ´�.				
				const NiPoint3 & n1 = normal[c];
				const NiPoint3 & n2 = normal[b];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;

				newNormal[count].Unitize();

				count++;
			}
		}
		else	//�ش� ���ؽ��� �ٿ�� ������
		{
			if (negative[c])	//�������ؽ��� �ٿ�� ������ ��� ��...
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const NiPoint3 & v1 = vertex[b];
				const NiPoint3 & v2 = vertex[c];
				const NiPoint3 &planeNormal = plane.GetNormal();

				float t = plane.Distance(v1) / (planeNormal.x * (v1.x - v2.x) + planeNormal.y * (v1.y - v2.y) + planeNormal.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				//�浹���� ��������Ʈ�� �ִ´�.
				
				const NiPoint3 & n1 = normal[b];
				const NiPoint3 & n2 = normal[c];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				newNormal[count].Unitize();

				count++;
			}

			//�ش����� ��������Ʈ�� �ִ´�.
			// Include current vertex
			newVertex[count] = vertex[b];
			newNormal[count] = normal[b];
			count++;
		}
	}
	
	// Return number of vertices in clipped polygon
	return (count);
}

BOOL	BulletDecalFactory::GenerateDecal( DecalAtChunk *pDecalChunk, const NiPoint3 &normal )
{
	int nVtxCount = (int)m_vecDecalVertices.size();
	int nTriangels = (int)m_vecDecalIndices.size();

	if( !nVtxCount || !nTriangels )
		return FALSE;

	int count = 0;
	NiTriShape *pDecalShape = NULL;
	NiTriShapePtr spDecalShape;
	std::vector<DECALVERTEX>::iterator iterVtx;
	std::vector<DecalTriIndex>::iterator iterIndex;

	// Ÿ�� ���� ���� ����
	NiPoint3 *pVPosition	= NiNew NiPoint3[ nVtxCount ];
	NiPoint3 *pVNormal		= NiNew NiPoint3[ nVtxCount ];
	NiPoint2 *pUV			= NiNew NiPoint2[ nVtxCount ];
	NiColorA *pkColor		= NiNew NiColorA[ nVtxCount ];

	unsigned short *pUsTriIndex	= NiAlloc( unsigned short, nTriangels * 3 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	if( !pVPosition || !pUsTriIndex || !pUV || !pkColor || !pVNormal )
	{
		goto Exit;
	}

	for( iterVtx = m_vecDecalVertices.begin(); iterVtx != m_vecDecalVertices.end(); ++iterVtx )
	{
		DECALVERTEX &vtxInfo = (*iterVtx);

		pVPosition[ count ] = vtxInfo.p;
		pVNormal[ count ]	= vtxInfo.n;
		pUV[ count ].x		= vtxInfo.tu;
		pUV[ count ].y		= vtxInfo.tv;
		pkColor[ count ]	= vtxInfo.diffuse;
		count++;	
	}
	
	count = 0;
	
	for( iterIndex = m_vecDecalIndices.begin(); iterIndex != m_vecDecalIndices.end(); ++iterIndex )
	{
		DecalTriIndex &triIndex = (*iterIndex);

		pUsTriIndex[ count++ ] =  triIndex.index[ 0 ];
		pUsTriIndex[ count++ ] =  triIndex.index[ 1 ];
		pUsTriIndex[ count++ ] =  triIndex.index[ 2 ];

	}	

	spDecalShape = NiNew NiTriShape( nVtxCount, pVPosition, pVNormal, NULL, pUV, 1, NiGeometryData::NBT_METHOD_NONE, nTriangels, /*NULL*/ pUsTriIndex);	

	if( !spDecalShape )
	{
		goto Exit;
	}

	// �ؽ���
	NiTexturingProperty *pTexture = (NiTexturingProperty*)m_spTexture->CreateClone(m_kCloning);

	// ���� ����
	NiAlphaProperty *pAlpha = NiNew NiAlphaProperty();
	pAlpha->SetAlphaBlending(true);
	pAlpha->SetAlphaTesting(true);
	pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	// ���� ����
	NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
	pkMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	pkMat->SetAlpha(1.0f);//----------->�� ���� ���� �����ؼ� ������ ������� ��.
	pkMat->SetShineness(10.0f);

	spDecalShape->AttachProperty( pAlpha );
	spDecalShape->AttachProperty( pkMat );
	spDecalShape->AttachProperty( pTexture );
	spDecalShape->SetTranslate( normal * fOffsetDitance );

	spDecalShape->Update(0.0f);
	spDecalShape->UpdateProperties();

	// �ش� ûũ�� ����
	if( pDecalChunk->AddDecal( spDecalShape ) )
		return TRUE;

	return FALSE;

Exit:

	if( pVPosition )	NiDelete[] pVPosition;
	if( pVNormal )		NiDelete[] pVNormal;
	if( pUV )			NiDelete[] pUV;
	if( pkColor )		NiDelete[] pkColor;
	if( pUsTriIndex )	NiFree( pUsTriIndex );

	return FALSE;

}