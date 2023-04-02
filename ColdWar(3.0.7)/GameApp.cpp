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
	m_hMenu=0;		//�޴� ID �޴� ������ LoadMenu(NULL,MAKEINTRESOURCE(���̵�));
	m_hIcon=0;
	//ms_pAccel = 0;
	
	m_nWidth = 1024;		//�ػ�
	m_nHeight = 768;	//�ػ�

	//������ ��Ÿ��
	m_ulWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		
	//�ð� ���� ���� �ʱ�ȭ
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

	m_bIME				= false;	// IME Ȱ��ȭ ����
	m_bIsHangul			= false;	// �ѿ� ����
	m_bActiveEditBox	= false;	// ����Ʈ �ڽ� Ȱ�� ����

	m_bExclusiveMouse	= true;		// ���콺 ���� ����

	m_CurrState			= NULL;		// ���� ȭ�� ����

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
	
	//ms_spWireframe = 0;		//���̾� ������ ������Ƽ
	m_spCursor = 0;			//Ŀ��
	
	m_spText = 0;
	m_spFont = 0;

	m_CurrState = NULL;

	m_spRenderer=0;			//������

	if(m_hWnd) DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	
	if(m_RenderWnd) DestroyWindow(m_RenderWnd);
	m_RenderWnd = NULL;

	//���� ��ü ����
	CSoundManager::DestroySoundMgr();
		
	NiImageConverter::SetImageConverter(NULL);

	NiSystemCursor::Shutdown();
	PostQuitMessage(0);

	return true;
}

bool CGameApp::CreateCursor()
{	
	//���� Ÿ�� �׷� ���´� - on-screenbackbuffer�� ����� ��ȸ�ϱ� ����
	const NiRenderTargetGroup* pkRTGroup =
		m_spRenderer->GetDefaultRenderTargetGroup();
	
	//���콺 Ŀ���� Ȱ�� ������ ������ rect ����
	NiRect<int> kRect;

	//Ŀ���� Ȱ�� ������ �����ֱ� ���� ���� Ÿ�ٿ��� width�� height
	kRect.m_left	= 0;
	kRect.m_top		= 0;
	kRect.m_right	= pkRTGroup->GetWidth(0);
	kRect.m_bottom	= pkRTGroup->GetHeight(0);

	//Ŀ�� ����
	m_spCursor = NiSystemCursor::Create(m_spRenderer,kRect,NiCursor::IMMEDIATE,
		NiSystemCursor::BULLSEYE,"Data/SystemCursors.tga",pkRTGroup);

	if(m_spCursor == 0)
	{
		NiMessageBox("CGameApp::CGameApp() - NiSystemCursor ���� ����\n",NULL);
		return false;
	}
	//Ŀ�� ��� ����
	//NiSystemCursor�� ĳ����
	NiSystemCursor* pkCursor = NiDynamicCast(NiSystemCursor,m_spCursor);
	pkCursor->SetType(NiSystemCursor::BULLSEYE);		//Ŀ�� ��� ����
	
	//Ŀ���� ȭ�� �߾ӿ� ��ġ
	m_spCursor->SetPosition(0.0f,kRect.m_right/2,kRect.m_bottom/2);
	m_spCursor->Show(true);

	return true;
}

bool CGameApp::CreateSound()					//���� �Ŵ���
{
	if(!CSoundManager::CreateSoundMgr(1.0f,0.5f,5000.0f))
	{
		NiMessageBox("SoundManger ���� ����",NULL);
		return false;
	}

	return true;
}


//������ ���� �Լ�
bool CGameApp::CreateMainWindow(LPCTSTR lpCaption,HINSTANCE hInstance, int iWinMode,bool bFullScreen)
{
	bool b_menu;			//true: �޴� ��� false: �޴� ��� ���� ����	
	
	NiWindowRef		pWnd = NULL;

	m_bFullScreen = bFullScreen;
	m_hInst       = hInstance;

	//�������� ũ�� ���� ����
	RECT rect = { 0, 0, m_nWidth, m_nHeight };

	//�޴� ��� ���� Ȯ��
	if(m_hMenu==0)	b_menu = false;
	else			b_menu = true;

	//������ Ŭ���� ���
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

	//������ Ŭ���� ���
	RegisterClass(&wndclass);

	//Ŭ���̾�Ʈ ������ ũ�⸦ �˷��ش�.
	AdjustWindowRect(&rect,m_ulWindowStyle,b_menu);
		
	//flags �� ����
	unsigned int uiFlags = WS_CHILD;

	//������ ����
	if(m_bFullScreen)  //Ǯ ��ũ���� ���
	{
		pWnd = CreateWindow(
			lpCaption,
			lpCaption,
			WS_POPUP,
			0,						//�ʱ� x��ġ
			0,						//�ʱ� y��ġ
			rect.right - rect.left,	//�������� width
			rect.bottom - rect.top, //�������� height
			m_hWnd,
			NULL,
			hInstance,
			this
			);
	}
	else			//������ ����� ���
	{
		uiFlags |= WS_VISIBLE;

		pWnd = CreateWindow(
			lpCaption,
			lpCaption,
			m_ulWindowStyle,
			0,						//������ x��ġ ��ǥ
			0,						//������ y��ġ ��ǥ
			rect.right - rect.left,	//�������� width
			rect.bottom - rect.top, //�������� height
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


//���ø����̼� �ʱ�ȭ 
bool CGameApp::Initialize()
{		
	//������ ����
	if(!CreateRenderer())
		return false;	

	//��ǲ �ý��� ����
	if(!CreateInputSystem())
		return false;
	
	//Ŀ�� ����
	if(!CreateCursor())
		return false;	
	
	if(!CreateSound())
		return false;

	SetMaxFrameRate(100.0f);

	//--- UI ���� �Լ� ���� ----
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
		
	//Ű �Է�
	ProcessInput();
	//������Ʈ ������
	UpdateFrame();
	//�� ����
	RenderFrame();

	//m_fFrameRate = 1.0f / m_fFrameTime;
//	float endTime = NiGetCurrentTimeInSec(); 
	
//	m_spText->sprintf("FPS = %f\n",1/(endTime-fStartTime));
}


//������ ����
bool CGameApp::CreateRenderer()
{
	const char* pcWorkingDir = ConvertMediaFilename("Shaders\\Generated"); //���̴� ���� ��� ����
    NiMaterial::SetDefaultWorkingDirectory(pcWorkingDir);
	
	NiWindowRef	pWnd;

	//NiDX9Renderer::Create �Լ��� �Ѱ� �� flags
	unsigned int uiFlags = 0;

	 NiDX9Renderer::DeviceDesc eDevType = NiDX9Renderer::DEVDESC_PURE;
     unsigned int uiAdapter = D3DADAPTER_DEFAULT;
       
	
	//NiDX9SystemDesc pointer�� �����ͷ� �ý��� Description�� ������ 
	const NiDX9SystemDesc *pkInfo = NiDX9Renderer::GetSystemDesc();

	
	//����� ī���͸� ���´�.
	unsigned int nAdapters = pkInfo->GetAdapterCount();

	//����� �ε��� 0���� ������ �����´�.
	const NiDX9AdapterDesc *pkAdapter = pkInfo->GetAdapter(0);

	//����� ���� ���
	char str[256];
	NiSprintf(str,256,"Adapter: %s\n",pkAdapter->GetStringDescription());
	OutputDebugString(str);
	

	//HAL Ÿ���� ����̽� ������ ���´�.
	const NiDX9DeviceDesc *pkDevice = pkAdapter->GetDevice(D3DDEVTYPE_HAL);

	//Device cap�� ����
	const D3DCAPS9& kD3dCaps9 = pkDevice->GetCaps();

	//������ ���� ���� ����
	NiDX9RendererEx::FrameBufferFormat nFrameBufferFormat = pkDevice->GetNearestFrameBufferFormat(!m_bFullScreen,32);

	//���ٽ� ���� ���� ������
	NiDX9RendererEx::DepthStencilFormat nDepthBufferFormat = pkDevice->GetNearestDepthStencilFormat(
				(D3DFORMAT)nFrameBufferFormat, (D3DFORMAT)nFrameBufferFormat,24,8);

	//Ǯ��ũ���� ���� �������� ���� ������ �ڵ��� �ٸ�.
	if(m_bFullScreen)
	{
		pWnd = m_hWnd;
		uiFlags |= NiDX9RendererEx::USE_FULLSCREEN;
	}
	else
	{
		pWnd = m_RenderWnd;
	}			
	
	//������ ����
	m_spRenderer = NiDX9RendererEx::Create(
			m_nWidth,
			m_nHeight,
			(NiDX9Renderer::FlagType)uiFlags,
			pWnd,
			pWnd,								//������ ��Ŀ��
			D3DADAPTER_DEFAULT,					//����Ʈ ����� ���
			NiDX9Renderer::DEVDESC_PURE,		//�ϵ���� ���ؽ� ���μ���, ���ؽ�/�ȼ� ���̵�, �Ϲ������� ���� ������ �۾��� ������ �� �ִ� �ϵ��� �����մϴ�. �ſ� ������ ����̽���.
			nFrameBufferFormat,					//������ ���� ���� BPP = 32
			nDepthBufferFormat,					//depth- ���ٽ� ���� ���� depth Bpp = 24, stensil = 8
			NiDX9Renderer::PRESENT_INTERVAL_ONE,// swap rate : ���ø����̼ǿ��� ������� ���÷��� ������ �����ϰ� ����
			NiDX9Renderer::SWAPEFFECT_DISCARD,  // SwapEffect : ���۹��� ����ڿ��� �����ϴ� ��� DISCARD = ���� ������ ���� ������ ��ü ����� ���
			NiDX9Renderer::FBMODE_DEFAULT,		// ������ ���� ��� : ���ø����̼��� ���� �� �ִ� ������ ���۳� �ڵ����� Ǯ ��ũ�� ��Ƽ�ٸ������ �����Ͽ� ������ ���ۿ� ���ϴ� Ư���� ������ ����
			1,									// ������� ���� ����Ʈ 1
			0				// DX9 ����� �ֻ��� : Ǯ��ũ�� �������� �ϵ����� ���� ��Ȯ�� �ֻ����� ����
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

	//�̹��� ������ ����
	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
		
	return true;
}


NiColor		CGameApp::GetBackGroundColor()
{
	return m_BackGroundColor;
}

//��ǲ �ý��� ����
bool CGameApp::CreateInputSystem()
{
	//��ǲ �Ķ���� ����
	NiDI8InputSystem::DI8CreateParams* pkNi8Param = 
        NiNew NiDI8InputSystem::DI8CreateParams();
	assert(pkNi8Param);

	pkNi8Param->SetRenderer(m_spRenderer);
	pkNi8Param->SetKeyboardUsage(
		 NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE);

	//NiInputSystem::EXCLUSIVE - �� ���ø����̼��� ��� ���콺 �̺�Ʈ�� ���������ϰ� ���ݴϴ�. 
	//NiInputSystem::NONEXCLUSIVE - ������ ���콺�� ���� �� ��� �÷��װ�
	pkNi8Param->SetMouseUsage(NiInputSystem::FOREGROUND | NiInputSystem::EXCLUSIVE);
	pkNi8Param->SetOwnerInstance(m_hInst);
	pkNi8Param->SetOwnerWindow(m_hWnd); 
	
	//inputSystem ����
	NiInputSystem::CreateParams* pkParams = pkNi8Param;
	m_spInputSystem = NiInputSystem::Create(pkParams);
	NiDelete pkParams;
	if(!m_spInputSystem)
	{
		NiMessageBox("CreateInputSystem: Creation failed.","NiApplication Error");
		return false;
	}

	//�ڵ����� ��ǲ �ý����� ������ ����̽��� ����� ���� �����Ѵ�.
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
		//�̵�� ���� ����
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
	

	// �̱� �׽�Ʈ
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

/*//��Ʈ ����
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
	//������ ���°� ���� ��� �����͸� NULL�� ������ְ� �ʱ�ȭ.
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

//Ŀ�� ��ġ �̵�
void CGameApp::MoveCursor(int x,int y)
{
	if(!m_spCursor)
		return;
	m_spCursor->SetPosition(0.0f,x,y);
}


/*//�� �׷��� �ܿ� �繰 �׸���
void CGameApp::RenderScreenItem()
{
	m_spRenderer->SetScreenSpaceCameraData();    
	m_spCursor->Draw();//Ŀ�� �׸���
	m_spText->Draw(m_spRenderer);
	
}
*/

//��� ������ ���⼭ ó�� 
void CGameApp::UpdateFrame()
{	
	m_StateManager.Update(m_fFrameTime);
}

//�Է� ó��
void CGameApp::ProcessInput()
{
	//��ǲ ��� ����
	SetInputMode();	

	if( m_CurrState ) m_CurrState->OnProcessInput();
}

void CGameApp::SetInputMode()
{
	if(!m_spInputSystem)
		return ;

	//��ǲ ��尡 mapped ���
	if(m_spInputSystem->GetMode() == NiInputSystem::MAPPED)
	{
		if(m_spInputSystem->UpdateActionMap() == NIIERR_DEVICELOST)
			m_spInputSystem->HandleDeviceChanges();
	}
	//��ǲ ��尡 immediate ���
	else
	{
		if(m_spInputSystem->UpdateAllDevices() == NIIERR_DEVICELOST)
			m_spInputSystem->HandleDeviceChanges();
	}	 
}


/*
//���콺 �Է� ó��
void CGameApp::ProcessMouse()
{
	NiInputMouse* pkMouse = m_spInputSystem->GetMouse();
	
	if(pkMouse)
	{
		int iX,iY,iZ = 0;
		if(pkMouse->GetPositionDelta(iX,iY,iZ))
		{
			//Ŀ���� �߾ӿ� ����
			if((iX != 0 ) || (iY !=0))
			{
				//m_spCursor->Move(0.0f,iX,iY);								
			}
		}
	}	
}*/


//���ø����̼� �ð� ����
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
	// ������ Data�� Minm Max�� ���Ѵ�.
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

//�޽��� ����o
int CGameApp::BeginGame()
{
	MSG		msg;

	//���� ����
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
				 m_bIsHangul = true; // �ѱ� ������... 
			 else
				 m_bIsHangul = false; // ���� ������... 

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
			CEGUI::System::getSingleton().injectCharSelect(false);      //IME������ ������ ������ ���� ��Ų��
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

   // ���� ����
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
	// IME ���� �������̶��
	else if(lParam & GCS_COMPSTR)       //IME ������
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

	//  IME �ڵ� ����
	ImmReleaseContext( hWnd, hImc );

}
