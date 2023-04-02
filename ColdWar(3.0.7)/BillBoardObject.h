#ifndef BILLBOARDOBJECT_H
#define BILLBOARDOBJECT_H

#include <NiMain.h>

class BillBoardObject : public NiMemObject
{
	NiBillboardNodePtr	m_spRootObject;
	NiFixedString		m_strBillBoardName;
	float				m_fScale;

public:

	BillBoardObject();
	BillBoardObject( const NiFixedString& strFileName );
	~BillBoardObject();

	BOOL ReleaseObject();

	void	SetScale( float fScale );
	float	GetScale();

	BOOL	GenerateBillBoard( NiFixedString& strFileName );
	NiBillboardNode* GetObject();
	NiBillboardNode* Clone();	
	NiFixedString	GetFileName();

};


#endif