#include "TerrainManager.h"

TerrainManager::TerrainManager()
{
	m_kGroundPick.SetPickType(NiPick::FIND_FIRST);
	m_kGroundPick.SetSortType(NiPick::NO_SORT);
	m_kGroundPick.SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	//m_kGroundPick.SetCoordinateType(NiPick::MODEL_COORDINATES);
	m_kGroundPick.SetFrontOnly(true);
	m_kGroundPick.SetObserveAppCullFlag(true);	//���� true�̸� 'app culled' �÷��� ������ ������ �ִ� ��ü�� ����ġ�� pick�� �ߴ�
	m_kGroundPick.SetReturnTexture(false);		//�ؽ��� ��ǥ�� �������� �ʴ´�.
	m_kGroundPick.SetReturnNormal(false);		//��� ���͸� �������� ����
	m_kGroundPick.SetReturnSmoothNormal(false); //3���� �������� ������ ��ֺ��͸� �������� ����
	m_kGroundPick.SetReturnColor(false);		//������ ���� ��ȯ���� ����

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

