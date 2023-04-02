#include "GameApp.h"
#include "CMenuState.h"
#include "CGameRoomState.h"
#include "WorldManager.h"
#include "NetWorkManager.h"
#include "strconv.h"

CMenuState::CMenuState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
	m_pNewGameBox		= NULL;
	m_pExitGameBox		= NULL;
	m_pConnectGameBox	= NULL;
	m_pCreateGameBox	= NULL;

	m_pBGChannel		= NULL;
}

bool CMenuState::Initialize()
{
	//�޴� ��� ���� �ε�
	if(!CSoundManager::GetInstance()->LoadSound("Data/Sound/Michael Kamen-01-Main Theme.mp3",eSoundCode::SOUND_BG_MENU,
		CSoundManager::DEF_SOUND_TYPE_2D))
	{
		NiMessageBox("������ ���ų� �߸��� ����Դϴ�.","Error");
		return false;
	}
	//���� ���
	if(!CSoundManager::GetInstance()->Play2D(eSoundCode::SOUND_BG_MENU,m_pBGChannel,true))
		return false;

	using namespace CEGUI;

	if (!CGameUIState::Initialize() || 0 == m_pGUI)
		return false;

	// Retrieve the window manager
	WindowManager& winMgr = WindowManager::getSingleton();

	char *szFont = "ahn_m";

	m_pNewGameBox		= NULL;
	m_pExitGameBox		= NULL;
	m_pConnectGameBox	= NULL;
	m_pCreateGameBox	= NULL;

	//============================================================
	//					IP ����
	//============================================================
	FILE *pFile = NULL;
	fopen_s(&pFile,"server.txt","r");

	if(pFile == NULL) return false;

	char address[30];
	::ZeroMemory( address, sizeof(address) );
	fgets(address,sizeof(address),pFile);
	fclose(pFile);

	// IP ����
	NetworkManager::SetIP( std::string( address ) );

	//============================================================


	//------------------------------------------------------------------------------------------
	//
	//					
	//									������ ������ Enable
	//	
	//
	//------------------------------------------------------------------------------------------

	// NewGame FrameWindow �ڽ� �����
	m_pNewGameBox = reinterpret_cast< CEGUI::FrameWindow* >( m_pGUI->getChild("Root/NewGameBox") );
	
	if (m_pNewGameBox)
	{
		m_pNewGameBox->setFont((CEGUI::utf8*)szFont);
		m_pNewGameBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Game Type")); 
		m_pNewGameBox->setTitleBarEnabled( true );
		m_pNewGameBox->setEnabled( false );
		m_pNewGameBox->setVisible( false );
	}

	// ExitGame FrameWindow �ڽ� �����
	m_pExitGameBox = reinterpret_cast< CEGUI::FrameWindow* >(m_pGUI->getChild("Root/ExitBox") );
	
	if (m_pExitGameBox)
	{
		m_pExitGameBox->setFont( (CEGUI::utf8*)szFont);
		m_pExitGameBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Are You Sure Exit Game Now?")); 
		m_pExitGameBox->setEnabled( false );
		m_pExitGameBox->setVisible( false );
	}

	// JoinGame FrameWindow �ڽ� �����
	m_pConnectGameBox = reinterpret_cast< CEGUI::FrameWindow* >( m_pGUI->getChild("Root/ConnectBox") );
	
	if (m_pConnectGameBox)
	{
		m_pConnectGameBox->setFont( (CEGUI::utf8*)szFont);
		m_pConnectGameBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Game Connect")); 
		m_pConnectGameBox->setEnabled( false );
		m_pConnectGameBox->setVisible( false );
	}

	m_pCreateGameBox = reinterpret_cast< CEGUI::FrameWindow* >( m_pGUI->getChild("Root/CreateGameBox") );

	if (m_pCreateGameBox)
	{
		m_pCreateGameBox->setFont( (CEGUI::utf8*)szFont);
		m_pCreateGameBox->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Create Game")); 
		m_pCreateGameBox->setEnabled( false );
		m_pCreateGameBox->setVisible( false );
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									��ư �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	
	//	NewGame ��ư �̺�Ʈ �߰�
	CEGUI::Window *pNewGame = m_pGUI->getChild("Root/Newgame");
	
	if (pNewGame)
		pNewGame->subscribeEvent( CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&CMenuState::Event_NewGameButtonClick, this));

	pNewGame->setFont( (CEGUI::utf8*)szFont);
	pNewGame->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� ����")); 

	//	Option ��ư �̺�Ʈ �߰�
	CEGUI::Window *pOption = m_pGUI->getChild("Root/Option");

	if (pOption)
		pOption->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CMenuState::Event_OptionButtonClick, this));

	pOption->setFont((CEGUI::utf8*)szFont);
	pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("����")); 

	//	Credit ��ư �̺�Ʈ �߰�
	CEGUI::Window *pCredit = m_pGUI->getChild("Root/Credit");

	if (pCredit)
		pCredit->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CMenuState::Event_CreditButtonClick, this));

	pCredit->setFont((CEGUI::utf8*)szFont);
	pCredit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("ũ����")); 

	//	ExitGame ��ư �̺�Ʈ �߰�
	CEGUI::Window *pExit = m_pGUI->getChild("Root/ExitGame");

	if (pExit)
		pExit->subscribeEvent( CEGUI::PushButton::EventClicked, 
								CEGUI::Event::Subscriber(&CMenuState::Event_ExitGameButtonClick, this));

	pExit->setFont( (CEGUI::utf8*)szFont);
	pExit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� ����")); 
	
	if( m_pNewGameBox )
	{
		//	CreateGame(�� �����) ��ư �̺�Ʈ �߰�
		CEGUI::Window *pCreateGame = m_pNewGameBox->getChild("Root/NewGameBox/CreateGame");

		if (pCreateGame)
			pCreateGame->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameButtonClick, this));

		pCreateGame->setFont( (CEGUI::utf8*)szFont);
		pCreateGame->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� �����")); 


		//	JoinGame(�� �����ϱ�) ��ư �̺�Ʈ �߰�
		CEGUI::Window *pJoinGame = m_pNewGameBox->getChild("Root/NewGameBox/JoinGame");

		if (pJoinGame)
			pJoinGame->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_JoinGameButtonClick, this));

		pJoinGame->setFont( (CEGUI::utf8*)szFont);
		pJoinGame->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���� �����ϱ�")); 

	}
	
	if( m_pExitGameBox )
	{
		//	������ Ȯ�� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pExitOk = m_pExitGameBox->getChild("Root/ExitBox/Ok_Btn");

		if (pExitOk)
			pExitOk->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ExitOkButtonClick, this));

		pExitOk->setFont( (CEGUI::utf8*)szFont);
		pExitOk->setText((CEGUI::utf8*)CONV_MBCS_UTF8("Ȯ��")); 

		//	������ ��� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pExitCancel = m_pExitGameBox->getChild("Root/ExitBox/Cancel_Btn");

		if (pExitCancel)
			pExitCancel->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ExitCancelButtonClick, this));

		pExitCancel->setFont( (CEGUI::utf8*)szFont);
		pExitCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���")); 
	}

	if( m_pConnectGameBox )
	{
		//	���� Ȯ�� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pConnectOk = m_pConnectGameBox->getChild("Root/ConnectBox/Ok_Btn");

		if (pConnectOk)
		{
			pConnectOk->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ConnectOkButtonClick, this));

			pConnectOk->subscribeEvent( CEGUI::PushButton::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ConnectOkButtonClick, this));
		}


		pConnectOk->setFont( (CEGUI::utf8*)szFont);
		pConnectOk->setText((CEGUI::utf8*)CONV_MBCS_UTF8("����")); 

		//	���� ��� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pConnectCancel = m_pConnectGameBox->getChild("Root/ConnectBox/Cancel_Btn");

		if (pConnectCancel)
		{
			pConnectCancel->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ConnectCancelButtonClick, this));
			
			pConnectCancel->subscribeEvent( CEGUI::PushButton::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_ConnectCancelButtonClick, this));
		}

		pConnectCancel->setFont( (CEGUI::utf8*)szFont);
		pConnectCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���")); 
	}

	if( m_pCreateGameBox )
	{
		//	���� Ȯ�� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pCreateGameOk = m_pCreateGameBox->getChild("Root/CreateGameBox/Ok_Btn");

		if (pCreateGameOk)
		{
			pCreateGameOk->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameBoxOkButtonClick, this));

			pCreateGameOk->subscribeEvent( CEGUI::PushButton::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameBoxOkButtonClick, this));

			pCreateGameOk->subscribeEvent( CEGUI::PushButton::EventMouseClick, 
										CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameBoxOkButtonClick, this));
		}


		pCreateGameOk->setFont( (CEGUI::utf8*)szFont);
		pCreateGameOk->setText((CEGUI::utf8*)CONV_MBCS_UTF8("����")); 

		//	���� ��� ��ư �̺�Ʈ �߰�
		CEGUI::Window *pCreateGameCancel = m_pCreateGameBox->getChild("Root/CreateGameBox/Cancel_Btn");

		if (pCreateGameCancel)
		{
			pCreateGameCancel->subscribeEvent(	CEGUI::PushButton::EventClicked, 
												CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameBoxCancelButtonClick, this));
			
			pCreateGameCancel->subscribeEvent(	CEGUI::PushButton::EventKeyUp, 
												CEGUI::Event::Subscriber(&CMenuState::Event_CreateGameBoxCancelButtonClick, this));
		}

		pCreateGameCancel->setFont( (CEGUI::utf8*)szFont);
		pCreateGameCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���")); 
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									����Ʈ �ڽ� �̺�Ʈ �߰�
	//	
	//
	//------------------------------------------------------------------------------------------

	if( m_pConnectGameBox )
	{
		//	IP �Է� Edit �ڽ� �̺�Ʈ �߰�
		CEGUI::Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");

		pIPEdit->setText( address );
		
		
		//	����Ű or ��Ű�� �������� �̺�Ʈ �߰�
		if (pIPEdit)
		{
			pIPEdit->subscribeEvent( CEGUI::Editbox::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_IPTextAdded, this));
		}

	
		//	IP �Է� Edit �ڽ� �̺�Ʈ �߰�
		CEGUI::Window *pIDEdit = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit");

		if (pIDEdit)
		{
			pIDEdit->subscribeEvent( CEGUI::Editbox::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_IDTextAdded, this));
		}



		CEGUI::Window *pIPStatic = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Static");
		if( pIPStatic )
		{
			pIPStatic->setFont( (CEGUI::utf8*)szFont);
			pIPStatic->setText((CEGUI::utf8*)CONV_MBCS_UTF8("������ : ")); 
		}

		CEGUI::Window *pIDStatic = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Static");
		if( pIDStatic )
		{
			pIDStatic->setFont( (CEGUI::utf8*)szFont);
			pIDStatic->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���̵� : ")); 
		}
	}

	if( m_pCreateGameBox )
	{
		//	ID �Է� Edit �ڽ� �̺�Ʈ �߰�
		CEGUI::Window *pIDEdit = m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit");

		//	����Ű or ��Ű�� �������� �̺�Ʈ �߰�
		if (pIDEdit)
		{
			pIDEdit->subscribeEvent( CEGUI::Editbox::EventKeyUp, 
										CEGUI::Event::Subscriber(&CMenuState::Event_CaptainIDTextAdded, this));

		}

		CEGUI::Window *pIDStatic = m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Static");
		if( pIDStatic )
		{
			pIDStatic->setFont( (CEGUI::utf8*)szFont);
			pIDStatic->setText((CEGUI::utf8*)CONV_MBCS_UTF8("���̵� : ")); 
		}

	}

	
	m_pBackImg = m_pGUI->getChild("Root/Background");
	
	m_pBackImg->setAlpha( 0.0f );

	m_fLastTime = m_fStartTime = NiGetCurrentTimeInSec();

	CGameApp::mp_Appication->SetActiveEditBox(true);

	CEGUI::MouseCursor::getSingleton().show();

	return true;
}


//====================================================================
//			
//						UI event handlers
//
//====================================================================

bool CMenuState::Event_CreateGameBoxOkButtonClick(const CEGUI::EventArgs &rE)
{

	using namespace CEGUI;
	
	if( !m_pCreateGameBox ) return false; 

	Window *pIDEdit = m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit");
	
	if( !pIDEdit ) return false;

	// ��Ű ó��
	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);
	const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(rE);
	
	if( ke.scancode == CEGUI::Key::Tab )
	{		 
		m_pCreateGameBox->getChild("Root/CreateGameBox/Cancel_Btn")->activate();

		return true;
	}
	else if(	ke.scancode == CEGUI::Key::Return ||
				me.button == CEGUI::MouseButton::LeftButton )
	{
		// ���� �� UI ��ȯ �� ���� ����
		const CEGUI::String& strID = pIDEdit->getText(); 

		// ��ȿ ���� �˻��� ��Ʈ��ũ �̱��濡 ���� ��ó
		if( strID.size() )
		{
			// ���� ����
			if( ConnectGame( true, strID ) )
			{
				
			}

			// Clear the text in the Editbox
			pIDEdit->setText("");

			m_pCreateGameBox->setEnabled( false );
			m_pCreateGameBox->setVisible( false );

		}
	}


	return true;
}


bool CMenuState::ConnectGame( bool bType, const CEGUI::String& strID )
{
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetCaptain( bType );
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetID( strID );
	rStateManager.ChangeState("GameRoom");

	return true;
}

bool CMenuState::ConnectGame( bool bType, const CEGUI::String& strID, const CEGUI::String& strIP)
{
	CStateManager &rStateManager = CGameApp::GetStateManager();	
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetCaptain( bType );
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetID( strID );
	((CGameRoomState*)rStateManager.GetStateClass("GameRoom"))->SetIP( strIP );
	rStateManager.ChangeState("GameRoom");

	return true;
}



bool CMenuState::Event_CreateGameBoxCancelButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pCreateGameBox ) return false;

	Window *pIDEdit = m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit");

	if( !pIDEdit ) return false;

	// ��Ű ó��
	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);

	switch( ke.scancode )
	{
	case CEGUI::Key::Tab :
		
		m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit")->activate();
			return true;

	case CEGUI::Key::Return :		

		pIDEdit->setText("");
		m_pCreateGameBox->setEnabled( false );
		m_pCreateGameBox->setVisible( false );
		
		return true;

	}

	pIDEdit->setText("");

	m_pCreateGameBox->setEnabled( false );
	m_pCreateGameBox->setVisible( false );

	return true;
}
/*
bool CMenuState::Event_IPEditBoxActive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(true);
	return true;
}
bool CMenuState::Event_IPEditBoxDeactive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(false);
	return true;
}

bool CMenuState::Event_IDEditBoxActive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(true);
	return true;
}
bool CMenuState::Event_IDEditBoxDeactive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(false);
	return true;
}

bool CMenuState::Event_IPMouseClick(const CEGUI::EventArgs &rE)
{
	m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit")->deactivate();
	m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit")->activate();

	return true;
}

bool CMenuState::Event_IDMouseClick(const CEGUI::EventArgs &rE)
{
	m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit")->deactivate();
	m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit")->activate();
	return true;
}

bool CMenuState::Event_CreateGameEditBoxActive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(true);
	return true;
}

bool CMenuState::Event_CreateGameEditBoxDeactive(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->SetActiveEditBox(false);
	return true;
}
*/

bool CMenuState::Event_CaptainIDTextAdded(const CEGUI::EventArgs &rE)
{

	using namespace CEGUI;

	if( !m_pCreateGameBox ) return false;

	Window *pIDEdit = m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit");
	
	if( !pIDEdit ) return false;

	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);
	
	switch( ke.scancode )
	{
	case CEGUI::Key::Return:

		// ��ȿ ���� �˻��� ��Ʈ��ũ �̱��濡 ���� ��ó
		if( pIDEdit->getText().size() )
		{
			// ���� ����
			if( ConnectGame( true, pIDEdit->getText() ) )
			{

			}


			// Clear the text in the Editbox
			pIDEdit->setText("");

			m_pCreateGameBox->setEnabled( false );
			m_pCreateGameBox->setVisible( false );

			return true;
		}

		pIDEdit->setText("");
		
		break;
	
	
	case CEGUI::Key::Tab :
		 
		m_pCreateGameBox->getChild("Root/CreateGameBox/Ok_Btn")->activate();

		break;
	}

	return true;
}

// NewGame ��ư�� Ŭ�������� NewGameBox�� Ȱ��ȭ
bool CMenuState::Event_NewGameButtonClick(const CEGUI::EventArgs &rE)
{
/*
	const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(rE);
	
	if(me.button == CEGUI::LeftButton)
	{
//		if(me.window->getName().compare("Root/Newgame") == 0)
//		{
//			popupMenu = reinterpret_cast<CEGUI::*>(winMgr.getWindow("Root/Popup/PopupMenus/FirstPopup/AutoPopup"));
//		}
	}
*/
	if( !m_pNewGameBox || !m_pConnectGameBox || !m_pExitGameBox || !m_pCreateGameBox) return false;

	if( m_pExitGameBox->isVisible() ) return false;

	m_pNewGameBox->setEnabled( !m_pNewGameBox->isVisible() );
	m_pNewGameBox->setVisible( !m_pNewGameBox->isVisible() );	

	if( m_pConnectGameBox->isVisible() )
	{
		m_pConnectGameBox->setVisible( false );
		m_pConnectGameBox->setEnabled( false );		
	}

	if( m_pCreateGameBox->isVisible() )
	{
		m_pCreateGameBox->setVisible( false );
		m_pCreateGameBox->setEnabled( false );
	}

	return true;
}

// ������ ��ư Ŭ�������� ExitBox�� Ȱ��ȭ
bool CMenuState::Event_ExitGameButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pExitGameBox || !m_pConnectGameBox || !m_pNewGameBox || !m_pCreateGameBox ) return false;

	m_pExitGameBox->setEnabled( !m_pExitGameBox->isVisible() );
	m_pExitGameBox->setVisible( !m_pExitGameBox->isVisible() );

	if( m_pConnectGameBox->isVisible() )
	{		
		Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");
		Window *pIDEdit = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit");
		
		if( !pIPEdit || !pIDEdit ) return false;

		// Clear the text in the Editbox
		pIPEdit->setText("");
		pIDEdit->setText("");

		m_pConnectGameBox->setEnabled( false );
		m_pConnectGameBox->setVisible( false );
	}

	if( m_pCreateGameBox->isVisible() )
	{
		m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit")->setText("");
		m_pCreateGameBox->setEnabled( false );
		m_pCreateGameBox->setVisible( false );
	}

	if( m_pNewGameBox->isVisible() )
	{
		m_pNewGameBox->setEnabled( !m_pNewGameBox->isVisible() );
		m_pNewGameBox->setVisible( !m_pNewGameBox->isVisible() );
	}

	return true;
}

// Join Game ��ư Ŭ�������� ConnectBox�� Ȱ��ȭ
bool CMenuState::Event_JoinGameButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pConnectGameBox || !m_pNewGameBox || !m_pExitGameBox ) return false;	

	if( m_pExitGameBox->isVisible() || m_pConnectGameBox->isVisible() ) return false;

	m_pNewGameBox->setEnabled( !m_pNewGameBox->isVisible() );
	m_pNewGameBox->setVisible( !m_pNewGameBox->isVisible() );

	m_pConnectGameBox->setEnabled( true );
	m_pConnectGameBox->setVisible( true );

	m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit")->activate();

	return true;
}

// Option ȭ������ ��ȯ
bool CMenuState::Event_OptionButtonClick(const CEGUI::EventArgs &rE)
{
//	CStateManager &rStateManager = CGameApp::GetStateManager();
//	rStateManager.ChangeState("GameRoomState");
	return true;
}

// Credit ȭ������ ��ȯ
bool CMenuState::Event_CreditButtonClick(const CEGUI::EventArgs &rE)
{

	CStateManager &rStateManager = CGameApp::GetStateManager();
	rStateManager.ChangeState("Credit");

	return true;
}



// �� ����� ������ ������ â ���
bool CMenuState::Event_CreateGameButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pConnectGameBox || !m_pNewGameBox || !m_pExitGameBox ) return false;	

	if( m_pExitGameBox->isVisible() || m_pConnectGameBox->isVisible() ) return false;

	m_pNewGameBox->setEnabled( !m_pNewGameBox->isVisible() );
	m_pNewGameBox->setVisible( !m_pNewGameBox->isVisible() );

	m_pCreateGameBox->setEnabled( true );
	m_pCreateGameBox->setVisible( true );

	m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit")->activate();

	return true;
}


// ���� ��û ��ư�� Ŭ�������� ��Ʈ��ũ�� �ش� IP�� ���� �õ�
bool CMenuState::Event_ConnectOkButtonClick(const CEGUI::EventArgs &rE)
{

	using namespace CEGUI;
	
	if( !m_pConnectGameBox ) return false; 

	Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");
	Window *pIDEdit = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit");
	
	if( !pIPEdit || !pIDEdit ) return false;

	// ��Ű ó��
	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);

	if( ke.scancode == CEGUI::Key::Tab )
	{		 
		m_pConnectGameBox->getChild("Root/ConnectBox/Cancel_Btn")->activate();

		return true;
	}
	
	// ��Ʈ��ũ ���� ó��
	const CEGUI::String& strIP = pIPEdit->getText(); 
	const CEGUI::String& strID = pIDEdit->getText(); 

	// ��ȿ ���� �˻��� ��Ʈ��ũ �̱��濡 ���� ��ó
	if( strIP.size() && strID.size() )
	{
//		NetworkManager::GetInstance()->SetIP( strIP.c_str() );
		if( ConnectGame( false, strID, strIP ) )
		{

		}

		// Clear the text in the Editbox
		pIPEdit->setText("");
		pIDEdit->setText("");

		m_pConnectGameBox->setEnabled( false );
		m_pConnectGameBox->setVisible( false );

		return true;
	}

	pIPEdit->setText("");
	pIDEdit->setText("");

	return false;
}

// ���ӹڽ����� ��� ��ư Ŭ��������
bool CMenuState::Event_ConnectCancelButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pConnectGameBox ) return false;

	Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");
	Window *pIDEdit = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit");

	if( !pIPEdit || !pIDEdit ) return false;

	// ��Ű ó��
	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);

	switch( ke.scancode )
	{
	case CEGUI::Key::Tab :
		
		m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit")->activate();
			return true;

	case CEGUI::Key::Return :

		pIPEdit->setText("");
		pIDEdit->setText("");

		m_pConnectGameBox->setEnabled( false );
		m_pConnectGameBox->setVisible( false );			
		return true;

	}

	pIPEdit->setText("");
	pIDEdit->setText("");

	m_pConnectGameBox->setEnabled( false );
	m_pConnectGameBox->setVisible( false );

	return true;
}

// ���ø����̼� ���� ó��
bool CMenuState::Event_ExitOkButtonClick(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->QuitApplication();

	return true;
}

// ����ڽ�c���� ��� ��ư Ŭ��������
bool CMenuState::Event_ExitCancelButtonClick(const CEGUI::EventArgs &rE)
{
	if( !m_pExitGameBox ) return false;

	m_pExitGameBox->setEnabled( false );
	m_pExitGameBox->setVisible( false );

	return true;
}

bool CMenuState::Event_IPTextAdded(const CEGUI::EventArgs &rE)
{
	using namespace CEGUI;

	if( !m_pConnectGameBox ) return false;

	CEGUI::Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");

	if( !pIPEdit ) return false;

	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);

	switch( ke.scancode )
	{
	case CEGUI::Key::Return:
	case CEGUI::Key::Tab:

		pIPEdit->deactivate();
		m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit")->activate();
		break;
	}

	return true;
}

bool CMenuState::Event_IDTextAdded(const CEGUI::EventArgs &rE)
{
	using namespace CEGUI;

	if( !m_pConnectGameBox ) return false;

	Window *pIPEdit = m_pConnectGameBox->getChild("Root/ConnectBox/IP_Edit");
	Window *pIDEdit = m_pConnectGameBox->getChild("Root/ConnectBox/ID_Edit");
	
	if( !pIPEdit || !pIDEdit ) return false;

	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(rE);
	
	CEGUI::Window *pOkBtn = NULL;

	switch( ke.scancode )
	{
	case CEGUI::Key::Return:

		// ��Ʈ��ũ ���� ó��

		// ��ȿ ���� �˻��� ��Ʈ��ũ �̱��濡 ���� ��ó
		if( pIPEdit->getText().size() && pIDEdit->getText().size() )
		{
			if( ConnectGame( false, pIDEdit->getText(), pIPEdit->getText() ) )
			{

			}

			// Clear the text in the Editbox
			pIPEdit->setText("");
			pIDEdit->setText("");

			m_pConnectGameBox->setEnabled( false );
			m_pConnectGameBox->setVisible( false );

			return true;
		}

		pIPEdit->setText("");
		pIDEdit->setText("");
		
		break;
	
	
	case CEGUI::Key::Tab :
		 
		m_pConnectGameBox->getChild("Root/ConnectBox/Ok_Btn")->activate();

		break;
	}
	
	return true;
}

bool CMenuState::Event_ResetCaret(const CEGUI::EventArgs &rE)
{

	return true;
}

void CMenuState::AddChatText(const CEGUI::String& pText)
{
	if(pText.size())
	{
	}
}


void CMenuState::OnUpdate(float fCurrentTime)
{
	// update the base state
	CGameUIState::OnUpdate(fCurrentTime);

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

    if (!pkKeyboard)
        return;

	float m_fCurrenTime = NiGetCurrentTimeInSec();

	if( m_pBackImg->getAlpha() < 1.0f )
	{				
		float fDeltaTime	= m_fCurrenTime - m_fLastTime;

		m_pBackImg->setAlpha( m_pBackImg->getAlpha() + (fDeltaTime) );

		if( m_pBackImg->getAlpha() + fDeltaTime >= 1.0f )
			m_pBackImg->setAlpha( 1.0f );	
	}

	m_fLastTime	= m_fCurrenTime;


    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
	{
		if( m_pConnectGameBox && m_pConnectGameBox->isVisible() )
		{
			m_pConnectGameBox->getChild("Root/ConnectBox/Ok_Btn")->setText("");
			m_pConnectGameBox->getChild("Root/ConnectBox/Cancel_Btn")->setText("");
			m_pConnectGameBox->setVisible( false );
			m_pConnectGameBox->setEnabled( false );
			return;
		}

		if( m_pCreateGameBox && m_pCreateGameBox->isVisible() )
		{
			m_pCreateGameBox->getChild("Root/CreateGameBox/ID_Edit")->setText("");
			m_pCreateGameBox->setEnabled( false );
			m_pCreateGameBox->setVisible( false );
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
	}
}

void CMenuState::OnLeave(const char* szNext)
{	
	//������� ����
	if(m_pBGChannel)
		m_pBGChannel->stop();

	if (!CGameApp::mp_Appication)
		return;

	m_pNewGameBox		= NULL;
	m_pExitGameBox		= NULL;
	m_pConnectGameBox	= NULL;
	m_pCreateGameBox	= NULL;
	m_pBackImg			= NULL;

	CGameApp::mp_Appication->SetActiveEditBox(false);

	CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();	

	CGameUIState::OnLeave(szNext);
}

