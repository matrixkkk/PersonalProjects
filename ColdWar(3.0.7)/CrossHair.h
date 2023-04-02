/*
	Ŭ������	: CrossHair
	Ŭ���� �뵵 : ũ�ν��� �����ϰ� ũ�ν��� �׷��ش�. 
*/

#ifndef CROSSHAIR_H_
#define CROSSHAIR_H_

#include "main.h"

#define CROSS_SHORT		1.0f
#define CROSS_LONG		10.0f

class CrossHair : public NiMemObject
{
private:
	NiScreenElementsArray	m_kScreenCross;		//ũ�ν���� �׸� ������Ʈ �׷�

	NiRect<int>			    m_kCrossRect;					//ũ�ν���� ����

	unsigned int m_iWidth;  
	unsigned int m_iHeight;
	
	float m_fInvScrW;							//��ũ�� width�� 1.0�� �ִ�ġ�� �� ��
	float m_fInvScrH;
	
public:
	CrossHair();
	~CrossHair();

	bool Create();
	void Update(float fAccuracy);
	void Draw();
	void SetColor(NiColor color);		//ũ�ν������ ������ ���� default ����

	NiRect<int> GetCrossRect();			//ũ�ν���� ������ ������
	
};

#endif
