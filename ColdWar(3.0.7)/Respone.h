/*
	Ŭ���� �̸� : CRespone
	Ŭ���� �뵵 : ĳ������ ���� ��ġ �� �׾��� ����� ������ ��ġ�� ���� �Ѵ�.

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
	
	//���� ���� ��ġ ��
	//NiTransform		GetStartPosition(DWORD eCamp);
	NiPoint3	GetStartPosition(DWORD eCamp);
	NiPoint3	GetResponePosition(BYTE eCamp, BYTE nterritory );


private:
	//private �Լ�

	std::vector<NiPoint3>	m_US_StartPos;		//US�� ������
	std::vector<NiPoint3>	m_EU_StartPos;		//EU�� ������

	std::vector<NiPoint3>	m_EU_Region01;		//1���� EU�� ������ ����
	std::vector<NiPoint3>	m_US_Region01;		//1���� US�� ������ ����

	std::vector<NiPoint3>	m_EU_Region03;		//3���� EU ����������
	std::vector<NiPoint3>	m_US_Region03;		//3���� US ����������
	
	std::vector<NiPoint3>	m_Public_Region02;	//2���� ���� ������ ����
	std::vector<NiPoint3>	m_Gerilla_Respone;	//�Ը��� ���� ������ ����

	//void	CreateRespones(std::vector<NiNode*>& vecPos,NiNode* pkNode);		//������ ��ġ, �������� �����̳ʿ� ����

	//NiNodePtr	m_spRespone;
	
	//std::vector<NiNode*>	m_vecEu_StartPos;	//Eu ������
};

#endif