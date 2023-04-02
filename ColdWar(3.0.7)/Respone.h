/*
	클래스 이름 : CRespone
	클래스 용도 : 캐릭터의 시작 위치 및 죽었을 경우의 리스폰 위치를 관리 한다.

*/

#ifndef CRESPONE_H_
#define CRESPONE_H_

#include "main.h"
#include <vector>

class CRespone : public NiMemObject
{
public:
	CRespone();
	~CRespone();

	bool	Create();
	
	//시작 랜덤 위치 줌
	//NiTransform		GetStartPosition(DWORD eCamp);
	NiPoint3	GetStartPosition(DWORD eCamp);
	NiPoint3	GetResponePosition(BYTE eCamp, BYTE nterritory );


private:
	//private 함수

	std::vector<NiPoint3>	m_US_StartPos;		//US군 시작점
	std::vector<NiPoint3>	m_EU_StartPos;		//EU군 시작점

	std::vector<NiPoint3>	m_EU_Region01;		//1지역 EU군 리스폰 지역
	std::vector<NiPoint3>	m_US_Region01;		//1지역 US군 리스폰 지역

	std::vector<NiPoint3>	m_EU_Region03;		//3지역 EU 리스폰지역
	std::vector<NiPoint3>	m_US_Region03;		//3지역 US 리스폰지역
	
	std::vector<NiPoint3>	m_Public_Region02;	//2지역 공통 리스폰 지역
	std::vector<NiPoint3>	m_Gerilla_Respone;	//게릴라 랜덤 리스폰 지역

	//void	CreateRespones(std::vector<NiNode*>& vecPos,NiNode* pkNode);		//리스폰 위치, 진영별로 컨테이너에 정리

	//NiNodePtr	m_spRespone;
	
	//std::vector<NiNode*>	m_vecEu_StartPos;	//Eu 시작점
};

#endif