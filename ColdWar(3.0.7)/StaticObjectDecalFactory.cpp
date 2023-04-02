#include "main.h"
#include "GameApp.h"
#include "StaticObjectDecalFactory.h"


//-----------------------------------------------------------------------------
//
//	스태틱 오브젝트의 경우 면을 자르는 수행을 하면 연산속도에
//	문제가 있어서, 오로지 노멀을 이용해 면을 자르는 작업없이 Attach 시킨다.
//	
//	Decal은 오브젝트 종류에 따라 데칼의 종류를 선택하여 Clon하여  픽킹된
//	위치를 이용하여 해당 Tile의 DecalAtChunk에 Attach 시킨다.
//
//------------------------------------------------------------------------------




StaticObjectDecalFactory::StaticObjectDecalFactory()
{

	m_spAlpha = NiNew NiAlphaProperty();
	m_spAlpha->SetAlphaBlending(true);
	m_spAlpha->SetAlphaTesting(true);
	m_spAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	m_spAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	m_spAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	m_spMaterial = NiNew NiMaterialProperty;
	m_spMaterial->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	m_spMaterial->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
	m_spMaterial->SetShineness(10.0f);

	m_mapKindOfDecals.RemoveAll();

	m_kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;

	Init();
}

StaticObjectDecalFactory::~StaticObjectDecalFactory()
{

	if( !m_mapKindOfDecals.IsEmpty() )
	{
		m_mapKindOfDecals.RemoveAll();
	}

	if( m_spAlpha ) m_spAlpha = 0;
	if( m_spMaterial ) m_spMaterial = 0;

}

BOOL	StaticObjectDecalFactory::Init()
{
//	AddDecalShapeAtMap( BRICK_DECAL, NiFixedString( "Decal.tga" ) );
//	AddDecalShapeAtMap( WOOD_DECAL, NiFixedString( "Decal1.tga" ) );


	float fSize = 0.075f;

	NiPoint3 *pVtx = NiNew NiPoint3[ 4 ];
	NiPoint3 *pNor = NiNew NiPoint3[ 4 ];
	NiPoint2 *pUV  = NiNew NiPoint2[ 4 ];
	NiColorA *pkColor	= NiNew NiColorA[ 4 ];

	unsigned short *pUsTriIndex	= NiAlloc( unsigned short, 6 );

	pVtx[ 0 ] = NiPoint3(  -fSize, 0.0f, -fSize );
	pVtx[ 1 ] = NiPoint3(   fSize, 0.0f, -fSize );
	pVtx[ 2 ] = NiPoint3(  -fSize, 0.0f,  fSize );
	pVtx[ 3 ] = NiPoint3(   fSize, 0.0f,  fSize );

	pNor[ 0 ] = NiPoint3(  0.0f, -1.0f, 0.0f );
	pNor[ 1 ] = NiPoint3(  0.0f, -1.0f, 0.0f );
	pNor[ 2 ] = NiPoint3(  0.0f, -1.0f, 0.0f );
	pNor[ 3 ] = NiPoint3(  0.0f, -1.0f, 0.0f );

	pUV[ 0 ] = NiPoint2( 0.0f, 1.0f );
	pUV[ 1 ] = NiPoint2( 1.0f, 1.0f );
	pUV[ 2 ] = NiPoint2( 0.0f, 0.0f );
	pUV[ 3 ] = NiPoint2( 1.0f, 0.0f );

	// 버텍스 컬러
	pkColor[ 0 ] = NiColorA( 1.0f, 1.0f, 1.0f, 0.0f );
	pkColor[ 1 ] = NiColorA( 1.0f, 1.0f, 1.0f, 0.0f );
	pkColor[ 2 ] = NiColorA( 1.0f, 1.0f, 1.0f, 0.0f );
	pkColor[ 3 ] = NiColorA( 1.0f, 1.0f, 1.0f, 0.0f );
	//		2━━3
	//		┃  ┃
	//		┃  ┃
	//		0━━1
	// 와인딩 오더 : 반시계 방향
	// 3 2 0 ,      1 3 0
	pUsTriIndex[ 0 ] = 3; pUsTriIndex[ 1 ] = 2; pUsTriIndex[ 2 ] = 0;
	pUsTriIndex[ 3 ] = 1; pUsTriIndex[ 4 ] = 3; pUsTriIndex[ 5 ] = 0;

	NiTriShapePtr spDecalShape = NiNew NiTriShape( 4, pVtx, pNor, pkColor, pUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex);

	if( spDecalShape )
	{
		CGameApp::SetMediaPath("Data/Object/Decal/");

		NiTexturingProperty *m_spTexture = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename( "Decal.tga" ) );
		m_spTexture->SetBaseFilterMode( NiTexturingProperty::FILTER_TRILERP );
		m_spTexture->SetBaseClampMode( NiTexturingProperty::CLAMP_MAX_MODES );
		m_spTexture->SetApplyMode( NiTexturingProperty::APPLY_MODULATE );

		NiAlphaProperty *m_spAlpha = NiNew NiAlphaProperty();
		m_spAlpha->SetAlphaBlending(true);
		m_spAlpha->SetAlphaTesting(true);
		m_spAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
		m_spAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
		m_spAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

		NiMaterialProperty *m_spMaterial = NiNew NiMaterialProperty;
		m_spMaterial->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
		m_spMaterial->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
		m_spMaterial->SetShineness(10.0f);

		spDecalShape->AttachProperty( m_spTexture );
		spDecalShape->AttachProperty( m_spAlpha );
		spDecalShape->AttachProperty( m_spMaterial );

		spDecalShape->Update( 0.0f );
		spDecalShape->UpdateProperties();

		m_mapKindOfDecals.SetAt( BRICK_DECAL, spDecalShape );
	}


	return TRUE;
}

BOOL	StaticObjectDecalFactory::AddDecalShapeAtMap( BYTE nType, const NiFixedString& strDecalName )
{

	NiStream kStream;
	NiTriShapePtr spDecalObject;

	CGameApp::SetMediaPath( "./Data/Object/Decal/" );

	// Load in the scenegraph for our world...
	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename("Decal.tga") );

	if (!bSuccess)
	{
		NiMessageBox("StaticObjectDecalFactory::AddDecalShapeAtMap() Nif file could not be loaded!", "NIF Error");
		return FALSE;
	}

	int iCount = kStream.GetObjectCount();
	assert( iCount > 0 );

	if( NiIsKindOf( NiTriShape, kStream.GetObjectAt(0) ) )
	{
		spDecalObject = (NiTriShape*)kStream.GetObjectAt(0);

		return TRUE;
	}
	else if( NiIsKindOf( NiNode, kStream.GetObjectAt(0) ) )
	{
		NiNode *pObject = (NiNode*)kStream.GetObjectAt(0);
		
		for( unsigned int ui = 0; ui < pObject->GetChildCount(); ui++ )
		{
			if( NiIsKindOf( NiTriShape, pObject->GetAt( ui ) ) )
			{

				//--------------- Clon 하여 생성한다. -----------------

				spDecalObject = (NiTriShape*)pObject->GetAt( ui );

				NiAlphaPropertyPtr		spAlpha		= (NiAlphaProperty*)m_spAlpha->CreateClone(m_kCloning);
				NiMaterialPropertyPtr	spMaterial	= (NiMaterialProperty*)m_spAlpha->CreateClone(m_kCloning);

				spDecalObject->AttachProperty( spAlpha );
				spDecalObject->AttachProperty( spMaterial );

				spDecalObject->Update( 0.0f );
				spDecalObject->UpdateProperties();
			
				m_mapKindOfDecals.SetAt( nType, spDecalObject );
				
				//-------------------------------------------------------

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	StaticObjectDecalFactory::GenerateDecal(  const NiPoint3 *vPos, const NiPoint3 *normal, BYTE nDecalType, DecalAtChunk *pDecalAtChunk )
{
	if( m_mapKindOfDecals.IsEmpty() )
		return FALSE;

	NiTriShapePtr spDecalShape;
	NiTriShape *spCloneDecalShape;

	m_mapKindOfDecals.GetAt( nDecalType, spDecalShape );

	if( spDecalShape )
	{
		NiPoint3 &vUnitY = -NiPoint3::UNIT_Y;

		NiPoint3 &cross = normal->Cross( vUnitY );
		cross.Unitize();

		// 두 노멀 사이의 축 벡터 구하여 사잇각 구하기
		float fAngle = NiACos( vUnitY.Dot( *normal ) / (NiAbs(normal->Length()) * NiAbs(-NiPoint3::UNIT_Y.Length())) );

		NiMatrix3 rot;
		rot.MakeRotation( fAngle, cross );

		spCloneDecalShape = (NiTriShape*)spDecalShape->Clone();

		spCloneDecalShape->SetTranslate( *vPos + (*normal * fOffsetDitance) );
		spCloneDecalShape->SetRotate( rot );

		NiAlphaProperty *pAlpha = (NiAlphaProperty*)spCloneDecalShape->GetProperty( NiProperty::ALPHA );

		if( !pAlpha )
		{
			pAlpha = NiNew NiAlphaProperty();

			pAlpha->SetAlphaBlending(true);
			pAlpha->SetAlphaTesting(true);
			pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
			pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
			pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

			spCloneDecalShape->AttachProperty( pAlpha );
		}

		NiMaterialProperty *pMat = (NiMaterialProperty*)spCloneDecalShape->GetProperty( NiProperty::MATERIAL );

		if( !pMat )
		{
			pMat = NiNew NiMaterialProperty();

			pMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
			pMat->SetAlpha(1.0f);//----------->이 안의 값을 조절해서 서서히 사라지게 함.
			pMat->SetShineness(10.0f);

			spCloneDecalShape->AttachProperty( pMat );
		}

		spCloneDecalShape->Update(0.0f);
		spCloneDecalShape->UpdateProperties();
		
		if( pDecalAtChunk->AddDecal( spCloneDecalShape ) )
			return TRUE;

	}


	return FALSE;
}


