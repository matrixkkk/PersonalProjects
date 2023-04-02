#include "main.h"
#include "SkyBox.h"
#include "GameApp.h"



SkyBox::SkyBox()
{
	// 카메라 회전행렬
	NiMatrix3 kRotX, kRotZ;
	kRotX.MakeXRotation( -NI_HALF_PI );
	kRotZ.MakeZRotation( NI_HALF_PI );
	
	m_matCameraRot = kRotZ * kRotX;

	// 버턱스 컬러 설정 밝기
	m_Color = NiColorA( 1.0f, 1.0f, 1.0f, 1.0f );

	m_spSkyNode		= NULL;
//	m_strFileName	= _T("");
	m_bDraw			= FALSE;

	m_vecSkyBoxTex.clear();

}

SkyBox::SkyBox( const NiFixedString& fileName )
{
	// 카메라 회전행렬
	NiMatrix3 kRotX, kRotZ;
	kRotX.MakeXRotation( -NI_HALF_PI );
	kRotZ.MakeZRotation( NI_HALF_PI );

	m_matCameraRot = kRotZ * kRotX;

	// 버턱스 컬러 설정 밝기
	m_Color = NiColorA( 1.0f, 1.0f, 1.0f, 1.0f );

	m_pCamera		= NULL;
	m_strFileName	= fileName;
	m_bDraw			= FALSE;

	m_vecSkyBoxTex.clear();

}

SkyBox::~SkyBox()
{
	ReleaseObject();
}


BOOL SkyBox::UpdateObject( float fElapsedTime )
{
	if( m_pCamera )
	{
		m_spSkyNode->SetTranslate( m_matCameraRot * m_pCamera->GetTranslate() );
		m_spSkyNode->Update( 0.0f );
	}

	return TRUE;
}

BOOL SkyBox::ReleaseObject()
{
	if( m_spSkyNode ) m_spSkyNode = 0;

	return TRUE;
}

void SkyBox::SetAppCulled( bool bCull )
{
	if( m_spSkyNode )
		m_spSkyNode->SetAppCulled( bCull );
}

void SkyBox::SetCamera( NiCamera* pCamera )
{
	m_pCamera = pCamera;
}

void SkyBox::SetColor( const NiColorA& color )
{

}

bool SkyBox::Generate( const NiFixedString& strTexFileName, const NiPoint3& vExtents )
{
	std::string filepath = "Data/SkyBox/";
	std::string _strTexFileName( strTexFileName );
	std::string fileNewPath = filepath + _strTexFileName + "/";
//	NiFixedString &fileNamePath = fileNewPath.c_str();

	CGameApp::SetMediaPath( fileNewPath.c_str() );

	m_spSkyNode = NiTCreate<NiNode>();
	m_spSkyNode->SetName("SkyBox");

	m_vExtents = vExtents;
	m_strFileName = strTexFileName;

	// 타일 기하 정보 생성
	NiPoint3 *pBottomVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pBottomTexUV		= NiNew NiPoint2[ 4 ];
	NiColorA* pkBottomColors	= NiNew NiColorA[ 4 ];	

	unsigned short *pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	//		3━━2
	//		┃  ┃
	//		┃  ┃
	//		0━━1
	// 와인딩 오더 : 반시계 방향

	//----------------------------------------------------------------
	//								BOTTOM
	//----------------------------------------------------------------
	pBottomVtx[ 0 ] = NiPoint3( -vExtents.x, -vExtents.y, -vExtents.z );
	pBottomVtx[ 1 ] = NiPoint3(  vExtents.x, -vExtents.y, -vExtents.z );
	pBottomVtx[ 2 ] = NiPoint3(  vExtents.x,  vExtents.y, -vExtents.z );
	pBottomVtx[ 3 ] = NiPoint3( -vExtents.x,  vExtents.y, -vExtents.z );

	pBottomTexUV[ 0 ] = NiPoint2(  0.0f,  1.0f);
    pBottomTexUV[ 1 ] = NiPoint2(  1.0f,  1.0f);
    pBottomTexUV[ 2 ] = NiPoint2(  1.0f,  0.0f);
    pBottomTexUV[ 3 ] = NiPoint2(  0.0f,  0.0f);

	pkBottomColors[0] = m_Color;
    pkBottomColors[1] = m_Color;
    pkBottomColors[2] = m_Color;
    pkBottomColors[3] = m_Color;


	pUsTriIndex[0] = 0;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 3;
    pUsTriIndex[3] = 1;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 3;

	NiTriShape *spBottom = NiNew NiTriShape( 4, pBottomVtx, NULL, pkBottomColors, pBottomTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex);
	spBottom->CalculateNormals();
    spBottom->SetName("Bottom");
    m_spSkyNode->AttachChild(spBottom);

	NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("bottom.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spBottom->AttachProperty(pkTexProp);

	
	//----------------------------------------------------------------
	//								TOP
	//----------------------------------------------------------------

	// 타일 기하 정보 생성
	NiPoint3 *pTopVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pTopTexUV		= NiNew NiPoint2[ 4 ];
	NiColorA* pkTopColors	= NiNew NiColorA[ 4 ];	

	pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );

	// Top
	pTopVtx[ 0 ] = NiPoint3( -vExtents.x,  vExtents.y, vExtents.z );
	pTopVtx[ 1 ] = NiPoint3(  vExtents.x,  vExtents.y, vExtents.z );
	pTopVtx[ 2 ] = NiPoint3(  vExtents.x, -vExtents.y, vExtents.z );
	pTopVtx[ 3 ] = NiPoint3( -vExtents.x, -vExtents.y, vExtents.z );

	pTopTexUV[ 0 ] = NiPoint2(  0.0f,  1.0f);
    pTopTexUV[ 1 ] = NiPoint2(  1.0f,  1.0f);
    pTopTexUV[ 2 ] = NiPoint2(  1.0f,  0.0f);
    pTopTexUV[ 3 ] = NiPoint2(  0.0f,  0.0f);

	pkTopColors[0] = m_Color;
    pkTopColors[1] = m_Color;
    pkTopColors[2] = m_Color;
    pkTopColors[3] = m_Color;

	pUsTriIndex[0] = 0;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 3;
    pUsTriIndex[3] = 1;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 3;

	NiTriShape *spTop = NiNew NiTriShape( 4, pTopVtx, NULL, pkTopColors, pTopTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex);
	spTop->CalculateNormals();
    spTop->SetName("Top");
    m_spSkyNode->AttachChild( spTop );

	pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("top.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spTop->AttachProperty(pkTexProp);

	//----------------------------------------------------------------
	//								LEFT
	//----------------------------------------------------------------

	// 타일 기하 정보 생성
	NiPoint3 *pLeftVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pLeftTexUV	= NiNew NiPoint2[ 4 ];
	NiColorA* pkLeftColors	= NiNew NiColorA[ 4 ];	

	pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	pLeftVtx[ 0 ] = NiPoint3( -vExtents.x, -vExtents.y, -vExtents.z );
	pLeftVtx[ 1 ] = NiPoint3( -vExtents.x,  vExtents.y, -vExtents.z );
	pLeftVtx[ 2 ] = NiPoint3( -vExtents.x,  vExtents.y,  vExtents.z );
	pLeftVtx[ 3 ] = NiPoint3( -vExtents.x, -vExtents.y,  vExtents.z );

	pLeftTexUV[ 0 ] = NiPoint2(  0.0f,  1.0f);
    pLeftTexUV[ 1 ] = NiPoint2(  1.0f,  1.0f);
    pLeftTexUV[ 2 ] = NiPoint2(  1.0f,  0.0f);
    pLeftTexUV[ 3 ] = NiPoint2(  0.0f,  0.0f);

	pkLeftColors[0] = m_Color;
    pkLeftColors[1] = m_Color;
    pkLeftColors[2] = m_Color;
    pkLeftColors[3] = m_Color;

	pUsTriIndex[0] = 0;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 3;
    pUsTriIndex[3] = 1;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 3;

	NiTriShape *spLeft = NiNew NiTriShape( 4, pLeftVtx, NULL, pkLeftColors, pLeftTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex );
	spLeft->CalculateNormals();
    spLeft->SetName("Left");
    m_spSkyNode->AttachChild( spLeft );

	pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("left.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spLeft->AttachProperty(pkTexProp);

	//----------------------------------------------------------------
	//								RIGHT
	//----------------------------------------------------------------

	// 타일 기하 정보 생성
	NiPoint3 *pRightVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pRightTexUV	= NiNew NiPoint2[ 4 ];
	NiColorA* pkRightColors	= NiNew NiColorA[ 4 ];	

	pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	pRightVtx[ 0 ] = NiPoint3( vExtents.x,  vExtents.y, -vExtents.z );
	pRightVtx[ 1 ] = NiPoint3( vExtents.x, -vExtents.y, -vExtents.z );
	pRightVtx[ 2 ] = NiPoint3( vExtents.x, -vExtents.y,  vExtents.z );
	pRightVtx[ 3 ] = NiPoint3( vExtents.x,  vExtents.y,  vExtents.z );

	pRightTexUV[ 0 ] = NiPoint2(  0.0f,  1.0f);
    pRightTexUV[ 1 ] = NiPoint2(  1.0f,  1.0f);
    pRightTexUV[ 2 ] = NiPoint2(  1.0f,  0.0f);
    pRightTexUV[ 3 ] = NiPoint2(  0.0f,  0.0f);

	pkRightColors[0] = m_Color;
    pkRightColors[1] = m_Color;
    pkRightColors[2] = m_Color;
    pkRightColors[3] = m_Color;

	pUsTriIndex[0] = 0;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 3;
    pUsTriIndex[3] = 1;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 3;

	NiTriShape *spRight = NiNew NiTriShape( 4, pRightVtx, NULL, pkRightColors, pRightTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex);
	spRight->CalculateNormals();
    spRight->SetName("Right");
    m_spSkyNode->AttachChild( spRight );

	pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("right.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spRight->AttachProperty(pkTexProp);



	//----------------------------------------------------------------
	//								FRONT
	//----------------------------------------------------------------

	// 타일 기하 정보 생성
	NiPoint3 *pFrontVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pFrontTexUV	= NiNew NiPoint2[ 4 ];
	NiColorA* pkFrontColors	= NiNew NiColorA[ 4 ];	

	pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	pFrontVtx[ 0 ] = NiPoint3( -vExtents.x, -vExtents.y, -vExtents.z );
	pFrontVtx[ 1 ] = NiPoint3(  vExtents.x, -vExtents.y, -vExtents.z );
	pFrontVtx[ 2 ] = NiPoint3(  vExtents.x, -vExtents.y,  vExtents.z );
	pFrontVtx[ 3 ] = NiPoint3( -vExtents.x, -vExtents.y,  vExtents.z );
	
	pFrontTexUV[ 0 ] = NiPoint2(  1.0f,  1.0f);
    pFrontTexUV[ 1 ] = NiPoint2(  0.0f,  1.0f);
    pFrontTexUV[ 2 ] = NiPoint2(  0.0f,  0.0f);
    pFrontTexUV[ 3 ] = NiPoint2(  1.0f,  0.0f);

	pkFrontColors[0] = m_Color;
    pkFrontColors[1] = m_Color;
    pkFrontColors[2] = m_Color;
    pkFrontColors[3] = m_Color;

	pUsTriIndex[0] = 3;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 0;
    pUsTriIndex[3] = 3;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 1;

	NiTriShape *spFront = NiNew NiTriShape( 4, pFrontVtx, NULL, pkFrontColors, pFrontTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex );
	spFront->CalculateNormals();
    spFront->SetName("Front");
    m_spSkyNode->AttachChild( spFront );

	pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("front.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spFront->AttachProperty(pkTexProp);

	//----------------------------------------------------------------
	//								BACK
	//----------------------------------------------------------------

	// 타일 기하 정보 생성
	NiPoint3 *pBackVtx		= NiNew NiPoint3[ 4 ];	
	NiPoint2 *pBackTexUV	= NiNew NiPoint2[ 4 ];
	NiColorA* pkBackColors	= NiNew NiColorA[ 4 ];	

	pUsTriIndex	= NiAlloc( unsigned short, 6 );
	::ZeroMemory( pUsTriIndex, sizeof(pUsTriIndex) );
	
	pBackVtx[ 0 ] = NiPoint3( -vExtents.x, vExtents.y, -vExtents.z );
	pBackVtx[ 1 ] = NiPoint3(  vExtents.x, vExtents.y, -vExtents.z );
	pBackVtx[ 2 ] = NiPoint3(  vExtents.x, vExtents.y,  vExtents.z );
	pBackVtx[ 3 ] = NiPoint3( -vExtents.x, vExtents.y,  vExtents.z );

	pBackTexUV[ 0 ] = NiPoint2(  0.0f,  1.0f);
    pBackTexUV[ 1 ] = NiPoint2(  1.0f,  1.0f);
    pBackTexUV[ 2 ] = NiPoint2(  1.0f,  0.0f);
    pBackTexUV[ 3 ] = NiPoint2(  0.0f,  0.0f);

	pkBackColors[0] = m_Color;
    pkBackColors[1] = m_Color;
    pkBackColors[2] = m_Color;
    pkBackColors[3] = m_Color;

	pUsTriIndex[0] = 0;
    pUsTriIndex[1] = 1;
    pUsTriIndex[2] = 3;
    pUsTriIndex[3] = 1;
    pUsTriIndex[4] = 2;
    pUsTriIndex[5] = 3;

	NiTriShape *spBack = NiNew NiTriShape( 4, pBackVtx, NULL, pkBackColors, pBackTexUV, 1, NiGeometryData::NBT_METHOD_NONE, 2, pUsTriIndex );
	spBack->CalculateNormals();
    spBack->SetName("Back");
	NiColorA *pTmpColor = spBack->GetColors();
	

    m_spSkyNode->AttachChild( spBack );


	pkTexProp = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename("back.bmp") );
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    spBack->AttachProperty(pkTexProp);

	NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty;
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spSkyNode->AttachProperty(pkVCProp);

	m_spSkyNode->Update( 0.0f );
	m_spSkyNode->UpdateNodeBound();
	m_spSkyNode->UpdateEffects();
	m_spSkyNode->UpdateProperties();
	

	return true;

}

void SkyBox::SetPosition( const NiPoint3& _vPosition )
{
	if( m_spSkyNode )
	{
		m_spSkyNode->SetTranslate( _vPosition );
		m_spSkyNode->Update(0.0f);
	}
}

/*
void	SkyBox::SaveData( CArchive& ar )
{
	NiPoint3 vCurrentPos = m_spSkyNode->GetTranslate();
	
	//크기
	ar << m_vExtents.x;
	ar << m_vExtents.y;
	ar << m_vExtents.z;

	//위치
	ar << vCurrentPos.x;
	ar << vCurrentPos.y;
	ar << vCurrentPos.z;
}

void	SkyBox::SaveNameData( std::ofstream& fwrite )
{
	fwrite << CW2A( m_strFileName ) << "\n";
}
*/




