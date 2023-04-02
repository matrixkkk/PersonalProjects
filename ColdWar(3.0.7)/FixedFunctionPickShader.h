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

#ifndef FIXEDFUNCTIONPICKSHADER_H
#define FIXEDFUNCTIONPICKSHADER_H

#if defined(WIN32)
#include <NiDirectXVersion.h>
#endif  //#if defined(WIN32)

#include <NiD3DRendererHeaders.h>
#include <NiD3DShader.h>

class ColorPicker;

class FixedFunctionPickShader : public NiD3DShader
{
public:
    FixedFunctionPickShader(ColorPicker* pkPicker);
    virtual ~FixedFunctionPickShader();

    // Override these functions to implement a custom pipeline...
    virtual unsigned int SetupShaderPrograms(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryData::RendererData* pkRendererData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, const NiBound& kWorldBound);

protected:
    bool CreateStagesAndPasses();
    bool PresetStages(NiD3DTextureStage* pkStage);

    ColorPicker* m_pkColorPicker;
};

typedef NiPointer<FixedFunctionPickShader> FixedFunctionPickShaderPtr;

#endif  //#ifndef FIXEDFUNCTIONPICKSHADER_H
