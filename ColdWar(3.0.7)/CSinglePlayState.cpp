#include "GameApp.h"
#include "CStateManager.h"
#include "CSinglePlayState.h"
#include "CLoadingDisplay.h"
#include "WorldManager.h"
#include "strconv.h"





#ifdef NET_TEST

//#include "WorldManager.h"
#include "CharacterManager.h"
#endif

/*
CSinglePlayState::CGamePlayState() 
{

	m_pMenuBox		= NULL;
	m_pChatEditBox	= NULL;
	m_pStaticInput	= NULL;
	m_pChatListBox	= NULL;

	m_bActivaChaEdit = false;

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );
	::ZeroMemory( m_pLogoImg, sizeof(m_pLogoImg) );	
}
*/

CSinglePlayState::CSinglePlayState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) : 
CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
	m_pMenuBox			= NULL;
	
	m_pBGChannel		= NULL;

}


CSinglePlayState::~CSinglePlayState()
{
//	ExitInputSystem();
}

bool CSinglePlayState::CreateUI()
{

	const char *szFont1 = "ahn_m";

	m_pMenuBox			= NULL;
	
	using namespace CEGUI;
	
	if (m_pGUI != 0)
		return false;
	
	m_pGUI = WindowManager::getSingleton().loadWindowLayout(String(m_LayoutFilename));

	if (!m_pGUI)
		return false;

	// Retrieve the window manager
	WindowManager& winMgr = WindowManager::getSingleton();

	

	//------------------------------------------------------------------------------------------
	//
	//					
	//									프레임 윈도우 Enable
	//	
	//
	//------------------------------------------------------------------------------------------

	// MenuBox FrameWindow 박스 숨기기
	m_pMenuBox = reinterpret_cast< CEGUI::FrameWindow* >( m_pGUI->getChild("Root/MenuBox") );
	
	if (m_pMenuBox)
	{
		m_pMenuBox->setFont((CEGUI::utf8*)szFont1);
		m_pMenuBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Game Menu")); 
		m_pMenuBox->setTitleBarEnabled( true );
		m_pMenuBox->setEnabled( false );
		m_pMenuBox->setVisible( false );
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									버튼 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	if( m_pMenuBox )
	{
		CEGUI::Window *pGameExit = m_pMenuBox->getChild("Root/MenuBox/Exit_Btn");

		if (pGameExit)
		{
			pGameExit->subscribeEvent(CEGUI::PushButton::EventClicked, 
									  CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxExitBtnClick, this));

			pGameExit->setFont((CEGUI::utf8*)szFont1);
			pGameExit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("나가기")); 

		}
		
		CEGUI::Window *pCancel = m_pMenuBox->getChild("Root/MenuBox/Cancel_Btn");

		if (pCancel)
		{
			pCancel->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxCancelBtnClick, this));

			pCancel->setFont((CEGUI::utf8*)szFont1);
			pCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("취소")); 
		}

		CEGUI::Window *pOption = m_pMenuBox->getChild("Root/MenuBox/Option_Btn");

		if (pOption)
		{
			pOption->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxOptionBtnClick, this));

			pOption->setFont((CEGUI::utf8*)szFont1);
			pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("옵션")); 
		}

		CEGUI::Window *pVote = m_pMenuBox->getChild("Root/MenuBox/Vote_Btn");

		if (pVote)
		{
			pVote->subscribeEvent(CEGUI::PushButton::EventClicked, 
								  CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxVoteBtnClick, this));

			pVote->setFont((CEGUI::utf8*)szFont1);
			pVote->setText((CEGUI::utf8*)CONV_MBCS_UTF8("투표")); 
		}
	}


	//------------------------------------------------------------------------------------------
	//
	//					
	//									스태틱 텍스트 설정
	//	
	//
	//------------------------------------------------------------------------------------------

	//	ID 입력 Edit 박스 이벤트 추가
	CEGUI::Window *pStatic = m_pGUI->getChild("Root/Chat_Static");

	if( pStatic ) pStatic->setVisible(false);

	pStatic	= m_pGUI->getChild("Root/Respone_Static");

	if( pStatic ) pStatic->setVisible(false);

	pStatic	= m_pGUI->getChild("Root/Territory_Static");

	if( pStatic ) pStatic->setVisible(false);

	pStatic	= m_pGUI->getChild("Root/Win_Static");

	if( pStatic ) pStatic->setVisible(false);

	//------------------------------------------------------------------------------------------
	//
	//					
	//									리스트 박스 설정
	//	
	//
	//------------------------------------------------------------------------------------------

	CEGUI::Window *pList = reinterpret_cast<CEGUI::Listbox*>(m_pGUI->getChild("Root/ChatListBox"));
	
	if( pList )	pList->setVisible( false );

	//------------------------------------------------------------------------------------------
	//
	//					
	//									에디트 박스 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	CEGUI::Window *pEdit = reinterpret_cast<CEGUI::Editbox*>( m_pGUI->getChild("Root/Chat_Edit") );

	if( pEdit )
	{
		pEdit->setVisible( false );
		pEdit->deactivate();
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									리스폰 진행바 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------
	CEGUI::Window *pRespone = reinterpret_cast<CEGUI::ProgressBar*>( m_pGUI->getChild("Root/Respone_Progressbar") );

	if( pRespone ) pRespone->setVisible( false );

	//------------------------------------------------------------------------------------------
	//
	//					
	//									멀티 컬럼 리스트 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/EU"))->setVisible( false );
	reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/USA"))->setVisible( false );	
	reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/Guerrilla"))->setVisible( false );

	CEGUI::MouseCursor::getSingleton().hide();

	return true;
}


bool CSinglePlayState::Initialize()
{
	//배경 사운드 로드
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Wind Ambient.wav",eSoundCode::SOUND_BG_PLAY,
		CSoundManager::DEF_SOUND_TYPE_2D))
	{
		NiMessageBox("파일이 없거나 잘못된 경로입니다.","Error");
		return false;
	}
	//음악 재생
	if(!CSoundManager::GetInstance()->Play2D(eSoundCode::SOUND_BG_PLAY,m_pBGChannel,true))
		return false;

	//모든 이펙트 사운드 로드
	if(!LoadAllEffectSound())
		return false;

	CGameApp::mp_Appication->SetIME(false);
	//------------------------------------------------------------------------------------------------------
	//
	//						게임 UI Event 생성 및 설정
	//
	//------------------------------------------------------------------------------------------------------

	if( !CreateUI() )
		return false;

	//------------------------------------------------------------------------------------------------------
	//
	//						게임 렌더링 데이타 생성 및 초기화
	//
	//------------------------------------------------------------------------------------------------------

	//월드 매니저 생성
	if(!WorldManager::Create( false ))
	{
		NiMessageBox("CPlayState::Init -- WorldManager Created Failed\n",NULL);
		return false;
	}	

	//월드 매니저 초기화
	if(!WorldManager::GetWorldManager()->Initialize())
	{
		NiMessageBox("CPlayState::Init -- WorldManager 초기화 실패\n",NULL);
		return false;
	}

	// 싱글모드 셋팅
	WorldManager::GetWorldManager()->SetNetMode( false );

	CLoadingDisplay::GetInstance()->Render(100.0f);

	CLoadingDisplay::Destroy();

	return true;
}

bool CSinglePlayState::LoadAllEffectSound()
{
	//총알 지나가는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/bulletair.wav",eSoundCode::SOUND_WAV_BULLETTAIR))
		return false;
	//깃발 뻇김
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/LostFlag.wav",eSoundCode::SOUND_WAV_LOSTFLAG))
		return false;
	//깃발 뺏음
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/CaptureFlag.wav",eSoundCode::SOUND_WAV_CAPTUREFLAG))
		return false;
	//총알 벽에 튕기는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Bullet_impact.wav",eSoundCode::SOUND_WAV_BULLETIMPACT))
		return false;
	//탄창 빼는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/clipout.wav",eSoundCode::SOUND_WAV_CLIPOUT))
		return false;
	//탄창끼는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/clipin.wav",eSoundCode::SOUND_WAV_CLIPIN))
		return false;
	//탄피 떨어지는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/sout.wav",eSoundCode::SOUND_WAV_SOUT))
		return false;
	//죽는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/death.wav",eSoundCode::SOUND_WAV_DEATH))
		return false;
	//총에 맞는 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/hit.wav",eSoundCode::SOUND_WAV_HIT))
		return false;
	//왼발 스텝
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/stepLeft.wav",eSoundCode::SOUND_WAV_STEPLEFT))
		return false;
	//오른발 스텝
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/stepRight.wav",eSoundCode::SOUND_WAV_STEPRIGHT))
		return false;
	//빈총 소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/empty.mp3",eSoundCode::SOUND_WAV_EMPTY))
		return false;
	//AK 총소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/AK_fire.wav",eSoundCode::SOUND_WAV_AKFIRE))
		return false;
	//파마스 총소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Famas_fire.wav",eSoundCode::SOUND_WAV_FAMASFIRE))
		return false;
	//M4 총소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/M4_fire.wav",eSoundCode::SOUND_WAV_M4FIRE))
		return false;
	//AWP 총소리
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/AWP_fire.wav",eSoundCode::SOUND_WAV_AWPFIRE))
		return false;
	
	return true;
}

void CSinglePlayState::OnRender()
{
	WorldManager::GetWorldManager()->Draw();	
}

	
void CSinglePlayState::OnUpdate(float fCurrentTime)
{
	// update the base state
	CGameUIState::OnUpdate(fCurrentTime);

	WorldManager::GetWorldManager()->Update();
}


void CSinglePlayState::ProcessKeyBoardForUI()
{
	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

    if (!pkKeyboard)
        return;

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
	{
		if( !m_pMenuBox->isVisible() )
		{
			m_pMenuBox->setVisible( true );
			m_pMenuBox->setEnabled( true );
			CEGUI::MouseCursor::getSingleton().show();
			WorldManager::GetWorldManager()->SetActiveMenuMode( true );
			return;
		}
		else
		{
			m_pMenuBox->setVisible( false );
			m_pMenuBox->setEnabled( false );
			CEGUI::MouseCursor::getSingleton().hide();
			WorldManager::GetWorldManager()->SetActiveMenuMode( false );
			return;
		}

	}
}

void CSinglePlayState::OnLeave(const char* szNext)
{
	//배경 사운드 중지
	 if(m_pBGChannel)
		 m_pBGChannel->stop();

	if (!CGameApp::mp_Appication)
		return;

	//월드 렌더링 Data Destroy
	WorldManager::Destroy();

	CGameUIState::OnLeave(szNext);
}


void CSinglePlayState::OnOverride(const char* szNext)
{
	CGameBaseState::OnOverride(szNext);
}


void CSinglePlayState::OnResume(const char* szPrevious)
{
	CGameBaseState::OnResume(szPrevious);
}


//키보드 처리 함수
void CSinglePlayState::OnProcessInput()
{
//	ProcessKeyBoard();
	ProcessKeyBoardForUI();
	ProcessMouse();
}

//마우스 처리 함수
void CSinglePlayState::ProcessMouse()
{
	
}


void CSinglePlayState::ProcessKeyBoard()
{
	NiInputSystem*	inputSystem = CGameApp::mp_Appication->GetInputSystem();

	//Update input Device
	if(!inputSystem)
		return;
		
	//키 입력 처리
	NiInputKeyboard* pkKeyboard = inputSystem->GetKeyboard();
	
	//자유 카메라인 경우
	//키보드 입력에 따른 각각의 처리
	if(pkKeyboard)
	{
	
#ifdef NET_TEST
		if(pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_1))
		{
			//와이어 프레임으로 씬을 렌더
			 //ms_spWireframe->SetWireframe(!ms_spWireframe->GetWireframe());

			NiPoint3 kPos(0.0f,30.0f + idx++,0.0f);
			NiMatrix3 kRot;
			NiTransform kTrans;

			kRot.MakeZRotation(4.0f);
			
			kTrans.m_Translate = kPos;
			kTrans.m_Rotate = kRot;
			kTrans.m_fScale = 1.0f;

			WorldManager::GetWorldManager()->AddCharacter(BranchOfMilitary::Gerilla,kTrans);
		}
		else if(pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_2))
		{
			CharacterManager* pCM = WorldManager::GetWorldManager()->GetCharacterManager();

			for(unsigned int i = 0;i<idx;i++)
			{
				CCharacter* OtherChar = pCM->GetCharacterIndex(i);

				NiNode* pkNode = OtherChar->GetCharRoot();
				NiPoint3 pos = pkNode->GetTranslate();

				pos.x++;
				pkNode->SetTranslate(pos);
			}
		}
#endif

	}
}

bool CSinglePlayState::Event_MenuBoxExitBtnClick(const CEGUI::EventArgs &rE)
{
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	rStateManager.ChangeState("MainMenu");

//	CGameApp::mp_Appication->QuitApplication();

	return true;
}
bool CSinglePlayState::Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE)
{
	if( m_pMenuBox ) m_pMenuBox->setVisible( false );

	return true;
}
bool CSinglePlayState::Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}
bool CSinglePlayState::Event_MenuBoxVoteBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}

