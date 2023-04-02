// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef OBJECTPICKMATERIAL_H
#define OBJECTPICKMATERIAL_H

#include <NiShader.h>
#include <NiMaterial.h>

class ColorPicker;

class ObjectPickMaterial : public NiMaterial
{
    NiDeclareRTTI;
public:
    static ObjectPickMaterial* Create(ColorPicker* pkPicker);
    virtual ~ObjectPickMaterial();

    virtual bool IsShaderCurrent(NiShader* pkShader, 
        const NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual NiShader* GetCurrentShader(const NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual void UnloadShaders();

protected:
    ObjectPickMaterial(ColorPicker* pkPicker);
    void LoadShaders();

    NiShaderPtr m_spFixedFunctionShader;
    NiShaderPtr m_spNonSkinnedShader;
    NiShaderPtr m_spSkinnedShader;

    ColorPicker* m_pkColorPicker;

    bool m_bShadersLoaded;
    bool m_bSM20Supported;
};

#endif // OBJECTPICKMATERIAL_H
