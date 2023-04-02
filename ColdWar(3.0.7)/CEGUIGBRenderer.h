#ifndef CEGUIGBRENDERER_H
#define CEGUIGBRENDERER_H

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"

#include <NiMain.h>
#include <NiScreenElements.h>

using namespace CEGUI;

class CEGUIGBTexture;

class CEGUIGBRenderer : public CEGUI::Renderer
{

	NiTObjectSet<NiScreenElementsPtr> m_kSetScreenElements;
	
	NiTPointerList<CEGUIGBTexture*> m_kListTextures;

	unsigned int m_uiActiveElements;
	
	CEGUI::Texture *m_pkPreviousTexture;
	
	NiScreenElementsPtr m_spImmediateElement;
	
	bool m_bQueuing;

	NiRenderer *m_pkGBRenderer;

public:
	
	CEGUIGBRenderer( NiRenderer *pkGBRenderer );
	virtual ~CEGUIGBRenderer();

	// 1���� ������Ʈ �߰�
	virtual void addQuad( const Rect& destRect, float fZ, const CEGUI::Texture* pTex,
		const CEGUI::Rect& textureRect, const CEGUI::ColourRect& colours,
		CEGUI::QuadSplitMode quad_split_mode );

	// ����
	virtual void doRender();
	
	// ���� ����Ʈ Ŭ����
	virtual void clearRenderList();

	// �� �ؽ��� ����( ������ ���� ��� ������.. )
	virtual CEGUI::Texture*	createTexture();

	virtual CEGUI::Texture*	createTexture(	const CEGUI::String& strFileName, 
											const CEGUI::String& resourceGroup );

	virtual	CEGUI::Texture*	createTexture(float size);

	virtual void destroyTexture( CEGUI::Texture* texture );

	virtual void destroyAllTextures();

	virtual bool	isQueueingEnabled(void) const	{return m_bQueuing;}

	virtual void	setQueueingEnabled(bool setting) {m_bQueuing = setting;}

	virtual float	getWidth(void) const		
	{ return (float)m_pkGBRenderer->GetDefaultBackBuffer()->GetWidth(); }
	
	virtual float	getHeight(void) const
	{ return (float)m_pkGBRenderer->GetDefaultBackBuffer()->GetHeight(); }
	
	virtual CEGUI::Size	getSize(void) const			
	{ return CEGUI::Size( getWidth(), getHeight() );}

	virtual CEGUI::Rect	getRect(void) const
	{ return CEGUI::Rect( CEGUI::Point(0,0), getSize()); }

	virtual	uint	getMaxTextureSize(void) const	{return 2048;}
	virtual	uint	getHorzScreenDPI(void) const	{return 96;}
	virtual	uint	getVertScreenDPI(void) const	{return 96;}

	void	setDisplaySize(const CEGUI::Size& sz);


};

#endif