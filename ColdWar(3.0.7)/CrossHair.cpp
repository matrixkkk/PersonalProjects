#include "CrossHair.h"

CrossHair::CrossHair() : m_kScreenCross(5)
{
	m_kCrossRect = NiRect<int>::NiRect(WINDOWWIDTH/2 - 5, WINDOWHEIGHT/2 - 5
		,WINDOWWIDTH/2 + 5,WINDOWHEIGHT/2 + 5);	//크로스헤어 영역 초기화 default 센터를 중심으로 10x10
	
	m_fInvScrW = 0.0f;
	m_fInvScrH = 0.0f;

	m_iWidth = 0;  
	m_iHeight = 0;

}

CrossHair::~CrossHair()
{

}

bool CrossHair::Create()
{
	const NiRenderTargetGroup* pkRTGroup = NiRenderer::GetRenderer()->
        GetDefaultRenderTargetGroup();
	
	m_iWidth  = pkRTGroup->GetWidth(0);
	m_iHeight = pkRTGroup->GetHeight(0);


	m_fInvScrW = 1.0f / m_iWidth;
    m_fInvScrH = 1.0f / m_iHeight;

	//크로스헤어 색상 설정
	NiMaterialProperty* pkProp = NiNew NiMaterialProperty;
	assert(pkProp);

	pkProp->SetEmittance(NiColor(0.0f,1.0f,0.0f));
	pkProp->SetAlpha(0.5f);

	
	NiScreenElements* pkCenter = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(pkCenter);
	pkCenter->SetName("Cross_Center");
	pkCenter->Insert(4);
	
	pkCenter->SetRectangle(0,0.0f,0.0f, 1.0f * m_fInvScrW, 1.0f * m_fInvScrH);
	pkCenter->SetTranslate(0.5f,0.5f,0.3f);
	pkCenter->AttachProperty(pkProp);

	pkCenter->UpdateProperties();
	pkCenter->Update(0.0f);
	m_kScreenCross.AddFirstEmpty(pkCenter);

	
	NiScreenElements* pkTop = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(pkTop);
	pkTop->SetName("Cross_Top");
	pkTop->Insert(4);
	
	pkTop->SetRectangle(0,0.0f,0.0f,CROSS_SHORT * m_fInvScrW,CROSS_LONG * m_fInvScrH);
	pkTop->SetTranslate(0.5f,0.4f,0.3f);
	pkTop->AttachProperty(pkProp);

	pkTop->UpdateProperties();
	pkTop->Update(0.0f);
	m_kScreenCross.AddFirstEmpty(pkTop);	

	NiScreenElements* pkLeft = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(pkLeft);
	pkLeft->SetName("Cross_Left");
	pkLeft->Insert(4);
	
	pkLeft->SetRectangle(0,0.0f,0.0f,CROSS_LONG * m_fInvScrW,CROSS_SHORT * m_fInvScrH);
	pkLeft->SetTranslate(0.4f,0.5f,0.3f);
	pkLeft->AttachProperty(pkProp);

	pkLeft->UpdateProperties();
	pkLeft->Update(0.0f);
	m_kScreenCross.AddFirstEmpty(pkLeft);

	NiScreenElements* pkRight = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(pkRight);
	pkRight->SetName("Cross_Right");
	pkRight->Insert(4);
	
	pkRight->SetRectangle(0,0.0f,0.0f,CROSS_LONG * m_fInvScrW,CROSS_SHORT * m_fInvScrH);
	pkRight->SetTranslate(0.6f,0.5f,0.3f);
	pkRight->AttachProperty(pkProp);

	pkRight->UpdateProperties();
	pkRight->Update(0.0f);
	m_kScreenCross.AddFirstEmpty(pkRight);

	NiScreenElements* pkBottom = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,0));
	assert(pkBottom);
	pkBottom->SetName("Cross_Bottom");
	pkBottom->Insert(4);
	
	pkBottom->SetRectangle(0,0.0f,0.0f,CROSS_SHORT * m_fInvScrW,CROSS_LONG * m_fInvScrH);
	pkBottom->SetTranslate(0.5f,0.6f,0.3f);
	pkBottom->AttachProperty(pkProp);

	pkBottom->UpdateProperties();
	pkBottom->Update(0.0f);
	m_kScreenCross.AddFirstEmpty(pkBottom);

	return true;
}

void CrossHair::Update(float fAccuracy)
{
	//크로스 헤어 최대값 지정 200 x 200
	if(fAccuracy >= 100)
		fAccuracy = 100;

	m_kCrossRect = NiRect<int>::NiRect( (int) (m_iWidth/2 - fAccuracy),
										(int) (m_iWidth/2 + fAccuracy),
										(int) (m_iHeight/2 - fAccuracy),
										(int) (m_iHeight/2 + fAccuracy));
		
	//크로스 헤어 위치 업데이트
	//- 원소 순서 : center, top, left, right, bottom
	//top
	NiScreenElements* pkElement = m_kScreenCross.GetAt(1);
	pkElement->SetTranslate( 0.5f , (m_kCrossRect.m_top - CROSS_LONG)* m_fInvScrH,0.3f);
	pkElement->Update(0.0f);

	//left
	pkElement = m_kScreenCross.GetAt(2);
	pkElement->SetTranslate( (m_kCrossRect.m_left - CROSS_LONG) * m_fInvScrW, 0.5f,0.3f);
	pkElement->Update(0.0f);

	//right
	pkElement = m_kScreenCross.GetAt(3);
	pkElement->SetTranslate( m_kCrossRect.m_right * m_fInvScrW, 0.5f, 0.3f);
	pkElement->Update(0.0f);

	//bottom
	pkElement = m_kScreenCross.GetAt(4);
	pkElement->SetTranslate( 0.5f , m_kCrossRect.m_bottom * m_fInvScrH,0.3f);
	pkElement->Update(0.0f);
	
}

void CrossHair::Draw()
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();

	if(m_kScreenCross.GetSize() == 0)
		return ;

	for(unsigned int i=0;i<m_kScreenCross.GetSize();i++)
	{
		NiScreenElements* pkElements = m_kScreenCross.GetAt(i);
		if(pkElements)
			pkElements->Draw(pkRenderer);
	}
}


NiRect<int> CrossHair::GetCrossRect()
{
	return m_kCrossRect;
}
