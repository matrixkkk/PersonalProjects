#include "main.h"
#include "CLoadingDisplay.h"
#include "GameApp.h"


CLoadingDisplay*	CLoadingDisplay::m_spLoadingDisplay = 0;

CLoadingDisplay::CLoadingDisplay() :
m_spSplashBackGround(0), 
m_spLoadingBar(0)
{

}

CLoadingDisplay::CLoadingDisplay(	const int &nXPos, const int &nYPos, 
									const int &nLBWidth, const int &nLBHeight,
									const NiFixedString& strSplash, const NiFixedString& strLoadingBar) :

m_nStart_XPos(nXPos),
m_nStart_YPos(nYPos),
m_nLoadingBarWidth(nLBWidth),
m_nLoadingBarHeight(nLBHeight),
m_strSplash(strSplash),
m_strLoadingBar(strLoadingBar)
{
	Intialize();
}


CLoadingDisplay::~CLoadingDisplay()
{
	m_spSplashBackGround = 0;
	m_spLoadingBar = 0;
}

CLoadingDisplay*	CLoadingDisplay::GetInstance()
{
	if( m_spLoadingDisplay )
	{
		return m_spLoadingDisplay;
	}

	return NULL;
}

bool CLoadingDisplay::Create(const int &nXPos, const int &nYPos, 
							 const int &nLBWidth, const int &nLBHeight,
							 const NiFixedString& strSplash, const NiFixedString& strLoadingBar)
{
	if( !m_spLoadingDisplay )
	{
		m_spLoadingDisplay = NiNew CLoadingDisplay( nXPos, nYPos, nLBWidth, nLBHeight, strSplash, strLoadingBar);

		return true;
	}

	return false;
}

/* Gamebryo식
  0.0, 0.0     1.0, 0.0
  0.0, 1.0     1.0, 1.0
  DirectX rhw식
  0, 0         width,0
  0, height    width,height
 */
 
bool CLoadingDisplay::Intialize()
{

	NiRenderer *pRenderer = NiRenderer::GetRenderer();

	m_nWidth	= pRenderer->GetDefaultBackBuffer()->GetWidth();
	m_nHeight	= pRenderer->GetDefaultBackBuffer()->GetHeight();

	m_spSplashBackGround = NiNew NiScreenElements( NiNew NiScreenElementsData(false,false,1) ); 

	if( !m_spSplashBackGround )
		goto End;
		
	m_spSplashBackGround->Insert(4); 
	m_spSplashBackGround->SetRectangle( 0, 0.0f, 0.0f, 1.0f, 1.0f ); 
	m_spSplashBackGround->SetTextures( 0, 0,  0.0f, 0.0f, 1.0f, 1.0f ); 


	CGameApp::SetMediaPath("Data/Texture/");
	NiTexturingProperty* pkTex = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(m_strSplash));

	// 텍스쳐의 어드레스 모드
	pkTex->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	pkTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	m_spSplashBackGround->AttachProperty(pkTex);

	// 알파
	NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty;
	pkAlpha->SetAlphaBlending(false);
	m_spSplashBackGround->AttachProperty(pkAlpha);

	m_spSplashBackGround->UpdateProperties();
	m_spSplashBackGround->UpdateBound();   // Bound설정
	m_spSplashBackGround->Update(0.0f);

	m_spLoadingBar = NiNew NiScreenElements(NiNew NiScreenElementsData(false,false,1)); 

	if( !m_spLoadingBar )
		goto End;

	unsigned int uiTexWidth	= pkTex->GetBaseTexture()->GetWidth();
	unsigned int uiTexHeight= pkTex->GetBaseTexture()->GetHeight();

	m_fLeft		= static_cast<float>(m_nStart_XPos) / static_cast<float>(uiTexWidth);							//static_cast<float>(m_nWidth);
	m_fTop		= static_cast<float>(m_nStart_YPos - m_nLoadingBarHeight) / static_cast<float>(uiTexHeight);	//static_cast<float>(m_nHeight);
	m_fWidth	= static_cast<float>(m_nLoadingBarWidth) / static_cast<float>(uiTexWidth);
	m_fHeight	= static_cast<float>(m_nLoadingBarHeight) / static_cast<float>(uiTexHeight);
		
	m_spLoadingBar->Insert(4); 
	m_spLoadingBar->SetRectangle( 0, m_fLeft, m_fTop, 0.0f, m_fHeight ); 
	m_spLoadingBar->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );

	pkTex = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(m_strLoadingBar));

	// 텍스쳐의 어드레스 모드
	pkTex->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	pkTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
	m_spLoadingBar->AttachProperty(pkTex);

	// 알파
	pkAlpha = NiNew NiAlphaProperty;
	pkAlpha->SetAlphaBlending(false);
	m_spLoadingBar->AttachProperty(pkAlpha);

	m_spLoadingBar->UpdateProperties();
	m_spLoadingBar->UpdateBound();   // Bound설정
	m_spLoadingBar->Update(0.0f);

	return true;

End:
	if( m_spSplashBackGround ) m_spSplashBackGround = 0;
	if( m_spLoadingBar ) m_spLoadingBar = 0;

	return false;	
}


	
void CLoadingDisplay::Destroy()
{
	if( m_spLoadingDisplay ) m_spLoadingDisplay = 0;
}

void	CLoadingDisplay::SetBackGround( const NiFixedString& strSplash )
{
	if( m_spSplashBackGround ) return;

	m_strSplash = strSplash;

	CGameApp::SetMediaPath("Data/Texture/");
	NiTexturingProperty* pkTex = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(m_strSplash));

	// 텍스쳐의 어드레스 모드
	pkTex->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	pkTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);

	m_spSplashBackGround->AttachProperty(pkTex);

	m_spSplashBackGround->UpdateProperties();
	m_spSplashBackGround->Update(0.0f);
}

void	CLoadingDisplay::SetLoadingBar( const NiFixedString& strLoadingBar )
{
	if( !m_spLoadingBar) return;

	m_strLoadingBar = strLoadingBar;

	CGameApp::SetMediaPath("Data/Texture/");
	NiTexturingProperty* pkTex = NiNew NiTexturingProperty( CGameApp::ConvertMediaFilename(m_strLoadingBar));

	// 텍스쳐의 어드레스 모드
	pkTex->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
	pkTex->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);

	m_spLoadingBar->AttachProperty(pkTex);

	m_spLoadingBar->UpdateProperties();
	m_spLoadingBar->Update(0.0f);

}

// m_fWidth	 목표 크기
// fProgress 목표 퍼센트지 Ex) 10%, 20%
void	CLoadingDisplay::UpdateLoadingBar( float fProgress )
{
	float fWidth = (m_fWidth * fProgress) / 100.0f;

	m_spLoadingBar->SetRectangle( 0, m_fLeft, m_fTop, fWidth, m_fHeight ); 
	m_spLoadingBar->UpdateBound();
	m_spLoadingBar->Update( 0.0f );
}

void	CLoadingDisplay::RenderClick()
{
	NiRenderer *pRenderer = NiRenderer::GetRenderer();

	if( NULL == pRenderer )
		return;

	pRenderer->BeginFrame();
	
	pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL);

	 pRenderer->SetScreenSpaceCameraData();

	m_spSplashBackGround->RenderImmediate( pRenderer );
	m_spLoadingBar->RenderImmediate( pRenderer );

    pRenderer->EndUsingRenderTargetGroup();	

    pRenderer->EndFrame();

	pRenderer->DisplayFrame();	
}


void CLoadingDisplay::Render( float fProgress )
{
	UpdateLoadingBar( fProgress );

	RenderClick();
}

