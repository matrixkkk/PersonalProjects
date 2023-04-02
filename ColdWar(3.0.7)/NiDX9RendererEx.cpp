#include "main.h"
#include "NiDX9RendererEx.h"

#include "NiDX9PersistentSrcTextureRendererData.h"


// Renderer creation
NiDX9RendererEx* NiDX9RendererEx::Create(unsigned int uiWidth, unsigned int uiHeight,
    unsigned int uiUseFlags, NiWindowRef kWndDevice, 
    NiWindowRef kWndFocus, unsigned int uiAdapter, 
    DeviceDesc eDesc, FrameBufferFormat eFBFormat , 
    DepthStencilFormat eDSFormat, PresentationInterval ePresentationInterval,
    SwapEffect eSwapEffect , unsigned int uiFBMode,
    unsigned int uiBackBufferCount, unsigned int uiRefreshRate)
{

	// Create the Direct3D9 interface
    if (CreateDirect3D9() == -1)
    {
        SetLastErrorString("Creation failed: Could not initialize DirectX9");
        Error("NiDX9Renderer::Create> Invalid device ID specified "
            "- FAILING\n");
        return NULL;
    }

    // Create an 'empty' instance of the renderer
    NiDX9RendererEx* pkR = NiNew NiDX9RendererEx;
    assert(pkR);

    bool bSuccess = pkR->Initialize(uiWidth, uiHeight, uiUseFlags, kWndDevice, 
        kWndFocus, uiAdapter, eDesc, eFBFormat, eDSFormat, 
        ePresentationInterval, eSwapEffect, uiFBMode, uiBackBufferCount, 
        uiRefreshRate);

    if (bSuccess == false)
    {
        NiDelete pkR;
        pkR = NULL;
    }
    else
    {
        // Registration of the DX9-specific
        // NiDX9PersistentSrcTextureRendererData::LoadBinary() that overrides
        // NiPersistentSrcTextureRendererData::LoadBinary();
        NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
        NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", NiDX9PersistentSrcTextureRendererData::CreateObject);
    }

    return pkR;
}

NiDX9IndexBufferManager* NiDX9RendererEx::GetIBManager() const 
{
	if( m_pkIBManager )
		return m_pkIBManager;

	return NULL;
}
/*
void NiDX9RendererEx::Do_RenderShape(NiTriShape* pkTriShape)
{

	NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_SHAPE);

	if (m_bDeviceLost)
		return;

	NiTriShapeData* pkData = (NiTriShapeData*)pkTriShape->GetModelData();
	assert(pkData);
	if (pkData->GetActiveVertexCount() == 0)
		return;

    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();
    NiTransform kWorld = pkTriShape->GetWorldTransform();
    NiBound kWorldBound = pkTriShape->GetWorldBound();

    bool bHWSkinned = IsHardwareSkinned(pkTriShape);

	if ( pkData->GetConsistency() == NiGeometryData::VOLATILE || (pkSkinInstance != NULL && !bHWSkinned) )
	{
		// Dynamic geometry group
		m_pkGeometryGroupManager->AddObjectToGroup( (NiGeometryGroup*)m_pkDynamicGroup, pkData, pkSkinInstance, bHWSkinned, 0, 0);
	}
	else
	{
		// Default geometry group
		m_pkGeometryGroupManager->AddObjectToGroup( (NiGeometryGroup*)m_pkDefaultGroup, pkData, pkSkinInstance, bHWSkinned, 0, 0);
	}

    if (bHWSkinned)
    {
		DrawSkinnedPrimitive(pkTriShape, pkData, pkSkinInstance, kWorld, kWorldBound);
	}
	else
	{
		DrawPrimitive( pkTriShape, pkData, pkSkinInstance, kWorld, kWorldBound, (NiGeometryBufferData*)pkData->GetRendererData() );
	}
}

*/

bool NiDX9RendererEx::IsHardwareSkinned( const NiGeometry* pkGeometry, const NiD3DShaderInterface* pkShader)
{
	return NiDX9Renderer::IsHardwareSkinned( pkGeometry, pkShader );
}

void NiDX9RendererEx::Do_RenderShape(NiTriShape* pkTriShape)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_SHAPE);

    if (m_bDeviceLost)
        return;

    NiTriShapeData* pkData = (NiTriShapeData*)pkTriShape->GetModelData();
    assert(pkData);
    if (pkData->GetActiveVertexCount() == 0)
        return;
	NiGeometryBufferData* pRenderData = (NiGeometryBufferData*)pkData->GetRendererData();

    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();
    NiTransform kWorld = pkTriShape->GetWorldTransform();
    NiBound kWorldBound = pkTriShape->GetWorldBound();

    bool bHWSkinned = IsHardwareSkinned(pkTriShape);

	if ( pkData->GetConsistency() == NiGeometryData::VOLATILE || (pkSkinInstance != NULL && !bHWSkinned) )
	{
		// Dynamic geometry group
		m_pkGeometryGroupManager->AddObjectToGroup((NiGeometryGroup*)m_pkDynamicGroup, pkData, pkSkinInstance, bHWSkinned, 0, 0);
	}
	else
	{
		// Default geometry group
		m_pkGeometryGroupManager->AddObjectToGroup((NiGeometryGroup*)m_pkDefaultGroup, pkData, pkSkinInstance, bHWSkinned, 0, 0);
	}

	pRenderData = (NiGeometryBufferData*)pkData->GetRendererData();

    if (bHWSkinned)
    {
		DrawSkinnedPrimitive(pkTriShape, pkData, pkSkinInstance, kWorld, kWorldBound);
	}
	else
	{
		DrawPrimitive( pkTriShape, pkData, pkSkinInstance, kWorld, kWorldBound, (NiGeometryBufferData*)pkData->GetRendererData() );
	}
}


void NiDX9RendererEx::DrawPrimitive(NiGeometry* pkGeometry, 
    NiGeometryData* pkData, NiSkinInstance* pkSkinInstance, 
    const NiTransform& kWorld, const NiBound& kWorldBound, 
    NiGeometryBufferData* pkBuffData)
{
    assert (!m_bDeviceLost);

    NiD3DShaderInterface* pkShader = NULL;
	
	if (pkGeometry)
    {
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShaderFromMaterial());

        if (pkShader == 0)
        {
            const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
            pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            pkShader = NiDynamicCast(NiD3DShaderInterface, 
                pkGeometry->GetShaderFromMaterial());
            pkGeometry->SetActiveMaterial(pkTempMat);
        }
    }
    else
    {
        // In the case of NiScreenTexture
        pkShader = m_spLegacyDefaultShader;
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("DrawPrimitive> No shader found for object %s!\n"
            "Using Error Shader!\n", pkGeometry->GetName());
        assert (NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();
    }

    assert (pkShader);

    assert(m_pkCurrProp);

    unsigned int uiRet;

    // Preprocess the pipeline
    uiRet = pkShader->PreProcessPipeline(pkGeometry, pkSkinInstance, 
        pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    if (uiRet != 0)
    {
        assert(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiRemainingPasses = pkShader->FirstPass();

	unsigned int uiMetricDPCalls = 0;

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(pkGeometry, pkSkinInstance, 
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, 
            kWorldBound);

        // Set the transformations
		// 이곳에서 SetTransForm 등의 월드 변환을 한다.
        uiRet = pkShader->SetupTransformations(pkGeometry, pkSkinInstance, 
            NULL, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, 
            kWorldBound);

        // Prepare the geometry for rendering
		// 이곳에서 버텍스의 SetStreamSource와 SetIndices등을 한다.
        pkBuffData = (NiGeometryBufferData*)
            pkShader->PrepareGeometryForRendering(pkGeometry, NULL, 
            pkBuffData, m_pkCurrProp);

        // Set the shader programs
        // This is to give the shader final 'override' authority
        uiRet = pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, NULL,
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

		// 쉐이더 관련
        m_pkRenderState->CommitShaderConstants();

		// 인덱스 버퍼가 있다면
		// NiGeometryBufferData에서 IndexBuffer를 관리하고 있다면
        if (pkBuffData->GetIB())
        {
            unsigned int uiStartIndex = 0;
            for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
            {
                unsigned int uiPrimitiveCount;
                const unsigned short* pusArrayLengths = 
                    pkBuffData->GetArrayLengths();
                if (pusArrayLengths)
                {
                    // Assumes multiple arrays mean strips
                    uiPrimitiveCount = pusArrayLengths[i] - 2;
                }
                else
                {
                    assert (pkBuffData->GetNumArrays() == 1);
                    uiPrimitiveCount = pkBuffData->GetTriCount();
                }

                m_pkD3DDevice9->DrawIndexedPrimitive(pkBuffData->GetType(), 
                    pkBuffData->GetBaseVertexIndex(), 0, 
                    pkBuffData->GetVertCount(), uiStartIndex, 
                    uiPrimitiveCount);

                uiStartIndex += uiPrimitiveCount + 2;

				uiMetricDPCalls++;
				NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                    uiPrimitiveCount);
				NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
					pkBuffData->GetVertCount());
            }
        }
        else
        {
            m_pkD3DDevice9->DrawPrimitive(pkBuffData->GetType(), 
                pkBuffData->GetBaseVertexIndex(), pkBuffData->GetTriCount());

			uiMetricDPCalls++;
			NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
				pkBuffData->GetTriCount());
			NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
				pkBuffData->GetTriCount() * 3);
        }

        // Perform any post-rendering steps
        uiRet = pkShader->PostRender(pkGeometry, pkSkinInstance, NULL, 
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    // Reset the normalize normals flag
    m_pkRenderState->SetInternalNormalizeNormals(false);

	NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
