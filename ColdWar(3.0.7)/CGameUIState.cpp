
//----------------------------------------------------------------------------

#include "CGameUIState.h"
//#include "GameObject.h"
#include <NiRenderer.h>



//----------------------------------------------------------------------------
//  GameUIState::GameUIState
//
/// @param pcGUIImageSetFilename  The name of the CEGUI *.imageset filename containing
///							      the GUI elements
/// @param pcGUILayoutFilename    The name of the CEGUI *.layout filename containing the
///                               state's GUI layout
///
/// Constructor
//----------------------------------------------------------------------------
CGameUIState::CGameUIState(const char *pcGUIImageSetFilename, const char *pcGUILayoutFilename) : CGameBaseState(),
	m_pGUI(0), m_ImageSetFilename(pcGUIImageSetFilename), m_LayoutFilename(pcGUILayoutFilename)
{
}

void CGameUIState::OnEnter(const char* szPrevious)
{
	if (0 == m_pGUI)
		Initialize();

	if (m_pGUI)
	{
		// Find the GUI root window
		CEGUI::Window *pRootWindow = CEGUI::System::getSingleton().getGUISheet();
		if (pRootWindow)
		{
			// Add our own GUI as a child of the root window
			pRootWindow->addChildWindow(m_pGUI);
		}
	}
}


void CGameUIState::OnLeave(const char* szNext)
{
	// We should not be able to reach this point without having initialized
	// the state
	if (0 == m_pGUI)
		return;

	// Find the GUI root window
	CEGUI::Window *pRootWindow = CEGUI::System::getSingleton().getGUISheet();
	if (pRootWindow)
	{
		// Remove our child window from the root
		pRootWindow->removeChildWindow(m_pGUI);
	}

	// Free resources
	CEGUI::WindowManager::getSingleton().destroyWindow(m_pGUI);
	m_pGUI = 0;

	if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(String(m_ImageSetName)))
		CEGUI::ImagesetManager::getSingleton().destroyImageset(String(m_ImageSetName));
}


bool CGameUIState::Initialize()
{
	if (m_pGUI != 0)
		return(false);

	// Load the GUI elements for the state's GUI
	CEGUI::Imageset *pIS = CEGUI::ImagesetManager::getSingleton().createImageset(String(m_ImageSetFilename));

	if (0 == pIS)
		return(false);

	m_ImageSetName = pIS->getName().c_str();

	m_pGUI = CEGUI::WindowManager::getSingleton().loadWindowLayout(String(m_LayoutFilename));

	if (!m_pGUI)
		return(false);

	return(true);
}

void CGameUIState::OnRender()
{
}

void CGameUIState::OnProcessInput()
{

}