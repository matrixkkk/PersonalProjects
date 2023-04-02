#include "Respone.h"


CRespone::CRespone()
{
	unsigned int uiSrand = (unsigned int)NiGetCurrentTimeInSec();
	NiSrand(uiSrand);
}

CRespone::~CRespone()
{
	m_US_StartPos.clear();
	m_EU_Region03.clear();
	m_US_Region03.clear();
	m_Public_Region02.clear();
	m_Gerilla_Respone.clear();
	m_EU_Region01.clear();
	m_US_Region01.clear();
}

bool CRespone::Create()
{
	//US 시작점 위치셋
	m_US_StartPos.push_back(NiPoint3(81.04f,91.77f,1.0f));
	m_US_StartPos.push_back(NiPoint3(79.61f,91.68f,1.0f));
	m_US_StartPos.push_back(NiPoint3(77.96f,91.92f,1.0f));
	m_US_StartPos.push_back(NiPoint3(76.53f,92.28f,1.0f));
	m_US_StartPos.push_back(NiPoint3(81.68f,93.63f,1.0f));
	m_US_StartPos.push_back(NiPoint3(79.87f,93.63f,1.0f));
	m_US_StartPos.push_back(NiPoint3(78.52f,93.86f,1.0f));
	m_US_StartPos.push_back(NiPoint3(76.89f,94.16f,1.0f));

	//EU 시작점 위치 셋
	m_EU_StartPos.push_back(NiPoint3(-63.0f,-90.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-61.0f,-89.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-60.0f,-89.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-58.0f,-88.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-62.0f,-91.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-61.0f,-91.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-59.0f,-90.0f,5.0f));
	m_EU_StartPos.push_back(NiPoint3(-58.0f,-90.0f,5.0f));

	//EU_Region01 위치 셋
	m_EU_Region01.push_back(NiPoint3(57.17f,10.98f,3.0f));
	m_EU_Region01.push_back(NiPoint3(57.95f,9.78f,3.0f));
	m_EU_Region01.push_back(NiPoint3(59.13f,8.83f,3.0f));
	m_EU_Region01.push_back(NiPoint3(57.66f,8.36f,3.0f));
	m_EU_Region01.push_back(NiPoint3(56.49f,9.72f,3.0f));
		
	//US_Region01 위치 셋
	m_US_Region01.push_back(NiPoint3(88.65f,42.27f,1.0f));
	m_US_Region01.push_back(NiPoint3(92.55f,52.87f,1.0f));
	m_US_Region01.push_back(NiPoint3(92.10f,55.76f,1.0f));
	m_US_Region01.push_back(NiPoint3(78.16f,56.92f,1.0f));
	m_US_Region01.push_back(NiPoint3(78.88f,53.23f,1.0f));
	
	//EU_Resion03 위치 셋
	m_EU_Region03.push_back(NiPoint3(-6.45f,-92.73f,5.0f));
	m_EU_Region03.push_back(NiPoint3(-5.39f,-91.93f,5.0f));
	m_EU_Region03.push_back(NiPoint3(-4.07f,-91.0f,5.0f));
	m_EU_Region03.push_back(NiPoint3(-5.34f,-94.0f,5.0f));
	m_EU_Region03.push_back(NiPoint3(-4.34f,-93.31f,5.0f));
	m_EU_Region03.push_back(NiPoint3(-3.18f,-92.43f,5.0f));
	
	//US_Region32 위치 셋
	m_US_Region03.push_back(NiPoint3(30.0f,-65.0f,5.0f));
	m_US_Region03.push_back(NiPoint3(31.0f,-64.0f,5.0f));
	m_US_Region03.push_back(NiPoint3(33.14f,-63.04f,5.0f));
	m_US_Region03.push_back(NiPoint3(31.4f,-66.41f,5.0f));
	m_US_Region03.push_back(NiPoint3(32.47f,-65.34f,5.0f));
	m_US_Region03.push_back(NiPoint3(33.82f,-64.51f,5.0f));
	
	//Public_Region02
	m_Public_Region02.push_back(NiPoint3(-23.52f,53.77f,5.0f));
	m_Public_Region02.push_back(NiPoint3(-24.12f,54.97f,5.0f));
	m_Public_Region02.push_back(NiPoint3(-24.38f,56.26f,5.0f));
	m_Public_Region02.push_back(NiPoint3(-24.81f,53.0f,5.0f));
	m_Public_Region02.push_back(NiPoint3(-25.28f,54.0f,5.0f));
	m_Public_Region02.push_back(NiPoint3(-25.42f,56.14f,5.0f));

	//게릴라 랜덤 리스폰 지역
	m_Gerilla_Respone.push_back(NiPoint3(-65.82f,1.39f,5.0f));
	m_Gerilla_Respone.push_back(NiPoint3(-23.32f,-17.22f,5.0f));
	m_Gerilla_Respone.push_back(NiPoint3(38.19f,-38.05f,5.0f));
	m_Gerilla_Respone.push_back(NiPoint3(44.35f,-6.78f,1.0f));
	m_Gerilla_Respone.push_back(NiPoint3(10.41f,50.57f,1.0f));
	m_Gerilla_Respone.push_back(NiPoint3(50.47f,53.87f,1.0f));
	
		
	return true;
}


NiPoint3 CRespone::GetStartPosition(DWORD eCamp)
{
	NiPoint3	returnPosition;

	unsigned int vecSize = 0;
	switch(eCamp)
	{
	case nsPlayerInfo::EU:
		{
			vecSize = m_EU_StartPos.size();
			int randNum = NiRand() % vecSize;

			returnPosition = m_EU_StartPos[randNum];
		}
		break;	
	case nsPlayerInfo::USA :
		{
			vecSize = m_US_StartPos.size();
			int randNum = NiRand() % vecSize;

			returnPosition = m_US_StartPos[randNum];
		}
		break;

	default:
		{
			vecSize = m_Gerilla_Respone.size();
			int randNum = NiRand() % vecSize;

			returnPosition = m_Gerilla_Respone[randNum];
		}
		break;
	}
	
	return returnPosition;
}

//	eCamp = 종족, nterritory = 현재 점령한 지역
NiPoint3	CRespone::GetResponePosition( BYTE eCamp, BYTE nterritory )
{
	NiPoint3	returnPosition;

	unsigned int vecSize = 0;
	
	switch(eCamp)
	{
	case nsPlayerInfo::EU :
		{
			switch( nterritory )
			{
			case nsPlayerInfo::TERRITORY_1 :
				{
					vecSize = m_EU_Region01.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_EU_Region01[randNum];
				}
				break;
			case nsPlayerInfo::TERRITORY_2 :
				{
					vecSize = m_Public_Region02.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_Public_Region02[randNum];
				}
				break;
			case nsPlayerInfo::TERRITORY_3 :
				{
					vecSize = m_EU_Region03.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_EU_Region03[randNum];
				}
				break;
			}
		}
		break;	

	case nsPlayerInfo::USA :
		{
			switch( nterritory )
			{
			case nsPlayerInfo::TERRITORY_1 :
				{
					vecSize = m_US_Region01.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_US_Region01[randNum];
				}
				break;
			case nsPlayerInfo::TERRITORY_2 :
				{
					vecSize = m_Public_Region02.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_Public_Region02[randNum];
				}
				break;
			case nsPlayerInfo::TERRITORY_3  :
				{
					vecSize = m_US_Region03.size();			
					int randNum = NiRand() % vecSize;
					returnPosition = m_US_Region03[randNum];
				}
				break;
			}
		}
		break;

	default:
		{
			vecSize = m_Gerilla_Respone.size();
			int randNum = NiRand() % vecSize;

			returnPosition = m_Gerilla_Respone[randNum];
		}
		break;
	}
	
	return returnPosition;	

}