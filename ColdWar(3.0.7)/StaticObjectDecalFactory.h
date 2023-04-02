#ifndef STATICOBJECTDECALFACTORY_H
#define STATICOBJECTDECALFACTORY_H

#include <vector>
#include "DecalAtChunk.h"



class StaticObjectDecalFactory : public NiMemObject
{

public:

	enum{ BRICK_DECAL, WOOD_DECAL };

private:

	NiCloningProcess		m_kCloning;
	
	NiAlphaPropertyPtr		m_spAlpha;
	NiMaterialPropertyPtr	m_spMaterial;

	NiTPointerMap< BYTE, NiTriShapePtr >	m_mapKindOfDecals;

public:

	StaticObjectDecalFactory();
	~StaticObjectDecalFactory();

	BOOL	AddDecalShapeAtMap( BYTE nType, const NiFixedString& strDecalName );

	BOOL	GenerateDecal(  const NiPoint3 *vPos, const NiPoint3 *normal, BYTE nDecalType, DecalAtChunk *pDecalAtChunk );

	BOOL	Init();

};



#endif
