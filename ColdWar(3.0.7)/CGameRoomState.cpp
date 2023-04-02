#include "GameApp.h"

#include "CStateManager.h"
#include "CGameRoomState.h"
#include "CSinglePlayState.h"
#include "NetWorkManager.h"
#include "SoundManager.h"

#include "strconv.h"

bool CGameRoomState::m_bCaptain = true;

char *szFont = "ahn_m";

CGameRoomState::CGameRoomState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
	m_pChatBox			= NULL;
	m_pPlayerBox		= NULL;
	m_pTitleBar			= NULL;
	m_pReadyAndStartBtn	= NULL;
	m_pJobComboBox		= NULL;
	m_pChatEditBox		= NULL;
	m_pChatListBox		= NULL;
	m_pPlayerListBox	= NULL;

//	m_pGuerrilla0		= NULL;
//	m_pGuerrilla1		= NULL;

	m_nCntTotalPlayers	= 0;
	m_nCntUSAPlayers	= 0;
	m_nCntEUPlayers		= 0;

	m_nMyPosition		= 0;

	m_nSelectedJopCode	= MARIN;

	m_bCaptain			= true;

//	::ZeroMemory( m_pUSA, sizeof(m_pUSA) );
//	::ZeroMemory( m_pEU, sizeof(m_pEU) );
	::ZeroMemory( m_pPlayers, sizeof(m_pPlayers) );
//	::ZeroMemory( m_strSelfID, sizeof(m_strSelfID) );

	m_pBGChannel = NULL;
}

bool CGameRoomState::Initialize()
{
	//�κ� ������� �ε�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/ThemaCity.mp3",eSoundCode::SOUND_BG_LOBBY,
		CSoundManager::DEF_SOUND_TYPE_2D))
	{
		NiMessageBox("������ ���ų� �߸��� ����Դϴ�.","Error");
		return false;
	}
	//���� ���
	if(!CSoundManager::GetInstance()->Play2D(eSoundCode::SOUND_BG_LOBBY,m_pBGChannel,true))
		return false;
	
	using namespace CEGUI;

	// UI ȭ��ǥ ��Ʈ��
	ImagesetManager::getSingleton().createImageset(String("leftarrow.imageset"));
	ImagesetManager::getSingleton().createImageset(String("rightarrow.imageset"));

	if (!CGameUIState::Initialize() || 0 == m_pGUI)
		return false;

	// Retrieve the window manager
	WindowManager& winMgr = WindowManager::getSingleton();	

	m_pChatBox			= NULL;
	m_pPlayerBox		= NULL;
	m_pTitleBar			= NULL;
	m_pReadyAndStartBtn	= NULL;
	m_pJobComboBox		= NULL;
	m_pChatEditBox		= NULL;
	m_pChatListBox		= NULL;
	m_pPlayerListBox	= NULL;
	
//	m_pGuerrilla0		= NULL;
//	m_pGuerrilla1		= NULL;

	m_nCntTotalPlayers	= 0;
	m_nCntUSAPlayers	= 0;
	m_nCntEUPlayers		= 0;

	m_nSelectedJopCode	= MARIN;

//	m_bCaptain			= true;

	::ZeroMemory( m_pPlayers, sizeof(m_pPlayers) );

//	::ZeroMemory( m_pUSA, sizeof(m_pUSA) );
//	::ZeroMemory( m_pEU, sizeof(m_pEU) );


	InitializeCriticalSection(&m_csPlayerStatic);

	//------------------------------------------------------------------------------------------
	//
	//					
	//									�� ���¿� UI ���� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------
	//
	//					
	//									������ ������ ����
	//	
	//
	//------------------------------------------------------------------------------------------
	
	// ä��â
	m_pChatBox = reinterpret_cast< CEGUI::FrameWindow* >( m_pGUI->getChild("Root/ChatWindow") );
	
	if (m_pChatBox)
	{
		m_pChatBox->setFont((CEGUI::utf8*)szFont);
		m_pChatBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Chatting")); 
	}

	// �÷����� ����Ʈ â
	m_pPlayerBox = reinterpret_cast< CEGUI::FrameWindow* >(m_pGUI->getChild("Root/PlayerWindow") );
	
	if (m_pPlayerBox)
	{
		m_pPlayerBox->setFont( (CEGUI::utf8*)szFont);
		m_pPlayerBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("PlayerList")); 
	}

	// TitleBar
	m_pTitleBar = reinterpret_cast< CEGUI::Titlebar* >( m_pGUI->getChild("Root/TitleBar") );
	
	if (m_pTitleBar)
	{

	}
	//------------------------------------------------------------------------------------------
	//
	//					
	//									��ư �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------
	//			Ÿ��Ʋ �ٿ� �ִ� ��ư
	//------------------------------------------------------------------------------------------
	
	//	���� ��ư �̺�Ʈ �߰�
	CEGUI::Window *pHelp = m_pTitleBar->getChild("Root/TitleBar/Help");
	
	if (pHelp)
	{
		pHelp->subscribeEvent( CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CGameRoomState::Event_HelpButtonClick, this));

		pHelp->setFont( (CEGUI::utf8*)szFont);
		pHelp->setText((CEGUI::utf8*)CONV_MBCS_UTF8("����")); 
	}

	//	�ɼ� ��ư �̺�Ʈ �߰�
	CEGUI::Window *pOption = m_pTitleBar->getChild("Root/TitleBar/Option");

	if (pOption)
	{
		pOption->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_OptionButtonClick, this));

		pOption->setFont((CEGUI::utf8*)szFont);
		pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�ɼ�")); 
	}

	//	���ư��� ��ư �̺�Ʈ �߰�
	CEGUI::Window *pExitRoom = m_pTitleBar->getChild("Root/TitleBar/ExitRoom");

	if (pExitRoom)
	{
		pExitRoom->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_ExitRoomButtonClick, this));

		pExitRoom->setFont((CEGUI::utf8*)szFont);
		pExitRoom->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���ư���")); 
	}

	
	//------------------------------------------------------------------------------------------
	//			��(USA, EU, Gerrilla) ���� �� ����, ���� ��ư
	//------------------------------------------------------------------------------------------

	//	USA ��ư �̺�Ʈ �߰�
	CEGUI::Window *pUSA = m_pGUI->getChild("Root/USA_Btn");

	if (pUSA)
	{
		pUSA->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_USAButtonClick, this));

		pUSA->setFont((CEGUI::utf8*)szFont);
		pUSA->setText((CEGUI::utf8*)CONV_MBCS_UTF8("USA")); 
	}

	//	EU ��ư �̺�Ʈ �߰�
	CEGUI::Window *pEU = m_pGUI->getChild("Root/EU_Btn");

	if (pEU)
	{
		pEU->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_EUButtonClick, this));

		pEU->setFont( (CEGUI::utf8*)szFont);
		pEU->setText((CEGUI::utf8*)CONV_MBCS_UTF8("E U")); 
	}


	//	Guerrilla0 ��ư �̺�Ʈ �߰�
	CEGUI::Window *pGuerrilla0 = m_pGUI->getChild("Root/Guerrilla0_Btn");

	if (pGuerrilla0)
	{
		pGuerrilla0->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_Guerrilla0ButtonClick, this));

		pGuerrilla0->setFont( (CEGUI::utf8*)szFont);
		pGuerrilla0->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Guerrilla1")); 
	}

	//	Guerrilla1 ��ư �̺�Ʈ �߰�
	CEGUI::Window *pGuerrilla1 = m_pGUI->getChild("Root/Guerrilla1_Btn");

	if (pGuerrilla1)
	{
		pGuerrilla1->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_Guerrilla1ButtonClick, this));

		pGuerrilla1->setFont( (CEGUI::utf8*)szFont);
		pGuerrilla1->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Guerrilla2")); 
	}

	//	ReadyAndStart ��ư �̺�Ʈ �߰�
	m_pReadyAndStartBtn = reinterpret_cast< CEGUI::ButtonBase* >( m_pGUI->getChild("Root/ReadyAndStart_Btn") );

	if (m_pReadyAndStartBtn)
	{
		m_pReadyAndStartBtn->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CGameRoomState::Event_ReadyAndStartButtonClick, this));

		m_pReadyAndStartBtn->setFont( (CEGUI::utf8*)szFont);
		

		if( m_bCaptain )
		{
			m_pReadyAndStartBtn->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� ����"));
		}
		else
		{
			m_pReadyAndStartBtn->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� �غ�"));
		}
	}

	//	�� ���� ��ư
	CEGUI::Window *pMapBtn = m_pGUI->getChild("Root/MapSelect_Btn");
	
	if (pMapBtn)
	{
		pMapBtn->subscribeEvent( CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CGameRoomState::Event_MapChange, this));

		pMapBtn->setFont( (CEGUI::utf8*)szFont);
		pMapBtn->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�� ����")); 
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	if( m_pChatBox )
	{
		//	IP �Է� Edit �ڽ� �̺�Ʈ �߰�
		m_pChatEditBox = reinterpret_cast<CEGUI::Editbox*>( m_pChatBox->getChild("Root/ChatWindow/Msg_Edit") );
		m_pChatEditBox->setFont( (CEGUI::utf8*)szFont );
		
		//	����Ű or ��Ű�� �������� �̺�Ʈ �߰�
		if (m_pChatEditBox)
		{
			m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, 
									CEGUI::Event::Subscriber(&CGameRoomState::Event_ChatTextAdded, this));

			m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventActivated, 
									CEGUI::Event::Subscriber(&CGameRoomState::Event_EditBoxActivate, this));

			m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventDeactivated, 
									CEGUI::Event::Subscriber(&CGameRoomState::Event_EditBoxDeactivate, this));

			
			
		}
	}
	//------------------------------------------------------------------------------------------
	//
	//					
	//									�޺� �ڽ� �̺�Ʈ �߰� �� ��� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	//	Guerrilla1 ��ư �̺�Ʈ �߰�
	m_pJobComboBox = reinterpret_cast< CEGUI::Combobox* >( m_pGUI->getChild("Root/JobComboBox") );
	const CEGUI::Image* sel_img = &ImagesetManager::getSingleton().getImageset("WonderlandLook")->getImage("MultiListSelectionBrush");
	if( m_pJobComboBox )
	{		
		ListboxTextItem* itm = NULL;

		// ������ ��
		itm = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("������"), MARIN);
		itm->setFont((CEGUI::utf8*)szFont);
		itm->setSelectionBrushImage(sel_img);
		m_pJobComboBox->addItem(itm);

		// ���ݼ� ��
		itm = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("���ݼ�"), SNIPER);
		itm->setFont((CEGUI::utf8*)szFont);
		itm->setSelectionBrushImage(sel_img);
		m_pJobComboBox->addItem(itm);
/*
		// �����Ͼ�
		itm = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("�����Ͼ�"), ENGINEER);
		itm->setFont((CEGUI::utf8*)szFont);
		itm->setSelectionBrushImage(sel_img);
		m_pJobComboBox->addItem(itm);
*/
		// ���� ��
		itm = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("������"), MEDIC);
		itm->setFont((CEGUI::utf8*)szFont);
		itm->setSelectionBrushImage(sel_img);
		m_pJobComboBox->addItem(itm);

		// ź�� ��
		itm = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("ź�ິ"), AMMO);
		itm->setFont((CEGUI::utf8*)szFont);
		itm->setSelectionBrushImage(sel_img);
		m_pJobComboBox->addItem(itm);

		m_pJobComboBox->setFont( (CEGUI::utf8*)szFont);
		// �б⸸ ����
		m_pJobComboBox->setReadOnly( true );
		
		// �̺�Ʈ �߰�( ����� �ٲܶ� ���� �߻� )
		m_pJobComboBox->subscribeEvent( CEGUI::Combobox::EventListSelectionAccepted, 
										CEGUI::Event::Subscriber(&CGameRoomState::Event_HandleSelectChange, this));
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� �̺�Ʈ �߰� �� ����
	//	
	//
	//------------------------------------------------------------------------------------------

	if( m_pChatBox )
	{
		m_pChatListBox = reinterpret_cast<CEGUI::Listbox*>( m_pChatBox->getChild("Root/ChatWindow/Chat_List") );
		m_pChatListBox->setFont((CEGUI::utf8*)szFont);
	}

	if( m_pPlayerBox )
	{
		m_pPlayerListBox = reinterpret_cast<CEGUI::Listbox*>( m_pPlayerBox->getChild("Root/PlayerWindow/Player_List") );
		m_pPlayerListBox->setFont((CEGUI::utf8*)szFont);
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����ƽ �ؽ�Ʈ ����
	//	
	//
	//------------------------------------------------------------------------------------------

	//	IP �Է� Edit �ڽ� �̺�Ʈ �߰�
	CEGUI::Window *pJobStatic = m_pGUI->getChild("Root/Job_Select_Static");
	if( pJobStatic )
	{
		pJobStatic->setFont((CEGUI::utf8*)szFont);
		pJobStatic->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� ����"));		
	}

	CEGUI::Window *pMsgStatic = m_pChatBox->getChild("Root/ChatWindow/Msg_Static");
	if( pMsgStatic )
	{
		pMsgStatic->setFont((CEGUI::utf8*)szFont);
		pMsgStatic->setText((CEGUI::utf8*)CONV_MBCS_UTF8("�Է� :"));		
	}

	char buf[25];
	::ZeroMemory(buf, sizeof(buf));

	for( int i = 0; i < MAXUNIT; i++ )
	{
		::ZeroMemory( buf, sizeof(buf) );
		sprintf_s( buf, "Root/USA_%d", i );

		m_pPlayers[ i ] = m_pGUI->getChild(buf);
		if( m_pPlayers[ i ] )
			m_pPlayers[ i ]->setFont((CEGUI::utf8*)szFont);
	}
	for( int i = 0; i < MAXUNIT; i++ )
	{
		::ZeroMemory( buf, sizeof(buf) );
		sprintf_s( buf, "Root/EU_%d", i );

		m_pPlayers[ i + MAXUNIT  ] = m_pGUI->getChild(buf);
		if( m_pPlayers[ i + MAXUNIT ] )
			m_pPlayers[ i + MAXUNIT ]->setFont((CEGUI::utf8*)szFont);
	}

	m_pPlayers[ MAXUNIT*2 ] = m_pGUI->getChild("Root/Guerrilla_0");
	if( m_pPlayers[ MAXUNIT*2 ] ) m_pPlayers[ MAXUNIT*2 ]->setFont((CEGUI::utf8*)szFont);

	m_pPlayers[ MAXUNIT*2 + 1 ] = m_pGUI->getChild("Root/Guerrilla_1");
	if( m_pPlayers[ MAXUNIT*2 + 1 ] ) m_pPlayers[ MAXUNIT*2 + 1 ]->setFont((CEGUI::utf8*)szFont);

	if( m_bCaptain && m_strSelfID.size())
	{
		SetStaticText( 0, nsPlayerInfo::SOLIDIER, m_strSelfID );
		m_pPlayers[ 0 ]->setAlpha( 1.0f );

		ListboxTextItem* chatItem = new ListboxTextItem( m_strSelfID, 0 );
		chatItem->setFont((CEGUI::utf8*)szFont);
		chatItem->setSelectionBrushImage(sel_img);			
		m_pPlayerListBox->addItem(chatItem);
		m_pPlayerListBox->ensureItemIsVisible(m_pPlayerListBox->getItemCount());
	}

	
	//------------------------------------------------------------------------------------------
	//
	//					
	//									�� �������̽� ����ƽ �̹��� ����
	//	
	//
	//------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------
	//
	//					
	//									�� ���¿��� ���� ����
	//	
	//
	//------------------------------------------------------------------------------------------
	
	//��Ʈ��ũ �Ŵ��� ���� �� �ʱ�ȭ
	// NetworkManager::Create() = true, false ���� ���� ���� �Ϲ� ���� ���� ������
	if(!NetworkManager::Create(m_bCaptain))
	{
		NiMessageBox("CGamePlayState::Init -- NetworkManager Created Failed",NULL);
		return false;
	}

	if( m_bCaptain )
	{
		NetworkManager::GetInstance()->GetServer()->SetCaptaionID( m_strSelfID );//(wchar_t*)(convMbcs2Uni(m_strSelfID.c_str()).c_str()) );
	}

	NetworkManager::GetInstance()->SetIP( m_strIP.c_str() );
	NetworkManager::GetInstance()->SetCaptain( m_bCaptain );

	//��Ʈ��ũ ����
	if(!NetworkManager::GetInstance()->Start())
	{
		NiMessageBox("CGamePlayState::Init -- ���� ���� ����",NULL);
		NetworkManager::GetInstance()->End();
		return false;
	}

	return true;
}


void CGameRoomState::InsertPlayerList( CEGUI::String& pszName, BYTE nID )
{
	const CEGUI::Image* sel_img = &ImagesetManager::getSingleton().getImageset("WonderlandLook")->getImage("MultiListSelectionBrush");

	ListboxTextItem* chatItem = new ListboxTextItem( pszName );
	chatItem->setFont((CEGUI::utf8*)szFont);
	chatItem->setSelectionBrushImage(sel_img);
	
	m_pPlayerListBox->addItem(chatItem);
	m_pPlayerListBox->ensureItemIsVisible(m_pPlayerListBox->getItemCount());

}

void CGameRoomState::SetID( const CEGUI::String& strID )
{
	m_strSelfID = strID;
}

void CGameRoomState::SetMyPosition( BYTE nPos )
{
	m_nMyPosition = nPos;
}

void CGameRoomState::SetStaticText( BYTE nPos, BYTE nJob, CEGUI::String& pszName )
{
	using namespace nsPlayerInfo;

	CEGUI::String stPlayerName;
	
	switch( nJob )
	{
	case SOLIDIER:

		stPlayerName = pszName + CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8("(����)"));
		break;

	case SNIPER:

		stPlayerName = pszName + CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8("(���ݼ�)"));
		break;

	case MEDIC:

		stPlayerName = pszName + CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8("(�ǹ���)"));
		break;

	case ASP:

		stPlayerName = pszName + CEGUI::String( (CEGUI::utf8*)CONV_MBCS_UTF8("(ź�ິ)"));
		break;

	}
	
	EnterCriticalSection(&m_csPlayerStatic);
	if( m_pPlayers[ nPos ] )m_pPlayers[ nPos ]->setText( (CEGUI::utf8*)stPlayerName.c_str() );
	LeaveCriticalSection(&m_csPlayerStatic);
}

const char*	CGameRoomState::GetID()
{
	if( m_strSelfID.size() )
		return m_strSelfID.c_str();

	return NULL;
}

void		CGameRoomState::SetIP( const CEGUI::String& ip )
{
	m_strIP	= ip;
}

void	CGameRoomState::SetEmptyStaticText( BYTE nPos )
{
	m_pPlayers[ nPos ]->setText("");
}

void	CGameRoomState::RemovePlayerList( const CEGUI::String& pszName, BYTE nID )
{
	ListboxTextItem* playerItem = NULL;//= static_cast<ListboxTextItem*>(m_pChatListBox->getListboxItemFromIndex(nID) );
	playerItem = (ListboxTextItem*)m_pPlayerListBox->findItemWithText( pszName, (ListboxItem*)m_pPlayerListBox->getListboxItemFromIndex(0) );

	if( playerItem )
		m_pPlayerListBox->removeItem( playerItem );

}

void	CGameRoomState::ChangePos( BYTE nOld, BYTE nNew )
{
	m_pPlayers[ nNew ]->setText( m_pPlayers[ nOld ]->getText() );
	m_pPlayers[ nNew ]->setAlpha( m_pPlayers[ nOld ]->getAlpha() );

	m_pPlayers[ nOld ]->setText("");
	m_pPlayers[ nOld ]->setAlpha(DEFAULT_ALPHA);
}


void	CGameRoomState::SetStaticTextAlpha( BYTE nPos, BYTE nType  )
{
	using namespace nsPlayerInfo;

	switch( nType )
	{
	case DEFAULT:
		m_pPlayers[ nPos ]->setAlpha( DEFAULT_ALPHA );
		break;
	case READY:
		m_pPlayers[ nPos ]->setAlpha( 1.0f );
		break;
	}
}


bool CGameRoomState::ChangeTeam( BYTE nTeam )
{

	// ����
	if( m_bCaptain )
	{
		if( nTeam == NetworkManager::GetInstance()->GetTeamType( NetworkManager::GetInstance()->GetServer()->GetPosAtRoom() ) )
			return false;

		NetworkManager::GetInstance()->GetServer()->ChangePos( nTeam );

	}
	// �Ϲ� ����
	else
	{
		if( nsPlayerInfo::READY == NetworkManager::GetInstance()->GetClient()->GetCurrentState() )
			return false;

		if( nTeam == NetworkManager::GetInstance()->GetTeamType( NetworkManager::GetInstance()->GetClient()->GetPosAtRoom() ) )
			return false;

		NetworkManager::GetInstance()->GetClient()->RequestChangeTeam( nTeam );
	}

	return true;

}

//====================================================================
//			
//						UI event handlers
//
//====================================================================

// ���� â
bool CGameRoomState::Event_HelpButtonClick(const CEGUI::EventArgs &rE)
{
	return true;
}


bool CGameRoomState::Event_MapChange(const CEGUI::EventArgs &rE)
{
	return true;
}

// �ɼ� ȭ������ �̵�
bool CGameRoomState::Event_OptionButtonClick(const CEGUI::EventArgs &rE)
{
	CStateManager &rStateManager = CGameApp::GetStateManager();
	//rStateManager.ChangeState("Option");
	return true;
}

// ���� �޴��� �̵�
bool CGameRoomState::Event_ExitRoomButtonClick(const CEGUI::EventArgs &rE)
{
	if( m_bCaptain )
	{
		NetworkManager::GetInstance()->GetServer()->SendExitAtRoom();
	}
	else
	{
		if( nsPlayerInfo::READY == NetworkManager::GetInstance()->GetClient()->GetCurrentState() )
			return false;
	}

	NetworkManager::GetInstance()->End();

	CStateManager &rStateManager = CGameApp::GetStateManager();
	rStateManager.ChangeState("MainMenu");

	return true;
}

//====================================================================
//
//					������ �ڸ� �ű� ��û
//
//====================================================================

bool CGameRoomState::Event_USAButtonClick(const CEGUI::EventArgs &rE)
{
	return ChangeTeam( nsPlayerInfo::USA );
}

bool CGameRoomState::Event_EUButtonClick(const CEGUI::EventArgs &rE)
{
	return ChangeTeam( nsPlayerInfo::EU );
}

bool CGameRoomState::Event_Guerrilla0ButtonClick(const CEGUI::EventArgs &rE)
{
	return ChangeTeam( nsPlayerInfo::GEURRILLA0 );
}	

bool CGameRoomState::Event_Guerrilla1ButtonClick(const CEGUI::EventArgs &rE)
{
	return ChangeTeam( nsPlayerInfo::GEURRILLA1 );
}

bool CGameRoomState::Event_ReadyAndStartButtonClick(const CEGUI::EventArgs &rE)
{
	// ����
	if( NetworkManager::GetInstance()->IsCaptain() )
	{
		// ���� ���� ó�� �� Ŭ���̾�Ʈ���� ����
		if( NetworkManager::GetInstance()->GetServer()->StartGame() )
		{

		}
	}
	// �Ϲ� ����
	else
	{
		NetworkManager::GetInstance()->GetClient()->RequestReady();
	}

	return true;
}

bool CGameRoomState::Event_ChatTextAdded(const CEGUI::EventArgs &rE)
{
	if( !m_pChatEditBox )
		return false;

	//AddChatText(m_pChatEditBox->getText());

	const CEGUI::String &strChat = m_pChatEditBox->getText();

	if( !strChat.size() )
		return false;

	// ä��â �׽�Ʈ ������� üũ
	if( 0 == strChat.compare( CEGUI::String("/Test") ))
	{
		CStateManager &rStateManager = CGameApp::GetStateManager();
		rStateManager.ChangeState("SingleTest");

		return true;
	}

	//----------------------------------------------
	//	 ������ ��� �����鿡�� ������ �ڽ��� UI ����
	//----------------------------------------------
	if( m_bCaptain )
	{		
		NetworkManager::GetInstance()->GetServer()->SendChatMsg( strChat, nsPlayerInfo::NET_ROOM_STATE );		
	}
	else
	//----------------------------------------------
	//	 �Ϲ� ������ ��� ChatMsg ��û
	//----------------------------------------------
	{
		NetworkManager::GetInstance()->GetClient()->RequestSendChat( strChat, nsPlayerInfo::NET_ROOM_STATE );
	}

	// Clear the text in the Editbox
	m_pChatEditBox->setText("");

	return true;
}

bool CGameRoomState::Event_EditBoxActivate(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(true);
	return true;
}

bool CGameRoomState::Event_EditBoxDeactivate(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(false);
	return true;
}


bool CGameRoomState::Event_HandleSelectChange(const CEGUI::EventArgs &rE)
{	
	// find the selected item in the combobox
    ListboxItem* item = m_pJobComboBox->findItemWithText( m_pJobComboBox->getText(), 0 );

	m_nSelectedJopCode = item->getID();

	if( m_bCaptain )
	{
		NetworkManager::GetInstance()->GetServer()->SendChangeJob( m_nSelectedJopCode );
	}
	else
	{
		NetworkManager::GetInstance()->GetClient()->RequestChangeJob( m_nSelectedJopCode );
	}

	return true;
}

void CGameRoomState::AddChatText(const CEGUI::String& pText)
{
	if(pText.size())
	{
		ListboxTextItem* chatItem;
		const CEGUI::Image* sel_img = &ImagesetManager::getSingleton().getImageset("WonderlandLook")->getImage("MultiListSelectionBrush");

		if(m_pChatListBox->getItemCount() == MAX_CHATLISTBOX_ITEM)
		{						
			chatItem = static_cast<ListboxTextItem*>(m_pChatListBox->getListboxItemFromIndex(0));
			chatItem->setAutoDeleted(false);
			m_pChatListBox->removeItem(chatItem);
			chatItem->setAutoDeleted(true);			
			chatItem->setFont((CEGUI::utf8*)szFont);			
			chatItem->setText( pText );
			chatItem->setSelectionBrushImage(sel_img);
		}
		else
		{	// Create a new listbox item
			chatItem = new ListboxTextItem( pText);
			chatItem->setFont((CEGUI::utf8*)szFont);			
			chatItem->setSelectionBrushImage(sel_img);
		}		
		
		m_pChatListBox->addItem(chatItem);
		m_pChatListBox->ensureItemIsVisible(m_pChatListBox->getItemCount());
	}
}


void CGameRoomState::OnUpdate(float fCurrentTime)
{
	// update the base state
	CGameUIState::OnUpdate(fCurrentTime);

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

    if (!pkKeyboard)
        return;

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
	{
		NetworkManager::GetInstance()->End();

		CStateManager &rStateManager = CGameApp::GetStateManager();
		rStateManager.ChangeState("MainMenu");

		/*
		if( m_pConnectGameBox && m_pConnectGameBox->isVisible() )
		{
			m_pConnectGameBox->getChild("Root/ConnectBox/Ok_Btn")->setText("");
			m_pConnectGameBox->getChild("Root/ConnectBox/Cancel_Btn")->setText("");
			m_pConnectGameBox->setVisible( false );
			m_pConnectGameBox->setEnabled( false );
			return;
		}

		if( m_pNewGameBox && m_pNewGameBox->isVisible() )
		{
			m_pNewGameBox->setVisible( false );
			m_pNewGameBox->setEnabled( false );
			return;
		}

		if( m_pExitGameBox && m_pExitGameBox->isVisible() )
		{
			CGameApp::mp_Appication->QuitApplication();
			return;
		}
		else
		{
			m_pExitGameBox->setEnabled( true );
			m_pExitGameBox->setVisible( true );
		}
		*/

	}
	
}

void CGameRoomState::OnLeave(const char* szNext)
{	
	//��������� ���ô�
	if(m_pBGChannel)
		m_pBGChannel->stop();

	if (!CGameApp::mp_Appication)
		return;

	m_pChatBox			= NULL;
	m_pPlayerBox		= NULL;
	m_pTitleBar			= NULL;
	m_pReadyAndStartBtn	= NULL;
	m_pJobComboBox		= NULL;
	m_pChatEditBox		= NULL;
	m_pChatListBox		= NULL;
	m_pPlayerListBox	= NULL;

	m_nCntTotalPlayers	= 0;
	m_nCntUSAPlayers	= 0;
	m_nCntEUPlayers		= 0;
	m_nMyPosition		= 0;

	m_nSelectedJopCode	= MARIN;

	m_bCaptain			= true;

	CGameApp::mp_Appication->SetActiveEditBox( false );
	::ZeroMemory( m_pPlayers, sizeof(m_pPlayers) );

	DeleteCriticalSection(&m_csPlayerStatic);

	if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(String("leftarrow")))
		CEGUI::ImagesetManager::getSingleton().destroyImageset(String("leftarrow"));

	if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(String("rightarrow")))
		CEGUI::ImagesetManager::getSingleton().destroyImageset(String("rightarrow"));

	CGameUIState::OnLeave(szNext);
}


