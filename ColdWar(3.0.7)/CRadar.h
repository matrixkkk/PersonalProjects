#ifndef CRADAR_H_
#define CRADAR_H_

#include "main.h"

struct RadarUnit
{
	NiPoint3	kPos;	//유닛의 위치
	NiPoint3	kTransformPos;	//전환된 위치
};

class CRadar : public NiMemObject
{
private:
	NiScreenTexture			*m_pkTexture;
	NiScreenElementsArray	m_kArrayElements;

	NiScreenElementsPtr		m_spElementUnit;
	
	std::vector<RadarUnit>	m_vecMyTeamUnit;
	std::vector<RadarUnit>  m_vecEnemyUnit;
	
	bool		FindUnit(NiPoint3 p,NiRect<int> kRect);
	void		Rotate2D(NiPoint2&	kAxis,NiPoint3& kPoint,float fRadius);	//2차원 회전
public:
	CRadar();
	~CRadar();

	bool	CreateRadar();
	void	Draw(NiRenderer* pkRenderer);
	void	Update();
};


#endif
