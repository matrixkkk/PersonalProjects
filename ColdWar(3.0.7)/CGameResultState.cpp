#include "GameApp.h"
#include "CStateManager.h"
#include "CGameResultState.h"
#include "GameSystemManager.h"
#include "CMenuState.h"
#include "CLoadingDisplay.h"
#include "strconv.h"


CGameResultState::CGameResultState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );

	m_fStartTime = 0.0f;
}

bool CGameResultState::Initialize()
{
	using namespace CEGUI;

	if (!CGameUIState::Initialize() || 0 == m_pGUI)
		return false;

	// Retrieve the window manager
	WindowManager& winMgr = WindowManager::getSingleton();

	char *szFont = "ahn_m";

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );

	
	//------------------------------------------------------------------------------------------
	//
	//					
	//									버튼 이벤트 추가
	//	
	//
	//------------------------------------------------------------------------------------------

	CEGUI::Window *pOk = m_pGUI->getChild("Root/Ok_Btn");

	if(pOk)
	{
		pOk->subscribeEvent( CEGUI::PushButton::EventClicked, 
										CEGUI::Event::Subscriber(&CGameResultState::Event_OkBtnClick, this));

		pOk->setFont( (CEGUI::utf8*)szFont);
		pOk->setText((CEGUI::utf8*)CONV_MBCS_UTF8("나가기")); 
	}

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

	m_pListMcl[ USA ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/USA_List"));
	m_pListMcl[ EU ]		= reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/EU_List"));
	m_pListMcl[ GUERRILLA ] = reinterpret_cast<MultiColumnList*>(m_pGUI->getChild("Root/Guerrilla_List"));	

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

	int nRowUSA = 0, nRowEU = 0, nRowGue = 0;
	char pKillDeath[5] = {0,};

	
	
	std::vector< GAME_USER_INFO >::iterator iterPlayer;
	
	for( iterPlayer = m_vecPlayers.begin(); iterPlayer != m_vecPlayers.end(); ++iterPlayer )
	{
		if( iterPlayer->userInfo.camp == nsPlayerInfo::USA )
		{
			m_pListMcl[this->USA]->addRow( iterPlayer->userInfo.id );

			m_pListMcl[this->USA]->setItem( new ListboxTextItem(iterPlayer->userRoomInfo.m_strID), ID, nRowUSA );
			
			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.kill );
			m_pListMcl[this->USA]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowUSA );

			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.death );
			m_pListMcl[this->USA]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowUSA );

			++nRowUSA;

		}
		else if( iterPlayer->userInfo.camp == nsPlayerInfo::EU )
		{
			m_pListMcl[this->EU]->addRow( iterPlayer->userInfo.id );

			m_pListMcl[this->EU]->setItem( new ListboxTextItem(iterPlayer->userRoomInfo.m_strID), ID, nRowEU );

			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.kill );
			m_pListMcl[this->EU]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowEU );

			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.death );
			m_pListMcl[this->EU]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowEU );

			++nRowEU;
		}
		else
		{
			m_pListMcl[this->GUERRILLA]->addRow( iterPlayer->userInfo.id );

			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(iterPlayer->userRoomInfo.m_strID), ID, nRowGue );

			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.kill );
			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(pKillDeath) , KILL, nRowGue );

			sprintf_s( pKillDeath, "%d", iterPlayer->userInfo.death );
			m_pListMcl[this->GUERRILLA]->setItem( new ListboxTextItem(pKillDeath), DEATH, nRowGue );

			++nRowGue;
		}
	}

	for( int i = 0; i < 3; i++ )
		m_pListMcl[i]->setVisible( true );

	CEGUI::MouseCursor::getSingleton().show();

	m_fStartTime = CGameApp::mp_Appication->GetAccumTime();

	return true;
}



void CGameResultState::SavePlayerData()
{

	SCOREMAP &playerScore = GameSystemManager::GetInstance()->GetScoreBoard();

	PlayersMap::iterator iterPlayer;
	PlayersMap &conPlayers = NetworkManager::GetInstance()->LockPlayers();

	for( iterPlayer = conPlayers.begin(); iterPlayer != conPlayers.end(); ++iterPlayer  )
	{
		iterPlayer->second.userInfo.kill = playerScore[ iterPlayer->second.userInfo.id ].kill;
		iterPlayer->second.userInfo.death = playerScore[ iterPlayer->second.userInfo.id ].death;
		m_vecPlayers.push_back( iterPlayer->second );
	}

	NetworkManager::GetInstance()->UnLockPlayers();

}

//====================================================================
//			
//						UI event handlers
//
//====================================================================

bool CGameResultState::Event_OkBtnClick(const CEGUI::EventArgs &rE)
{
	CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();
		rStateManager.ChangeState("MainMenu");

	return true;
}

void CGameResultState::OnUpdate(float fCurrentTime)
{
	// update the base state
	CGameUIState::OnUpdate(fCurrentTime);

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();

    if (!pkKeyboard)
        return;

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE) )//||
	//	(fCurrentTime - m_fStartTime >= 10.0f) )
	{
		CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();
		rStateManager.ChangeState("MainMenu");
	} 
	
}

void CGameResultState::OnLeave(const char* szNext)
{	
	if (!CGameApp::mp_Appication)
		return;

	if( m_vecPlayers.empty() )
		m_vecPlayers.clear();

	::ZeroMemory( m_pListMcl, sizeof(m_pListMcl) );

	m_fStartTime = 0.0f;

	CGameUIState::OnLeave(szNext);
}

