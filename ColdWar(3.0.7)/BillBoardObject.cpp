#include "main.h"
#include "BillBoardObject.h"
#include "GameApp.h"


BillBoardObject::BillBoardObject()
{
//	m_strBillBoardName = _T("");
	m_fScale = 1.0f;
}

BillBoardObject::BillBoardObject( const NiFixedString& strFileName ) :
m_strBillBoardName( strFileName )
{

}

BillBoardObject::~BillBoardObject()
{
	if( m_spRootObject ) m_spRootObject = 0;
}

NiBillboardNode* BillBoardObject::GetObject()
{
	return m_spRootObject;
}

void	BillBoardObject::SetScale( float fScale )
{
	m_fScale = fScale;
}

float	BillBoardObject::GetScale()
{
	return m_fScale;
}


BOOL	BillBoardObject::ReleaseObject()
{
	if( m_spRootObject ) m_spRootObject = 0;
	return TRUE;
}

NiBillboardNode* BillBoardObject::Clone()
{
	if( m_spRootObject )
	{
		
		NiCloningProcess kCloning;
		kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;

		return (NiBillboardNode*)m_spRootObject->Clone(kCloning)  ;
	}

	return NULL;
}


BOOL	BillBoardObject::GenerateBillBoard( NiFixedString& strFileName )
{
	NiStream kStream;

	CGameApp::SetMediaPath("./Data/Object/BillBoard/");

	bool bSuccess = kStream.Load( CGameApp::ConvertMediaFilename( strFileName ) );
	
	if( bSuccess )
	{
		if( NiIsKindOf( NiBillboardNode, kStream.GetObjectAt(0) ) )
		{
			m_spRootObject = (NiBillboardNode*)kStream.GetObjectAt(0);

			m_strBillBoardName = strFileName;

			return TRUE;
		}
		else if( NiIsKindOf( NiNode, kStream.GetObjectAt(0) ) )
		{
			NiNode *pObject = (NiNode*)kStream.GetObjectAt(0);
			
			for( unsigned int ui = 0; ui < pObject->GetChildCount(); ui++ )
			{
				if( NiIsKindOf( NiBillboardNode, pObject->GetAt( ui ) ) )
				{
					m_spRootObject = (NiBillboardNode*)pObject->GetAt( ui );
					m_spRootObject->Update(0.0f);
					m_spRootObject->UpdateProperties();

					m_strBillBoardName = strFileName;
					
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


NiFixedString	BillBoardObject::GetFileName()
{
	return m_strBillBoardName;
}