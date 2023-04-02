#include "CStartSplashState.h"
#include "GameApp.h"
//#include "../GameObject.h"

//----------------------------------------------------------------------------
//  StartupSplash::StartupSplash
//
/// @param pcGUIImageSetFilename  The name of the CEGUI *.imageset filename containing
///							      the GUI elements
/// @param pcGUILayoutFilename    The name of the CEGUI *.layout filename containing the
///                               state's GUI layout
///
/// Constructor
//----------------------------------------------------------------------------
CStartSplashState::CStartSplashState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) :
	CGameUIState(pcGUIImageSetFilename, pcGUILayoutFilename)
{
}

//----------------------------------------------------------------------------
//  StartupSplash::Update
//
/// Update the state per frame. In this case, we simply wait for the game
/// runtime to reach 1 second, then transition to the next state.
///
/// @param  fCurrentTime	The current game simulation time
//----------------------------------------------------------------------------
void CStartSplashState::OnUpdate(float fCurrentTime)
{
	CGameUIState::OnUpdate(fCurrentTime);

	// After (arbitrarily) 1 second, transition to the next state. Normally, other things
	// would be going on, such as loading assets, that would trigger the transition.
	// This artificial timing is simply illustrative
	if ( fCurrentTime >= 3.0)
	{
		CStateManager &rStateManager = CGameApp::mp_Appication->GetStateManager();
		rStateManager.ChangeState("MainMenu");
	}
}