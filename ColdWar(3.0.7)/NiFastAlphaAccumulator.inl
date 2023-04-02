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

#include <NiAlphaProperty.h>
#include <NiCamera.h>
#include <NiGeometry.h>
#include <NiPropertyState.h>

//---------------------------------------------------------------------------
inline NiFastAlphaAccumulator::NiFastAlphaAccumulator()
{
    /* */
}
//---------------------------------------------------------------------------
inline NiFastAlphaAccumulator::~NiFastAlphaAccumulator()
{
    /* */
}
//---------------------------------------------------------------------------
inline void NiFastAlphaAccumulator::FinishAccumulating()
{
    if (!m_pkCamera)
        return;

    while (m_kItems.GetSize())
    {
        NiGeometry* pkGeom = m_kItems.RemoveHead();
        assert(pkGeom);
        pkGeom->RenderImmediate(NiRenderer::GetRenderer());
    }

    while (m_kNoTestItems.GetSize())
    {
        NiGeometry* pkGeom = m_kNoTestItems.RemoveHead();
        assert(pkGeom);
        pkGeom->RenderImmediate(NiRenderer::GetRenderer());
    }

    NiAccumulator::FinishAccumulating();
}
//---------------------------------------------------------------------------
inline void NiFastAlphaAccumulator::RegisterObjectArray(
    NiVisibleArray& kArray)
{
    assert(m_pkCamera);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

    const unsigned int uiQuantity = kArray.GetCount();
    for (unsigned int i = 0; i < uiQuantity; i++)
    {
        NiGeometry& kObject = kArray.GetAt(i);

        const NiAlphaProperty* pkAlpha = 
            kObject.GetPropertyState()->GetAlpha();

        // If alpha blended, then add to the list - otherwise,
        // draw immediately
        if (pkAlpha->GetAlphaBlending() && (pkAlpha->GetTestRef() != 255))
        {
            if (pkAlpha->GetAlphaTesting())
                m_kItems.AddTail(&kObject);
            else
                m_kNoTestItems.AddTail(&kObject);
        }
        else
        {
            kObject.RenderImmediate(pkRenderer);
        }
    }
}
//---------------------------------------------------------------------------

