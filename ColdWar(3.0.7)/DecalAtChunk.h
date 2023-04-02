#ifndef DECALATCHUNK_H
#define DECALATCHUNK_H

#include "DecalObject.h"

const float fDecalMaxLifeTime = 10.0f;						// Decal�� �ִ� ���� �ð� 
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

	//		NiNode Object ����
	NiNode* GetRootObject();

	//		Update
	void	UpdateObject( float fElapsedTime );

	//		�ø�
	void	SetAppCulled( bool bCull );

	//		��Į�� ûũ�� ����ġ
	BOOL	AddDecal( NiTriShape* pDecalObject );

	void	SetLiftTime( float fTime );

	void	SetTileIndex( int xIndex, int yIndex );

	void	GetTileIndex( int *xIndex, int *yIndex );

};


#endif