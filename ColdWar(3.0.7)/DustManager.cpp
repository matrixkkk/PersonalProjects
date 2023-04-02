#include "DustManager.h"
#include "WorldManager.h"

CDustManager *CDustManager::ms_pDustManager = NULL;


CDustManager::CDustManager()
{
	m_fLifeTime	= 0.0f;
	m_fDelayTime= 0.0f;
	m_nOffset	= 0;
	m_pDustShape= NULL;
}

CDustManager::CDustManager( float fDelayTime, float fLifeTime, int nfOffset )
{
	m_fDelayTime	= fDelayTime;
	m_fLifeTime		= fLifeTime;
	m_nOffset		= nfOffset;	
	m_pDustShape	= NULL;
}

CDustManager::~CDustManager()
{
	if( !m_vecDusts.empty() )
	{
		VecDusts::iterator iterDust;

		for( iterDust = m_vecDusts.begin(); iterDust != m_vecDusts.end(); ++iterDust )
		{
			NiDelete (*iterDust);
		}

		m_vecDusts.clear();
	}	

	if( m_pDustShape )
	{
		NiDelete m_pDustShape;
		m_pDustShape = NULL;
	}
}

bool CDustManager::CreateDustShape( char *pNifName, char *pTexture )
{
	if( !m_pDustShape )
	{
		m_pDustShape = NiNew CDust( m_fDelayTime, m_nOffset, m_nOffset );

		if( m_pDustShape )
			if( m_pDustShape->Initialize( pNifName, pTexture ) )
				return true;
	}

	return false;
}

bool CDustManager::CreateManager( float fDelayTime, float fLifeTime, int fOffset, char* pNifname, char* pTexName )
{
	if( !ms_pDustManager )
	{
		ms_pDustManager = NiNew CDustManager( fDelayTime,  fLifeTime, fOffset );		

		if(	ms_pDustManager )
		{
			if( ms_pDustManager->CreateDustShape( pNifname, pTexName ) )
				return true;
		}
	}

	return false;
}

void CDustManager::DestroyManager()
{

	if( ms_pDustManager )
	{
		NiDelete ms_pDustManager;
		ms_pDustManager = NULL;
	}
}

CDustManager* CDustManager::GetInstance()
{
	return ms_pDustManager;
}

void CDustManager::AddDust( const NiPoint3 &vPos, const NiPoint3 &vNor )
{
	const NiPoint3 &vUnitZ = NiPoint3::UNIT_Z;

	NiPoint3 &cross = vNor.Cross( vUnitZ );
	cross.Unitize();

	// �� ��� ������ �� ���� ���Ͽ� ���հ� ���ϱ�
	float fAngle = NiACos( vUnitZ.Dot( vNor ) / (NiAbs(vNor.Length()) * NiAbs(-NiPoint3::UNIT_Z.Length())) );

	// �浹������ ����� �̿��Ͽ� ȸ����� ���ϱ�
	NiTransform vTransform;
	vTransform.m_Rotate.MakeRotation( fAngle, cross );
	vTransform.m_Translate	= vPos;
	vTransform.m_fScale		= DUST_DEFAULT_SCALE;

	// New Dust ����
	CDust *pNewDust = NiNew CDust();

	// ���� Dust�� Clon
	m_pDustShape->ClonDust( pNewDust );
	
	// ������ ��ü�� �� Transform �Ҵ� �� Property �ʱ�ȭ
	pNewDust->SetDustTransform( vTransform );
	pNewDust->GetObject()->UpdateProperties();

	// �ڷᱸ���� ���
	m_vecDusts.push_back( pNewDust );
}

void CDustManager::Update( float fElapsedTime )
{
	if( m_vecDusts.empty() )
		return;

	VecDusts::iterator iterDust;

	for( iterDust = m_vecDusts.begin(); iterDust != m_vecDusts.end();  )
	{
		if( (*iterDust)->Update( fElapsedTime ) )
		{
			// ������ ���� Dust�� ����
			NiDelete (*iterDust);
			iterDust = m_vecDusts.erase( iterDust );
		}
		else
			++iterDust;
	}
}

void CDustManager::Render( NiCamera* pkCamera )
{
	if( m_vecDusts.empty() )
		return;

	std::for_each( m_vecDusts.begin(), m_vecDusts.end(), std::bind2nd( std::mem_fun( &CDust::RenderClick ), pkCamera ) );

}
