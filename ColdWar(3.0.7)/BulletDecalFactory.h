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

	// ��Į�� ������ 6���� ��� ����
	void Init(const NiPoint3* center, const NiPoint3* normal, const NiPoint3* tangent, const float &width, const float &height, const float &depth );

	// ��鿡 ���ؼ� ���� �������� �߶� ���ο� ������ ���� ����
	void ClipMesh( std::vector<DecalTriIndex>& vecDecalTri, const NiPoint3 *vertex, const NiPoint3 *normal);

	// �߷����� ���� ������ ���ؽ��� �ﰢ�� �ε��� ����Ʈ ����
	bool AddPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal);

	unsigned int ClipPolygon(unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal) const;
	
	static unsigned int ClipPolygonAgainstPlane(const NiPlane& plane, unsigned int vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal);

	// ���������� ��Į�� �ʿ��� ���ؽ��� �ε��� ������ NiTrishape�� �����Ͽ� DecalAtChunk�� Attach
	BOOL	GenerateDecal( DecalAtChunk *pDecalChunk, const NiPoint3 &normal );


};




#endif