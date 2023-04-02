#include "main.h"
#include "Frustum.h"


FrustumEx::FrustumEx()
{
	m_pCamera = NULL;
	m_fPlaneEpsilon	= PLANE_EPSILON;

	m_niFrustumPlanes.EnableAllPlanes();

	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::NEAR_PLANE );
	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::TOP_PLANE );
	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::BOTTOM_PLANE );

}
FrustumEx::FrustumEx( NiCamera *pCamera )
{
	
	m_pCamera		= pCamera;
	m_fPlaneEpsilon	= PLANE_EPSILON;

	m_niFrustumPlanes.EnableAllPlanes();

	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::NEAR_PLANE );
	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::TOP_PLANE );
	m_niFrustumPlanes.DisablePlane( NiFrustumPlanes::BOTTOM_PLANE );
	
	GenerateFrustum();
}

FrustumEx::~FrustumEx()
{
}

void FrustumEx::GenerateFrustum()
{
	if( m_pCamera == NULL )
		return;

	m_niFrustumPlanes.Set( *m_pCamera );
	
}

BOOL FrustumEx::InInFrustum( const NiPoint3& pVector )
{
	
	float fDistance = FLT_MAX;

	for( unsigned int i = NiFrustumPlanes::NEAR_PLANE;  i < NiFrustumPlanes::MAX_PLANES;  i++ )
	{
		const NiPlane &plane = m_niFrustumPlanes.GetPlane( i );

		if( NiPlane::NEGATIVE_SIDE == plane.WhichSide( pVector ) )
		{
			if( (fDistance = NiAbs(plane.Distance( pVector ))) > m_fPlaneEpsilon )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL FrustumEx::IsInSphere( const NiPoint3& pVector, float fradius )
{

	float fDistance = FLT_MAX;
	float fLimitDis = m_fPlaneEpsilon + fradius;

	for( unsigned int i = NiFrustumPlanes::NEAR_PLANE;  i < NiFrustumPlanes::MAX_PLANES;  i++ )
	{
		const NiPlane &plane = m_niFrustumPlanes.GetPlane( i );

		if( NiPlane::NEGATIVE_SIDE == plane.WhichSide( pVector ) )
		{
			if( (fDistance = NiAbs(plane.Distance( pVector ))) > fLimitDis )
				return FALSE;
		}

	}
	
	return TRUE;
}

BOOL FrustumEx::GetCameraPos( NiPoint3 *CameraPos )
{
	if( m_pCamera == NULL )
		return FALSE;

	*CameraPos = m_pCamera->GetTranslate();

	return TRUE;
}

void	FrustumEx::SetCamera( NiCamera* pCamera )
{
	m_pCamera = pCamera;
}

/*
NiFrustumPlanes::NEAR_PLANE		Index of the near plane.
NiFrustumPlanes::FAR_PLANE		Index of the far plane.
NiFrustumPlanes::LEFT_PLANE		Index of the left plane.
NiFrustumPlanes::RIGHT_PLANE	Index of the right plane.
NiFrustumPlanes::TOP_PLANE		Index of the top plane.
NiFrustumPlanes::BOTTOM_PLANE	Index of the bottom plane.
NiFrustumPlanes::MAX_PLANES		Not an index - this is the count of planes, and can be used as a bound on any iteration over the planes
*/

/*
D3DXPlaneFromPoints( &m_plane[0], &m_vtx[2], &m_vtx[6], &m_vtx[7] );// 원 평면(far)
D3DXPlaneFromPoints( &m_plane[1], &m_vtx[0], &m_vtx[3], &m_vtx[7] );// 좌 평면(left)
D3DXPlaneFromPoints( &m_plane[2], &m_vtx[1], &m_vtx[5], &m_vtx[6] );// 우 평면(right)
D3DXPlaneFromPoints( &m_plane[3], &m_vtx[4], &m_vtx[7], &m_vtx[6] );// 상 평면(top)
D3DXPlaneFromPoints( &m_plane[4], &m_vtx[0], &m_vtx[1], &m_vtx[2] );// 하 평면(bottom)
D3DXPlaneFromPoints( &m_plane[5], &m_vtx[0], &m_vtx[4], &m_vtx[5] );// 근 평면(near)
*/