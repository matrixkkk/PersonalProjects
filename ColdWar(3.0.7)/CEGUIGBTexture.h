#ifndef CEGUIGBTEXTURE_H
#define CEGUIGBTEXTURE_H

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include "CEGUIGBRenderer.h"

using namespace CEGUI;

class CEGUIGBTexture : public CEGUI::Texture
{
private:

	friend	CEGUI::Texture* CEGUIGBRenderer::createTexture(void);
	friend	CEGUI::Texture* CEGUIGBRenderer::createTexture(const String& filename, const String& resourceGroup);
	friend	CEGUI::Texture* CEGUIGBRenderer::createTexture(float size);
	friend	void	 CEGUIGBRenderer::destroyTexture(Texture* texture);


	CEGUIGBTexture( CEGUI::Renderer* owner);
	virtual ~CEGUIGBTexture(void);

public:
	virtual	ushort	getWidth(void) const;		
	//	{return m_spGBTexture->GetWidth();}

	virtual	ushort	getHeight(void) const;		
	//	{return m_spGBTexture->GetHeight();}

	virtual void loadFromFile(const CEGUI::String& filename, 
		const CEGUI::String& resourceGroup);

	virtual void loadFromMemory(const void* buffPtr, 
		uint buffWidth, uint buffHeight, PixelFormat pixelFormat);

	NiTexturePtr m_spGBTexture;
};

#endif	