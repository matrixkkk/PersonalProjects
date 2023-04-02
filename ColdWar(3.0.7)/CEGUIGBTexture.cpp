#include "CEGUIGBTexture.h"
#include "CEGUIGBRenderer.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"

#include "CEGUIDefaultResourceProvider.h"

#include <NiRenderer.h>

//---------------------------------------------------------------------------
CEGUIGBTexture::CEGUIGBTexture(CEGUI::Renderer* owner) :
	Texture(owner), m_spGBTexture(0)
{
}
//---------------------------------------------------------------------------
CEGUIGBTexture::~CEGUIGBTexture(void)
{
}

ushort	CEGUIGBTexture::getWidth(void) const
{
	if( m_spGBTexture )
		return m_spGBTexture->GetWidth();
	return 0;
}

ushort	CEGUIGBTexture::getHeight(void) const		
{
	if( m_spGBTexture )
		return m_spGBTexture->GetHeight();
	return 0;
}
//---------------------------------------------------------------------------
void CEGUIGBTexture::loadFromFile(const CEGUI::String& filename, 
                                        const CEGUI::String& resourceGroup)
{	
	ResourceProvider *pkRes = System::getSingleton().getResourceProvider();
	DefaultResourceProvider *pkDefaultRes = (DefaultResourceProvider*)pkRes;

    char pcFinalFilename[255] = "./UI/imagesets/";

	NiStrcat(pcFinalFilename, 255, filename.c_str());

	m_spGBTexture = NiSourceTexture::Create(pcFinalFilename);
}
//---------------------------------------------------------------------------
void CEGUIGBTexture::loadFromMemory(const void* buffPtr, 
	uint buffWidth, uint buffHeight, Texture::PixelFormat pixFormat)
{
	// 일주일 동안 삽질.....
	NiPixelFormat kDesc(
		NiPixelFormat::FORMAT_RGBA,
		NiPixelFormat::COMP_RED,NiPixelFormat::REP_FIXED,8,
		NiPixelFormat::COMP_GREEN,NiPixelFormat::REP_FIXED,8,
		NiPixelFormat::COMP_BLUE,NiPixelFormat::REP_FIXED,8,
		NiPixelFormat::COMP_ALPHA,NiPixelFormat::REP_FIXED,8);

	NiPixelData *pkData = NiNew NiPixelData(buffWidth, buffHeight, kDesc);
	memcpy(pkData->GetPixels(), buffPtr, buffWidth * buffHeight * 4);

	m_spGBTexture = NiSourceTexture::Create(pkData);
}
//---------------------------------------------------------------------------

