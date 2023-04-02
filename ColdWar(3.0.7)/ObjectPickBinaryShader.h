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

#ifndef OBJECTPICKBINARYSHADER_H
#define OBJECTPICKBINARYSHADER_H

#if defined(WIN32)
#include <NiDirectXVersion.h>
#endif  //#if defined(WIN32)

#include <NiBinaryShader.h>

class ColorPicker;

class ObjectPickBinaryShader : public NiBinaryShader
{
    NiDeclareRTTI;
public:
    ObjectPickBinaryShader();
    virtual ~ObjectPickBinaryShader();

    // Overridden function
    virtual unsigned int SetupShaderPrograms(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryData::RendererData* pkRendererData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, const NiBound& kWorldBound);

    void SetColorPicker(ColorPicker* pkPicker);

protected:
    ColorPicker* m_pkColorPicker;
};

NiSmartPointer(ObjectPickBinaryShader);

#endif  //OBJECTPICKBINARYSHADER_H
