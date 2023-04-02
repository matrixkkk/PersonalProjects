#include "main.h"
#include "DecalObject.h"
#include "GameApp.h"

DecalObject::DecalObject()
{
	m_strDecalName = "";
	m_fScale = 1.0f;
}

DecalObject::DecalObject( const NiFixedString& strFileName, int nType ) :
m_strDecalName( strFileName ), 
m_nType( nType )
{

}

DecalObject::~DecalObject()
{
	if( m_spDecalShape ) m_spDecalShape = 0;
}

BOOL DecalObject::ReleaseObject()
{
	if( m_spDecalShape ) m_spDecalShape = 0;
	return TRUE;

}

void	DecalObject::SetScale( float fScale )
{
	m_fScale = fScale;
}

float	DecalObject::GetScale()
{
	return m_fScale;
}

int		DecalObject::IsDecalType()
{
	return m_nType;
}

BOOL	DecalObject::GenerateDecal( const NiFixedString& strFileName, int nType )
{

	NiStream kStream;

	CGameApp::SetMediaPath("./Data/Object/Decal/");

	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename( strFileName ) );
	
	if( bSuccess )
	{
		m_nType = nType;

		if( NiIsKindOf( NiTriShape, kStream.GetObjectAt(0) ) )
		{
			m_spDecalShape = (NiTriShape*)kStream.GetObjectAt(0);

			m_strDecalName = strFileName;

			return TRUE;
		}
		else if( NiIsKindOf( NiNode, kStream.GetObjectAt(0) ) )
		{
			NiNode *pObject = (NiNode*)kStream.GetObjectAt(0);
			
			for( unsigned int ui = 0; ui < pObject->GetChildCount(); ui++ )
			{
				if( NiIsKindOf( NiTriShape, pObject->GetAt( ui ) ) )
				{
					m_spDecalShape = (NiTriShape*)pObject->GetAt( ui );
					m_spDecalShape->Update(0.0f);
					m_spDecalShape->UpdateProperties();

					m_strDecalName = strFileName;
					
					return TRUE;
				}
			}
		}
	}

	return TRUE;
}

NiTriShape* DecalObject::GetObject()
{
	return m_spDecalShape;
}

NiTriShape* DecalObject::Clone()
{
	if( m_spDecalShape )
	{		
		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;

		return (NiTriShape*)m_spDecalShape->Clone(kCloning)  ;
	}

	return NULL;

}

NiFixedString	DecalObject::GetFileName()
{
	return m_strDecalName;
}