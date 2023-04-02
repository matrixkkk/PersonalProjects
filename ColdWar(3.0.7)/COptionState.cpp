#include "COptionState.h"
#include "GameApp.h"
#include "NetWorkManager.h"
#include "strconv.h"

char *szFont1 = "ahn_m";

COptionState::COptionState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
/*	m_pMenuBox		= NULL;
	m_pChatEditBox	= NULL;
	m_pStaticInput	= NULL;
	m_pChatListBox	= NULL;

	m_bActivaChaEdit = false;

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );
	::ZeroMemory( m_pLogoImg, sizeof(m_pLogoImg) );	
	*/
}

bool COptionState::Initialize()
{/*
	using namespace CEGUI;

	ImagesetManager::getSingleton().createImageset(String("USA.imageset"));
	ImagesetManager::getSingleton().createImageset(String("EU.imageset"));
	ImagesetManager::getSingleton().createImageset(String("Guerrilla.imageset"));

	if (!CGameUIState::Initialize() || 0 == m_pGUI)
		return false;

	// Retrieve the window manager
	WindowManager& winMgr = WindowManager::getSingleton();

	InitializeCriticalSection(&m_csMcl);

	m_pMenuBox		= NULL;
	m_pChatEditBox	= NULL;
	m_pStaticInput	= NULL;
	m_pChatListBox	= NULL;

	m_bActivaChaEdit= false;

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );
	::ZeroMemory( m_pLogoImg, sizeof(m_pLogoImg) );

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
									  CEGUI::Event::Subscriber(&COptionState::Event_MenuBoxExitBtnClick, this));

			pGameExit->setFont((CEGUI::utf8*)szFont1);
			pGameExit->setText((CEGUI::utf8*)CONV_MBCS_UTF8("나가기")); 

		}
		
		CEGUI::Window *pCancel = m_pMenuBox->getChild("Root/MenuBox/Cancel_Btn");

		if (pCancel)
		{
			pCancel->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&COptionState::Event_MenuBoxCancelBtnClick, this));

			pCancel->setFont((CEGUI::utf8*)szFont1);
			pCancel->setText((CEGUI::utf8*)CONV_MBCS_UTF8("취소")); 
		}

		CEGUI::Window *pOption = m_pMenuBox->getChild("Root/MenuBox/Option_Btn");

		if (pOption)
		{
			pOption->subscribeEvent(CEGUI::PushButton::EventClicked, 
									CEGUI::Event::Subscriber(&COptionState::Event_MenuBoxOptionBtnClick, this));

			pOption->setFont((CEGUI::utf8*)szFont1);
			pOption->setText((CEGUI::utf8*)CONV_MBCS_UTF8("옵션")); 
		}

		CEGUI::Window *pVote = m_pMenuBox->getChild("Root/MenuBox/Vote_Btn");

		if (pVote)
		{
			pVote->subscribeEvent(CEGUI::PushButton::EventClicked, 
								  CEGUI::Event::Subscriber(&COptionState::Event_MenuBoxVoteBtnClick, this));

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
	m_pStaticInput = m_pGUI->getChild("Root/Chat_Static");

	if( m_pStaticInput )
	{
		m_pStaticInput->setFont((CEGUI::utf8*)szFont1);
		m_pStaticInput->setText((CEGUI::utf8*)CONV_MBCS_UTF8("입력:"));		
		m_pStaticInput->setVisible(false);
	}

	//------------------------------------------------------------------------------------------
	//
	//					
	//									리스트 박스 설정
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pChatListBox =  reinterpret_cast<CEGUI::Listbox*>(m_pGUI->getChild("Root/ChatListBox"));
	
	if(m_pChatListBox) 
		m_pChatListBox->setFont((CEGUI::utf8*)szFont1);

	//------------------------------------------------------------------------------------------
	//
	//					
	//									스태틱 이미지 설정
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pLogoImg[ USA ]		= reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/USA_Image") );

	m_pLogoImg[ EU ]		= reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/EU_Image") );

	m_pLogoImg[ GUERRILLA ] = reinterpret_cast<CEGUI::Window*>( m_pGUI->getChild("Root/Guerrilla_Image") );


	//------------------------------------------------------------------------------------------
	//
	//					
	//									에디트 박스 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	m_pChatEditBox = reinterpret_cast<CEGUI::Editbox*>( m_pGUI->getChild("Root/Chat_Edit") );
	m_pChatEditBox->setFont( (CEGUI::utf8*)szFont1 );
	
	//	엔터키 or 탭키를 눌렀을때 이벤트 추가
	if (m_pChatEditBox)
	{
		m_pChatEditBox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, 
								CEGUI::Event::Subscriber(&COptionState::Event_ChatTextAdded, this));
	}

	m_pChatEditBox->setVisible( false );

	//------------------------------------------------------------------------------------------
	//
	//					
	//									멀티 컬럼 리스트 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	String strId	= (CEGUI::utf8*)CONV_MBCS_UTF8("닉네임");
	String strKill	= (CEGUI::utf8*)CONV_MBCS_UTF8("킬");	
	String strDeath	= (CEGUI::utf8*)CONV_MBCS_UTF8("데스");

	m_pListMcl[ USA ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/USA"));
	m_pListMcl[ EU ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/EU"));
	m_pListMcl[ GUERRILLA ] = reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/Guerrilla"));	

	for( int i = 0; i < 3; i++ )
	{
		m_pListMcl[i]->setSelectionMode(CEGUI::MultiColumnList::SelectionMode::RowMultiple);
		m_pListMcl[i]->setShowVertScrollbar(false);
		m_pListMcl[i]->setShowHorzScrollbar(false);
		// Ascending	오름차순
		// Descending	내림차순
		m_pListMcl[i]->setSortDirection( ListHeaderSegment::SortDirection::Ascending );
			
		m_pListMcl[i]->setVisible( true );

		m_pListMcl[i]->addColumn(strId,		ID,		cegui_absdim(95));
		m_pListMcl[i]->addColumn(strKill,	KILL,	cegui_absdim(30));
		m_pListMcl[i]->addColumn(strDeath,	DEATH,	cegui_absdim(30));
		
	}
/*
	int nRowUSA = 0, nRowEU = 0, nRowGue = 0;
	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();
	char pKillDeath[1] = {0};

	

	for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer )
	{
		if( iterPlayer->second.userInfo.camp == nsPlayerInfo::USA )
		{
			m_pListMcl[this->USA]->addRow( iterPlayer->second.userInfo.id );

			m_pListMcl[this->USA]->setItem( new ListboxTextItem(iterPlayer->second.userRoomInfo.m_strID), ID, nRowUSA );
			
			sprintf_s( pKillDeath, "%d", iterPlayer->second.userInfo.kill );
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
	

	for( int i = 0; i < 3; i++ )
	{
		m_pListMcl[i]->setVisible( false );
		m_pLogoImg[i]->setVisible( false );
	}

*/
/*
	NetworkManager::GetInstance()->UnLockPlayers();
	
	m_pListMcl[USA]->addRow(0);
	m_pListMcl[USA]->addRow(1);
	m_pListMcl[USA]->addRow(2);

	ListboxTextItem *item = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("아쿰"));
	m_pListMcl[USA]->setItem( item, 0, 0 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("1") , 1, 0 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("10"), 2, 0 );

	item = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("서현"));
	m_pListMcl[USA]->setItem( item, 0, 1 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("9") , 1, 1 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("11"), 2, 1 );

	item = new ListboxTextItem((CEGUI::utf8*)CONV_MBCS_UTF8("또띠"));
	m_pListMcl[USA]->setItem( item, 0, 2 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("3") , 1, 2 );
	m_pListMcl[USA]->setItem( new ListboxTextItem("11"), 2, 2 );

	// 값 변경시 마다 실행한다.
	m_pListMcl[USA]->setSortColumn( KILL );	

	
	int a = m_pListMcl[USA]->getRowID( 0 ); 
	int a1 = m_pListMcl[USA]->getRowID( 1 ); 
	int a2 = m_pListMcl[USA]->getRowID( 2 ); 
	
	// 클라 고유 아이디를 이용하여 row값을 얻어온다.
	int b = m_pListMcl[USA]->getRowWithID( 0 ); 
	int b1 = m_pListMcl[USA]->getRowWithID( 1 ); 
	int b2 = m_pListMcl[USA]->getRowWithID( 2 ); 

*/

	return true;
}

/*
void COptionState::UpdateKill( BYTE nID, BYTE nTeam, BYTE nKill )
{
	unsigned int nRow =  m_pListMcl[ nTeam ]->getRowWithID( nID );

	char buf[3]={0,};
	_itoa_s( nKill, buf, sizeof(buf), 10 );

	m_pListMcl[ nTeam ]->setItem( new CEGUI::ListboxTextItem(buf), KILL, nRow );

	m_pListMcl[ nTeam ]->setSortColumn( KILL );	

}

void COptionState::UpdateDeath( BYTE nID, BYTE nTeam, BYTE nDeath )
{
	unsigned int nRow =  m_pListMcl[ nTeam ]->getRowWithID( nID );

	char buf[3]={0,};
	_itoa_s( nDeath, buf, sizeof(buf), 10 );

	m_pListMcl[ nTeam ]->setItem( new CEGUI::ListboxTextItem(buf), DEATH, nRow );
}

bool COptionState::Event_ChatTextAdded(const CEGUI::EventArgs &rE)
{

	if( !m_pChatEditBox && m_pChatEditBox->isVisible() )
		return false;

	const CEGUI::String &strChat = m_pChatEditBox->getText();

	if( !strChat.size() )
	{
		return false;
	}

	AddChatText(strChat);
/*
	//----------------------------------------------
	//	 방장의 경우 유저들에게 보내고 자신은 UI 갱신
	//----------------------------------------------
	if( NetworkManager::GetInstance()->IsCaptain() )
	{		
		NetworkManager::GetInstance()->GetServer()->SendChatMsg( strChat, nsPlayerInfo::NET_GAME_STATE );		
	}
	else
	//----------------------------------------------
	//	 일반 유저의 경우 ChatMsg 요청
	//----------------------------------------------
	{
		NetworkManager::GetInstance()->GetClient()->RequestSendChat( strChat, nsPlayerInfo::NET_GAME_STATE );
	}
*//*
	// Clear the text in the Editbox
	m_pChatEditBox->setText("");

	return true;
}
bool COptionState::Event_MenuBoxExitBtnClick(const CEGUI::EventArgs &rE)
{
	CGameApp::mp_Appication->QuitApplication();

	return true;
}
bool COptionState::Event_MenuBoxCancelBtnClick(const CEGUI::EventArgs &rE)
{
	if( m_pMenuBox ) m_pMenuBox->setVisible( false );

	return true;
}
bool COptionState::Event_MenuBoxOptionBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}
bool COptionState::Event_MenuBoxVoteBtnClick(const CEGUI::EventArgs &rE)
{
	return true;
}

void COptionState::AddChatText(const CEGUI::String& pText)
{
	if(pText.size())
	{
		ListboxTextItem* chatItem;
		const CEGUI::Image* sel_img = &ImagesetManager::getSingleton().getImageset("WonderlandLook")->getImage("MultiListSelectionBrush");

		if(m_pChatListBox->getItemCount() == MAX_CHATLISTBOX_ITEM_IN_GAME)
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
	}
}
*/

void COptionState::OnUpdate(float fCurrentTime)
{
	CGameUIState::OnUpdate(fCurrentTime);

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

    if (!pkKeyboard)
        return;
/*
    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
	{
		
		if( !m_pMenuBox->isVisible() )
		{
			m_pMenuBox->setVisible( true );
			m_pMenuBox->setEnabled( true );
			return;
		}
		else
		{
			m_pMenuBox->setVisible( false );
			m_pMenuBox->setEnabled( false );
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
//			m_bActivaChaEdit = true;
			m_pChatEditBox->setVisible( true );
			m_pStaticInput->setVisible( true );
			m_pChatEditBox->activate();
		}
		else
		{
			m_pChatEditBox->setVisible( false );
			m_pStaticInput->setVisible( false );
		}
	}
	*/
}

void COptionState::OnLeave(const char* szNext)
{

	if (!CGameApp::mp_Appication)
		return;

//	DeleteCriticalSection(&m_csMcl);

	CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();

	CGameUIState::OnLeave(szNext);

}