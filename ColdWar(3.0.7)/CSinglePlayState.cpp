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
	//									������ ������ Enable
	//	
	//
	//------------------------------------------------------------------------------------------

	// MenuBox FrameWindow �ڽ� �����
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
	//									��ư �̺�Ʈ �߰�
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
			pGameExit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("������")); 

		}
		
		CEGUI::Window *pCancel = m_pMenuBox->getChild("Root/MenuBox/Cancel_Btn");

		if (pCancel)
		{
			pCancel->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxCancelBtnClick, this));

			pCancel->setFont((CEGUI::utf8*)szFont1);
			pCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���")); 
		}

		CEGUI::Window *pOption = m_pMenuBox->getChild("Root/MenuBox/Option_Btn");

		if (pOption)
		{
			pOption->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxOptionBtnClick, this));

			pOption->setFont((CEGUI::utf8*)szFont1);
			pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�ɼ�")); 
		}

		CEGUI::Window *pVote = m_pMenuBox->getChild("Root/MenuBox/Vote_Btn");

		if (pVote)
		{
			pVote->subscribeEvent(CEGUI::PushButton::EventClicked, 
								  CEGUI::Event::Subscriber(&CSinglePlayState::Event_MenuBoxVoteBtnClick, this));

			pVote->setFont((CEGUI::utf8*)szFont1);
			pVote->setText((CEGUI::utf8*)CONV_MBCS_UTF8("��ǥ")); 
		}
	}


	//------------------------------------------------------------------------------------------
	//
	//					
	//									����ƽ �ؽ�Ʈ ����
	//	
	//
	//------------------------------------------------------------------------------------------

	//	ID �Է� Edit �ڽ� �̺�Ʈ �߰�
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
	//									����Ʈ �ڽ� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	CEGUI::Window *pList = reinterpret_cast<CEGUI::Listbox*>(m_pGUI->getChild("Root/ChatListBox"));
	
	if( pList )	pList->setVisible( false );

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� �̺�Ʈ �߰�
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
	//									������ ����� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------
	CEGUI::Window *pRespone = reinterpret_cast<CEGUI::ProgressBar*>( m_pGUI->getChild("Root/Respone_Progressbar") );

	if( pRespone ) pRespone->setVisible( false );

	//------------------------------------------------------------------------------------------
	//
	//					
	//									��Ƽ �÷� ����Ʈ �̺�Ʈ �߰�
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
	//��� ���� �ε�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Wind Ambient.wav",eSoundCode::SOUND_BG_PLAY,
		CSoundManager::DEF_SOUND_TYPE_2D))
	{
		NiMessageBox("������ ���ų� �߸��� ����Դϴ�.","Error");
		return false;
	}
	//���� ���
	if(!CSoundManager::GetInstance()->Play2D(eSoundCode::SOUND_BG_PLAY,m_pBGChannel,true))
		return false;

	//��� ����Ʈ ���� �ε�
	if(!LoadAllEffectSound())
		return false;

	CGameApp::mp_Appication->SetIME(false);
	//------------------------------------------------------------------------------------------------------
	//
	//						���� UI Event ���� �� ����
	//
	//------------------------------------------------------------------------------------------------------

	if( !CreateUI() )
		return false;

	//------------------------------------------------------------------------------------------------------
	//
	//						���� ������ ����Ÿ ���� �� �ʱ�ȭ
	//
	//------------------------------------------------------------------------------------------------------

	//���� �Ŵ��� ����
	if(!WorldManager::Create( false ))
	{
		NiMessageBox("CPlayState::Init -- WorldManager Created Failed\n",NULL);
		return false;
	}	

	//���� �Ŵ��� �ʱ�ȭ
	if(!WorldManager::GetWorldManager()->Initialize())
	{
		NiMessageBox("CPlayState::Init -- WorldManager �ʱ�ȭ ����\n",NULL);
		return false;
	}

	// �̱۸�� ����
	WorldManager::GetWorldManager()->SetNetMode( false );

	CLoadingDisplay::GetInstance()->Render(100.0f);

	CLoadingDisplay::Destroy();

	return true;
}

bool CSinglePlayState::LoadAllEffectSound()
{
	//�Ѿ� �������� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/bulletair.wav",eSoundCode::SOUND_WAV_BULLETTAIR))
		return false;
	//��� �P��
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/LostFlag.wav",eSoundCode::SOUND_WAV_LOSTFLAG))
		return false;
	//��� ����
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/CaptureFlag.wav",eSoundCode::SOUND_WAV_CAPTUREFLAG))
		return false;
	//�Ѿ� ���� ƨ��� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Bullet_impact.wav",eSoundCode::SOUND_WAV_BULLETIMPACT))
		return false;
	//źâ ���� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/clipout.wav",eSoundCode::SOUND_WAV_CLIPOUT))
		return false;
	//źâ���� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/clipin.wav",eSoundCode::SOUND_WAV_CLIPIN))
		return false;
	//ź�� �������� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/sout.wav",eSoundCode::SOUND_WAV_SOUT))
		return false;
	//�״� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/death.wav",eSoundCode::SOUND_WAV_DEATH))
		return false;
	//�ѿ� �´� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/hit.wav",eSoundCode::SOUND_WAV_HIT))
		return false;
	//�޹� ����
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/stepLeft.wav",eSoundCode::SOUND_WAV_STEPLEFT))
		return false;
	//������ ����
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/stepRight.wav",eSoundCode::SOUND_WAV_STEPRIGHT))
		return false;
	//���� �Ҹ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/empty.mp3",eSoundCode::SOUND_WAV_EMPTY))
		return false;
	//AK �ѼҸ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/AK_fire.wav",eSoundCode::SOUND_WAV_AKFIRE))
		return false;
	//�ĸ��� �ѼҸ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Famas_fire.wav",eSoundCode::SOUND_WAV_FAMASFIRE))
		return false;
	//M4 �ѼҸ�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/M4_fire.wav",eSoundCode::SOUND_WAV_M4FIRE))
		return false;
	//AWP �ѼҸ�
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
	//��� ���� ����
	 if(m_pBGChannel)
		 m_pBGChannel->stop();

	if (!CGameApp::mp_Appication)
		return;

	//���� ������ Data Destroy
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


//Ű���� ó�� �Լ�
void CSinglePlayState::OnProcessInput()
{
//	ProcessKeyBoard();
	ProcessKeyBoardForUI();
	ProcessMouse();
}

//���콺 ó�� �Լ�
void CSinglePlayState::ProcessMouse()
{
	
}


void CSinglePlayState::ProcessKeyBoard()
{
	NiInputSystem*	inputSystem = CGameApp::mp_Appication->GetInputSystem();

	//Update input Device
	if(!inputSystem)
		return;
		
	//Ű �Է� ó��
	NiInputKeyboard* pkKeyboard = inputSystem->GetKeyboard();
	
	//���� ī�޶��� ���
	//Ű���� �Է¿� ���� ������ ó��
	if(pkKeyboard)
	{
	
#ifdef NET_TEST
		if(pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_1))
		{
			//���̾� ���������� ���� ����
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

