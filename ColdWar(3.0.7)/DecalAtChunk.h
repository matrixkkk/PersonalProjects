#ifndef DECALATCHUNK_H
#define DECALATCHUNK_H

#include "DecalObject.h"

const float fDecalMaxLifeTime = 10.0f;						// Decal의 최대 지속 시간 
const float fOffsetDitance = 0.01001f;

struct DecalTriIndex
{
	unsigned short index[3];
};

class DecalAtChunk : public NiMemObject
{

	NiNodePtr			m_spDecalChunk;	

	float				m_fTime;

	int					m_nTileXIndex, m_nTileYIndex;

	NiTPointerList< NiTriShapePtr >	m_listDecals;

public:

	DecalAtChunk();

	//		Release
	BOOL	ReleaseObject();

	//		NiNode Object 리턴
	NiNode* GetRootObject();

	//		Update
	void	UpdateObject( float fElapsedTime );

	//		컬링
	void	SetAppCulled( bool bCull );

	//		데칼을 청크에 어태치
	BOOL	AddDecal( NiTriShape* pDecalObject );

	void	SetLiftTime( float fTime );

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

};


#endif