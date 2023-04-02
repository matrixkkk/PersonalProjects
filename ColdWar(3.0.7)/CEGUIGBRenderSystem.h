// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement
// with Emergent Game Technologies and may not be copied or disclosed 
// except in accordance with the terms of that agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef CEGUIGBRENDERSYSTEM_H
#define CEGUIGBRENDERSYSTEM_H

#include <NiMain.h>
#include <NiInputSystem.h>

#pragma warning (disable:4530) // disable warning about not using exceptions
#include <CEGUI.h>
#include "CEGUIGBRenderer.h"

class CEGUIGBRenderSystem :  public NiRefObject
{
public:
    CEGUIGBRenderSystem();
	~CEGUIGBRenderSystem();

	void PerformRendering();	

protected:

	virtual void ProcessInput();

	void FillMapping();

protected:

	CEGUIGBRenderer* m_pkCEGUIRenderer;

	unsigned char	m_ucPreviouslyPressedKey;
	float m_fTimeSinceEmitKey;
	float m_fKeyRepeatDelay;
	float m_fKeyRepeatRate;

	unsigned char m_aucNIIToDIMapping[NiInputKeyboard::KEY_TOTAL_COUNT];

};

NiSmartPointer(CEGUIGBRenderSystem);

#endif