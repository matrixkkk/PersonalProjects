#ifndef NIDX9RENDEREREX_H
#define NIDX9RENDEREREX_H

#include "NiDX9Renderer.h"
#include "NiGeometryGroup.h"
#include "NiD3DGeometryGroupManager.h"

class NiGeometryGroup;
class NiD3DGeometryGroupManager;

class NiDX9RendererEx : public NiDX9Renderer
{
	
//	NiDeclareRTTI;

public:

	// Renderer creation
    static NiDX9RendererEx* Create(unsigned int uiWidth, unsigned int uiHeight,
        unsigned int uiUseFlags, NiWindowRef kWndDevice, 
        NiWindowRef kWndFocus, unsigned int uiAdapter = D3DADAPTER_DEFAULT, 
        DeviceDesc eDesc = DEVDESC_PURE,
        FrameBufferFormat eFBFormat = FBFMT_UNKNOWN, 
        DepthStencilFormat eDSFormat = DSFMT_UNKNOWN, 
        PresentationInterval ePresentationInterval = PRESENT_INTERVAL_ONE,
        SwapEffect eSwapEffect = SWAPEFFECT_DEFAULT,
        unsigned int uiFBMode = FBMODE_DEFAULT,
        unsigned int uiBackBufferCount = 1, 
        unsigned int uiRefreshRate = REFRESHRATE_DEFAULT);

	NiDX9IndexBufferManager* GetIBManager() const;

	NiD3DGeometryGroupManager* GetGeometryGroupManager(){ return m_pkGeometryGroupManager; }

	NiUnsharedGeometryGroup* GetUnSharedGeometryGroup(){ return m_pkDefaultGroup; }


	void DrawPrimitive(NiGeometry* pkGeometry, NiGeometryData* pkData, 
		NiSkinInstance* pkSkinInstance, const NiTransform& kWorld, 
		const NiBound& kWorldBound, NiGeometryBufferData* pkBuffData);
/*
	void DrawSkinnedPrimitive(NiGeometry* pkGeometry, NiGeometryData* pkData, 
		const NiSkinInstance* pkSkinInstance, const NiTransform& kWorld, 
		const NiBound& kWorldBound);
*/
	bool IsHardwareSkinned( const NiGeometry* pkGeometry, const NiD3DShaderInterface* pkShader = NULL);

protected:

	void Do_RenderShape(NiTriShape* pkTriShape);

	
};

typedef NiPointer<NiDX9RendererEx> NiDX9RendererExPtr;

#endif