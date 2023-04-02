#ifndef GBOBJECT_H
#define GBOBJECT_H


class GBObject : public NiMemObject
{

public:

	virtual NiNode* GetObject()							= 0;
	virtual BOOL	UpdateObject( float fElapsedTime  )	= 0;
	virtual BOOL	ReleaseObject()						= 0;

};


#endif