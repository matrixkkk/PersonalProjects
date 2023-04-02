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

#include "ColorPicker.h"

#include <NiD3DRendererHeaders.h>
#include <NiD3DShaderFactory.h>

#include "ObjectPickMaterial.h"

unsigned int ColorPicker::ms_uiDefaultPickW = 16;
unsigned int ColorPicker::ms_uiDefaultPickH = 16;

unsigned int ColorPicker::ms_uiColorIncrement = 0x00000002;
unsigned int ColorPicker::ms_uiPickDataAllocBlockSize = 16;

//---------------------------------------------------------------------------
ColorPicker::ColorPicker(NiRenderer* pkRenderer, unsigned int uiScreenW, 
    unsigned int uiScreenH) : //NiMemObject(),
    m_uiScreenWidth(uiScreenW),
    m_uiScreenHeight(uiScreenH),
    m_eFormat(D3DFMT_UNKNOWN),
    m_pkObjectPickDataFreePool(NULL)
{
    m_spRenderer = pkRenderer;

    NiD3DRenderer* pkD3DRenderer = 
        NiSmartPointerCast(NiD3DRenderer, m_spRenderer);
    m_pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    assert(m_pkD3DDevice);
    D3D_POINTER_REFERENCE(m_pkD3DDevice);

    // Set reset notification function

    pkD3DRenderer->AddResetNotificationFunc(
        ColorPicker::ResetNotificationFunction, this);

    CreateRenderTarget();
    CreatePropertyAndEffectStates();
    CreateMaterial();

    // Setup the geometry array
    m_pkColorObjectMap = NiNew NiTMap<unsigned int, ObjectPickData*>;
    assert(m_pkColorObjectMap);
}
//---------------------------------------------------------------------------
ColorPicker::~ColorPicker()
{
    FreeObjectPickData();

    if (m_pkD3DReadTexture)
        D3D_POINTER_RELEASE(m_pkD3DReadTexture);
    D3D_POINTER_RELEASE(m_pkD3DDevice);

    m_pkD3DTexture = 0;
    m_pkD3DReadTexture = 0;
    m_pkD3DDevice = 0;
}
//---------------------------------------------------------------------------
void ColorPicker::StartPick(NiCamera* pkCamera, unsigned int uiPickX, 
    unsigned int uiPickY)
{
    m_uiCurrentColor = 0;

    assert (m_kNonSkinnedGeometry.GetSize() == 0);
    assert (m_kSkinnedGeometry.GetSize() == 0);

    m_spCamera = pkCamera;

    m_kSaveFrustum = pkCamera->GetViewFrustum();
    NiFrustum kNewFrustum;

    m_fScaleHorz = (m_kSaveFrustum.m_fRight - m_kSaveFrustum.m_fLeft) / 
        m_uiScreenWidth;
    m_fScaleVert = (m_kSaveFrustum.m_fTop - m_kSaveFrustum.m_fBottom) / 
        m_uiScreenHeight;

    kNewFrustum.m_fNear = m_kSaveFrustum.m_fNear;
    kNewFrustum.m_fFar = m_kSaveFrustum.m_fFar;

    float fCenterH = m_fScaleHorz * uiPickX;
    float fCenterV = m_fScaleVert * uiPickY;
    float fOffsetH = m_fScaleHorz * (ms_uiDefaultPickW / 2);
    float fOffsetV = m_fScaleVert * (ms_uiDefaultPickH / 2);

    kNewFrustum.m_fLeft = m_kSaveFrustum.m_fLeft + (fCenterH - fOffsetH);
    kNewFrustum.m_fRight = kNewFrustum.m_fLeft + (fOffsetH * 2);
    kNewFrustum.m_fTop = m_kSaveFrustum.m_fTop - (fCenterV - fOffsetV);
    kNewFrustum.m_fBottom = kNewFrustum.m_fTop - (fOffsetV * 2);

    m_spCamera->SetViewFrustum(kNewFrustum);
    m_spCamera->Update(0.0f);

    ResetColorObjectMap();

    // Open the render target for geometry rendering (clear in the process)
    m_spRenderer->BeginUsingRenderTargetGroup(
        m_spRenderTargetGroup, NiRenderer::CLEAR_ALL);

    //  Set the camera data
    m_spRenderer->SetCameraData(m_spCamera);
}
//---------------------------------------------------------------------------
void ColorPicker::RecursivePickRender(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiTriShape, pkObject))
    {
        NiTriShape* pkTriShape = (NiTriShape*)pkObject;
        NiTriShapeData* pkData = (NiTriShapeData*)pkTriShape->GetModelData();
        if (pkTriShape->GetSkinInstance())
            m_kSkinnedGeometry.AddTail(pkTriShape);
        else
            m_kNonSkinnedGeometry.AddTail(pkTriShape);
    }
    else if (NiIsKindOf(NiTriStrips, pkObject))
    {
        NiTriStrips* pkTriStrip = (NiTriStrips*)pkObject;
        NiTriStripsData* pkData = (NiTriStripsData*)pkTriStrip->GetModelData();
        if (pkTriStrip->GetSkinInstance())
            m_kSkinnedGeometry.AddTail(pkTriStrip);
        else
            m_kNonSkinnedGeometry.AddTail(pkTriStrip);
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        unsigned int uiNumChildren = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiNumChildren; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild && (!pkChild->GetAppCulled()))
                RecursivePickRender(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
const NiAVObject* ColorPicker::EndPick()
{
    const NiAVObject* pkPicked = NULL;

    // Do the skinned objects first
    NiGeometry* pkFirstObject = NULL;
    const NiMaterial* pkFirstMaterial = NULL;

    // Start the batch
    m_spRenderer->BeginBatch(m_spPropertyState, 0);
    NiTListIterator kIter = m_kSkinnedGeometry.GetHeadPos();
    while (kIter)
    {
        NiGeometry* pkObject = m_kSkinnedGeometry.GetNext(kIter);
        assert (pkObject);
        if (pkFirstObject == NULL)
        {
            // Replace the first object's material
            pkFirstObject = pkObject;
            pkFirstMaterial = pkObject->GetActiveMaterial();
            pkFirstObject->ApplyAndSetActiveMaterial(m_spObjPickMaterial);
        }
        if (NiIsKindOf(NiTriShape, pkObject))
            m_spRenderer->BatchRenderShape((NiTriShape*)pkObject);
        else
            m_spRenderer->BatchRenderStrips((NiTriStrips*)pkObject);
    }
    m_kSkinnedGeometry.RemoveAll();

    //  End the batch
    m_spRenderer->EndBatch();

    // Replace the first object's material
    if (pkFirstObject)
        pkFirstObject->SetActiveMaterial(pkFirstMaterial);

    // Now do the nonskinned objects
    pkFirstObject = NULL;
    pkFirstMaterial = NULL;

    // Start the batch
    m_spRenderer->BeginBatch(m_spPropertyState, 0);
    kIter = m_kNonSkinnedGeometry.GetHeadPos();
    while (kIter)
    {
        NiGeometry* pkObject = m_kNonSkinnedGeometry.GetNext(kIter);
        assert (pkObject);
        if (pkFirstObject == NULL)
        {
            // Replace the first object's material
            pkFirstObject = pkObject;
            pkFirstMaterial = pkObject->GetActiveMaterial();
            pkFirstObject->ApplyAndSetActiveMaterial(m_spObjPickMaterial);
        }
        if (NiIsKindOf(NiTriShape, pkObject))
            m_spRenderer->BatchRenderShape((NiTriShape*)pkObject);
        else
            m_spRenderer->BatchRenderStrips((NiTriStrips*)pkObject);
    }
    m_kNonSkinnedGeometry.RemoveAll();

    //  End the batch
    m_spRenderer->EndBatch();

    // Replace the first object's material
    if (pkFirstObject)
        pkFirstObject->SetActiveMaterial(pkFirstMaterial);

    // Close the render target to allow rendering to complete
    m_spRenderer->EndUsingRenderTargetGroup();

    //  Now, see what color at the point of the pixel
    D3DLOCKED_RECT kLockRect;
    RECT kCheckRect;

    kCheckRect.left = ms_uiDefaultPickW / 2;
    kCheckRect.top = ms_uiDefaultPickH / 2;
    kCheckRect.right = kCheckRect.left + 1;
    kCheckRect.bottom = kCheckRect.top + 1;

    //  We have to copy the rendered texture to the 'read' texture
    D3DSurfacePtr pkSrc, pkDst;

    m_pkD3DTexture->GetSurfaceLevel(0, &pkSrc);
    m_pkD3DReadTexture->GetSurfaceLevel(0, &pkDst);

    D3DTexturePtr pkLockTexture = NULL;


    m_pkD3DDevice->GetRenderTargetData(pkSrc, pkDst);

    pkSrc->Release();
    pkDst->Release();


    pkLockTexture = m_pkD3DReadTexture;
    if (SUCCEEDED(pkLockTexture->LockRect(0, &kLockRect, &kCheckRect, 
        D3DLOCK_READONLY)))
    {
        //  Find the pixel...
        unsigned int uiColor = 0x00000000;

        // Only support 32-bit rendered texture formats.
        switch (m_eFormat)
        {
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
			uiColor = *((unsigned int*)(kLockRect.pBits));

            break;
        default:
            assert(0);
            break;
        }

        char acTemp[1024];

        NiSprintf(acTemp, 1024, "Color Picked = 0x%08x\n", uiColor);
        NiOutputDebugString(acTemp);

        pkLockTexture->UnlockRect(0);

        // Use the color to look up the picked object.
        ObjectPickData* pkPickData;

        pkPickData = GetPickData(uiColor);
        if (pkPickData && pkPickData->m_pkGeometry)
        {
            NiSprintf(acTemp, 1024, "Object Picked      = %s\n",
                pkPickData->m_pkGeometry->GetName());
            NiOutputDebugString(acTemp);

            const NiGeometry* pkGeometry = pkPickData->m_pkGeometry;
            NiAVObject* pkParent = ((NiAVObject*)(pkGeometry))->GetParent();
            pkPicked = pkGeometry;
            if (pkParent)
            {
                NiSprintf(acTemp, 1024, "Object Picked (P)  = %s\n",
                    pkParent->GetName());
                NiOutputDebugString(acTemp);

                NiAVObject* pkGrandParent = pkParent->GetParent();
                if (pkGrandParent)
                {
                    NiSprintf(acTemp, 1024, "Object Picked (GP) = %s\n",
                        pkGrandParent->GetName());
                    NiOutputDebugString(acTemp);
                }
            }
        }
    }

    //  Restore the camera
    m_spCamera->SetViewFrustum(m_kSaveFrustum);
    m_spCamera->Update(0.0f);
    m_spCamera = 0;

    return pkPicked;
}
//---------------------------------------------------------------------------
void ColorPicker::CreateRenderTarget()
{
    m_spRenderTexture = NiRenderedTexture::Create(ms_uiDefaultPickW,
        ms_uiDefaultPickH, m_spRenderer);
    assert(m_spRenderTexture);

    m_spRenderTargetGroup = NiRenderTargetGroup::Create(
        m_spRenderTexture->GetBuffer(),
        m_spRenderer, true, true);

    NiD3DRenderedTextureData* pkRenderData = 
        (NiD3DRenderedTextureData*)m_spRenderTexture->GetRendererData();

    m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());

    D3DSURFACE_DESC kSurfDesc;

    m_pkD3DTexture->GetLevelDesc(0, &kSurfDesc);

    //  We also need to create a texture that we can lock and read for 
    //  getting the pixel color out...
    m_eFormat = kSurfDesc.Format;
    m_pkD3DDevice->CreateTexture(kSurfDesc.Width, kSurfDesc.Height,
        1, 0, kSurfDesc.Format, D3DPOOL_SYSTEMMEM, &m_pkD3DReadTexture, 0);

    assert(m_pkD3DReadTexture);
}
//---------------------------------------------------------------------------
void ColorPicker::CreatePropertyAndEffectStates()
{
    m_spPropertyState = NiNew NiPropertyState();
    assert(m_spPropertyState);

    NiZBufferProperty* pkZBuffer = NiNew NiZBufferProperty();
    assert(pkZBuffer);
    pkZBuffer->SetZBufferTest(true);
    pkZBuffer->SetZBufferWrite(true);
    m_spPropertyState->SetProperty(pkZBuffer);

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty;
    m_spPropertyState->SetProperty(pkTexProp);
}
//---------------------------------------------------------------------------
void ColorPicker::CreateMaterial()
{
    m_spObjPickMaterial = ObjectPickMaterial::Create(this);
}
//---------------------------------------------------------------------------
bool ColorPicker::ResetNotificationFunction(bool bBeforeReset, 
    void* pvData)
{
    ColorPicker* pkColorPicker = (ColorPicker*)pvData;
    return pkColorPicker->HandleReset(bBeforeReset);
}
//---------------------------------------------------------------------------
bool ColorPicker::HandleReset(bool bBeforeReset)
{
    // Only need to handle what happens after the device is reset
    if (!bBeforeReset)
    {
        // Re-grab cached D3D texture because it has been released and
        // recreated as a result of the device reset
        NiD3DRenderedTextureData* pkRenderData = 
            (NiD3DRenderedTextureData*)m_spRenderTexture->GetRendererData();
        m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());
    }

    return true;
}
//---------------------------------------------------------------------------
ColorPicker::ObjectPickData* ColorPicker::GetPickData(unsigned int uiColor)
{
    assert(m_pkColorObjectMap);

    ObjectPickData* pkPickData;

    // This is to eliminate issues with the alpha value read from the
    // back buffer. On some cards, it will be 0, on others 0xff...
    uiColor |= 0xff000000;
    if (m_pkColorObjectMap->GetAt(uiColor, pkPickData))
        return pkPickData;

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int ColorPicker::DetermineNextAvailableColor()
{
    m_uiCurrentColor += ms_uiColorIncrement;

    return m_uiCurrentColor;
}
//---------------------------------------------------------------------------
void ColorPicker::ResetColorObjectMap()
{
    ObjectPickData* pkPickData;

    //  Empty the map and put the entries on the free list
    unsigned int uiColor;
    NiTMapIterator pos = m_pkColorObjectMap->GetFirstPos();
    while (pos)
    {
        m_pkColorObjectMap->GetNext(pos, uiColor, pkPickData);
        if (pkPickData)
        {
            pkPickData->m_pkNext = m_pkObjectPickDataFreePool;
            m_pkObjectPickDataFreePool = pkPickData;

            pkPickData->m_pkGeometry = 0;
        }
    }
    m_pkColorObjectMap->RemoveAll();
}
//---------------------------------------------------------------------------
void ColorPicker::AllocateObjectPickDataBlock()
{
    ObjectPickData* pkPickData;

    for (unsigned int ui = 0; ui < ms_uiPickDataAllocBlockSize; ui++)
    {
        pkPickData = NiNew ObjectPickData();

        pkPickData->m_pkNext = m_pkObjectPickDataFreePool;
        m_pkObjectPickDataFreePool = pkPickData;
    }
}
//---------------------------------------------------------------------------
void ColorPicker::FreeObjectPickData()
{
    //  Empty the free list
    ObjectPickData* pkPickData = m_pkObjectPickDataFreePool;
    ObjectPickData* pkNextPickData;
    while(pkPickData)
    {
        pkNextPickData = pkPickData->m_pkNext;
        NiDelete pkPickData;
        pkPickData = pkNextPickData;
    }
    m_pkObjectPickDataFreePool = 0;

    //  Empty and delete the map
    unsigned int uiColor;
    NiTMapIterator pos = m_pkColorObjectMap->GetFirstPos();
    while (pos)
    {
        m_pkColorObjectMap->GetNext(pos, uiColor, pkPickData);
        if (pkPickData)
            NiDelete pkPickData;
    }
    m_pkColorObjectMap->RemoveAll();
    NiDelete m_pkColorObjectMap;
    m_pkColorObjectMap = 0;
}
//---------------------------------------------------------------------------
unsigned int ColorPicker::GetColorForObject(const NiGeometry* pkGeometry)
{
    unsigned int uiColor = DetermineNextAvailableColor();

    if (!m_pkObjectPickDataFreePool)
        AllocateObjectPickDataBlock();
    assert(m_pkObjectPickDataFreePool);

    ObjectPickData* pkPickData = m_pkObjectPickDataFreePool;
    if (m_pkObjectPickDataFreePool->m_pkNext)
        m_pkObjectPickDataFreePool = m_pkObjectPickDataFreePool->m_pkNext;
    else
        m_pkObjectPickDataFreePool = 0;

    NiSkinInstance* pkSkin = pkGeometry->GetSkinInstance(); 

    pkPickData->m_pkNext = 0;
    pkPickData->m_pkGeometry = pkGeometry;

    // This is to eliminate issues with the alpha value read from the
    // back buffer. On some cards, it will be 0, on others 0xff...
    uiColor |= 0xff000000;
    m_pkColorObjectMap->SetAt(uiColor, pkPickData);

    return uiColor;
}
//---------------------------------------------------------------------------
