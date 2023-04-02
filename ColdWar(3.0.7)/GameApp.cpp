#include "GameApp.h"

#include "Weapon.h"
#include "ObjectPick.h"

//------------- UI -------------
#include "CEGUIGBRenderSystem.h"
#include "CStartSplashState.h"
#include "CCreditState.h"
#include "CMenuState.h"
#include "CGameRoomState.h"
#include "CGameResultState.h"
#include "CSinglePlayState.h"
#include "CGamePlayState.h"
#include "COptionState.h"
//------------------------------
#include "CTerritory.h"


#include <CEGUIDefaultResourceProvider.h>

#include <NSBShaderLib.h>
#include <NSBShader.h>
#include <NSFParserLib.h>

int __cdecl IsLeadByte(int _C) {return ((unsigned char)(_C) & 0x80); }

/*
#pragma comment(lib,"NiBinaryShaderLibDX9.lib")
#pragma comment(lib,"NSBShaderLibDX9.lib")
#pragma comment(lib,"NSFParserLibDX9.lib")

*/

bool CGameApp::m_bIME = false;
char CGameApp::ms_acMediaPath[NI_MAX_PATH];
char CGameApp::ms_acTempMediaFilename[NI_MAX_PATH];

CGameApp*			CGameApp::mp_Appication = 0;

CGameApp::CGameApp() : m_CurrState(0) 
{	
	m_hWnd = NULL;
	m_hMenu=0;		//메뉴 ID 메뉴 생성시 LoadMenu(NULL,MAKEINTRESOURCE(아이디값));
	m_hIcon=0;
	//ms_pAccel = 0;
	
	m_nWidth = 1024;		//해상도
	m_nHeight = 768;	//해상도

	//윈도우 스타일
	m_ulWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		
	//시간 측정 변수 초기화
	m_fLastTime = -1.0f;
    m_fAccumTime = 0.0f;
    m_fCurrentTime = 0.0f;
    m_fFrameTime = 0.0f;
    m_iClicks = 0;
    m_iTimer = 1;
    m_iMaxTimer = 30;
    m_fFrameRate = 0.0f;
    m_fLastFrameRateTime = 0.0f;
    m_iLastFrameRateClicks = 0;

    m_fMinFramePeriod = 0.0f;
    m_fLastFrame = 0.0f;
    m_fCullTime = 0.0f;
    m_fRenderTime = 0.0f;
    m_fUpdateTime = 0.0f;
    m_fBeginUpdate = 0.0f;
    m_fBeginCull = 0.0f;
    m_fBeginRender = 0.0f; 

	m_BackGroundColor = NiColor( 0.0f,0.0f,0.0f );

	m_bIME				= false;	// IME 활성화 여부
	m_bIsHangul			= false;	// 한영 여부
	m_bActiveEditBox	= false;	// 에디트 박스 활성 여부

	m_bExclusiveMouse	= true;		// 마우스 점유 여부

	m_CurrState			= NULL;		// 현재 화면 상태

	m_strFont = "ahn_m";

	InitializeCriticalSection(&m_cs);
	//m_bClicked = false;
}

CGameApp::~CGameApp()
{
	
}
bool CGameApp::ShutDown()
{
	DeleteCriticalSection(&m_cs);

	m_bIME = false;

	m_StateManager.Release();

	m_spCEGUIGBRenderer = 0;
	
	//ms_spWireframe = 0;		//와이어 프레임 프로퍼티
	m_spCursor = 0;			//커서
	
	m_spText = 0;
	m_spFont = 0;

	m_CurrState = NULL;

	m_spRenderer=0;			//렌더러

	if(m_hWnd) DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	
	if(m_RenderWnd) DestroyWindow(m_RenderWnd);
	m_RenderWnd = NULL;

	//사운드 객체 제거
	CSoundManager::DestroySoundMgr();
		
	NiImageConverter::SetImageConverter(NULL);

	NiSystemCursor::Shutdown();
	PostQuitMessage(0);

	return true;
}

bool CGameApp::CreateCursor()
{	
	//렌더 타겟 그룹 얻어온다 - on-screenbackbuffer의 사이즈를 조회하기 위해
	const NiRenderTargetGroup* pkRTGroup =
		m_spRenderer->GetDefaultRenderTargetGroup();
	
	//마우스 커서의 활동 범위를 저장할 rect 생성
	NiRect<int> kRect;

	//커서의 활동 범위를 정해주기 위해 랜더 타겟에서 width와 height
	kRect.m_left	= 0;
	kRect.m_top		= 0;
	kRect.m_right	= pkRTGroup->GetWidth(0);
	kRect.m_bottom	= pkRTGroup->GetHeight(0);

	//커서 생성
	m_spCursor = NiSystemCursor::Create(m_spRenderer,kRect,NiCursor::IMMEDIATE,
		NiSystemCursor::BULLSEYE,"Data/SystemCursors.tga",pkRTGroup);

	if(m_spCursor == 0)
	{
		NiMessageBox("CGameApp::CGameApp() - NiSystemCursor 생성 실패\n",NULL);
		return false;
	}
	//커서 모양 변경
	//NiSystemCursor로 캐스팅
	NiSystemCursor* pkCursor = NiDynamicCast(NiSystemCursor,m_spCursor);
	pkCursor->SetType(NiSystemCursor::BULLSEYE);		//커서 모양 변경
	
	//커서를 화면 중앙에 배치
	m_spCursor->SetPosition(0.0f,kRect.m_right/2,kRect.m_bottom/2);
	m_spCursor->Show(true);

	return true;
}

bool CGameApp::CreateSound()					//사운드 매니저
{
	if(!CSoundManager::CreateSoundMgr(1.0f,0.5f,5000.0f))
	{
		NiMessageBox("SoundManger 생성 실패",NULL);
		return false;
	}

	return true;
}


//윈도우 생성 함수
bool CGameApp::CreateMainWindow(LPCTSTR lpCaption,HINSTANCE hInstance, int iWinMode,bool bFullScreen)
{
	bool b_menu;			//true: 메뉴 사용 false: 메뉴 사용 하지 않음	
	
	NiWindowRef		pWnd = NULL;

	m_bFullScreen = bFullScreen;
	m_hInst       = hInstance;

	//윈도우의 크기 영역 설정
	RECT rect = { 0, 0, m_nWidth, m_nHeight };

	//메뉴 사용 여부 확인
	if(m_hMenu==0)	b_menu = false;
	else			b_menu = true;

	//윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style				= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc        = StaticWndProc;
	wndclass.cbClsExtra			= 0;
	wndclass.cbWndExtra			= 0;
	wndclass.hInstance			= hInstance;
	wndclass.hIcon				= LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground		= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName		= NULL;
	wndclass.lpszClassName		= lpCaption;

	//윈도우 클래스 등록
	RegisterClass(&wndclass);

	//클라이언트 영역의 크기를 알려준다.
	AdjustWindowRect(&rect,m_ulWindowStyle,b_menu);
		
	//flags 값 설정
	unsigned int uiFlags = WS_CHILD;

	//윈도우 생성
	if(m_bFullScreen)  //풀 스크린일 경우
	{
		pWnd = CreateWindow(
			lpCaption,
			lpCaption,
			WS_POPUP,
			0,						//초기 x위치
			0,						//초기 y위치
			rect.right - rect.left,	//윈도우의 width
			rect.bottom - rect.top, //윈도우의 height
			m_hWnd,
			NULL,
			hInstance,
			this
			);
	}
	else			//윈도우 모드일 경우
	{
		uiFlags |= WS_VISIBLE;

		pWnd = CreateWindow(
			lpCaption,
			lpCaption,
			m_ulWindowStyle,
			0,						//윈도우 x위치 좌표
			0,						//윈도우 y위치 좌표
			rect.right - rect.left,	//윈도우의 width
			rect.bottom - rect.top, //윈도우의 height
			m_hWnd,
			NULL,
			hInstance,
			this
			);
	}
	m_hWnd = pWnd;

	m_RenderWnd = CreateWindow(
		lpCaption,
		NULL,
		uiFlags,
		0,
		0,
		m_nWidth,
		m_nHeight,
		pWnd,
		NULL,
		hInstance,
		NULL	
		);
	
	ShowWindow(m_hWnd,iWinMode);
	UpdateWindow(m_hWnd);		
	
	return true;	
}

LRESULT CALLBACK CGameApp::StaticWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

    CGameApp *Destination = reinterpret_cast< CGameApp* >( GetWindowLong( hWnd, GWL_USERDATA ) );
    
    if (Destination) return Destination->MainWndProc( hWnd, Message, wParam, lParam );
    
    return DefWindowProc( hWnd, Message, wParam, lParam );
} 


//어플리케이션 초기화 
bool CGameApp::Initialize()
{		
	//렌더러 생성
	if(!CreateRenderer())
		return false;	

	//인풋 시스템 생성
	if(!CreateInputSystem())
		return false;
	
	//커서 생성
	if(!CreateCursor())
		return false;	
	
	if(!CreateSound())
		return false;

	SetMaxFrameRate(100.0f);

	//--- UI 관련 함수 실행 ----
	InitializeGUIFramework();

	m_StateManager.Init();

	RegisterBaseStates();	
	//--------------------------

//	m_pkMuzelSplash = NiNew CMuzelSplash( 0.01f );

//	m_pkMuzelSplash->Initialize();
    
	return true;
}

void CGameApp::OnIdle()
{
	float fStartTime = NiGetCurrentTimeInSec();
	
	if(!MeasureTime())
		return;
		
	//키 입력
	ProcessInput();
	//업데이트 프레임
	UpdateFrame();
	//씬 렌더
	RenderFrame();

	//m_fFrameRate = 1.0f / m_fFrameTime;
//	float endTime = NiGetCurrentTimeInSec(); 
	
//	m_spText->sprintf("FPS = %f\n",1/(endTime-fStartTime));
}


//렌더러 생성
bool CGameApp::CreateRenderer()
{
	const char* pcWorkingDir = ConvertMediaFilename("Shaders\\Generated"); //쉐이더 폴더 경로 지정
    NiMaterial::SetDefaultWorkingDirectory(pcWorkingDir);
	
	NiWindowRef	pWnd;

	//NiDX9Renderer::Create 함수에 넘겨 줄 flags
	unsigned int uiFlags = 0;

	 NiDX9Renderer::DeviceDesc eDevType = NiDX9Renderer::DEVDESC_PURE;
     unsigned int uiAdapter = D3DADAPTER_DEFAULT;
       
	
	//NiDX9SystemDesc pointer에 포인터로 시스템 Description을 가져옴 
	const NiDX9SystemDesc *pkInfo = NiDX9Renderer::GetSystemDesc();

	
	//어댑터 카운터를 얻어온다.
	unsigned int nAdapters = pkInfo->GetAdapterCount();

	//어뎁터의 인덱스 0번의 정보를 가져온다.
	const NiDX9AdapterDesc *pkAdapter = pkInfo->GetAdapter(0);

	//어댑터 종류 출력
	char str[256];
	NiSprintf(str,256,"Adapter: %s\n",pkAdapter->GetStringDescription());
	OutputDebugString(str);
	

	//HAL 타입의 디바이스 정보를 얻어온다.
	const NiDX9DeviceDesc *pkDevice = pkAdapter->GetDevice(D3DDEVTYPE_HAL);

	//Device cap을 얻어옴
	const D3DCAPS9& kD3dCaps9 = pkDevice->GetCaps();

	//프레임 버퍼 포맷 얻어옴
	NiDX9RendererEx::FrameBufferFormat nFrameBufferFormat = pkDevice->GetNearestFrameBufferFormat(!m_bFullScreen,32);

	//스텐실 버퍼 포멧 가져옴
	NiDX9RendererEx::DepthStencilFormat nDepthBufferFormat = pkDevice->GetNearestDepthStencilFormat(
				(D3DFORMAT)nFrameBufferFormat, (D3DFORMAT)nFrameBufferFormat,24,8);

	//풀스크린일 때와 윈도우일 때랑 윈도우 핸들이 다름.
	if(m_bFullScreen)
	{
		pWnd = m_hWnd;
		uiFlags |= NiDX9RendererEx::USE_FULLSCREEN;
	}
	else
	{
		pWnd = m_RenderWnd;
	}			
	
	//렌더러 생성
	m_spRenderer = NiDX9RendererEx::Create(
			m_nWidth,
			m_nHeight,
			(NiDX9Renderer::FlagType)uiFlags,
			pWnd,
			pWnd,								//윈도우 포커스
			D3DADAPTER_DEFAULT,					//디폴트 어댑터 사용
			NiDX9Renderer::DEVDESC_PURE,		//하드웨어 버텍스 프로세싱, 버텍스/픽셀 셰이딩, 일반적으로 높은 성능의 작업을 수행할 수 있는 하드웨어를 지정합니다. 매우 강력한 디바이스임.
			nFrameBufferFormat,					//프레임 버퍼 포맷 BPP = 32
			nDepthBufferFormat,					//depth- 스텐실 버퍼 포맷 depth Bpp = 24, stensil = 8
			NiDX9Renderer::PRESENT_INTERVAL_ONE,// swap rate : 어플리케이션에서 모니터의 리플레시 구간을 생략하게 해줌
			NiDX9Renderer::SWAPEFFECT_DISCARD,  // SwapEffect : 백퍼버를 사용자에게 제시하는 방식 DISCARD = 가장 빠르고 가장 적합한 교체 방식을 사용
			NiDX9Renderer::FBMODE_DEFAULT,		// 프레임 버퍼 모드 : 어플리케이션이 읽을 수 있는 프레임 버퍼나 자동적인 풀 스크린 안티앨리어싱을 포함하여 프레임 버퍼에 원하는 특별한 동작을 지정
			1,									// 백버퍼의 개수 디폴트 1
			0				// DX9 모니터 주사율 : 풀스크린 렌더러가 하드웨어에서 쓰일 정확한 주사율을 지정
			);
	
	
	if(!m_spRenderer)
	{
		NiMessageBox("Direct3D renderer creation error: ",NULL);
		return false;
	}
	else
	{
		m_BackGroundColor = NiColor(0.0f,0.0f,0.0f);
		m_spRenderer->SetBackgroundColor( m_BackGroundColor );
	}

	//이미지 컨버터 생성
	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
		
	return true;
}


NiColor		CGameApp::GetBackGroundColor()
{
	return m_BackGroundColor;
}

//인풋 시스템 생성
bool CGameApp::CreateInputSystem()
{
	//인풋 파라메터 생성
	NiDI8InputSystem::DI8CreateParams* pkNi8Param = 
        NiNew NiDI8InputSystem::DI8CreateParams();
	assert(pkNi8Param);

	pkNi8Param->SetRenderer(m_spRenderer);
	pkNi8Param->SetKeyboardUsage(
		 NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE);

	//NiInputSystem::EXCLUSIVE - 이 어플리케이션이 모든 마우스 이벤트를 “소유”하게 해줍니다. 
	//NiInputSystem::NONEXCLUSIVE - 윈도우 마우스를 따로 쓸 경우 플래그값
	pkNi8Param->SetMouseUsage(NiInputSystem::FOREGROUND | NiInputSystem::EXCLUSIVE);
	pkNi8Param->SetOwnerInstance(m_hInst);
	pkNi8Param->SetOwnerWindow(m_hWnd); 
	
	//inputSystem 생성
	NiInputSystem::CreateParams* pkParams = pkNi8Param;
	m_spInputSystem = NiInputSystem::Create(pkParams);
	NiDelete pkParams;
	if(!m_spInputSystem)
	{
		NiMessageBox("CreateInputSystem: Creation failed.","NiApplication Error");
		return false;
	}

	//자동으로 인풋 시스템의 생성은 디바이스의 상수에 의해 시작한다.
	NiInputErr eErr = m_spInputSystem->CheckEnumerationStatus();
	switch (eErr)
    {
    case NIIERR_ENUM_NOTRUNNING:
        assert(!"EnumerateDevices failed?");
        return false;
    case NIIERR_ENUM_FAILED:
        assert(!"CheckEnumerationStatus> FAILED!");
        return false;
    case NIIERR_ENUM_COMPLETE:
    case NIIERR_ENUM_NOTCOMPLETE:
    default:
        break;
    }

	if(!m_spInputSystem->OpenMouse() || !m_spInputSystem->OpenKeyboard())
	{
		 NiMessageBox("CreateInputSystem: Mouse or keyboard failed to open.", 
            "NiApplication Error");
        return false;
	}
	

	return true;
}

bool CGameApp::InitializeGUIFramework()
{ 

	m_spCEGUIGBRenderer = NiNew CEGUIGBRenderSystem();

	if (!m_spCEGUIGBRenderer)
		return(false);

	CEGUI::DefaultResourceProvider *pRP = 
		reinterpret_cast<CEGUI::DefaultResourceProvider*>( CEGUI::System::getSingleton().getResourceProvider() );

	if( NULL != pRP )
	{
		//미디어 파일 지정
		SetMediaPath("./UI/");

		pRP->setResourceGroupDirectory("schemes", ConvertMediaFilename("schemes/"));
		pRP->setResourceGroupDirectory("imagesets", ConvertMediaFilename("imagesets/"));
		pRP->setResourceGroupDirectory("fonts", ConvertMediaFilename("fonts/"));
		pRP->setResourceGroupDirectory("layouts", ConvertMediaFilename("layouts/"));
		pRP->setResourceGroupDirectory("looknfeels", ConvertMediaFilename("looknfeel/"));
	}

	// Load the basic CEGUI resources, control schemes, fonts, and cursors, that this application will use
	CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"WonderlandLook.scheme");
	CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"VanillaSkin.scheme");
//	CEGUI::SchemeManager::getSingleton().loadScheme("WindowsLook.scheme");
//	CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");	
//	CEGUI::SchemeManager::getSingleton().loadScheme("SleekSpace.scheme");	
    
	CEGUI::System::getSingleton().setDefaultMouseCursor((CEGUI::utf8*)"WonderlandLook", (CEGUI::utf8*)"MouseArrow");
	
	
	if( !CEGUI::FontManager::getSingleton().isFontPresent((CEGUI::utf8*)"ahn_m.font") )
	{
		CEGUI::FontManager::getSingleton().createFont((CEGUI::utf8*)"ahn_m.font");
		CEGUI::System::getSingleton().setDefaultFont((CEGUI::utf8*)"ahn_m");
	}
/*
	if( !CEGUI::FontManager::getSingleton().isFontPresent( "ahn_m" ) )
	{
		CEGUI::FontManager::getSingleton().createFont("ahn_m.font");
	}
*/
//	CEGUI::System::getSingleton().setDefaultFont( (CEGUI::utf8*)"ahn_m" ); 

	// Now, create a background window that will be the root for all GUI windows. Individual states
	// will add and remove their own GUI elements as children of this, upon entry and exit. This background
	// window does not contribute anything visually, and only exists for convenience.
	ImagesetManager::getSingleton().createImageset(String("RootWindowBackground.imageset"));
	CEGUI::Window *pBackground = CEGUI::WindowManager::getSingleton().loadWindowLayout("RootWindow.layout");

	if (!pBackground)
		return(false);

	CEGUI::System::getSingleton().setGUISheet(pBackground);

	//--------------------------------------------------------------------------
	// Next, integrate CEGUI into the Gamebryo frame rendering system
	//--------------------------------------------------------------------------

	// We'll render the GUI in the screen space render step, which is sensible since the
	// user interface will commonly be presented in screen space.
	return(true);
}

void CGameApp::RegisterBaseStates()
{
	
	m_StateManager.RegisterState( "StartupSplash", NiNew CStartSplashState("StartupSplashBackground.imageset", "StartupSplash.layout"));		
//	m_StateManager.RegisterState( "Option",		NiNew COptionState("CreditBackground.imageset",	"GamePlay1.layout"		));	
	m_StateManager.RegisterState( "Credit",		NiNew CCreditState("CreditBackground.imageset",	"Credit.layout"		));	
	m_StateManager.RegisterState( "MainMenu",	NiNew CMenuState("MainMenuBackground.imageset",	"MainMenu.layout"));	
	m_StateManager.RegisterState( "GameRoom",	NiNew CGameRoomState("RoomStateBackground.imageset", "RoomState.layout" ));	
	m_StateManager.RegisterState( "GamePlay",	NiNew CGamePlayState("","GamePlay.layout"));	
	m_StateManager.RegisterState( "GameResult",	NiNew CGameResultState("GameResultBackground.imageset","GameResult.layout"));	
	m_StateManager.RegisterState( "SingleTest",		NiNew CSinglePlayState("","GamePlay.layout"));	
	

	// 싱글 테스트
	/*
	m_StateManager.RegisterState( "StartupSplash",	NiNew CStartSplashState("StartupSplashBackground.imageset", "StartupSplash.layout"));
	m_StateManager.RegisterState( "MainMenu",		NiNew CMenuState("MainMenuBackground.imageset",	"MainMenu.layout"));	
	m_StateManager.RegisterState( "GameRoom",		NiNew CGameRoomState("RoomStateBackground.imageset", "RoomState.layout" ));	
	m_StateManager.RegisterState( "GamePlay",		NiNew CGamePlayState("","GamePlay.layout"));	
	m_StateManager.RegisterState( "GameResult",		NiNew CGameResultState("GameResultBackground.imageset","GameResult.layout"));	
	m_StateManager.RegisterState( "SingleTest",		NiNew CSinglePlayState("","GamePlay.layout"));	
	*/
	m_CurrState = m_StateManager.GetCurrentStateClass();
}

/*//폰트 생성
bool CGameApp::CreateFont()
{
	SetMediaPath("Data/");
	m_spFont = NiFont::Create(m_spRenderer,ConvertMediaFilename("CourierNew10.NFF"));
	assert(m_spFont);

	NiColorA kColor(1.0f,1.0f,1.0f,1.0f);
	m_spText = NiNew NiString2D(m_spFont,NiFontString::COLORED,512,NULL,kColor,32,360);
	assert(m_spText);

	return true;
}
*/

const char*	CGameApp::GetFontName() const
{
	return m_strFont.c_str();
}

/*
bool CGameApp::ChangeState(CGameState* state)
{
	//지정된 상태가 있을 경우 포인터를 NULL로 만들어주고 초기화.
	if(m_CurrState)
	{
		m_CurrState->Cleanup();
		m_CurrState = NULL;
	}
	m_CurrState = state;
	if(!m_CurrState->Init())
		return false;

	return true;
}
*/

void CGameApp::RenderFrame()
{
	if( NULL == m_spRenderer )
		return;

	m_spRenderer->BeginFrame();
	
	m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL);

	if( m_CurrState ) m_CurrState->OnRender();	

	m_spCEGUIGBRenderer->PerformRendering();

//	m_pkMuzelSplash->Update( 0.0f );
//	m_pkMuzelSplash->RenderClick();

    m_spRenderer->EndUsingRenderTargetGroup();	

    m_spRenderer->EndFrame();

	m_spRenderer->DisplayFrame();	
}

//커서 위치 이동
void CGameApp::MoveCursor(int x,int y)
{
	if(!m_spCursor)
		return;
	m_spCursor->SetPosition(0.0f,x,y);
}


/*//씬 그래프 외에 사물 그리기
void CGameApp::RenderScreenItem()
{
	m_spRenderer->SetScreenSpaceCameraData();    
	m_spCursor->Draw();//커서 그리기
	m_spText->Draw(m_spRenderer);
	
}
*/

//모든 갱신을 여기서 처리 
void CGameApp::UpdateFrame()
{	
	m_StateManager.Update(m_fFrameTime);
}

//입력 처리
void CGameApp::ProcessInput()
{
	//인풋 모드 셋팅
	SetInputMode();	

	if( m_CurrState ) m_CurrState->OnProcessInput();
}

void CGameApp::SetInputMode()
{
	if(!m_spInputSystem)
		return ;

	//인풋 모드가 mapped 모드
	if(m_spInputSystem->GetMode() == NiInputSystem::MAPPED)
	{
		if(m_spInputSystem->UpdateActionMap() == NIIERR_DEVICELOST)
			m_spInputSystem->HandleDeviceChanges();
	}
	//인풋 모드가 immediate 모드
	else
	{
		if(m_spInputSystem->UpdateAllDevices() == NIIERR_DEVICELOST)
			m_spInputSystem->HandleDeviceChanges();
	}	 
}


/*
//마우스 입력 처리
void CGameApp::ProcessMouse()
{
	NiInputMouse* pkMouse = m_spInputSystem->GetMouse();
	
	if(pkMouse)
	{
		int iX,iY,iZ = 0;
		if(pkMouse->GetPositionDelta(iX,iY,iZ))
		{
			//커서를 중앙에 고정
			if((iX != 0 ) || (iY !=0))
			{
				//m_spCursor->Move(0.0f,iX,iY);								
			}
		}
	}	
}*/


//어플리케이션 시간 측정
bool CGameApp::MeasureTime()
{
	// start performance measurements
    if (m_fLastTime == -1.0f)
    {
        m_fLastTime = NiGetCurrentTimeInSec();
        m_fAccumTime = 0.0f;
        m_iClicks = 0;
    }

    // measure time
    m_fCurrentTime = NiGetCurrentTimeInSec();
    float fDeltaTime = m_fCurrentTime - m_fLastTime;

    if (fDeltaTime == 0.0f)
        return true;

    if (fDeltaTime < 0.0f)
        fDeltaTime = 0.0f;
    m_fLastTime = m_fCurrentTime;
    m_fAccumTime += fDeltaTime;

    // frame rate limiter
    if (m_fAccumTime < (m_fLastFrame + m_fMinFramePeriod))
		return false;

    m_fFrameTime = m_fAccumTime - m_fLastFrame;
    m_fLastFrame = m_fAccumTime;

    return true;
}


void CGameApp::EnterCS()
{
	EnterCriticalSection(&m_cs);
}

void CGameApp::LeaveCS()
{
	LeaveCriticalSection(&m_cs);
}


const char* CGameApp::ConvertMediaFilename(const char *pcFilename)
{
	memset(ms_acTempMediaFilename,0,sizeof(ms_acTempMediaFilename));
	NiSprintf(ms_acTempMediaFilename, NI_MAX_PATH, "%s%s", ms_acMediaPath,pcFilename);
	NiPath::Standardize(ms_acTempMediaFilename);

	return ms_acTempMediaFilename;
}

void CGameApp::SetMediaPath(const char* pcPath)
{
	NiStrcpy(ms_acMediaPath, NI_MAX_PATH, pcPath);	
}
const char* CGameApp::GetMediaPath()
{
	 return ms_acMediaPath;	
}

void CGameApp::SetMaxFrameRate(float fMax)
{
	if(fMax - 1e-5f)
		m_fMinFramePeriod = 1e-5f;
	else
		m_fMinFramePeriod = 1.0f / fMax;
}

void CGameApp::GetMinMax( NiAVObject* pkObject, NiPoint3 *min, NiPoint3 *max )
{
	// 렌더링 Data의 Minm Max를 구한다.
	const NiBound &kBound = pkObject->GetWorldBound();

	const NiPoint3 &kCenter = kBound.GetCenter();
	float fRadius = kBound.GetRadius();

	min->x = kCenter.x - fRadius;
	min->y = kCenter.y - fRadius;

	max->x = kCenter.x + fRadius;	
	max->y = kCenter.y + fRadius;
}

void CGameApp::ChangeNodeAlpha(NiAVObject *pkObject, float fAlpha)
{
	if( NiIsKindOf(NiNode, pkObject) )
    {
		NiNode *pkNode = (NiNode *)pkObject;

		for( unsigned int i = 0 ; i < pkNode->GetArrayCount() ; i++ )
			   if( pkNode->GetAt(i) )
					  ChangeNodeAlpha(pkNode->GetAt(i), fAlpha);
    }

    NiMaterialProperty	*pkMat		= (NiMaterialProperty *)pkObject->GetProperty(NiProperty::MATERIAL);
    NiAlphaProperty		*pkAlpha	= (NiAlphaProperty *)	pkObject->GetProperty(NiProperty::ALPHA);
    NiStencilProperty	*pkStencil	= (NiStencilProperty *)	pkObject->GetProperty(NiProperty::STENCIL);

    if( pkMat )	pkMat->SetAlpha(fAlpha);
    if( pkAlpha ) { pkAlpha->SetAlphaBlending(true); pkAlpha->SetAlphaTesting(true); pkAlpha->SetNoSorter(false); }
    if( pkStencil ) pkStencil->SetDrawMode((fAlpha<1.0f)?NiStencilProperty::DrawMode::DRAW_CCW : NiStencilProperty::DrawMode::DRAW_BOTH);
}

//메시지 루프o
int CGameApp::BeginGame()
{
	MSG		msg;

	//메인 루프
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
		{
			if(msg.message == WM_QUIT) break;

			if(!TranslateAccelerator(m_hWnd,ms_pAccel,&msg))
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			OnIdle();
		}
	}

	return 0;
}

LRESULT CALLBACK CGameApp::MainWndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	 switch (uiMsg) 
    {
	case  WM_CREATE:
		break;
	case WM_CLOSE:
			PostQuitMessage(0);
			break;
	case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN :

	//	if( ZFLog::g_sLog ) ZFLog::g_sLog->Log(L"WM_KEYDOWN");
		switch( wParam )
		{
		case VK_PROCESSKEY:
			 DWORD conv, sentence; 
			 HIMC h_imc = ImmGetContext(hWnd); 
			 ImmGetConversionStatus(h_imc, &conv, &sentence); 

			 if(conv & IME_CMODE_LANGUAGE)
				 m_bIsHangul = true; // 한글 상태임... 
			 else
				 m_bIsHangul = false; // 영문 상태임... 

			 ImmReleaseContext(hWnd, h_imc);
			 break;
		}
		break;
	case WM_SIZE:
		break;
	case WM_IME_NOTIFY :
//		if( ZFLog::g_sLog ) ZFLog::g_sLog->Log("WM_IME_NOTIFY");
		break;
	case WM_IME_SETCONTEXT:
		
		if(m_bActiveEditBox) 
		{
//			if( ZFLog::g_sLog ) ZFLog::g_sLog->Log("WM_IME_SETCONTEXT");
			lParam = 0;
		}
		break;
	case WM_IME_STARTCOMPOSITION :
		
		if(m_bActiveEditBox)
		{
//			if( ZFLog::g_sLog ) ZFLog::g_sLog->Log("WM_IME_STARTCOMPOSITION");
			m_bIME = true;
		}
		break;
	case WM_IME_ENDCOMPOSITION :
		
		if(m_bActiveEditBox)
		{
//			if( ZFLog::g_sLog ) ZFLog::g_sLog->Log("WM_IME_ENDCOMPOSITION");
			CEGUI::System::getSingleton().injectCharSelect(false);      //IME조합이 끝나면 셀렉을 해제 시킨다
			m_bIME = false;
		}
		break;
	case WM_IME_COMPOSITION :	
		if(m_bActiveEditBox)
		{
//			if( ZFLog::g_sLog ) ZFLog::g_sLog->Log("WM_IME_COMPOSITION");
			IMEComposition( hWnd, lParam );
		}
		break;
	case WM_CHAR:
//		if( ZFLog::g_sLog ) ZFLog::g_sLog->Log(L"WM_CHAR");
//		if( IsLeadByte(wParam) )
//			CEGUI::System::getSingleton().injectChar((CEGUI::utf32)wParam);
		break;
	default:

		break;
    }

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

void	CGameApp::QuitApplication()
{
	::PostMessage(m_hWnd, WM_DESTROY, 0, 0);

}

void CGameApp::IMEComposition( HWND hWnd, LPARAM lParam )
{
	HIMC hImc = ImmGetContext(hWnd);

	if( hImc == NULL ||
		!CEGUI::System::getSingletonPtr() ) return;

   // 변수 설정
   int		   nLength = 0;
   wchar_t	wszComp[4] = {0,};

	if( lParam & GCS_RESULTSTR )
	{
		nLength = ImmGetCompositionStringW( hImc, GCS_RESULTSTR, NULL, 0 );

		if( nLength > 0 )
		{
			ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszComp, nLength );

			for( int i = 0; i < nLength; i++ )
			{
				if( wszComp[i] != 0 )
				{
					CEGUI::System::getSingleton().injectChar( wszComp[i] );
				}
			}
		}
	}
	// IME 문자 조합중이라면
	else if(lParam & GCS_COMPSTR)       //IME 조합중
	{
		nLength = ImmGetCompositionStringW( hImc, GCS_COMPSTR, NULL, 0 );

		if( nLength > 0 )
		{
			ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszComp, nLength );

			for( int  i = 0; i < nLength; i++ )
			{
				if( wszComp[i] != 0 )
				{
					CEGUI::System::getSingleton().injectChar(wszComp[i]);
				    CEGUI::System::getSingleton().injectCharSelect(true);
				}
			}
		}
	}

	//  IME 핸들 해제
	ImmReleaseContext( hWnd, hImc );

}
