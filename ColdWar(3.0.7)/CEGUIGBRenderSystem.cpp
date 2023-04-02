#include "CEGUIGBRenderSystem.h"
#include <CEGUIDefaultResourceProvider.h>

using namespace CEGUI;

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif  //#ifndef DIRECTINPUT_VERSION
#include <DInput.h>
#include "GameApp.h"

CEGUIGBRenderSystem::CEGUIGBRenderSystem() :
m_fKeyRepeatDelay(0.25f), 
m_fKeyRepeatRate(0.05f)
{

	FillMapping();

	CEGUI::System *pGUISys = CEGUI::System::getSingletonPtr();

	if (!pGUISys)
	{
		m_pkCEGUIRenderer = new CEGUIGBRenderer( CGameApp::mp_Appication->GetRenderer() );
		pGUISys = new CEGUI::System(m_pkCEGUIRenderer);
		pGUISys = CEGUI::System::getSingletonPtr();
	}
	else
	{
		m_pkCEGUIRenderer = (CEGUIGBRenderer*)pGUISys->getRenderer();
	}

	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

    CEGUI::DefaultResourceProvider* rp = (CEGUI::DefaultResourceProvider*)
        CEGUI::System::getSingleton().getResourceProvider();

	CGameApp::SetMediaPath("./UI/");

    rp->setResourceGroupDirectory("schemes",		CGameApp::ConvertMediaFilename("schemes/"));
    rp->setResourceGroupDirectory("imagesets",		CGameApp::ConvertMediaFilename("imagesets/"));
    rp->setResourceGroupDirectory("fonts",			CGameApp::ConvertMediaFilename("fonts/"));
    rp->setResourceGroupDirectory("layouts",		CGameApp::ConvertMediaFilename("layouts/"));
    rp->setResourceGroupDirectory("looknfeels",		CGameApp::ConvertMediaFilename("looknfeel/"));
    rp->setResourceGroupDirectory("lua_scripts",	CGameApp::ConvertMediaFilename("lua_scripts/"));

    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

}

CEGUIGBRenderSystem::~CEGUIGBRenderSystem()
{
	delete m_pkCEGUIRenderer;
}

void CEGUIGBRenderSystem::PerformRendering()
{
    ProcessInput();
    CEGUI::System::getSingleton().renderGUI();
}

void CEGUIGBRenderSystem::FillMapping()
{
    memset((void*)m_aucNIIToDIMapping, 0, sizeof(m_aucNIIToDIMapping));

	m_aucNIIToDIMapping[NiInputKeyboard::KEY_ESCAPE] = DIK_ESCAPE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_1] = DIK_1;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_2] = DIK_2;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_3] = DIK_3;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_4] = DIK_4;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_5] = DIK_5;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_6] = DIK_6;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_7] = DIK_7;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_8] = DIK_8;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_9] = DIK_9;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_0] = DIK_0;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MINUS] = DIK_MINUS;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_EQUALS] = DIK_EQUALS;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_BACK] = DIK_BACK;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_TAB] = DIK_TAB;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_Q] = DIK_Q;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_W] = DIK_W;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_E] = DIK_E;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_R] = DIK_R;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_T] = DIK_T;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_Y] = DIK_Y;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_U] = DIK_U;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_I] = DIK_I;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_O] = DIK_O;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_P] = DIK_P;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LBRACKET] = DIK_LBRACKET;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RBRACKET] = DIK_RBRACKET;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RETURN] = DIK_RETURN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LCONTROL] = DIK_LCONTROL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_A] = DIK_A;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_S] = DIK_S;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_D] = DIK_D;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F] = DIK_F;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_G] = DIK_G;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_H] = DIK_H;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_J] = DIK_J;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_K] = DIK_K;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_L] = DIK_L;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SEMICOLON] = DIK_SEMICOLON;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_APOSTROPHE] = DIK_APOSTROPHE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_GRAVE] = DIK_GRAVE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LSHIFT] = DIK_LSHIFT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_BACKSLASH] = DIK_BACKSLASH;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_Z] = DIK_Z;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_X] = DIK_X;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_C] = DIK_C;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_V] = DIK_V;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_B] = DIK_B;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_N] = DIK_N;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_M] = DIK_M;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_COMMA] = DIK_COMMA;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_PERIOD] = DIK_PERIOD;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SLASH] = DIK_SLASH;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RSHIFT] = DIK_RSHIFT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MULTIPLY] = DIK_MULTIPLY;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LMENU] = DIK_LMENU;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SPACE] = DIK_SPACE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_CAPITAL] = DIK_CAPITAL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F1] = DIK_F1;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F2] = DIK_F2;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F3] = DIK_F3;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F4] = DIK_F4;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F5] = DIK_F5;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F6] = DIK_F6;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F7] = DIK_F7;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F8] = DIK_F8;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F9] = DIK_F9;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F10] = DIK_F10;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMLOCK] = DIK_NUMLOCK;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SCROLL] = DIK_SCROLL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD7] = DIK_NUMPAD7;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD8] = DIK_NUMPAD8;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD9] = DIK_NUMPAD9;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SUBTRACT] = DIK_SUBTRACT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD4] = DIK_NUMPAD4;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD5] = DIK_NUMPAD5;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD6] = DIK_NUMPAD6;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_ADD] = DIK_ADD;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD1] = DIK_NUMPAD1;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD2] = DIK_NUMPAD2;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD3] = DIK_NUMPAD3;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPAD0] = DIK_NUMPAD0;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_DECIMAL] = DIK_DECIMAL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_OEM_102] = DIK_OEM_102;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F11] = DIK_F11;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F12] = DIK_F12;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F13] = DIK_F13;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F14] = DIK_F14;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_F15] = DIK_F15;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_KANA] = DIK_KANA;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_ABNT_C1] = DIK_ABNT_C1;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_CONVERT] = DIK_CONVERT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NOCONVERT] = DIK_NOCONVERT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_YEN] = DIK_YEN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_ABNT_C2] = DIK_ABNT_C2;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPADEQUALS] = DIK_NUMPADEQUALS;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_PREVTRACK] = DIK_PREVTRACK;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_AT] = DIK_AT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_COLON] = DIK_COLON;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_UNDERLINE] = DIK_UNDERLINE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_KANJI] = DIK_KANJI;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_STOP] = DIK_STOP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_AX] = DIK_AX;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_UNLABELED] = DIK_UNLABELED;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NEXTTRACK] = DIK_NEXTTRACK;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPADENTER] = DIK_NUMPADENTER;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RCONTROL] = DIK_RCONTROL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MUTE] = DIK_MUTE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_CALCULATOR] = DIK_CALCULATOR;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_PLAYPAUSE] = DIK_PLAYPAUSE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MEDIASTOP] = DIK_MEDIASTOP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_VOLUMEDOWN] = DIK_VOLUMEDOWN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_VOLUMEUP] = DIK_VOLUMEUP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBHOME] = DIK_WEBHOME;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NUMPADCOMMA] = DIK_NUMPADCOMMA;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_DIVIDE] = DIK_DIVIDE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SYSRQ] = DIK_SYSRQ;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RMENU] = DIK_RMENU;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_PAUSE] = DIK_PAUSE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_HOME] = DIK_HOME;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_UP] = DIK_UP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_PRIOR] = DIK_PRIOR;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LEFT] = DIK_LEFT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RIGHT] = DIK_RIGHT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_END] = DIK_END;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_DOWN] = DIK_DOWN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_NEXT] = DIK_NEXT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_INSERT] = DIK_INSERT;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_DELETE] = DIK_DELETE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_LWIN] = DIK_LWIN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_RWIN] = DIK_RWIN;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_APPS] = DIK_APPS;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_POWER] = DIK_POWER;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_SLEEP] = DIK_SLEEP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WAKE] = DIK_WAKE;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBSEARCH] = DIK_WEBSEARCH;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBFAVORITES] = DIK_WEBFAVORITES;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBREFRESH] = DIK_WEBREFRESH;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBSTOP] = DIK_WEBSTOP;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBFORWARD] = DIK_WEBFORWARD;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_WEBBACK] = DIK_WEBBACK;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MYCOMPUTER] = DIK_MYCOMPUTER;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MAIL] = DIK_MAIL;
    m_aucNIIToDIMapping[NiInputKeyboard::KEY_MEDIASELECT] = DIK_MEDIASELECT;

	m_ucPreviouslyPressedKey = 0;
}


//---------------------------------------------------------------------------
void CEGUIGBRenderSystem::ProcessInput()
{
	if( CGameApp::mp_Appication->IsIME() == true ) return;

	CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectTimePulse(CGameApp::mp_Appication->GetFrameTime());

	int					iX, iY, iZ;

	NiInputMouse*		pkMouse = CGameApp::mp_Appication->GetInputSystem()->GetMouse();

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

	
	if(pkKeyboard)
	{		
		unsigned char acState[256];
		GetKeyboardState(acState);
		HKL hLayout = GetKeyboardLayout(0);
		unsigned char ucJustPressedKey = 0;

		for(int iKeyCode = NiInputKeyboard::KEY_NOKEY + 1; 
			iKeyCode < NiInputKeyboard::KEY_TOTAL_COUNT; iKeyCode++)
		{
			NiInputKeyboard::KeyCode eKeyCode = 
				(NiInputKeyboard::KeyCode)iKeyCode;

			unsigned char ucDIK = m_aucNIIToDIMapping[iKeyCode];

			if(pkKeyboard->KeyWasPressed(eKeyCode))
			{
				sys.injectKeyDown(ucDIK);

				ucJustPressedKey = ucDIK;
			}

			if(pkKeyboard->KeyWasReleased(eKeyCode))
			{
				sys.injectKeyUp(ucDIK);

				if(ucDIK == m_ucPreviouslyPressedKey)
					m_ucPreviouslyPressedKey = 0;
			}
		}

		if(ucJustPressedKey)
		{
			m_ucPreviouslyPressedKey = ucJustPressedKey;
			m_fTimeSinceEmitKey = m_fKeyRepeatRate - m_fKeyRepeatDelay;
		}

		if(ucJustPressedKey || (m_ucPreviouslyPressedKey && 
			m_fTimeSinceEmitKey > m_fKeyRepeatRate))
		{
			WORD awAscii[2];
			UINT uiVirtualKey = 
				MapVirtualKeyEx(m_ucPreviouslyPressedKey, 1, hLayout);
			int iNumChars = ToAsciiEx(uiVirtualKey, m_ucPreviouslyPressedKey, 
				acState, awAscii, 0, hLayout);

			for(int i = 0; i < iNumChars; i++) 
				sys.injectChar(awAscii[i] & 0xff);

			if(m_fTimeSinceEmitKey > m_fKeyRepeatRate)
			{
				sys.injectKeyDown(m_ucPreviouslyPressedKey);
				m_fTimeSinceEmitKey = 0.0f;
			}
		}
		
		m_fTimeSinceEmitKey += CGameApp::mp_Appication->GetFrameTime();
	}

	if(pkMouse)
	{
        if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT))
            sys.injectMouseButtonDown(CEGUI::LeftButton);
        if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_RIGHT))
            sys.injectMouseButtonDown(CEGUI::RightButton);
        if(pkMouse->ButtonWasPressed(NiInputMouse::NIM_MIDDLE))
            sys.injectMouseButtonDown(CEGUI::MiddleButton);

        pkMouse->GetPositionDelta(iX, iY, iZ);
        if(iZ) 
			sys.injectMouseWheelChange((float)iZ * 0.1f);
		if(iX || iY) 
            sys.injectMouseMove(float(iX), float(iY));

		if(pkMouse->ButtonWasReleased(NiInputMouse::NIM_LEFT))
			sys.injectMouseButtonUp(CEGUI::LeftButton);
		if(pkMouse->ButtonWasReleased(NiInputMouse::NIM_RIGHT))
			sys.injectMouseButtonUp(CEGUI::RightButton);
		if(pkMouse->ButtonWasReleased(NiInputMouse::NIM_MIDDLE))
			sys.injectMouseButtonUp(CEGUI::MiddleButton);
	}
}