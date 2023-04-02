//----------------------------------------------------------
//  File: GameApp.h 
//
//  Desc: 게임 렌더링 엔진 클래스, 전체적인 게임의 렌더링 및
//		  입출력을 담당한다.
//  written by 김현구
//-------------------------------------------------------------  

#ifndef GAMEAPP_H_
#define GAMEAPP_H_

#include <NiFont.h>
#include <NiString2D.h>
#include <NiColor.h>
#include "main.h"
#include "GameState.h"
#include "NiDX9RendererEx.h"
#include "SoundManager.h"
#include "resource4.h"

//------------- UI -------------
#include "CBaseState.h"
#include "CStateManager.h"
#include "CEGUIGBRenderSystem.h"
//------------------------------

NiSmartPointer(NiFastAlphaAccumulator);

class CGameApp : public NiMemObject
{
public:
	//------------------------------------------
	// 생성자 및 소멸자
	//------------------------------------------
	CGameApp();
	~CGameApp();

	//------------------------------------------
	// public 변수
	//-----------------------------------------

	static CGameApp* mp_Appication;					//어플리케이션 접근을 위한 정적 포인터
	
	//-------------------------------------------
	// public 함수 
    //-------------------------------------------
	
	
	//	MessageProc
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MainWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	
	//Media Path
	static void		SetMediaPath(const char* pcPath);			//미디어  파일 경로
	const char*		GetMediaPath();
	static const char*		ConvertMediaFilename(const char* pcFilename);
	static void GetMinMax( NiAVObject* pkObject, NiPoint3 *min, NiPoint3 *max );

	//	오브젝트의 투명정도를 변경
	static void ChangeNodeAlpha(NiAVObject *pkObject, float fAlpha);	

	// 렌더러
	NiRenderer*		GetRenderer()		 { return m_spRenderer;}
	
	//InputSystem
	NiInputSystem*	GetInputSystem() { return  m_spInputSystem; }  
	unsigned int	GetAppWidth()	 { return  m_nWidth; }
	unsigned int	GetAppHeight()	 { return  m_nHeight; }
	HWND			GetAppHandle()	 { return  m_hWnd;	}
	float			GetAccumTime()	 { return  m_fAccumTime; }
	NiCursor*		GetCursor()		 { return  m_spCursor; }
	CBaseState*		GetState()		 { return  m_CurrState; }
	void			SetState(CBaseState* pState){ m_CurrState = pState; }
	
	//윈도우 초기화 함수
	bool		Initialize();
	bool		CreateMainWindow(LPCTSTR lpCaption,HINSTANCE hInstance, int iWinMode,bool bFullScreen=false);
	bool		ShutDown();

	// 어플리케이션 시작
	int			BeginGame();

	
	//idle processing
	void		OnIdle();				//메인 루프
	void		RenderFrame();			//프레임 렌더
	void		UpdateFrame();			//업데이트	
	void		ProcessInput();			//키 입력 처리
	void		SetInputMode();			//인풋 모드 셋팅
	
	void		MoveCursor(int x,int y);//커서 이동
//	bool		ChangeState(CGameState* state);
	
	NiColor		GetBackGroundColor();

	void		QuitApplication();		// 어플리케이션 종료
	float		GetFrameTime(){ return m_fFrameTime; }

	bool IsIME(){ return m_bIME; }		// IME 활성 여부
	void SetIME( bool bIme ){ m_bIME = bIme; }

	bool IsActiveEditBox(){ return m_bActiveEditBox; }
	void SetActiveEditBox( bool bActive ){ m_bActiveEditBox = bActive; }

	const char*	GetFontName() const;
	
	// 스테이트 관리자
	inline static CStateManager &GetStateManager()
	{
		return(mp_Appication->m_StateManager);
	}

	//----- 스레드 관련 함수
	void		EnterCS();
	void		LeaveCS();
	
private:
	//---------------------------------------------------
	// private 함수
	//---------------------------------------------------
	bool		CreateRenderer();				//렌더러 생성
	bool		CreateInputSystem();			//인풋 시스템 생성
	bool		CreateCursor();					//커서 생성
	//bool		CreateFont();					//폰트 객체 생성
	bool		CreateSound();					//사운드 매니저

	bool		MeasureTime();					//Idle루프 시간 측정

	void		SetMaxFrameRate(float fMax);	//프레임율 제한

	bool		InitializeGUIFramework();		// 게임브리오를 위한 CEGUI 렌더러 설정
	void		RegisterBaseStates();			// 게임 스테이트 상태 생성 및 등록

	void		IMEComposition( HWND hWnd, LPARAM lParam );	// IME 상태 한글처리 함수
	
	//-----------------------------------------------------
	// private 변수
	//-----------------------------------------------------
			
	//현재 상태 지정변수
	CBaseState*			m_CurrState;
	
	//기본 오브젝트
	NiRendererPtr		m_spRenderer;
	NiInputSystemPtr	m_spInputSystem;

	CStateManager			m_StateManager;					// 스테이트 관리자
	CEGUIGBRenderSystemPtr	m_spCEGUIGBRenderer;			// 게임브리오용 GEGUI 렌더러

	NiCursorPtr				m_spCursor;
		
	//---------------------------------------------------------
	//윈도우 생성 변수
	NiWindowRef			 m_hWnd;							//메인 윈도우 핸들
	NiWindowRef			 m_RenderWnd;						//렌더 윈도우 핸들
	NiInstanceRef		 m_hInst;
	HMENU				 m_hMenu;							//메뉴 ID
	HICON				 m_hIcon;							//아이콘
	
	NiAcceleratorRef     ms_pAccel;
	
	bool				 m_bFullScreen;						//풀스크린
	bool				 m_bExclusiveMouse;					//마우스 소유 제어 타입
	bool				 m_bIsHangul;						//영한 여부
	bool				 m_bActiveEditBox;					//에디트 박스 사용 여부
	unsigned long	     m_ulWindowStyle;					//윈도우 스타일
	unsigned int		 m_nWidth;							//풀 스크린 일때의 width픽셀값
	unsigned int		 m_nHeight;							//풀 스크린 일때의 height의 픽셀값

	//---------------------------------------------------------

	//---------------------------------------------------------
	//시간 관련 변수
	
	int m_iMaxTimer;
    int m_iTimer;
    float m_fCurrentTime; // Time reported by system
    float m_fLastTime; // Last time reported by system
    float m_fAccumTime; // Time elapsed since application start
    float m_fFrameTime; // Time elapsed since previous frame
    int m_iClicks;
    float m_fFrameRate;
    float m_fLastFrameRateTime;
    int m_iLastFrameRateClicks;

	float m_fCullTime;
    float m_fRenderTime;
    float m_fUpdateTime;
    float m_fBeginUpdate;
    float m_fBeginCull;
    float m_fBeginRender;

	// 프레임 락을 위한 변수
    float m_fMinFramePeriod;
    float m_fLastFrame;
	
	CRITICAL_SECTION	m_cs;


	//---------------------------------------------------------
	
	//화면에 문자 출력
	NiFontPtr			m_spFont;
	NiString2DPtr		m_spText;

	std::string			m_strFont;

	NiColor				m_BackGroundColor;	// 백그라운드 컬러

protected:
	//미디어 파일 경로 
    static char ms_acMediaPath[NI_MAX_PATH];
    static char ms_acTempMediaFilename[NI_MAX_PATH];

	static bool			 m_bIME;							//IME 여부
	
};



#endif
