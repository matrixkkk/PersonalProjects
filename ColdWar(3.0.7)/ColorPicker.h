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

#ifndef COLORPICKER_H
#define COLORPICKER_H


#include <NiDirectXVersion.h>
#include <NiD3DRendererHeaders.h>

#include <NiMain.h>
#include <NiCursor.h>

//#define _LIB
//#define _DX9


#include <NiDX9RenderedTextureData.h>
typedef NiDX9RenderedTextureData NiD3DRenderedTextureData;

class ColorPicker : public NiMemObject
{
public:
	ColorPicker() {}
    ColorPicker(NiRenderer* pkRenderer, unsigned int uiScreenW, 
        unsigned int uiScreenH);
    ~ColorPicker();

    void StartPick(NiCamera* pkCamera, unsigned int uiPickX, 
        unsigned int uiPickY);
    void RecursivePickRender(NiAVObject* pkObject);
    const NiAVObject* EndPick();

    void ToggleZBuffer();

    NiRenderedTexture* GetRenderTexture()
    {
        return m_spRenderTexture;
    }

    // This function should only be accessed by the shaders that use the 
    // ColorPicker.
    unsigned int GetColorForObject(const NiGeometry* pkGeometry);

protected:
    void CreateRenderTarget();
    void CreatePropertyAndEffectStates();
    void CreateMaterial();

    // Functions for handling device reset
    static bool ResetNotificationFunction(bool bBeforeReset, void* pvData);
    bool HandleReset(bool bBeforeReset);


    NiRendererPtr m_spRenderer;
    D3DDevicePtr m_pkD3DDevice;

    // Width/Height parameters
    static unsigned int ms_uiDefaultPickW;
    static unsigned int ms_uiDefaultPickH;

    unsigned int m_uiScreenWidth;
    unsigned int m_uiScreenHeight;

    NiCameraPtr m_spCamera;

    // Viewport rect
    float m_fScaleHorz;
    float m_fScaleVert;
    NiFrustum m_kSaveFrustum;

    // The target rendering surface...
    D3DFORMAT m_eFormat;
    NiRenderedTexturePtr m_spRenderTexture;
    NiRenderTargetGroupPtr m_spRenderTargetGroup;
    D3DTexturePtr m_pkD3DTexture;
    D3DTexturePtr m_pkD3DReadTexture;

    // PropertyState for Z buffering
    NiPropertyStatePtr m_spPropertyState;

    // Shader for setting up the pick rendering
    NiMaterialPtr m_spObjPickMaterial;

    // Lists of objects to be rendered
    NiTList<NiGeometry*> m_kNonSkinnedGeometry;
    NiTList<NiGeometry*> m_kSkinnedGeometry;

    // ObjectPickData - tracks the color-geometry matches
    class ObjectPickData : public NiMemObject
    {
    public:
        const NiGeometry* m_pkGeometry;

        ObjectPickData* m_pkNext;

        ObjectPickData()
        {
            m_pkGeometry = 0;
            m_pkNext = 0;
        }
    };

    ObjectPickData* GetPickData(unsigned int uiColor);

    //  Functions for accessing the color/object mapping
    unsigned int DetermineNextAvailableColor();
    void ResetColorObjectMap();
    void AllocateObjectPickDataBlock();
    void FreeObjectPickData();

    // Color incrementing...
    static unsigned int ms_uiColorIncrement;
    unsigned int m_uiCurrentColor;

    // Current geometry being rendered
    static unsigned int ms_uiPickDataAllocBlockSize;
    ObjectPickData* m_pkObjectPickDataFreePool;

    NiTMap<unsigned int, ObjectPickData*>* m_pkColorObjectMap;
};

typedef NiPointer<ColorPicker> ColorPickerPtr;

#endif  //#ifndef COLORPICKER_H
