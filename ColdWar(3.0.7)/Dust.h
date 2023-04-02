#ifndef DUST_H
#define DUST_H


#include <NiMain.h>
#include <NiBillboardNode.h>
#include <NiTextureTransform.h>
#include <NiTexturingProperty.h>

class CDust : public NiRefObject
{

	enum{ SMALL, MEDIUM, LARGE, BILL_COUNT };

	NiNodePtr				m_spDustRoot;
	NiTexturingPropertyPtr	m_spDustTex;
	NiTextureTransform*		m_pktTexTransForm;

	float	m_fDelayTime;
	float	m_fLastTime;
	float	m_fLifeTime;

	int		m_nXCurOffset;
	int		m_nYCurOffset;

	int		m_nXOffset;
	int		m_nYOffset;

	float	m_fOffsetSpacing;

public:

	CDust();
	CDust( float fDelay, int xOffset = 5, int yOffset = 5);
	~CDust();

	NiNode*	GetObject(){ return m_spDustRoot; }
	void	SetObject( NiNode *pObject );
	void	SetTransform( const NiTransform &vTransform );
	
	void DelayTime( float fDelay ){ m_fDelayTime = fDelay; }
	void SetOffSetSpacing( float fSpacing ){ m_fOffsetSpacing = fSpacing; }
	void SetOffSet( int xOffset, int yOffset ){ m_nXOffset=xOffset; m_nYOffset=yOffset; }
	void SetDefaultAttribute( float fDelay, float fSpacing, int xOffset, int yOffset );

	void SetPosAndDir( const NiPoint3& vPos, const NiPoint3& vDir );
	void SetDustTransform( const NiTransform& vTrnaform );

	virtual bool Initialize( char *pNifName, char *pTex );
	virtual bool Update( float fElapsedTime );
	virtual void RenderClick( NiCamera* pkCamera );

	virtual void ClonDust( CDust *pClon );

	

};

NiSmartPointer(CDust); 

#endif