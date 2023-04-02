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

#include "ObjectPickBinaryShader.h"
#include "ColorPicker.h"

NiImplementRTTI(ObjectPickBinaryShader, NiBinaryShader);

//---------------------------------------------------------------------------
ObjectPickBinaryShader::ObjectPickBinaryShader() :
    NiBinaryShader(),
    m_pkColorPicker(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
ObjectPickBinaryShader::~ObjectPickBinaryShader()
{
    /* */
}
//---------------------------------------------------------------------------
unsigned int ObjectPickBinaryShader::SetupShaderPrograms(
    NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{

    //  Determine a color for the object and assign it
    unsigned int uiColor = 
        m_pkColorPicker->GetColorForObject(pkGeometry);

    unsigned char ucAlpha = (uiColor >> 24) & 0xff;
    unsigned char ucRed   = (uiColor >> 16) & 0xff;
    unsigned char ucGreen = (uiColor >>  8) & 0xff;
    unsigned char ucBlue  = (uiColor >>  0) & 0xff;

    float afPickColor[4] = 
    { 
        (float)ucRed / 255.0f + (1.0f / 511.0f), 
        (float)ucGreen / 255.0f + (1.0f / 511.0f), 
        (float)ucBlue / 255.0f + (1.0f / 511.0f), 
        1.0f 
    };
    
    NiD3DShaderFactory::UpdateGlobalShaderConstant("PickColor",
        sizeof(afPickColor), &afPickColor);

    return NiBinaryShader::SetupShaderPrograms(pkGeometry, pkSkin, pkPartition,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
void ObjectPickBinaryShader::SetColorPicker(ColorPicker* pkPicker)
{
    m_pkColorPicker = pkPicker;
}
//---------------------------------------------------------------------------
