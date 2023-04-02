/*
	클래스명	: CrossHair
	클래스 용도 : 크로스헤어를 정의하고 크로스헤어를 그려준다. 
*/

#ifndef CROSSHAIR_H_
#define CROSSHAIR_H_

#include "main.h"

#define CROSS_SHORT		1.0f
#define CROSS_LONG		10.0f

class CrossHair : public NiMemObject
{
private:
	NiScreenElementsArray	m_kScreenCross;		//크로스헤어 그릴 엘리먼트 그룹

	NiRect<int>			    m_kCrossRect;					//크로스헤어 영역

	unsigned int m_iWidth;  
	unsigned int m_iHeight;
	
	float m_fInvScrW;							//스크린 width를 1.0을 최대치로 한 값
	float m_fInvScrH;
	
public:
	CrossHair();
	~CrossHair();

	bool Create();
	void Update(float fAccuracy);
	void Draw();
	void SetColor(NiColor color);		//크로스헤어의 색갈을 변경 default 연두

	NiRect<int> GetCrossRect();			//크로스헤어 영역을 가져옴
	
};

#endif
