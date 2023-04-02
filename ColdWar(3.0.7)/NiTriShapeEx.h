#ifndef NITRISHAPEEX_H
#define NITRISHAPEEX_H


#include "NiTriShape.h"
#include "NiDX9RendererEx.h"

class NiDX9RendererEx;

class NiTriShapeEx : public NiTriShape
{
//    NiDeclareRTTI;
    NiDeclareClone(NiTriShapeEx);
    NiDeclareStream;
    NiDeclareViewerStrings;

	NiGeometryBufferData*	m_pGeometryBufferData;

	D3DIndexBufferPtr		m_pIB;
	unsigned int			m_uiIndexCount;
	unsigned int			m_uiIBCount;
	BOOL					m_bOldIB;


public:

	void SetIB( D3DIndexBufferPtr pIB, unsigned int uiIndexCount, unsigned int uiIBSize );
	void SetIB( D3DIndexBufferPtr pIB );
	void GetGeometryBufferData();

	// The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriShapeEx(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
        unsigned short usNumTextureSets, NiGeometryData::DataFlags eNBTMethod,
        unsigned short usTriangles, unsigned short* pusTriList);

    // The constructed object shares the input data.
    NiTriShapeEx(NiTriShapeData* pkModelData);
	~NiTriShapeEx();

	virtual void RenderImmediate(NiRenderer* pkRenderer);

protected:
    // streaming support
    NiTriShapeEx();

};


typedef NiPointer<NiTriShapeEx> NiTriShapeExPtr;

#endif