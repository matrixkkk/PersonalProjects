#ifndef EXPLOSIONDECALFACTORY_H
#define EXPLOSIONDECALFACTORY_H

#include <vector>
#include "DecalAtChunk.h"


class ExplosionDecalFactory : public NiMemObject
{

	NiCloningProcess	m_kCloning;

	int					m_nCntVtx;
	int					m_nCtnTri;
	int					m_nCntRect;

	NiTexturingPropertyPtr	m_spTexture;
	NiAlphaPropertyPtr		m_spAlpha;
	NiMaterialPropertyPtr	m_spMaterial;

public:

	ExplosionDecalFactory( int nCntRect );
	~ExplosionDecalFactory();


	BOOL	GenerateDecal(  const NiPoint3 *vertex, const NiPoint3 *normal, std::vector<int>& vecDecalTri, DecalAtChunk *pDecalAtChunk );

	BOOL	Init();

};

#endif