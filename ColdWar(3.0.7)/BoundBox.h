#ifndef BOUNDBOX_H
#define BOUNDBOX_H



class BoundBox : public NiMemObject
{
	
	NiTriShapePtr			m_spBox;
	NiPoint3				m_vMin, m_vMax;
	NiWireframePropertyPtr	m_spWireFrame;
	
public:

	BoundBox( const NiPoint3& vMin, const NiPoint3& vMax );
	~BoundBox();

	void SetMinMax( const NiPoint3& min, const NiPoint3& max );
	

	void GetMinMax( NiPoint3 *min, NiPoint3 *max )
	{
		*min = m_vMin;
		*max = m_vMax;
	}

	BOOL IsCollision( const NiPoint3 &vPoint );
	BOOL RayCollision( const NiPoint3& pos, const NiPoint3& dir, NiPick *pPick, float *fDist );	
	virtual NiAVObject*	GenerateAABB( const NiPoint3& vMin, const NiPoint3& vMax );

	void SetAppCulled( bool bCull );
	void UpdateMax( float minY, float maxY );
	void Include ( const NiPoint3& vPos );

	// 바운딩 박스 그리기
	void RenderImmediate( NiRenderer *pRenderer );	
	void ReleaseObject();
	NiAVObject* GetBoundObject();

	void			Translation( float x, float y, float z );
	void			Scaling( float fDel );
	void			RotationX( float fAngle );
	void			RotationY( float fAngle );
	void			RotationZ( float fAngle );

	void			SetPosition( const NiPoint3& vPos );
	void			ResetMinMax();
	

};

#endif