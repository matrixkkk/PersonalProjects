#include "GameApp.h"
#include "CStateManager.h"
#include "CGamePlayState.h"
#include "CGameResultState.h"
#include "CLoadingDisplay.h"
#include "WorldManager.h"
#include "strconv.h"
#include "ZFLog.h"


const char *szFont1 = "ahn_m";


#ifdef NET_TEST

//#include "WorldManager.h"
#include "CharacterManager.h"
#endif

/*
CGamePlayState::CGamePlayState() 
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

CGamePlayState::CGamePlayState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) : 
CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
	idx					= 0;
	m_pMenuBox			= NULL;
	m_pChatEditBox		= NULL;
	m_pStaticInput		= NULL;
	m_pChatListBox		= NULL;
	m_pStaticRespone	= NULL;
	m_pStaticTerritory	= NULL;
	m_pProgressbarRespone = NULL;
	m_pProgressbarRebirth = NULL;
	m_pStaticWin		= NULL;

	m_bActivaChaEdit	= false;


	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );
	::ZeroMemory( m_pLogoImg, sizeof(m_pLogoImg) );	

	m_fTimeTerritory	= 0.0f;
	m_fTimeWinGame		= 0.0f;

	m_pBGChannel		= NULL;

}


CGamePlayState::~CGamePlayState()
{
//	ExitInputSystem();
}

bool CGamePlayState::CreateUI()
{

	m_pMenuBox			= NULL;
	m_pChatEditBox		= NULL;
	m_pStaticInput		= NULL;
	m_pChatListBox		= NULL;
	m_pStaticRespone	= NULL;
	m_pStaticTerritory	= NULL;	
	m_pProgressbarRespone = NULL;
	m_pProgressbarRebirth = NULL;
	m_pStaticWin		= NULL;

	m_bActivaChaEdit	= false;

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );
	::ZeroMemory( m_pLogoImg, sizeof(m_pLogoImg) );	

	m_fTimeTerritory = 0.0f;
	m_fTimeWinGame	= 0.0f;

	using namespace CEGUI;
	
	if (m_pGUI != 0)
		return false;
	
	ImagesetManager::getSingleton().createImageset(String("USA.imageset"));
	ImagesetManager::getSingleton().createImageset(String("EU.imageset"));
	ImagesetManager::getSingleton().createImageset(String("Guerrilla.imageset"));

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
									  CEGUI::Event::Subscriber(&CGamePlayState::Event_MenuBoxExitBtnClick, this));

			pGameExit->setFont((CEGUI::utf8*)szFont1);
			pGameExit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("������")); 

		}
		
		CEGUI::Window *pCancel = m_pMenuBox->getChild("Root/MenuBox/Cancel_Btn");

		if (pCancel)
		{
			pCancel->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CGamePlayState::Event_MenuBoxCancelBtnClick, this));

			pCancel->setFont((CEGUI::utf8*)szFont1);
			pCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���")); 
		}

		CEGUI::Window *pOption = m_pMenuBox->getChild("Root/MenuBox/Option_Btn");

		if (pOption)
		{
			pOption->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CGamePlayState::Event_MenuBoxOptionBtnClick, this));

			pOption->setFont((CEGUI::utf8*)szFont1);
			pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�ɼ�")); 
		}

		CEGUI::Window *pVote = m_pMenuBox->getChild("Root/MenuBox/Vote_Btn");

		if (pVote)
		{
			pVote->subscribeEvent(CEGUI::PushButton::EventClicked, 
								  CEGUI::Event::Subscriber(&CGamePlayState::Event_MenuBoxVoteBtnClick, this));

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
	m_pStaticInput = m_pGUI->getChild("Root/Chat_Static");

	if( m_pStaticInput )
	{
		m_pStaticInput->setFont((CEGUI::utf8*)szFont1);
		m_pStaticInput->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�Է�:"));		
		m_pStaticInput->setVisible(false);
	}

	m_pStaticRespone	= m_pGUI->getChild("Root/Respone_Static");
	if( m_pStaticRespone )
	{
		m_pStaticRespone->setFont((CEGUI::utf8*)szFont1);
		m_pStaticRespone->setText((CEGUI::utf8*)CONV_MBCS_UTF8("����� �׾����ϴ�. 5���Ŀ� �ǻ�Ƴ��ϴ�."));		
		m_pStaticRespone->setVisible(false);
	}

	m_pStaticTerritory	= m_pGUI->getChild("Root/Territory_Static");
	if( m_pStaticTerritory )
	{
		m_pStaticTerritory->setFont((CEGUI::utf8*)szFont1);
		m_pStaticTerritory->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���󿡸����� KPU�� �����Ͽ����ϴ�."));		
		m_pStaticTerritory->setVisible(false);
	}

	m_pStaticWin	= m_pGUI->getChild("Root/Win_Static");
	if( m_pStaticWin )
	{
		m_pStaticWin->setFont((CEGUI::utf8*)szFont1);
		m_pStaticWin->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���󿡸����� �̰���ϴ�."));		
		m_pStaticWin->setVisible(false);
	}


	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pChatListBox =  reinterpret_cast<CEGUI::Listbox*>(m_pGUI->getChild("Root/ChatListBox"));
	
	if(m_pChatListBox) 
		m_pChatListBox->setFont((CEGUI::utf8*)szFont1);

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����ƽ �̹��� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pLogoImg[ USA ]		= reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/USA_Image") );

	m_pLogoImg[ EU ]		= reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/EU_Image") );

	m_pLogoImg[ GUERRILLA ] = reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/Guerrilla_Image") );


	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pChatEditBox = reinterpret_cast<CEGUI::Editbox*>( m_pGUI->getChild("Root/Chat_Edit") );
	m_pChatEditBox->setFont( (CEGUI::utf8*)szFont1 );
		
	//	����Ű or ��Ű�� �������� �̺�Ʈ �߰�
	if (m_pChatEditBox)
	{
		m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, 
								CEGUI::Event::Subscriber(&CGamePlayState::Event_ChatTextAdded, this));

		m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventActivated, 
									CEGUI::Event::Subscriber(&CGamePlayState::Event_EditBoxActivate, this));

		m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventDeactivated, 
									CEGUI::Event::Subscriber(&CGamePlayState::Event_EditBoxDeactivate, this));
	}

	m_pChatEditBox->setVisible( false );
	m_pChatEditBox->setEnabled( false );
	m_pChatEditBox->deactivate();

	//------------------------------------------------------------------------------------------
	//
	//					
	//									������ ����� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pProgressbarRespone = reinterpret_cast<CEGUI::ProgressBar*>( m_pGUI->getChild("Root/Respone_Progressbar") );

	if( m_pProgressbarRespone )
	{
		m_pProgressbarRespone->setProgress( 0.0f );
		m_pProgressbarRespone->setVisible( false );
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									��Ȱ ����� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pProgressbarRebirth = reinterpret_cast<CEGUI::ProgressBar*>( m_pGUI->getChild("Root/Rebirth_Progressbar") );

	if( m_pProgressbarRebirth )
	{
		m_pProgressbarRebirth->setProgress( 0.0f );
		m_pProgressbarRebirth->setVisible( false );
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									��Ƽ �÷� ����Ʈ �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	String strId	= (CEGUI::utf8*)CONV_MBCS_UTF8("�г���");
	String strKill	= (CEGUI::utf8*)CONV_MBCS_UTF8("ų");	
	String strDeath	= (CEGUI::utf8*)CONV_MBCS_UTF8("����");

	m_pListMcl[ EU ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/EU"));
	m_pListMcl[ USA ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/USA"));	
	m_pListMcl[ GUERRILLA ] = reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/Guerrilla"));	

	for( int i = 0; i < 3; i++ )
	{
		m_pListMcl[i]->setSelectionMode(CEGUI::MultiColumnList::SelectionMode::RowMultiple);
		m_pListMcl[i]->setShowVertScrollbar(false);
		m_pListMcl[i]->setShowHorzScrollbar(false);
		// Ascending	��������
		// Descending	��������
		m_pListMcl[i]->setSortDirection( ListHeaderSegment::SortDirection::Ascending );
			
		m_pListMcl[i]->setVisible( true );

		m_pListMcl[i]->addColumn(strId,		ID,		cegui_absdim(95));
		m_pListMcl[i]->addColumn(strKill,	KILL,	cegui_absdim(30));
		m_pListMcl[i]->addColumn(strDeath,	DEATH,	cegui_absdim(30));
		
	}

	int nRowUSA = 0, nRowEU = 0, nRowGue = 0;
	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	char pKillDeath[2] = {0,};

	for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		if( iterPlayer->second.userInfo.camp == nsPlayerInfo::USA )
		{
			m_pListMcl[this->USA]->addRow( iterPlayer->second.userInfo.id );

			m_pListMcl[this->USA]->setItem( new ListboxTextItem(iterPlayer->second.userRoomInfo.m_strID), ID, nRowUSA );
			
			sprintf_s( pKillDeath, "%d", iterPlayer->second.	userInfo.kill );
			m_pListMcl[this->USA]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowUSA );

			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.death );
			m_pListMcl[this->USA]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowUSA );

			++nRowUSA;

		}
		else if( iterPlayer->second.userInfo.camp == nsPlayerInfo::EU )
		{
			m_pListMcl[this->EU]->addRow( iterPlayer->second.userInfo.id );

			m_pListMcl[this->EU]->setItem( new ListboxTextItem(iterPlayer->second.userRoomInfo.m_strID), ID, nRowEU );

			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.kill );
			m_pListMcl[this->EU]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowEU );

			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.death );
			m_pListMcl[this->EU]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowEU );

			++nRowEU;
		}
		else
		{
			m_pListMcl[this->GUERRILLA]->addRow( iterPlayer->second.userInfo.id );

			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(iterPlayer->second.userRoomInfo.m_strID), ID, nRowGue );

			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.kill );
			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowGue );

			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.death );
			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowGue );

			++nRowGue;
		}
	}

	NetworkManager::GetInstance()->UnLockPlayers();

	for( int i = 0; i < 3; i++ )
	{
		m_pListMcl[i]->setVisible( false );
		m_pLogoImg[i]->setVisible( false );
	}

	CEGUI::MouseCursor::getSingleton().hide();

	return true;

}


bool CGamePlayState::Initialize()
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
	if(!WorldManager::Create( true ))
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

	

//	InitializeCriticalSection(&m_csMcl);
	InitializeCriticalSection(&m_csChatListBox);

	//------------------------------------------------------------------------------------------------------
	//					���� �̶�� ���ӿ� ������ Ŭ���� �ε� �Ϸ� �޼����� ��ٸ���.
	//------------------------------------------------------------------------------------------------------
	if( !NetworkManager::GetInstance()->IsCaptain() )
	{
		NetworkManager::GetInstance()->GetClient()->RequestGameJoin();				
	}

	CLoadingDisplay::GetInstance()->Render(100.0f);

		DWORD ret = NetworkManager::GetInstance()->WaitComplete();

	if( WAIT_FAILED		!= ret &&
		WAIT_ABANDONED	!= ret &&
		WAIT_TIMEOUT	!= ret )
	{
		NetworkManager::GetInstance()->ResetStartEvent();
		CLoadingDisplay::Destroy();
	}

	//------------------------------------------------------------------------------------------------------
	//					���� ��� ��ȯ �۽�
	//------------------------------------------------------------------------------------------------------
	if( NetworkManager::GetInstance()->IsCaptain() )
	{
		NetworkManager::GetInstance()->GetServer()->SendStartGame();
	}

	return true;
}

bool CGamePlayState::LoadAllEffectSound()
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

/*
void CGamePlayState::OnEnter(const char* szPrevious)
{
	CGameUIState::OnEnter(szPrevious);

	//���� �Ŵ��� ����
	if(!WorldManager::Create())
	{
		NiMessageBox("CPlayState::Init -- WorldManager Created Failed\n",NULL);
		return;
	}	

	//���� �Ŵ��� �ʱ�ȭ
	if(!WorldManager::GetWorldManager()->Initialize())
	{
		NiMessageBox("CPlayState::Init -- WorldManager �ʱ�ȭ ����\n",NULL);
		return;
	}

	// ���� �̶�� ���ӿ� ������ Ŭ�� �Ϸ� �޼����� ��ٸ���.
	if( !NetworkManager::GetInstance()->IsCaptain() )
	{
		NetworkManager::GetInstance()->GetClient()->RequestGameJoin();				
	}

	CLoadingDisplay::GetInstance()->Render(100.0f);

	DWORD ret = NetworkManager::GetInstance()->WaitComplete();

	if( WAIT_FAILED		!= ret &&
		WAIT_ABANDONED	!= ret &&
		WAIT_TIMEOUT	!= ret )
	{
		NetworkManager::GetInstance()->ResetStartEvent();
		CLoadingDisplay::Destroy();
	}

	if( NetworkManager::GetInstance()->IsCaptain() )
	{
		NetworkManager::GetInstance()->GetServer()->SendStartGame();
	}
}
*/


void CGamePlayState::OnRender()
{
	WorldManager::GetWorldManager()->Draw();	
}

	
void CGamePlayState::OnUpdate(float fCurrentTime)
{
	// update the base state
	CGameUIState::OnUpdate(fCurrentTime);

	if( m_pStaticTerritory->isVisible() )
	{
		if( fCurrentTime - m_fTimeTerritory >= TERRITORY_DISPLAY_TIME )
		{
			m_pStaticTerritory->setVisible( false );			
			m_fTimeTerritory = 0.0f;
		}	
	}

	if( m_pStaticWin->isVisible() )
	{
		if( fCurrentTime  - m_fTimeWinGame >= WIN_DISPLAY_TIME )
		{
			m_pStaticTerritory->setVisible( false );			
			m_fTimeWinGame = 0.0f;
			// ==========================================================
			//				���� ��� ȭ�� ��ȯ
			// ==========================================================

			CStateManager &rStateManager = CGameApp::GetStateManager();	
			CGameResultState* pResult = (CGameResultState*)rStateManager.GetStateClass("GameResult");	
			pResult->SavePlayerData();

			rStateManager.ChangeState("GameResult");

			// ==========================================================
		}
	}

	WorldManager::GetWorldManager()->Update();
}


void CGamePlayState::ProcessKeyBoardForUI()
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

		if( m_pChatEditBox->isVisible() )
		{
			m_pChatEditBox->setText("");
			m_pChatEditBox->setVisible( false );
		}

	}

	if(pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_TAB))
	{
		for( int i = 0; i < 3; i++ )
		{
			m_pListMcl[i]->setVisible( true );
			m_pLogoImg[i]->setVisible( true );
		}
	}

	if(pkKeyboard->KeyWasReleased(NiInputKeyboard::KEY_TAB))
	{
		for( int i = 0; i < 3; i++ )
		{
			m_pListMcl[i]->setVisible( false );
			m_pLogoImg[i]->setVisible( false );
		}
	} 

	if( pkKeyboard->KeyWasReleased(NiInputKeyboard::KEY_RETURN))
	{
		if( !m_pChatEditBox->isVisible() )
		{
			m_bActivaChaEdit = true;
			m_pChatEditBox->setVisible( true );
			m_pStaticInput->setVisible( true );
			m_pChatEditBox->activate();
			CGameApp::mp_Appication->SetActiveEditBox( true );
		}
		else
		{
			m_bActivaChaEdit = false;
			m_pChatEditBox->setVisible( false );
			m_pStaticInput->setVisible( false );
			m_pChatEditBox->deactivate();
			CGameApp::mp_Appication->SetActiveEditBox( false );
		}
	}

}

void CGamePlayState::OnLeave(const char* szNext)
{
	//��� ���� ����
	 if(m_pBGChannel)
		 m_pBGChannel->stop();

	if (!CGameApp::mp_Appication)
		return;

	DeleteCriticalSection(&m_csChatListBox);

	//��Ʈ��ũ ����
	NetworkManager::Destroy();

	//���� ������ Data Destroy
	WorldManager::Destroy();

	CGameUIState::OnLeave(szNext);
}


void CGamePlayState::OnOverride(const char* szNext)
{
	CGameBaseState::OnOverride(szNext);
}


void CGamePlayState::OnResume(const char* szPrevious)
{
	CGameBaseState::OnResume(szPrevious);
}


//Ű���� ó�� �Լ�
void CGamePlayState::OnProcessInput()
{
//	ProcessKeyBoard();
	ProcessKeyBoardForUI();
	ProcessMouse();
}

//���콺 ó�� �Լ�
void CGamePlayState::ProcessMouse()
{
	
}


void CGamePlayState::ProcessKeyBoard()
{
	NiInputSystem*	inputSystem = CGameApp::mp_Appication->GetInputSystem();

	//Update input Device
	if(!inputSystem)
		return;
		
	//Ű �Է� ó��
	NiInputKeyboard* pkKeyboard = inputSystem->GetKeyboard();
	}

void CGamePlayState::SetResponning( bool bRespone )
{
	m_pProgressbarRespone->setProgress( 0.0f );
	m_pProgressbarRespone->setVisible( bRespone );
	m_pStaticRespone->setVisible( bRespone );
}

void CGamePlayState::SetRebirthing( bool bRebirth )
{
	m_pProgressbarRebirth->setProgress( 0.0f );
	m_pProgressbarRebirth->setVisible( bRebirth );
}

void CGamePlayState::ActiveTerritory( CEGUI::String& pszName, BYTE nArea )
{
	CEGUI::String strEventMsg;
	char pEventMsg[36];
	::ZeroMemory( pEventMsg, sizeof(pEventMsg) );

	sprintf_s( pEventMsg, " ���� %d ������ �����Ͽ����ϴ�.", nArea );

	strEventMsg = pszName +  CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8( pEventMsg ) );

	m_pStaticTerritory->setText( strEventMsg );
	m_pStaticTerritory->setVisible( true );

	m_fTimeTerritory = CGameApp::mp_Appication->GetAccumTime();
}

void CGamePlayState::UpdateResponeProgress( float fTime )
{

	float fValue = 1.0f * fTime / RESPONE_TIME;

	m_pProgressbarRespone->setProgress( fValue );

	if( fValue >= 1.0f )
	{
		SetResponning( false );
	}
}

// ��Ȱ ��Ű�� ���α׷����� ����
void CGamePlayState::UpdateRebirthProgress( float fTime )
{

	//float fValue = 1.0f * fTime / RESPONE_TIME; // ��Ȱ Ÿ�� protocol.h�� ����

	m_pProgressbarRebirth->setProgress( fTime );

	if( fTime >= 1.0f )
	{
		SetRebirthing( false );
	}
}

void CGamePlayState::WinGame( BYTE nTeam, bool bWin )
{
	
	char pEventMsg[36];
	::ZeroMemory( pEventMsg, sizeof(pEventMsg) );
	
	if( bWin )
	{
		switch( nTeam )
		{
		case nsPlayerInfo::USA:
			sprintf_s( pEventMsg, "USA ���� �¸��Ͽ����ϴ�." );
			break;

		case nsPlayerInfo::EU:
			sprintf_s( pEventMsg, "EU ���� �¸��Ͽ����ϴ�.");
			break;
		}
	}
	else
	{
		switch( nTeam )
		{
		case nsPlayerInfo::USA:
			sprintf_s( pEventMsg, "EU ���� �й��Ͽ����ϴ�." );
			break;

		case nsPlayerInfo::EU:
			sprintf_s( pEventMsg, "USA ���� �й��Ͽ����ϴ�.");
			break;
		}
	}

	m_pStaticWin->setText( (CEGUI::utf8*)CONV_MBCS_UTF8(pEventMsg) );
	m_pStaticWin->setVisible( true );	

	m_fTimeWinGame = CGameApp::mp_Appication->GetAccumTime();
}


void CGamePlayState::UpdateKill( BYTE nID, BYTE nTeam, BYTE nKill )
{
	unsigned int nRow =  m_pListMcl[ nTeam - 10 ]->getRowWithID( nID );

	char buf[3]={0,};
	_itoa_s( nKill, buf, sizeof(buf), 10 );

	m_pListMcl[ nTeam - 10 ]->setItem( new CEGUI::ListboxTextItem(buf), KILL, nRow );

	m_pListMcl[ nTeam - 10 ]->setSortColumn( KILL );	

}

void CGamePlayState::UpdateDeath( BYTE nID, BYTE nTeam, BYTE nDeath )
{
	unsigned int nRow =  m_pListMcl[ nTeam - 10 ]->getRowWithID( nID );

	char buf[3]={0,};
	_itoa_s( nDeath, buf, sizeof(buf), 10 );

	m_pListMcl[ nTeam - 10 ]->setItem( new CEGUI::ListboxTextItem(buf), DEATH, nRow );
}

bool CGamePlayState::Event_ChatTextAdded(const CEGUI::EventArgs &rE)
{

	if( !m_pChatEditBox && m_pChatEditBox->isVisible() )
		return false;

	const CEGUI::String &strChat = m_pChatEditBox->getText();

	if( !strChat.size() )
	{
		return false;
	}

	//----------------------------------------------
	//	 ������ ��� �����鿡�� ������ �ڽ��� UI ����
	//----------------------------------------------
	if( NetworkManager::GetInstance()->IsCaptain() )
	{		
		NetworkManager::GetInstance()->GetServer()->SendChatMsg( strChat, nsPlayerInfo::NET_GAME_STATE );		
	}
	else
	//----------------------------------------------
	//	 �Ϲ� ������ ��� ChatMsg ��û
	//----------------------------------------------
	{
		NetworkManager::GetInstance()->GetClient()->RequestSendChat( strChat, nsPlayerInfo::NET_GAME_STATE );
	}

	// Clear the text in the Editbox
	m_pChatEditBox->setText("");

	return true;
}
bool CGamePlayState::Event_MenuBoxExitBtnClick(const CEGUI::EventArgs &rE)
{
	if( NetworkManager::GetInstance()->IsCaptain() )
	{
		PACKET_EXIT_GAMEPLAY stPacket;
		PlayersMap::iterator iterPlayer;
		PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

		for( iterPlayer = ++(conPlayers.begin()); iterPlayer != conPlayers.end(); ++iterPlayer )
		{
			NetworkManager::GetInstance()->GetServer()->RegisterSendMsg(iterPlayer->second.pClientInfo,(UCHAR*)&stPacket,stPacket.size);
		}
	}

	CStateManager &rStateManager = CGameApp::GetStateManager();	
	rStateManager.ChangeState("MainMenu");

//	CGameApp::mp_Appication->QuitApplication();

	return true;
}
bool CGamePlayState::Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE)
{
	if( m_pMenuBox ) m_pMenuBox->setVisible( false );

	return true;
}
bool CGamePlayState::Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}
bool CGamePlayState::Event_MenuBoxVoteBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}

bool CGamePlayState::Event_EditBoxActivate		(const CEGUI::EventArgs &rE)
{

	CGameApp::mp_Appication->SetActiveEditBox(true);
	return true;
}

bool CGamePlayState::Event_EditBoxDeactivate	(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(false);
	return true;
}

void CGamePlayState::AddChatText(const CEGUI::String& pText)
{
	if(pText.size())
	{
		ListboxTextItem* chatItem;

		EnterCriticalSection(&m_csChatListBox);
		const CEGUI::Image* sel_img = &ImagesetManager::getSingleton().getImageset("WonderlandLook")->getImage("MultiListSelectionBrush");

		if(m_pChatListBox->getItemCount() == MAX_CHATLISTBOX_ITEM_IN_GAME )
		{						
			chatItem = static_cast<ListboxTextItem*>(m_pChatListBox->getListboxItemFromIndex(0));
			chatItem->setAutoDeleted(false);
			m_pChatListBox->removeItem(chatItem);
			chatItem->setAutoDeleted(true);			
			chatItem->setFont((CEGUI::utf8*)szFont1);			
			chatItem->setText( pText );
			chatItem->setSelectionBrushImage(sel_img);
		}
		else
		{	// Create a new listbox item
			chatItem = new ListboxTextItem( pText);
			chatItem->setFont((CEGUI::utf8*)szFont1);			
			chatItem->setSelectionBrushImage(sel_img);
		}		
		
		m_pChatListBox->addItem(chatItem);
		m_pChatListBox->ensureItemIsVisible(m_pChatListBox->getItemCount());
		LeaveCriticalSection(&m_csChatListBox);
	}
}