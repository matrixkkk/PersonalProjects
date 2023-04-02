#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	m_kGroundPick.SetPickType(NiPick::FIND_FIRST);
	m_kGroundPick.SetSortType(NiPick::NO_SORT);
	m_kGroundPick.SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	//m_kGroundPick.SetCoordinateType(NiPick::MODEL_COORDINATES);
	m_kGroundPick.SetFrontOnly(true);
	m_kGroundPick.SetObserveAppCullFlag(true);	//값이 true이면 'app culled' 플래그 집합을 가지고 있는 객체와 마주치면 pick이 중단
	m_kGroundPick.SetReturnTexture(false);		//텍스쳐 좌표를 리턴하지 않는다.
	m_kGroundPick.SetReturnNormal(false);		//노멀 벡터를 리턴하지 않음
	m_kGroundPick.SetReturnSmoothNormal(false); //3개의 정점에서 보간된 노멀벡터를 리턴하지 않음
	m_kGroundPick.SetReturnColor(false);		//정점의 색을 반환하지 않음

}

TerrainManager::~TerrainManager()
{
}

NiAVObject* TerrainManager::GetGroundSceneGraph()
{
	return m_spTerrain;
}

bool TerrainManager::GetHeight(float fx, float fy, float &fHeight)
{
	//if(m_kGroundPick.PickObjects(NiPoint3(fx,fy,500.0f),-NiPoint3::UNIT_Z
	return true;
}

bool TerrainManager::GetHeight(float fx, float fy, float &fHeight, NiPoint3 &kNormal)
{
	return true;
}

bool TerrainManager::GetIntersection(const NiPoint3 &kStart, const NiPoint3 &kDir, NiPoint3 &kLoc)
{
	return true;
}

bool TerrainManager::Initiaize(const char *pcFilename)
{
	NiStream kStream;

	assert(pcFilename);

	bool bLoaded = kStream.Load("Data/world/terrain.nif");
	if(!bLoaded)
	{
		NiMessageBox("Terrain::Initialize -- terrain.nif cound not Load","NIF error");
		return false;
	}
	m_spTerrain = (NiNode*)kStream.GetObjectAt(0);

	m_kGroundPick.SetTarget(m_spTerrain);

	return true;
}

