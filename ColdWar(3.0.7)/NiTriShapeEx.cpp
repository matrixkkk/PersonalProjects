#include "main.h"
#include "NiTriShapeEx.h"


//NiImplementRTTI(NiTriShapeEx,NiTriShape);


NiTriShapeEx::NiTriShapeEx(unsigned short usVertices, NiPoint3* pkVertex,
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
    unsigned short usNumTextureSets, NiGeometryData::DataFlags eNBTMethod,
	unsigned short usTriangles, unsigned short* pusTriList) :

		NiTriShape( NiNew NiTriShapeData(usVertices, pkVertex, pkNormal,
		pkColor, pkTexture, usNumTextureSets, eNBTMethod, usTriangles, pusTriList))
{
	
	m_pGeometryBufferData	= NULL;
	m_pIB					= NULL;
	m_bOldIB				= FALSE;
	m_uiIndexCount	= 0;
	m_uiIBCount		= 0;

}



NiTriShapeEx::NiTriShapeEx(NiTriShapeData* pkModelData) : NiTriShape( pkModelData )
{

}


NiTriShapeEx::NiTriShapeEx()
{

}

NiTriShapeEx::~NiTriShapeEx()
{

}

void NiTriShapeEx::GetGeometryBufferData()
{
	if( !m_pGeometryBufferData )
	{
		m_pGeometryBufferData = (NiGeometryBufferData*)(NiTriShapeData*)GetModelData()->GetRendererData();
	}

}

void NiTriShapeEx::SetIB( D3DIndexBufferPtr pIB, unsigned int uiIndexCount, unsigned int uiIBSize )
{
	
	if( !m_pGeometryBufferData )
		return;

	((NiTriShapeData*)GetModelData())->SetActiveTriangleCount( uiIndexCount / 3 );
	((NiTriShapeData*)GetModelData())->SetTriangleCount( uiIndexCount / 3 );	
//	m_pGeometryBufferData->SetIndices( uiIndexCount / 3, uiIndexCount / 3, NULL, NULL, 1 );
	m_pGeometryBufferData->SetIB( pIB, uiIndexCount, uiIBSize );	
	
	m_pIB			= pIB;
	m_uiIndexCount	= uiIndexCount;
	m_uiIBCount		= uiIBSize;

}

void NiTriShapeEx::SetIB( D3DIndexBufferPtr pIB )
{
	m_pIB = pIB;
}

void NiTriShapeEx::RenderImmediate(NiRenderer* pkRenderer)
{
/*
	if( NULL != m_pGeometryBufferData )		
		m_pGeometryBufferData->SetIB( m_pIB, m_uiIndexCount, m_uiIBCount );

	NiD3DShaderInterface* pkShader = NULL;
	
	pkShader = NiDynamicCast(NiD3DShaderInterface,  GetShaderFromMaterial());

    if (pkShader == 0)
    {
        const NiMaterial* pkTempMat = GetActiveMaterial();
		if( !pkTempMat )
		{
			int  i = 1;
		}
		else
		{
			int  i = 0;
		}
    }
*/


	if( !m_pGeometryBufferData )
	{
		m_pGeometryBufferData = (NiGeometryBufferData*)((NiTriShapeData*)GetModelData())->GetRendererData();
//		m_pGeometryBufferData
	}

	NiTriShape::RenderImmediate( pkRenderer );
}

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTriShapeEx);
//---------------------------------------------------------------------------
void NiTriShapeEx::CopyMembers(NiTriShapeEx* pkDest,NiCloningProcess& kCloning)
{
    NiTriShape::CopyMembers((NiTriShape*)pkDest, kCloning);
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriShapeEx);
//---------------------------------------------------------------------------
void NiTriShapeEx::LoadBinary(NiStream& kStream)
{
    NiTriShape::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTriShapeEx::LinkObject(NiStream& kStream)
{
    NiTriShape::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShapeEx::RegisterStreamables(NiStream& kStream)
{
    return NiTriShape::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriShapeEx::SaveBinary(NiStream& kStream)
{
    NiTriShape::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShapeEx::IsEqual(NiObject* pkObject)
{
    return NiTriShape::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiTriShapeEx::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTriShape::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTriShape::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------