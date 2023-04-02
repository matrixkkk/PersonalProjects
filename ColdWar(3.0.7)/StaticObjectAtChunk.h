#ifndef STATICOBJECTATCHUNK_H
#define STATICOBJECTATCHUNK_H

#include <vector>
#include <string>
#include <algorithm>


class StaticObjectAtChunk : public NiMemObject
{	
	
	NiNodePtr			m_spRootStaticObject;
	NiNodePtr			m_spRootForCollision;

	int					m_nTileXIndex, m_nTileYIndex;

	std::vector< NiAVObject* >		m_vecObject;
	
public:

	enum{	ENTIRE_INCLUDE, HALF_INCLUDE };

	StaticObjectAtChunk();


	//		Release
	BOOL	ReleaseObject();

	//		NiNode Object 리턴
	NiNode* GetRootObject();

	void	AddStaticObject( NiNode *pObject, int type );

	void	SetRenderingTransform( const NiTransform &transform );

	void	AddObjectForCollision( NiAVObject *pObject );

	NiNode*	GetCollsionRoot();

	//		컬링
	void	SetAppCulled( bool bCull );

	//		현재 존의 빌보드 갯수
	int		GetCountObject();

	//		크기 조정
	void	SetCurrentScaling( int nType, float fScale );
	
	//		알파값을 조정하여 보이게 될 기준 거리 셋팅
	void	SetDistanceApplyAlpha( float fDistance );

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

	BOOL	PickRayAtObjects( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickPos, NiPoint3 *pickNor, NiPick *pPick, float *fDist );

};


#endif