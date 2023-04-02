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

#include "FixedFunctionPickShader.h"
#include "ColorPicker.h"

//---------------------------------------------------------------------------
FixedFunctionPickShader::FixedFunctionPickShader(ColorPicker* pkPicker) :
    m_pkColorPicker(pkPicker)
{
    //  Create the local stages and passes we will use...
    CreateStagesAndPasses();

    m_pkRenderStateGroup = NiD3DRenderStateGroup::GetFreeRenderStateGroup();
#if !defined(_XENON)
    m_pkRenderStateGroup->SetRenderState(D3DRS_LIGHTING, FALSE, true);
#endif
    m_pkRenderStateGroup->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, true);
    m_pkRenderStateGroup->SetRenderState(D3DRS_ZWRITEENABLE, TRUE, true);
    m_pkRenderStateGroup->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE, true);

    SetName("FixedFunctionPickShader");

    // This is the best (and only) implementation of this shader
    m_bIsBestImplementation = true;
}
//---------------------------------------------------------------------------
FixedFunctionPickShader::~FixedFunctionPickShader()
{
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int FixedFunctionPickShader::SetupShaderPrograms(
    NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiD3DShader::SetupShaderPrograms(pkGeometry, pkSkin, pkPartition, 
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);

    //  Determine a color for the object and assign it
    unsigned int uiColor = 
        m_pkColorPicker->GetColorForObject(pkGeometry);

    // Set up render state with appropriate color
    m_pkD3DRenderState->SetRenderState(D3DRS_TEXTUREFACTOR, uiColor);

    return 0;
}
//---------------------------------------------------------------------------
bool FixedFunctionPickShader::CreateStagesAndPasses()
{
    NiD3DPassPtr spPass = NiD3DPass::CreateNewPass();
    assert(spPass);

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    assert(spStage);

    //  Preset the stages
    PresetStages(spStage);

    spPass->AppendStage(spStage);
    m_kPasses.SetAt(0, spPass);
    m_uiPassCount = 1;

    return true;
}
//---------------------------------------------------------------------------
bool FixedFunctionPickShader::PresetStages(NiD3DTextureStage* pkStage)
{
    pkStage->SetStage(0);
    pkStage->SetTexture(0);
    pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TFACTOR);
    pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
    pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 0);
    
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSU, 
        D3DTADDRESS_WRAP);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSV, 
        D3DTADDRESS_WRAP);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSW, 
        D3DTADDRESS_WRAP);
    
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MAGFILTER, 
        D3DTEXF_LINEAR);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MINFILTER, 
        D3DTEXF_LINEAR);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MIPFILTER, 
        D3DTEXF_NONE);

    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    return true;
}
//---------------------------------------------------------------------------
