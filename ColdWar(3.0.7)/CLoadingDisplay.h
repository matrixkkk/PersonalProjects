#ifndef CLODINGDISPLAY_H
#define CLODINGDISPLAY_H

#include <NiMain.h>
#include <NiScreenElements.h>


// 일단 작업 이미지의 기준 크기를 1024, 768 해상도로 정한다.

class CLoadingDisplay : public NiRefObject
{

	NiScreenElementsPtr	m_spSplashBackGround;
	NiScreenElementsPtr	m_spLoadingBar;
	
	static CLoadingDisplay*	m_spLoadingDisplay;

	int m_nStart_XPos;
	int m_nStart_YPos;

	int	m_nLoadingBarWidth;
	int	m_nLoadingBarHeight;

	int	m_nWidth;
	int	m_nHeight;	

	float m_fLeft, m_fTop, m_fWidth, m_fHeight;

	NiFixedString m_strSplash;
	NiFixedString m_strLoadingBar;
	
public:

	static CLoadingDisplay*	GetInstance();

	static bool Create( const int &nXPos, const int &nYPos, 
						const int &nLBWidth, const int &nLBHeight,
						const NiFixedString& strSplash, const NiFixedString& strLoadingBar );

	static void Destroy();

	virtual bool Intialize();

	void	SetBackGround( const NiFixedString& strSplash );
	void	SetLoadingBar( const NiFixedString& strLoadingBar );
	void	SetPosition( int &nXpos, int &nYpos );

	virtual void	UpdateLoadingBar( float fProgress );
	virtual void	RenderClick();
	virtual void	Render( float fProgress );

protected:

	CLoadingDisplay();
	CLoadingDisplay(const int &nXPos, const int &nYPos, 
					const int &nLBWidth, const int &nLBHeight,
					const NiFixedString& strSplash, const NiFixedString& strLoadingBar);

	CLoadingDisplay( CLoadingDisplay& );
	~CLoadingDisplay();

};

NiSmartPointer(CLoadingDisplay);


#endif