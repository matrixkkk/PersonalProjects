#ifndef BULLETDECAL_H
#define BULLETDECAL_H

#include <vector>
#include "DecalAtChunk.h"


const long maxDecalVertices = 256;
const float DECALEPSILON = 0.25F;



struct DECALVERTEX
{
	NiPoint3 p;
	NiPoint3 n;
	float 		diffuse;
	float       tu, tv;
};

class BulletDecalFactory	: public NiMemObject
{
	NiPoint3		m_decalCenter;
	NiPoint3		m_decalNormal;
	NiPoint3		m_tangent;
	NiPoint3		m_binormal;

	float			m_width;
	float			m_height;
	float			m_fdepth;

	NiPlane			m_leftPlane;
	NiPlane			m_rightPlane;
	NiPlane			m_bottomPlane;
	NiPlane			m_topPlane;
	NiPlane			m_frontPlane;
	NiPlane			m_backPlane;
	
	std::vector< DECALVERTEX >		m_vecDecalVertices;
	std::vector< DecalTriIndex >	m_vecDecalIndices;

	NiTexturingPropertyPtr	m_spTexture;
	NiCloningProcess		m_kCloning;
	
public:

	BulletDecalFactory();
	virtual ~BulletDecalFactory();

	// 데칼을 컷팅할 6개의 평면 생성
	void Init(const NiPoint3* center, const NiPoint3* normal, const NiPoint3* tangent, const float &width, const float &height, const float &depth );

	// 평면에 의해서 실제 폴리곤을 잘라서 새로운 폴리곤 구조 형성
	void ClipMesh( std::vector<DecalTriIndex>& vecDecalTri, const NiPoint3 *vertex, const NiPoint3 *normal);

	// 잘려져서 새로 생성된 버텍스및 삼각형 인덱스 리스트 저장
	bool AddPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal);

	unsigned int ClipPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal) const;
	
	static unsigned int ClipPolygonAgainstPlane(const NiPlane& plane, unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal);

	// 최종적으로 데칼에 필요한 버텍스와 인덱스 정보로 NiTrishape를 생성하여 DecalAtChunk에 Attach
	BOOL	GenerateDecal( DecalAtChunk *pDecalChunk, const NiPoint3 &normal );


};




#endif