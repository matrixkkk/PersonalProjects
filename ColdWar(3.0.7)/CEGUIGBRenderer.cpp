#include "CEGUIGBRenderer.h"
#include "CEGUIGBTexture.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"


NiScreenElements* CreateElement(CEGUIGBTexture *pkBaseTexture)
{
    NiScreenElements *pkElement = NiNew NiScreenElements( NiNew NiScreenElementsData(false, true, 1) );

    // Use alpha blending
    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty();
    pkAlphaProp->SetAlphaBlending(true);
    pkElement->AttachProperty(pkAlphaProp);

    // Don't use lighting.
    NiVertexColorProperty* pkVertexColorProp = NiNew NiVertexColorProperty();
    pkVertexColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pkElement->AttachProperty(pkVertexColorProp);

    // Set texture
    NiTexturingProperty *pkTexturingProp = NiNew NiTexturingProperty;
    
	if(pkBaseTexture)
        pkTexturingProp->SetBaseTexture( pkBaseTexture->m_spGBTexture );
    
	pkElement->AttachProperty(pkTexturingProp);

    pkElement->Insert(4);
    pkElement->UpdateProperties();

    return pkElement;
}

//---------------------------------------------------------------------------
CEGUIGBRenderer::CEGUIGBRenderer(NiRenderer *pkGamebryoRenderer)
{
	m_pkGBRenderer = pkGamebryoRenderer;
	m_spImmediateElement = 0;

	clearRenderList();

    d_identifierString = "CEGUIGBRenderer - Unofficial Gamebryo based "
		"renderer module for CEGUI";

    m_spImmediateElement = CreateElement(NULL);

//	m_bQueuing = false;
}
//---------------------------------------------------------------------------
CEGUIGBRenderer::~CEGUIGBRenderer(void)
{
	destroyAllTextures();
}
//---------------------------------------------------------------------------
void CEGUIGBRenderer::addQuad(const Rect& dest_rect, float z, 
	const Texture* tex, const Rect& texture_rect, const ColourRect& colours, 
	QuadSplitMode quad_split_mode)
{
  	int iHandle = 0;
	NiScreenElements *pkElement = NULL;

	if(!m_bQueuing)
	{
        pkElement = m_spImmediateElement;

        CEGUIGBTexture *pkTex = (CEGUIGBTexture*)tex;

        NiTexturingProperty *pkTexturingProp = (NiTexturingProperty *)pkElement->GetProperty(NiProperty::TEXTURING);

        pkTexturingProp->SetBaseTexture( pkTex->m_spGBTexture );
	}
	else if(m_pkPreviousTexture == tex)
	{
		pkElement = m_kSetScreenElements.GetAt( m_uiActiveElements - 1 );

		iHandle = pkElement->Insert(4);
	}
	else
	{
        if(m_uiActiveElements >= m_kSetScreenElements.GetSize())
        {
            pkElement = CreateElement( (CEGUIGBTexture*)tex );
            m_kSetScreenElements.Add( pkElement );
        }
        else
        {
            pkElement = m_kSetScreenElements.GetAt( m_uiActiveElements );
            iHandle = pkElement->Insert(4);

            CEGUIGBTexture *pkTex = (CEGUIGBTexture*)tex;

            NiTexturingProperty *pkTexturingProp = reinterpret_cast< NiTexturingProperty * >( pkElement->GetProperty(NiProperty::TEXTURING) );
            pkTexturingProp->SetBaseTexture(pkTex->m_spGBTexture);
        }

		m_uiActiveElements++;
		m_pkPreviousTexture = (Texture*)tex;
	}

	float fWidth = (float)m_pkGBRenderer->GetDefaultBackBuffer()->GetWidth();
	float fHeight = (float)m_pkGBRenderer->GetDefaultBackBuffer()->GetHeight();

	pkElement->SetRectangle(iHandle,	dest_rect.d_left / fWidth		, dest_rect.d_top / fHeight,
										dest_rect.getWidth() / fWidth	, dest_rect.getHeight() / fHeight);
	
	pkElement->SetTextures(iHandle, 0, 
		texture_rect.d_left, texture_rect.d_top,
		texture_rect.d_right, texture_rect.d_bottom);

	pkElement->SetColors(iHandle, 
		NiColorA(colours.d_top_left.getRed(), 
			colours.d_top_left.getGreen(), 
			colours.d_top_left.getBlue(), 
			colours.d_top_left.getAlpha()), 
		NiColorA(colours.d_bottom_left.getRed(), 
			colours.d_bottom_left.getGreen(), 
			colours.d_bottom_left.getBlue(), 
			colours.d_bottom_left.getAlpha()), 
		NiColorA(colours.d_bottom_right.getRed(), 
			colours.d_bottom_right.getGreen(), 
			colours.d_bottom_right.getBlue(), 
			colours.d_bottom_right.getAlpha()), 
		NiColorA(colours.d_top_right.getRed(), 
			colours.d_top_right.getGreen(), 
			colours.d_top_right.getBlue(), 
			colours.d_top_right.getAlpha()));

    if(!m_bQueuing)
        pkElement->RenderImmediate(m_pkGBRenderer);
}
//---------------------------------------------------------------------------
void CEGUIGBRenderer::doRender(void)
{
	m_pkGBRenderer->SetScreenSpaceCameraData();

	for(unsigned int i = 0; i < m_uiActiveElements; i++)
	{
		NiScreenElements* pkElement = m_kSetScreenElements.GetAt(i);

        if (pkElement) pkElement->RenderImmediate(m_pkGBRenderer);
	}
}
//---------------------------------------------------------------------------
void CEGUIGBRenderer::clearRenderList(void)
{
	m_uiActiveElements = 0;
	m_pkPreviousTexture = 0;

	for(unsigned int i = 0; i < m_kSetScreenElements.GetSize(); i++)
	{
		NiScreenElements* pkElement = m_kSetScreenElements.GetAt(i);

        if (pkElement)
			pkElement->RemoveAll();
	}
}
//---------------------------------------------------------------------------
Texture* CEGUIGBRenderer::createTexture(void)
{
	CEGUIGBTexture* tex = new CEGUIGBTexture(this);
	m_kListTextures.AddTail(tex);
	return tex;
}
//---------------------------------------------------------------------------
Texture* CEGUIGBRenderer::createTexture(const String& filename, 
											  const String& resourceGroup)
{
	CEGUIGBTexture* tex = reinterpret_cast< CEGUIGBTexture* >( createTexture() );

	try
	{
	    tex->loadFromFile(filename, resourceGroup);
	}
	catch (RendererException&)
	{
	    delete tex;
	    throw;
	}
	return tex;
}
//---------------------------------------------------------------------------
Texture* CEGUIGBRenderer::createTexture(float size)
{
	return createTexture();
}
//---------------------------------------------------------------------------
void CEGUIGBRenderer::destroyTexture(Texture* texture)
{
	if(texture)
		(reinterpret_cast< CEGUIGBTexture *>(texture))->m_spGBTexture = 0;
}
//---------------------------------------------------------------------------
void CEGUIGBRenderer::destroyAllTextures(void)
{
	m_kSetScreenElements.RemoveAll();

	NiTListIterator iter = m_kListTextures.GetHeadPos();
	
	while(iter)
	{
		CEGUIGBTexture *pkTex = m_kListTextures.GetNext(iter);
		destroyTexture(pkTex);
	}
	m_kListTextures.RemoveAll();
}
//---------------------------------------------------------------------------
