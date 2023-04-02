#include "NetworkManager.h"
#include "GameApp.h"
#include "CRadar.h"
//#include "ZFLog.h"

CRadar::CRadar()
{
	m_pkTexture = NULL;
}

CRadar::~CRadar()
{
	if(m_pkTexture)
		NiDelete m_pkTexture;

	//���̴� ��ũ�� �ؽ��� ����
	m_kArrayElements.RemoveAll();

	//���� ����
	m_vecMyTeamUnit.clear();
	m_vecEnemyUnit.clear();

	m_pkTexture = NULL;
	m_spElementUnit = NULL;
}


bool CRadar::CreateRadar()
{
	//���̴� �׸� �ҷ�����	
	NiSourceTexture* pkSrcTexRadar = NiSourceTexture::Create("Data/UI/radar2.tga");
	assert(pkSrcTexRadar);
	pkSrcTexRadar->SetName("radar");
	m_pkTexture = NiNew NiScreenTexture(pkSrcTexRadar);
	assert(m_pkTexture);
	m_pkTexture->AddNewScreenRect(0,0,120,120,0,0);

	//���� ������Ʈ ����
	m_spElementUnit = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(m_spElementUnit);
	m_spElementUnit->SetName("unit");
	m_spElementUnit->Insert(4);
	
	return true;
}

void CRadar::Draw(NiRenderer* pkRenderer)
{
	//���̴� �̹��� �׸���
	m_pkTexture->Draw(pkRenderer);

	//���� �׸���
	for(unsigned int i=0; i<m_kArrayElements.GetSize();i++)
	{
		NiScreenElements* pkElement = m_kArrayElements.GetAt(i);
		if(pkElement)
			pkElement->Draw(pkRenderer);
	}
}

void CRadar::Rotate2D(NiPoint2 &kAxis,NiPoint3 &kPoint,float fRadius)
{
	NiPoint3	kOrigin;		//�������� �̵��� ��ǥ
	NiMatrix3	kRot;
	
	/*kRot.SetEntry(0,0,NiCos(fRadius));
	kRot.SetEntry(0,1,-NiSin(fRadius));
	kRot.SetEntry(0,2,0.0f);
	
	kRot.SetEntry(1,0,NiSin(fRadius));
	kRot.SetEntry(1,1,NiCos(fRadius));
	kRot.SetEntry(1,2,0.0f);
	
	kRot.SetEntry(2,0,0.0f);
	kRot.SetEntry(2,1,0.0f);
	kRot.SetEntry(2,2,1.0f);*/
	

	//��ȯ�� ��ǥ�� ���� �߽����� �̵�
	kOrigin.x = kPoint.x - kAxis.x;
	kOrigin.y = kPoint.y - kAxis.y;
	kOrigin.z = 1.0f;

	float  x;
	float  y;

	x = kOrigin.x * NiCos(fRadius) + (-NiSin(fRadius) * kOrigin.y);
	y = kOrigin.x * NiSin(fRadius) + (NiCos(fRadius) * kOrigin.y);
	
	x *= -1.0f;
	y *= -1.0f;
	
	//���� ��ġ��.
	kPoint.x = x + kAxis.x;
	kPoint.y = y + kAxis.y;
	kPoint.z = 1.0f;
}

void CRadar::Update()
{

	const NiRenderTargetGroup* pkRTGroup = NiRenderer::GetRenderer()->
        GetDefaultRenderTargetGroup();
	
	unsigned int uiWidth  = pkRTGroup->GetWidth(0);
	unsigned int uiHeight = pkRTGroup->GetHeight(0);


	float fInvScrW = 1.0f / uiWidth;
    float fInvScrH = 1.0f / uiHeight;
	
	NiPoint3	kUserPos;
	float		fUserHeading;

	//��Ʈ��ũ �Ŵ������� �������� ���� �����̳ʸ� ������
	PlayersMap& mapPlayer = NetworkManager::GetInstance()->LockPlayers();
	
	DWORD dwSelfID = NetworkManager::GetInstance()->GetID();
	
	kUserPos = mapPlayer[ dwSelfID ].userInfo.pos;
	fUserHeading = mapPlayer[ dwSelfID ].userInfo.Heading;	
	
	//���� ���� ĳ���͸� �ֺ����� 60 x 60 ������ ����
	NiRect<int>		kRadarRect;

	kRadarRect.m_left  = (int) kUserPos.x - 30;
	kRadarRect.m_right = (int) kUserPos.x + 30;
	kRadarRect.m_top   = (int) kUserPos.y - 30;
	kRadarRect.m_bottom= (int) kUserPos.y + 30;

	//������ �ش�Ǵ� ���� ���� �����̳� ����
	m_vecMyTeamUnit.clear();
	m_vecEnemyUnit.clear();

	PlayersMap::iterator iterPlayer;
	for(iterPlayer = mapPlayer.begin();iterPlayer!=mapPlayer.end(); ++iterPlayer)
	{
		DWORD	selfCamp = mapPlayer[dwSelfID].userInfo.camp;
		//�ڱ� �ڽ��� ������ �ʿ� �����Ƿ� �н�
		if(iterPlayer->first == dwSelfID)// || iterPlayer->second.userInfo.camp != selfCamp )
			continue;
		
		//�ٸ� �÷��̾� ��ġ ������
		NiPoint3 kPoint = iterPlayer->second.userInfo.pos;
		if(FindUnit(kPoint,kRadarRect))
		{
			//unit�� �����ؼ� ���Ϳ� �߰�.
			RadarUnit radarUnit;
			radarUnit.kPos = kPoint;
			radarUnit.kTransformPos.x = kPoint.x + (60.0f - kUserPos.x);
			radarUnit.kTransformPos.y = kPoint.y + (60.0f - kUserPos.y);
			radarUnit.kTransformPos.z = 1.0f;
			//2���� ȸ�� ��ȯ �Լ�
			Rotate2D(NiPoint2(60.0f,60.0f),radarUnit.kTransformPos,fUserHeading);
//			
			//�Ʊ��� ������ ������ �����̳ʿ� ��´�.
			if(iterPlayer->second.userInfo.camp == selfCamp)
				m_vecMyTeamUnit.push_back(radarUnit);
			else
				m_vecEnemyUnit.push_back(radarUnit);				
		}
	}
		
	NetworkManager::GetInstance()->UnLockPlayers();	


	//Elements ����
	m_kArrayElements.RemoveAll();	//���� ���ҵ� ����

	//���̴��� �Ʊ� ǥ�� ����
	for(unsigned int i=0;i<m_vecMyTeamUnit.size();i++)
	{		
		//���� ���͸��� ������Ƽ
		NiMaterialProperty*		pkProp = NiNew NiMaterialProperty;
		assert(pkProp);

		pkProp->SetEmittance(NiColor(0.0f,0.0f,1.0f));
		pkProp->SetAlpha(0.5f);

		NiScreenElements* pkClonElement = (NiScreenElements* )m_spElementUnit->Clone();
		NiPoint3 kPos;
		kPos = m_vecMyTeamUnit[i].kTransformPos;
		
		pkClonElement->SetRectangle(0,0.0f,0.0f,2.0f * fInvScrW,2.0f * fInvScrH);
		pkClonElement->SetTranslate(kPos.y * fInvScrW ,kPos.x * fInvScrH,0.3f);
		pkClonElement->AttachProperty(pkProp);
		pkClonElement->UpdateProperties();
		pkClonElement->Update(0.0f);

		m_kArrayElements.AddFirstEmpty(pkClonElement);
	}

	//���̴��� ���� ǥ��
	for(unsigned int i=0;i<m_vecEnemyUnit.size();i++)
	{		
		//���� ���͸��� ������Ƽ
		NiMaterialProperty*		pkProp = NiNew NiMaterialProperty;
		assert(pkProp);

		pkProp->SetEmittance(NiColor(1.0f,0.0f,0.0f));
		pkProp->SetAlpha(0.5f);

		NiScreenElements* pkClonElement = (NiScreenElements* )m_spElementUnit->Clone();
		NiPoint3 kPos;
		kPos = m_vecEnemyUnit[i].kTransformPos;
		
		pkClonElement->SetRectangle(0,0.0f,0.0f,2.0f * fInvScrW,2.0f * fInvScrH);
		pkClonElement->SetTranslate(kPos.y * fInvScrW ,kPos.x * fInvScrH,0.3f);
		pkClonElement->AttachProperty(pkProp);
		pkClonElement->UpdateProperties();
		pkClonElement->Update(0.0f);

		m_kArrayElements.AddFirstEmpty(pkClonElement);
	}

}

bool CRadar::FindUnit(NiPoint3 p,NiRect<int> kRect)
{
	if( (int)p.x < kRect.m_left )  return false;
	if( (int)p.x > kRect.m_right)  return false;
	if( (int)p.y < kRect.m_top)    return false;
	if( (int)p.y > kRect.m_bottom) return false;

	return true;
}

