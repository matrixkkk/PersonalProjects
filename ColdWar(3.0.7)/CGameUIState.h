#ifndef CGAMEUISTATE_H
#define CGAMEUISTATE_H

#include "CGameBaseState.h"
#include <CEGUI.h>
#include "CEGUIGBRenderSystem.h"

using namespace CEGUI;



class CGameUIState : public CGameBaseState
{

public:

	CGameUIState(const char *pcGUIImageSetFilename = 0, const char *pcGUILayoutFilename = 0);

	virtual void OnEnter(const char* szPrevious);
	virtual void OnLeave(const char* szNext);
	virtual void OnProcessInput();
	virtual void OnRender();

protected:

	virtual bool Initialize();

protected:

	Window *m_pGUI;							//최상위 윈도우
	NiString m_ImageSetFilename;			//Image Set FileName
	NiString m_LayoutFilename;				//LayOut

	NiString m_ImageSetName;				//Image Set Name

};

#endif