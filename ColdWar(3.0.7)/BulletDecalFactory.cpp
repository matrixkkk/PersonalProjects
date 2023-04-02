#include "main.h"
#include "BulletDecalFactory.h"
#include "GameApp.h"

// ----------------------------------------------------------------------------------------------------------
//
//
//		픽킹 지점과, 노멀을 이용하여 해당위치에 데칼 폴리곤을 생성하는 Factory 역할의 클래스
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
	m_width			= 0.0f;		// 데칼 width
	m_height		= 0.0f;		// 데칼 height
	m_fdepth		= 0.0f;		// 데칼 depth

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
	///-- center 픽킹된 점 normal 픽킹된 삼각형을 이루는 노멀벡터, tangent 중심에서 한점과의 벡터
	///-- width, height, depth - 데칼의 너비와 높이 깊이 - 이값은 임위로 결정가능하다.		
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
	///-- 바운드 평면들을 구한다. 앞에 세요소는 평면의 수직벡터 요소 뒤에는 d값 
	float d			= m_decalCenter.Dot( m_tangent );

	m_leftPlane		= NiPlane(m_tangent,  -(width * 0.5F -d));
	m_rightPlane	= NiPlane(-m_tangent,  -(width * 0.5F +d));

	d				= m_decalCenter.Dot(m_binormal);
	m_bottomPlane	= NiPlane(m_binormal,  -(height * 0.5F -d));
	m_topPlane		= NiPlane(-m_binormal, -(height * 0.5F +d));

	///-- 전단면과 후단면을 결정
	d				= m_decalCenter.Dot(m_decalNormal);
	m_frontPlane	= NiPlane(-m_decalNormal, -(depth + d));
	m_backPlane		= NiPlane(m_decalNormal, -(depth - d));	

	m_vecDecalVertices.clear();
	m_vecDecalIndices.clear();
	
}

void BulletDecalFactory::ClipMesh( std::vector<DecalTriIndex>& vecDecalTri, const NiPoint3 *vertex, const NiPoint3 *normal )
{
	///-- triangleCount - Clip할 메쉬의 총 삼각형갯수
	///-- triangle - 구성 인덱스들 의 포인터
	///-- vertex - 구성정점들 의 포인터
	///-- normal - 구성노말들 의 포인터	
	NiPoint3		newVertex[9];
	NiPoint3		newNormal[9];

	// Clip one triangle at a time
	// 한번에 하나의 폴리곤을 Clip한다.
	std::vector< DecalTriIndex >::iterator iterTri;
	::ZeroMemory( newVertex, sizeof(newVertex) );
	::ZeroMemory( newNormal, sizeof(newNormal) );

	for( iterTri = vecDecalTri.begin(); iterTri != vecDecalTri.end(); ++iterTri )
	{
		unsigned short &i1 = iterTri->index[0];		//		3━━4
		unsigned short &i2 = iterTri->index[1];		//		┃  ┃
		unsigned short &i3 = iterTri->index[2];		//		┃  ┃
													//		0━━1
		const NiPoint3 & v1 = vertex[ i1 ];			// 와인딩 오더 : 반시계 방향
		const NiPoint3 & v2 = vertex[ i2 ];			// 4 3 0 ,      1 4 0
		const NiPoint3 & v3 = vertex[ i3 ];
													// 3 -- 4	//      4
													// | /		//   /  |
													// 0		// 0 -- 1			
		NiPoint3 cross = (v2 - v1).UnitCross( (v3 - v1) );
//		cross.Unitize();	

		/// 구성 폴리곤의 법선과 데칼의 법선을 비교한다.
		/// 여기서 걸러지는 폴리곤은 데칼의 표면과는 반대쪽을 바라보고 있는 것들이다.
		/// Cos 값이 0.25F 보다 작다는건 -76도 ~ 76도 사이에 들어오는 것들만 취하겠다는 뜻이다.
		/// decalEpsilon 값도 임의로 결정할수 있다.
		if( m_decalNormal.Dot( cross ) > DECALEPSILON )
		{
			newVertex[0] = v1;
			newVertex[1] = v2;
			newVertex[2] = v3;
			
			newNormal[0] = normal[i1];
			newNormal[1] = normal[i2];
			newNormal[2] = normal[i3];
			
			///-- 평면에 위해 폴리곤을 절단하는 함수를 호출
			long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
			if ((count != 0) && (!AddPolygon(count, newVertex, newNormal)))
			{
				break;
			}
		}
	}

	// Assign texture mapping coordinates
	// 텍스쳐 맵핑 좌표들을 지정
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
	// 6개 평면들에 대해 Clip한다.
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
	for (unsigned int i = 2; i < vertexCount; i++)				///vertexCount 가 4이면 TriangleCount 2가 증가한다.
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

		///-- 해당 알파값은 내적의 범위 [decalEpsilon, 1]을 알파값 범위 [0, 255]로 비례 시킨것이다.
		stDecalVtx.diffuse = (float)D3DCOLOR_ARGB( (alpha > 0) ? alpha : 0 , 255 , 255 , 255 );

		m_vecDecalVertices.push_back( stDecalVtx );
	}
	
	return true;
}
	




unsigned int BulletDecalFactory::ClipPolygonAgainstPlane(const NiPlane &plane, unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal )
{
	bool	negative[10];
	
	// Classify vertices
	// 정점들을 분류한다.
	int negativeCount = 0;
	bool neg = false;

	for ( unsigned int a = 0; a < vertexCount; a++)
	{
		///-- 평면과 정점을 내적시켜 음수가 나오면 정점은 평면의 안쪽에 속하게된다.
		///-- 여기서 음수가 나오는것은 바운드 영역을 벗어난 값이라는 뜻이다.
		///-- 중심값을 (0.0f, 0.0f, 0.0f)로 해서 각 바운드 평면을 구해보면 알수있다.
		neg = NiPlane::NEGATIVE_SIDE == plane.WhichSide(vertex[a]) ? true : false;

		negative[a] = neg;
		negativeCount += neg;
	}
	
	// Discard this polygon if it's completely culled
	// 완벽히 컬링되었다면 해당폴리곤은 무시한다.
	if (negativeCount == vertexCount) 
	{
		return (0);
	}
	
	unsigned int count = 0;
	for (unsigned int b = 0; b < vertexCount; b++)
	{
		// c is the index of the previous vertex
		// c 는 이전 버텍스의 인덱스이다.
		int c = (b != 0) ? b - 1 : vertexCount - 1;
		
		if (negative[b])	// 해당버텍스는 바운드 영역을 벗어난 값..
		{
			if (!negative[c])  //이전버텍스가 바운드 영역내 값 있으면
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const NiPoint3 & v1 = vertex[c];
				const NiPoint3 & v2 = vertex[b];
				const NiPoint3 &planeNormal = plane.GetNormal();

				///-- DotProduct(plane, v1) - v1에서 평면까지 내적길이
				///-- (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				///-- v1에서 v2 평면 내적길이
				float t = plane.Distance(v1) / (planeNormal.x * (v1.x - v2.x) + planeNormal.y * (v1.y - v2.y) + planeNormal.z * (v1.z - v2.z));
				///-- 평면과의 충돌지점
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				//충돌점을 정점리스트에 넣는다.				
				const NiPoint3 & n1 = normal[c];
				const NiPoint3 & n2 = normal[b];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;

				newNormal[count].Unitize();

				count++;
			}
		}
		else	//해당 버텍스가 바운드 영역내
		{
			if (negative[c])	//다음버텍스가 바운드 영역을 벗어난 값...
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const NiPoint3 & v1 = vertex[b];
				const NiPoint3 & v2 = vertex[c];
				const NiPoint3 &planeNormal = plane.GetNormal();

				float t = plane.Distance(v1) / (planeNormal.x * (v1.x - v2.x) + planeNormal.y * (v1.y - v2.y) + planeNormal.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				//충돌점을 정점리스트에 넣는다.
				
				const NiPoint3 & n1 = normal[b];
				const NiPoint3 & n2 = normal[c];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				newNormal[count].Unitize();

				count++;
			}

			//해당점도 정점리스트에 넣는다.
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

	// 타일 기하 정보 생성
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

	// 텍스쳐
	NiTexturingProperty *pTexture = (NiTexturingProperty*)m_spTexture->CreateClone(m_kCloning);

	// 알파 블랜딩
	NiAlphaProperty *pAlpha = NiNew NiAlphaProperty();
	pAlpha->SetAlphaBlending(true);
	pAlpha->SetAlphaTesting(true);
	pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	// 알파 조절
	NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
	pkMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	pkMat->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
	pkMat->SetShineness(10.0f);

	spDecalShape->AttachProperty( pAlpha );
	spDecalShape->AttachProperty( pkMat );
	spDecalShape->AttachProperty( pTexture );
	spDecalShape->SetTranslate( normal * fOffsetDitance );

	spDecalShape->Update(0.0f);
	spDecalShape->UpdateProperties();

	// 해당 청크에 삽입
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