#ifndef DUSTMANAGER_H
#define DUSTMANAGER_H

#include <list>
#include <algorithm>
#include <functional>
#include "Dust.h"


const float DUST_LIFE_TIME		= 0.7f;
const float DUST_DEFAULT_SCALE	= 0.008f;


class CDustManager : public NiMemObject
{

	typedef std::list< CDust* >	VecDusts;

	static CDustManager	*ms_pDustManager;
	
	float		m_fLifeTime;
	float		m_fDelayTime;
	int			m_nOffset;

	CDust		*m_pDustShape;

	VecDusts	m_vecDusts;


public:

	bool CreateDustShape( char *pNifName, char *pTexture );

	static bool CreateManager( float fDelayTime, float fLifeTime, int fOffset, char* pNifname, char* pTexName );
	static void DestroyManager();
	static CDustManager* GetInstance();

	void	AddDust( const NiPoint3& vPos, const NiPoint3& vNor);

	void	SetLifeTime( float fLifeTime ){ m_fLifeTime = fLifeTime; }

	virtual void Update( float fElapsedTime );
	virtual void Render( NiCamera* pkCamera );

protected:

	CDustManager();
	CDustManager( float fDelayTime, float fLifeTime, int nOffset );
	CDustManager( CDustManager& );
	virtual ~CDustManager();
	
};

#endif