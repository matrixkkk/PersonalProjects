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

	//		NiNode Object ����
	NiNode* GetRootObject();

	void	AddStaticObject( NiNode *pObject, int type );

	void	SetRenderingTransform( const NiTransform &transform );

	void	AddObjectForCollision( NiAVObject *pObject );

	NiNode*	GetCollsionRoot();

	//		�ø�
	void	SetAppCulled( bool bCull );

	//		���� ���� ������ ����
	int		GetCountObject();

	//		ũ�� ����
	void	SetCurrentScaling( int nType, float fScale );
	
	//		���İ��� �����Ͽ� ���̰� �� ���� �Ÿ� ����
	void	SetDistanceApplyAlpha( float fDistance );

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

	BOOL	PickRayAtObjects( const NiPoint3& pos, const NiPoint3& dir, NiPoint3 *pickPos, NiPoint3 *pickNor, NiPick *pPick, float *fDist );

};


#endif