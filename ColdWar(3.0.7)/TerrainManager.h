/*
	클래스 이름 : TerrainManager
	클래스 설명 : 지형을 관리하는 클래스로 이것은 캐릭터의 중력을 계산하기 위해 테스트용 클래스이다.
				  후에 김대호가 넘겨준 지형데이터의 처리를 하기 위한 클래스로 수정이 필요하다.
*/
#ifndef TERRAINMANAGER_H_
#define TERRAINMANAGER_H_

#include <NiPick.h>


class TerrainManager
{
	NiAVObjectPtr	m_spTerrain;
	NiPick			m_kGroundPick;

public:
	TerrainManager();
	~TerrainManager();

	bool	Initiaize(const char* pcFilename);
	
	//x,y 좌표를 받아서 그 위치의 높이를 레퍼런스로 넘겨준다.
	bool	GetHeight(float fx,float fy, float& fHeight);
	//높이와 함께 노멀도 넘겨준다.(사용여부는 아직 미지수)
	bool	GetHeight(float fx,float fy, float& fHeight, NiPoint3& kNormal);
	//피킹광선의 시작 벡터와 끝 벡터를 넘겨줘서 피킹되는 위치를 가져온다.
	bool	GetIntersection(const NiPoint3& kStart, const NiPoint3& kDir, NiPoint3& kLoc);

	NiAVObject* GetGroundSceneGraph();
};


#endif

