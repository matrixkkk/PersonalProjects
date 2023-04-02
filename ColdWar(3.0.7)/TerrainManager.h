/*
	Ŭ���� �̸� : TerrainManager
	Ŭ���� ���� : ������ �����ϴ� Ŭ������ �̰��� ĳ������ �߷��� ����ϱ� ���� �׽�Ʈ�� Ŭ�����̴�.
				  �Ŀ� ���ȣ�� �Ѱ��� ������������ ó���� �ϱ� ���� Ŭ������ ������ �ʿ��ϴ�.
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
	
	//x,y ��ǥ�� �޾Ƽ� �� ��ġ�� ���̸� ���۷����� �Ѱ��ش�.
	bool	GetHeight(float fx,float fy, float& fHeight);
	//���̿� �Բ� ��ֵ� �Ѱ��ش�.(��뿩�δ� ���� ������)
	bool	GetHeight(float fx,float fy, float& fHeight, NiPoint3& kNormal);
	//��ŷ������ ���� ���Ϳ� �� ���͸� �Ѱ��༭ ��ŷ�Ǵ� ��ġ�� �����´�.
	bool	GetIntersection(const NiPoint3& kStart, const NiPoint3& kDir, NiPoint3& kLoc);

	NiAVObject* GetGroundSceneGraph();
};


#endif

