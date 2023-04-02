#ifndef DECALOBJECT_H
#define DECALOBJECT_H


#include <NiSystem.h>
#include <NiMain.h>

class DecalObject : public NiMemObject
{

	NiTriShapePtr		m_spDecalShape;
	NiFixedString		m_strDecalName;
	float				m_fScale;
	int					m_nType;
	float				m_fWidth;
	float				m_fHeight;
	float				m_fDepth;

public:

	DecalObject();
	DecalObject( const NiFixedString& strFileName,int nType );
	~DecalObject(); 

	BOOL	ReleaseObject();
	void	SetScale( float fScale );
	float	GetScale();
	int		IsDecalType();

	BOOL			GenerateDecal( const NiFixedString& strFileNae, int nType );
	NiTriShape*		GetObject();
	NiTriShape*		Clone();	
	NiFixedString	GetFileName();
};



#endif 