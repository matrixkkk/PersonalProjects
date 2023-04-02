#include "main.h"
#include "DecalManager.h"
#include "DecalAtChunk.h"
#include "GameApp.h"



DecalAtChunk::DecalAtChunk()
{
	
	m_spDecalChunk = NiNew NiNode();	

	m_fTime = ::g_fDecalLifeTime;

	NiAlphaProperty *pAlpha = NiNew NiAlphaProperty();
	pAlpha->SetAlphaBlending(true);
	pAlpha->SetAlphaTesting(true);
	pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
	pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
	pAlpha->SetTestMode(NiAlphaProperty::TEST_ALWAYS);

	NiMaterialProperty* pkMat = NiNew NiMaterialProperty;
	pkMat->SetEmittance(NiColor(1.0f, 1.0f, 1.0f));
	pkMat->SetAlpha(1.0f);//----------->�� ���� ���� �����ؼ� ������ ������� ��.
	pkMat->SetShineness(10.0f);

	m_spDecalChunk->AttachProperty( pAlpha );
	m_spDecalChunk->AttachProperty( pkMat );

	m_spDecalChunk->Update( 0.0 );
	m_spDecalChunk->UpdateEffects();
	m_spDecalChunk->UpdateProperties();

	m_spDecalChunk->SetAppCulled( false );

}

//		Release
BOOL	DecalAtChunk::ReleaseObject()
{
	if( m_spDecalChunk )
	{
		if( m_spDecalChunk->GetChildCount() )
			return TRUE;

		NiTriShapePtr spDecal;

		for( unsigned int ui = 0; ui < m_spDecalChunk->GetChildCount(); ui++ )
		{
			if( NiIsKindOf( NiTriShape, m_spDecalChunk->GetAt( ui ) ) )
			{
				NiAlphaPropertyPtr		spAlpha = (NiAlphaProperty*)m_spDecalChunk->GetAt( ui )->GetProperty( NiProperty::ALPHA );
				NiWireframePropertyPtr	spWire	= (NiWireframeProperty*)m_spDecalChunk->GetAt( ui )->GetProperty( NiProperty::WIREFRAME );
				NiMaterialPropertyPtr	spMat	= (NiMaterialProperty*)m_spDecalChunk->GetAt( ui )->GetProperty( NiProperty::MATERIAL );

				m_spDecalChunk->GetAt( ui )->DetachAllProperties();
				if( spAlpha )	spAlpha = 0;
				if( spWire )	spWire = 0;
				if( spMat )		spWire = 0;

				spDecal = NiSmartPointerCast( NiTriShape, m_spDecalChunk->DetachChild( m_spDecalChunk->GetAt( ui ) ) );
				spDecal = 0;
			}
		}
		m_spDecalChunk->Update( 0.0f );

		if( !m_listDecals.IsEmpty() )
		{
			m_listDecals.RemoveAll();
		}
		return TRUE;
	}

	return FALSE;
}

//		NiNode Object ����
NiNode* DecalAtChunk::GetRootObject()
{
	return m_spDecalChunk;
}

//		Update
void	DecalAtChunk::UpdateObject( float fElapsedTime )
{
	if( m_spDecalChunk->GetChildCount() <= 0 )
		return;


	float fLifeTime = 0.0f;
	NiFloatExtraData *pExtData = NULL;
	NiTriShape* pDecal = NULL;
	float fAlpha = 0.0f;
	bool bDetach = false;

	NiTListIterator iterDecal = m_listDecals.GetHeadPos();

	while( iterDecal )
	{
		
		pDecal = m_listDecals.Get( iterDecal );

		if( pDecal )
		{
			pExtData = (NiFloatExtraData*)pDecal->GetExtraData( "LifeTime" );

			fLifeTime = pExtData->GetValue();
			fLifeTime -= fElapsedTime;			

			if( fLifeTime <= 0.0f )
			{
				NiTriShapePtr spTempDecal = NiSmartPointerCast( NiTriShape, m_spDecalChunk->DetachChild( pDecal ) );
				
				m_listDecals.RemovePos( iterDecal );

				if( spTempDecal )
				{
					bDetach = true;
					spTempDecal = 0;
				}
			}
			else
			{
				NiAlphaProperty *pAlpha = (NiAlphaProperty*)pDecal->GetProperty( NiProperty::ALPHA );

				fAlpha = 0.1f * fLifeTime;
				CGameApp::ChangeNodeAlpha( pDecal, fAlpha );	

				iterDecal = m_listDecals.GetNextPos( iterDecal );

				pExtData->SetValue( fLifeTime );
			
			}
		}
	}
	

	if( bDetach ) m_spDecalChunk->Update( 0.0f );			
	
}

//		�ø�
void	DecalAtChunk::SetAppCulled( bool bCull )
{
	if( m_spDecalChunk ) m_spDecalChunk->SetAppCulled( bCull );
}

//		��Į�� ûũ�� ����ġ
BOOL	DecalAtChunk::AddDecal( NiTriShape *pDecalObject )
{
	if( !pDecalObject )
		return FALSE;


	NiFloatExtraData *pExtData = NiNew NiFloatExtraData( m_fTime );
	
	if( pExtData )
	{
		// ź�� ������ Ÿ�� ����
		if( pDecalObject->AddExtraData( "LifeTime", pExtData ) )
		{				
			m_spDecalChunk->AttachChild( pDecalObject );
			m_spDecalChunk->Update( 0.0f );

			// �ڷᱸ���� ���� ��? for���߿� DetachChild �ϸ� ������... ���� NiTriShape ������ List�� �����ؼ� Update�� �ϰ������� ó��
			// ������ ���� �ʿ�
			m_listDecals.AddTail( pDecalObject );

			return TRUE;
		}	
	}

	return FALSE;
	
}

void	DecalAtChunk::SetLiftTime( float fTime )
{
	m_fTime = fTime;

}

void	DecalAtChunk::SetTileIndex( int xIndex, int yIndex )
{
	m_nTileXIndex = xIndex;
	m_nTileYIndex = yIndex;
}

void	DecalAtChunk::GetTileIndex( int *xIndex, int *yIndex )
{
	*xIndex = m_nTileXIndex;
	*yIndex	= m_nTileYIndex;
}