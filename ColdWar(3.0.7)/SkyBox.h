#ifndef SKYBOX_H
#define SKYBOX_H


#include <vector>
#include <string>

#include "GBObject.h"

class SkyBox : public GBObject
{

	NiNodePtr			m_spSkyNode;
	NiCamera			*m_pCamera;

	NiFixedString		m_strFileName;
	BOOL				m_bDraw;
	NiMatrix3			m_matCameraRot;
	NiColorA			m_Color;
	NiPoint3			m_vExtents;

	std::vector< NiSourceTexture* >	m_vecSkyBoxTex;

public:
	

	enum{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,	
	COUNT
	};

	NiNode* GetObject(){ return m_spSkyNode; }
	BOOL UpdateObject( float fElapsedTime );
	BOOL ReleaseObject();

	SkyBox();
	SkyBox( const NiFixedString& fileName );
	~SkyBox();

	virtual bool Generate( const NiFixedString& strTexFileName, const NiPoint3& vExtents );	

	void SetPosition( const NiPoint3& _vPosition );
	void SetAppCulled( bool bCull );
	void SetCamera( NiCamera* pCamera );
	void SetColor( const NiColorA& color );

	NiFixedString GetFileName(){ return m_strFileName; }
	NiNode* GetRootObject(){ return m_spSkyNode; }

	
	// 파일 저장

//	void	SaveData( CArchive& ar );
//	void	SaveNameData( std::ofstream& fwrite );

	


};

#endif