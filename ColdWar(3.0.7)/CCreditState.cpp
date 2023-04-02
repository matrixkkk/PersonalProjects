#include "CCreditState.h"
#include "GameApp.h"

CCreditState::CCreditState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
}

bool CCreditState::Initialize()
{
	using namespace CEGUI;

	if (!CGameUIState::Initialize() || 0 == m_pGUI)
		return false;

	m_pBackImg = m_pGUI->getChild("Root/Background");
	
	m_pBackImg->setAlpha( 0.0f );

	m_fLastTime = m_fStartTime = NiGetCurrentTimeInSec();

	return true;

}

void CCreditState::OnUpdate(float fCurrentTime)
{
	CGameUIState::OnUpdate(fCurrentTime);

	NiInputKeyboard*	pkKeyboard = CGameApp::mp_Appication->GetInputSystem()->GetKeyboard();
	NiInputMouse*		pkMouse = CGameApp::mp_Appication->GetInputSystem()->GetMouse();

    if (!pkKeyboard || !pkMouse)
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

	if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE) ||
		pkMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT) ||
		pkMouse->ButtonWasPressed(NiInputMouse::NIM_RIGHT) ||
		(m_fLastTime - m_fStartTime >= 10.0f) )
	{
		CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();
		rStateManager.ChangeState("MainMenu");
	}   

}