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

#include "ObjectPickMaterial.h"
#if !defined(_XENON)
#include "FixedFunctionPickShader.h"
#endif
#include "ObjectPickBinaryShader.h"
#include <NiD3DShaderFactory.h>

NiImplementRTTI(ObjectPickMaterial, NiMaterial);

//---------------------------------------z------------------------------------
ObjectPickMaterial::ObjectPickMaterial(ColorPicker* pkPicker) :
    NiMaterial("ObjectPickMaterial"),
    m_pkColorPicker(pkPicker),
    m_bShadersLoaded(false),
    m_bSM20Supported(false)
{
#if defined(WIN32)

    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NiD3DShaderFactory* pkFactory = (NiD3DShaderFactory*)NiShaderFactory::GetInstance();
   
	m_bSM20Supported = true;
/*	m_bSM20Supported = (

		(pkRenderer->GetPixelShaderVersion() >= pkFactory->CreatePixelShaderVersion(2, 0)) &&
        (pkRenderer->GetVertexShaderVersion() >= pkFactory->CreateVertexShaderVersion(2, 0))
		
		);
*/
#elif defined(_XENON)
    m_bSM20Supported = true;
#elif defined(_PS3)
    m_bSM20Supported = true;
#else
#error "Unsupported renderer!"
#endif

    LoadShaders();
}
//---------------------------------------------------------------------------
ObjectPickMaterial::~ObjectPickMaterial()
{
    /* */
}
//---------------------------------------------------------------------------
ObjectPickMaterial* ObjectPickMaterial::Create(ColorPicker* pkPicker)
{
    NiMaterial* pkMaterial = GetMaterial("ObjectPickMaterial");
    if (pkMaterial)
    {
        assert (NiIsKindOf(ObjectPickMaterial, pkMaterial));
        return (ObjectPickMaterial*)pkMaterial;
    }
    else
    {
        return NiNew ObjectPickMaterial(pkPicker);
    }
}
//---------------------------------------------------------------------------
bool ObjectPickMaterial::IsShaderCurrent(NiShader* pkShader, 
    const NiGeometry* pkGeom, const NiSkinInstance* pkSkin, 
    const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState, 
    unsigned int uiMaterialExtraData)
{
    // This is such a simple determination that we might as well do the whole
    // thing each frame
    return (GetCurrentShader(pkGeom, pkSkin, pkPropState, 
        pkEffectState, uiMaterialExtraData) == pkShader);
}
//---------------------------------------------------------------------------
NiShader* ObjectPickMaterial::GetCurrentShader(const NiGeometry* pkGeom, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState,
    unsigned int uiMaterialExtraData)
{
    if (!m_bShadersLoaded)
    {
        LoadShaders();
        if (!m_bShadersLoaded)
        {
            // Shader failed to load!
            return NULL;
        }
    }
    if (m_bSM20Supported)
    {
        if (pkSkin)
            return m_spSkinnedShader;
        else
            return m_spNonSkinnedShader;
    }
    else
    {
        return m_spFixedFunctionShader;
    }
}
//---------------------------------------------------------------------------
void ObjectPickMaterial::UnloadShaders()
{
    m_spFixedFunctionShader = NULL;
    m_spNonSkinnedShader = NULL;
    m_spSkinnedShader = NULL;

    m_bShadersLoaded = false;
}
//---------------------------------------------------------------------------
void ObjectPickMaterial::LoadShaders()
{
    if (!m_bShadersLoaded)
    {
        if (m_bSM20Supported)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();

            ObjectPickBinaryShaderPtr spNonSkinnedShader = (ObjectPickBinaryShader*)pkFactory->RetrieveShader("ObjectPickNonSkinnedShader", 0);
            ObjectPickBinaryShaderPtr spSkinnedShader = (ObjectPickBinaryShader*)pkFactory->RetrieveShader("ObjectPickSkinnedShader", 0);

            if (spNonSkinnedShader && spSkinnedShader)
            {
                spNonSkinnedShader->SetColorPicker(m_pkColorPicker);
                spSkinnedShader->SetColorPicker(m_pkColorPicker);
                m_spNonSkinnedShader = spNonSkinnedShader;
                m_spSkinnedShader = spSkinnedShader;
                m_spNonSkinnedShader->Initialize();
                m_spSkinnedShader->Initialize();
                m_bShadersLoaded = true;
            }
        }
        else
        {
#if !defined(_XENON)
            m_spFixedFunctionShader = NiNew FixedFunctionPickShader(
                m_pkColorPicker);
            if (m_spFixedFunctionShader)
            {
                m_spFixedFunctionShader->Initialize();
                m_bShadersLoaded = true;
            }
#else
            assert (false && "Xenon should always support SM2.0");
            m_spFixedFunctionShader = NULL;
#endif
        }
    }
}
//---------------------------------------------------------------------------
