#ifndef CMUZELSPLASH_H
#define CMUZELSPLASH_H

#include <NiMain.h>
#include <NiScreenElements.h>
#include <NiTextureTransform.h>
#include <NiTexturingProperty.h>

class CMuzelSplash : public NiRefObject
{
	enum{ FRONT, HORIZON, VERTICAL };

	NiTriStripsPtr			m_spSplash[ 3 ];
	NiTextureTransform*		m_pktTexTransForm[ 2 ];

	float	m_fDelayTime;
	float	m_fLastTime;
	float	m_fCurrenTime;

	int		m_nXCurOffset[ 2 ];
	int		m_nYCurOffset[ 2 ];

	int		m_nXOffset[ 2 ];
	int		m_nYOffset[ 2 ];

	float	m_fOffsetSpacing[ 2 ];

	bool	m_bActive;


public:

	CMuzelSplash(	float fDelay, 
					int xOffset = 3, int yOffset = 3, 
					int xOffset1 = 2, int yOffset1 = 2);

	~CMuzelSplash();

	void	SetXFrontOffset( int x ){ m_nXOffset[FRONT] = x; }
	void	SetYFrontOffset( int y ){ m_nYOffset[FRONT] = y; }

	void	SetXHVOffset( int x ){ m_nXOffset[HORIZON] = x; }
	void	SetYHVOffset( int y ){ m_nYOffset[HORIZON] = y; }

	virtual bool Initialize( NiNode* pCharObject, char *pTex0, char *pTex1 );
	virtual void Update( float fElapsedTime, bool bUpdate = false );
	virtual void RenderClick();

};

NiSmartPointer(CMuzelSplash); 

#endif