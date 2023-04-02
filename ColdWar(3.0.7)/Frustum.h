#ifndef FRUSTUM_H
#define FRUSTUM_H

//#include "ZFLog.h"

const float PLANE_EPSILON = 30.0f;

class FrustumEx : public NiMemObject{	

	NiCamera			*m_pCamera;
	NiFrustumPlanes		m_niFrustumPlanes;	
	float				m_fPlaneEpsilon;

public:	

	FrustumEx();
	FrustumEx( NiCamera *pCamera );
	~FrustumEx();

	void	GenerateFrustum();
	BOOL	InInFrustum( const NiPoint3& pVector );
	BOOL	IsInSphere( const NiPoint3& pVector, float fradius );
	BOOL	GetCameraPos( NiPoint3 *CameraPos);

	void	SetPlaneEpsilon( float planeEpsilon ){	m_fPlaneEpsilon = planeEpsilon; }
	float	GetPlaneEpsilon(){ return m_fPlaneEpsilon; }
	void	SetCamera( NiCamera* pCamera );

};

#endif